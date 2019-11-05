/*
 * CMPT 300 Assignment 1
 * Group Name: MuseYao
 * Group Member:
 * zhouyaoz@sfu.ca (301368735)
 * yla395@sfu.ca (301270712)
 */

I. File list
------------
README.txt

prj1/mystring:
mystring.c   		string implementation
mystring.h  		string header
test_mystring.c 	string test file
Makefile			Makefile to compile, excute and run all files

prj1/list:
list.c 			linked list implementation
list.h 			linked ist header
test_list.c 	linked list test file
Makefile		Makefile to compile, excute and run all files


Program can be built using Makefile.



II. Content
----------
A. Mystring

In mystring.c, it contains several basic functions, such as mystrlen, mystrcpy, mystrcmp and mystrdup.


B. Linked list

1. In list.c, there are several basic functions including initialization, insert from head or tail, count number of nodes, find a node, deletion and sort.


2. list_sort()
The basic algorithm for this function is to save the all items in the list to an array and then use quick sort in the array. Finally, insert all sorted items back to the list.



III. Sorting Algorithm Analysis
------------------------------

QuickSort: 
   Quicksort should run in N log N for best and average case scenarios.
   It is reasonably efficient on binary comparisons.
