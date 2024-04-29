Name(s): Evan Prizel, Lincoln Mercuro
Project Choice: Mark-Sweep Garbage Collector For muScheme
Time Spent on Assignment:

------------------------
Description of solution: 
------------------------
Our solution started with implementing the mark procedure, which implements the mark phase
of a mark-sweep garbage collector. This procedure operates as follows: when an object is created,
it's marked bit is set to 0. During the mark phase, we set the marked bit for all reachable
objects to 1. To perform this operation we need to do a graph traversal to visit all the roots.

Next, we had to implement the remainder of the mark-sweep algorithm, which includes allocation,
unmark, and sweep phases. The allocloc function in a mark-and-sweep garbage collector allocates memory 
by searching for unmarked objects in the heap. If the heap is uninitialized, it adds a new page. The 
function iterates through the heap to find and return an unmarked object, marking it as live and updating
allocation counters. If no unmarked objects are found, it performs a mark phase, resets the heap pointers, 
and tries allocation again. If still unsuccessful, it expands the heap by adding another page and recurses 
into itself for another allocation attempt, ensuring efficient memory use by reusing spaces and minimizing 
frequent heap expansions.

Lastly, we collected statistics about our program. After every collection, the program prints:

[GC stats: heap size 24 live data 24 ratio 1.00]

Show the heap size, live data, and their ratio. 

---------------------------
How we tested our solution:
---------------------------

We tested our program using files eval.scm and evaltest.scm, as suggested by the project description.
We compared our output from testing with these files to file eval_evaltest.soln.out.

TODO may need to expand on this

------------------------------------------
Description of our solutions functionality:
------------------------------------------

Currently our, solution is producing correct output up until a certain point, where it crashes. 

TODO finish this once project is working
