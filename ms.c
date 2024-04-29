#include "all.h"
/* ms.c ((elided)) (THIS CAN'T HAPPEN -- claimed code was not used) */
/* private declarations for mark-and-sweep collection 267a */
typedef struct Mvalue Mvalue;
struct Mvalue {
    Value v;
    unsigned live:1;
};
/* private declarations for mark-and-sweep collection 267c */
#ifndef GCHYPERDEBUG /*OMIT*/
#define GROWTH_UNIT 24 /* increment in which the heap grows, in objects */
#else /*OMIT*/
#define GROWTH_UNIT 3 /*OMIT*/
#endif /*OMIT*/
typedef struct Page Page;
struct Page {
    Mvalue pool[GROWTH_UNIT];
    Page *tl;
};
/* private declarations for mark-and-sweep collection 267d */
Page *pagelist, *curpage;
Mvalue *hp, *heaplimit;
/* private declarations for mark-and-sweep collection 268c */
static void visitloc          (Value *loc);
static void visitvalue        (Value v);
static void visitenv          (Env env);
static void visitexp          (Exp exp);
static void visitexplist      (Explist es);
static void visitframe        (Frame *fr);
static void visitstack        (Stack s);
static void visittest         (UnitTest t);
static void visittestlists    (UnitTestlistlist uss);
static void visitregister     (Register reg);
static void visitregisterlist (Registerlist regs);
static void visitroots        (void);
static void mark              (void);
/* private declarations for mark-and-sweep collection S641a */
static int nalloc = 0;              /* total number of allocations */
static int ncollections = 0;        /* total number of collections */
static int nmarks = 0;              /* total number of cells marked */
/* ms.c 267b */
bool gc_uses_mark_bits = true;
/* ms.c 267e */
static void makecurrent(Page *page) {
    assert(page != NULL);
    curpage = page;
    hp = &page->pool[0];
    heaplimit = &page->pool[GROWTH_UNIT];
}
/* ms.c 268a */
static int heapsize;            /* OMIT */
static void addpage(void) {
    Page *page = calloc(1, sizeof(*page));
    assert(page != NULL);

/* tell the debugging interface that each object on [[page]] has been acquired 282d */
    {   unsigned i;
        for (i = 0; i < sizeof(page->pool)/sizeof(page->pool[0]); i++)
            gc_debug_post_acquire(&page->pool[i].v, 1);
    }

    if (pagelist == NULL) {
        pagelist = page;
    } else {
        assert(curpage != NULL && curpage->tl == NULL);
        curpage->tl = page;
    }
    makecurrent(page);
    heapsize += GROWTH_UNIT;   /* OMIT */
}

// PT 1: Implement MARK
// Implement a mark procedure. Write a procedure mark that implements the
// mark phase of a mark‐and‐sweep garbage collector. At each collection, tra‐
// verse the root set and mark each reachable Mvalue as live. Use the visiting
// procedures in Sections 4.4.2 and N.1.1. Your mark procedure should call the
// appropriate visiting procedure for each of the roots.
static void mark() {
    int startN = nmarks;
    visitroots();
    ncollections++;
    int liveData = nmarks - startN;
    printf("[GC stats: heap size %d live data %d ratio %.2f]\n", heapsize, liveData, (double)heapsize/liveData);
    if (ncollections % 10 == 0) {
        printf("[Mem stats: allocated %d heap size %d ratio %.2f]\n", nalloc, heapsize, (double)nalloc/heapsize);
    }
}

