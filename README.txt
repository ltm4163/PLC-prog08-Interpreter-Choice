Name(s): Evan Prizel, Lincoln Mercuro
Project Choice: Mark-Sweep Garbage Collector For uScheme
Time Spent on Assignment: 8hrs

------------------------
Description of solution: 
------------------------
Our solution started with implementing the mark procedure, which implements the mark phase
of a mark-sweep garbage collector. This procedure operates as follows: when an object is created,
it's marked bit is set to 0 (with calloc). During the mark phase, we set the marked bit for all reachable
objects to 1. To perform this operation we need to do a graph traversal to visit all the roots.

Next, we had to implement the remainder of the mark-sweep algorithm, which includes the allocation,
unmark, and sweep phases. The allocloc function in a mark-and-sweep garbage collector allocates memory 
by searching for unmarked objects in the heap. If the heap is uninitialized, it adds a new page. The 
function iterates through the heap starting at the heap pointer to find and return an unmarked object.
As we iterate through the heap, any marked objects are unmarked when they are passed. This provides the unmark
and sweep functionality as we allocate. If an unmarked object is found, we properly free (as needed) and 
allocate the object, then return the address to the object. If no unmarked objects are found, it performs 
the mark phase, resets the heap pointers to the first page, and tries allocation again using the algorithm 
described above. If an unmarked object is still not found, it expands the heap by adding another page and 
recurses for another allocation attempt, ensuring efficient memory use by reusing spaces and minimizing 
frequent heap expansions.

Lastly, we collected statistics about our program. 
  - After every collection, the program prints garbage collector statistics:
    [GC stats: heap size # live data # ratio #.##]
  - After every 10 collections, or after the interpreter exits, the program prints memory statistics:
    [Mem stats: allocated # heap size # ratio #.##]
  - When the interpreter exits, the program will print total number of collections and marks during
    all collections:
    [Total GC work: # collections marked # objects; #.## marks/allocation]

---------------------------
How we tested our solution:
---------------------------

We tested our program using files eval.scm and evaltest.scm, as suggested by the project description.
We compared our output from testing with these files to file eval_evaltest.soln.out. In order to get
accurate analysis of our program, we implemented the gathering of statistics before testing the entire
interpreter. We did this so that we could analyze any areas of our program that were incorrect. For
example, if we had less live data than expected then we can assume there is an issue with marking.
Another example is if there is a larger heap size then expected, we can assume there may be an issue
with the allocation algorithm.

------------------------------------------
Description of our solutions functionality:
------------------------------------------

Currently our solution performs exactly as expected, with the expected output from eval_evaltest.soln.out 
matching the output from our program. The garbage collector uses the algorithms described in "Description
of solution:" for mark, unmark, sweep, and allocate. We also collect statistics and print out the results
on different intervals, as described earlier.
