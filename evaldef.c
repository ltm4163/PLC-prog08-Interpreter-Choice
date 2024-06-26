#include "all.h"
/* evaldef.c 159e */
Env evaldef(Def d, Env env, Echo echo) {
    reset_overflow_check();     /* OMIT */
    switch (d->alt) {
    case VAL:    /* evaluate [[val]] binding and return new environment 160a */
                 {
                     pushframe(*d->val.exp, roots.stack);
                     if (find(d->val.name, env) == NULL)
                         env = bindalloc(d->val.name, unspecified(), env);
                     *d->val.exp = topframe(roots.stack)->form;
                     popframe(roots.stack);
                     Value v = eval(d->val.exp, env);
                     *find(d->val.name, env) = v;

/* if [[echo]] calls for printing, print either [[v]] or the bound name S311e */
                     if (echo == ECHOING) {
                         if (d->val.exp->alt == LAMBDAX)
                             print("%n\n", d->val.name);
                         else
                             print("%v\n", v);
                     }
                     return env;
                 }
    case EXP:
    /* evaluate expression, assign to [[it]], and return new environment 160b */
                 {
                     Value v = eval(d->exp, env);
                     Value *itloc = find(strtoname("it"), env);
                     /* if [[echo]] calls for printing, print [[v]] S312a */
                     if (echo == ECHOING)
                         print("%v\n", v);
                     if (itloc == NULL) {
                         return bindalloc(strtoname("it"), v, env);
                     } else {
                         *itloc = v;
                         return env;
                     }
                 }
    case DEFINE:
              /* evaluate function definition and return new environment 160c */
                 return evaldef(mkVal(d->define.name, mkLambdax(d->define.lambda
                                                                             )),
                                env, echo);
    case DEFS:                                                     /*OMIT*/
        for (Deflist ds = d->defs; ds != NULL; ds = ds->tl)      /*OMIT*/
            env = evaldef(ds->hd, env, echo);                      /*OMIT*/
        return env;                                                /*OMIT*/
    }
    assert(0);
}
/* evaldef.c S310e */
void readevalprint(XDefstream xdefs, Env *envp, Echo echo) {
    roots.globals.internal.pending_tests =
                              mkULL(NULL, roots.globals.internal.pending_tests);
    roots.registers = NULL;  // clean up after syntax error

    for (XDef xd = getxdef(xdefs); xd; xd = getxdef(xdefs)) {
        /* lower extended definition [[xd]] as needed S311a */
        /* not in uScheme */
        /* lower extended definition [[xd]] as needed S351e */
        lowerXdef(xd);
        switch (xd->alt) {
        case DEF:
            *envp = evaldef(xd->def, *envp, echo);
            break;
        case USE:
            /* read in a file and update [[*envp]] S311c */
            {
                const char *filename = nametostr(xd->use);
                FILE *fin = fopen(filename, "r");
                if (fin == NULL)
                    runerror("cannot open file \"%s\"", filename);
                readevalprint(filexdefs(filename, fin, NOT_PROMPTING), envp,
                                                                          echo);
                fclose(fin);
            }
            break;
        case TEST:
            roots.globals.internal.pending_tests->hd =
                  mkUL(xd->test, roots.globals.internal.pending_tests->hd);
            break;
        default:
            assert(0);
        }
    }

    process_tests(roots.globals.internal.pending_tests->hd, *envp);
    roots.globals.internal.pending_tests = popULL(
                                          roots.globals.internal.pending_tests);
}