// PT 2: Implement ALLOCLOC
// Implement a markandsweep allocator. Create an allocator that, together with
// your mark procedure from Exercise 9, forms a complete mark‐and‐sweep sys‐
// tem.
// • The allocator must implement not only allocation but also the unmark
//   and sweep phases of collection. It will sweep through the heap from the
//   first page to the last page. Instead of just taking the location pointed to
//   by hp, as in chunk 268b, the allocator must check to see if the location
//   is marked live. If marked, it was live at the last collection, so it cannot
//   be used to satisfy the allocation request. Skip past it and mark it not
//   live.
//   When the allocator finds an unmarked object, it sweeps past the object
//   and returns it to satisfy the allocation request, just as in chunk 268b.
//   If the allocator reaches the end of the heap without finding an un‐
//   marked object, it calls mark.
// • In the new system, end‐of‐page is not the same as end‐of‐heap.
//   White areas, to the left of the heap pointer, have been used to satisfy
//   previous allocation requests. Areas marked with gray diamonds, to the
//   right of the heap pointer, are potentially available to satisfy future allo‐
//   cation requests.5 When hp reaches heaplimit, the allocator must look
//   at the next page; modify the code in chunk 268b to make it so. Only
//   after there are no more pages may the allocator call mark.
// • After your allocator calls mark, have it call makecurrent(pagelist),
//   which will reset hp, heaplimit, and curpage to point into the first page.
// • The allocator now guarantees that when mark is called, the entire heap
//   is unmarked (Exercise 12). But mark cannot guarantee to recover any‐
//   thing; every cell on the heap might be live. The allocator may have to
//   enlarge the heap by adding a new page. Write a procedure growheap
//   for this purpose.
//
// You may find it helpful to split allocloc into two functions: one that attempts
// to allocate without calling mark, but sometimes fails, and another that may
// call the first function, mark, and growheap.
/* ms.c ((prototype)) 268b */
Value* allocloc(void) {

    if (pagelist == NULL) {
        addpage();
    }

    // Loop through the pages
    while (curpage != NULL) {
        // Try to find an unmarked object again
        while (hp < heaplimit) {
            Mvalue *m = hp;
            if (!m->live) {
                // Found an unmarked object
                /* tell the debugging interface that [[&hp->v]] is about to be allocated 282e */
                gc_debug_pre_allocate(&hp->v);
                nalloc++;
                return &(hp++)->v;
            } else {
                // Found a marked object
                // Skip past it and mark it as not live
                m->live = 0;
            }
            hp++; // Move to the next object
        }

        // If it exists, move to the next page, else break
        if (curpage->tl != NULL) {
            makecurrent(curpage->tl);
        } else {
            break;
        }
    }

    // If all objects are marked, mark-and-sweep, then retry allocation
    mark(); // Mark phase
    makecurrent(pagelist); // Reset heap pointers to the first page

    // Loop through the pages
    while (curpage != NULL) {
        // Try to find an unmarked object again
        while (hp < heaplimit) {
            Mvalue *m = hp;
            if (!m->live) {
                // Found an unmarked object
                /* tell the debugging interface that [[&hp->v]] is about to be allocated 282e */
                gc_debug_pre_allocate(&hp->v);
                nalloc++;
                return &(hp++)->v;
            } else {
                // Found a marked object
                // Skip past it and mark it as not live
                m->live = 0;
            }
            hp++; // Move to the next object
        }

        // If it exists, move to the next page, else break
        if (curpage->tl != NULL) {
            makecurrent(curpage->tl);
        } else {
            break;
        }
    }

    // If no unmarked objects found, grow the heap
    addpage();
    
    // Try allocation again after growing the heap
    return allocloc();
}

/* ms.c 269b */
static void visitenv(Env env) {
    for (; env; env = env->tl)
        visitloc(env->loc);
}
/* ms.c ((prototype)) 269c */
static void visitloc(Value *loc) {
    Mvalue *m = (Mvalue*) loc;
    if (!m->live) {
        m->live = 1;
        nmarks++;
        visitvalue(m->v);
    }
}
/* ms.c 269d */
static void visitregister(Value *reg) {
    visitvalue(*reg);
}
/* ms.c 269e */
static void visitvalue(Value v) {
    switch (v.alt) {
    case NIL:
    case BOOLV:
    case NUM:
    case SYM:
    case PRIMITIVE:
        return;
    case PAIR:
        visitloc(v.pair.car);
        visitloc(v.pair.cdr);
        return;
    case CLOSURE:
        visitexp(v.closure.lambda.body);
        visitenv(v.closure.env);
        return;
    default:
        assert(0);
        return;
    }
    assert(0);
}
/* ms.c S363a */
static void visitexp(Exp e) {
    switch (e->alt) {
    /* cases for [[visitexp]] S363b */
    case LITERAL:
        visitvalue(e->literal);
        return;
    case VAR:
        return;
    case IFX:
        visitexp(e->ifx.cond);
        visitexp(e->ifx.truex);
        visitexp(e->ifx.falsex);
        return;
    case WHILEX:
        visitexp(e->whilex.cond);
        visitexp(e->whilex.body);
        return;
    case BEGIN:
        visitexplist(e->begin);
        return;
    /* cases for [[visitexp]] S364a */
    case SET:
        visitexp(e->set.exp);
        return;
    case LETX:
        visitexplist(e->letx.es);
        visitexp(e->letx.body);
        return;
    case LAMBDAX:
        visitexp(e->lambdax.body);
        return;
    case APPLY:
        visitexp(e->apply.fn);
        visitexplist(e->apply.actuals);
        return;
    /* cases for [[visitexp]] S364b */
    case BREAKX:
        return;
    case CONTINUEX:
        return;
    case RETURNX:
        visitexp(e->returnx);
        return;
    case THROW:
        visitexp(e->throw.exp);
        return;
    case TRY_CATCH:
        visitexp(e->try_catch.handler);
        visitexp(e->try_catch.body);
        return;
    case LONG_GOTO:
        visitexp(e->long_goto.exp);
        return;
    case LONG_LABEL:
        visitexp(e->long_label.body);
        return;
    case LOWERED:
        visitexp(e->lowered.before);
        return;
    case LOOPBACK:
        return;
    /* cases for [[visitexp]] S364c */
    case ENV:
        visitenv(e->env.contents);
        return;
    case HOLE:
        return;
    }
    assert(0);
}
/* ms.c S364d */
static void visitexplist(Explist es) {
    for (; es; es = es->tl)
        visitexp(es->hd);
}
/* ms.c S365a */
static void visitregisterlist(Registerlist regs) {
    for ( ; regs != NULL; regs = regs->tl)
        visitregister(regs->hd);
}
/* ms.c S365b */
/* representation of [[struct Stack]] S343a */
struct Stack {
    int size;
    Frame *frames;  // memory for 'size' frames
    Frame *sp;      // points to first unused frame
};
static void visitstack(Stack s) {
    Frame *fr;
    for (fr = s->frames; fr < s->sp; fr++) {
        visitframe(fr);
    }
}
/* ms.c S365c */
static void visitframe(Frame *fr) {
    visitexp(&fr->form);
    if (fr->syntax != NULL)
        visitexp(fr->syntax);
}
/* ms.c S365d */
static void visittestlists(UnitTestlistlist uss) {
    UnitTestlist ul;

    for ( ; uss != NULL; uss = uss->tl)
        for (ul = uss->hd; ul; ul = ul->tl)
            visittest(ul->hd);
}
/* ms.c S365e */
static void visittest(UnitTest t) {
    switch (t->alt) {
    case CHECK_EXPECT:
        visitexp(t->check_expect.check);
        visitexp(t->check_expect.expect);
        return;
    case CHECK_ASSERT:
        visitexp(t->check_assert);
        return;
    case CHECK_ERROR:
        visitexp(t->check_error);
        return;
    }
    assert(0);
}
/* ms.c S366a */
static void visitroots(void) {
    visitenv(*roots.globals.user);
    visittestlists(roots.globals.internal.pending_tests);
    visitstack(roots.stack);
    visitregisterlist(roots.registers);
}
/* ms.c ((prototype)) S377g */
void printfinalstats(void) { 
    printf("[Mem stats: allocated %d heap size %d ratio %.2f]\n", nalloc, heapsize, (double)nalloc/heapsize);
    printf("[Total GC work: %d collections marked %d objects; %.2f marks/allocation]\n", ncollections, nmarks, (double)nmarks/nalloc);
}
/* ms.c ((prototype)) S377h */
void avoid_unpleasant_compiler_warnings(void) {
    (void)visitroots;
}
