# cdsl 
	Data Structure library for C / C++.   
	
	
## About
	light-weight implementation of various data structures, including sigle / double linked list, 
	binary search tree, some balanced tree (red-black tree, splay tree), binary heap.

## Note
    Recursion is used for some of the balanced tree implementations to minimize the size of node 
    type (no pointer to parent node is required). it's my design choice to use this in my own 
    developed kernel where the kernel has fair amount of stack (order of a few killobytes) compared 
    to the stack usage when handling 2,000,000 nodes in red-black tree. (it is 2kbytes at maximum in my 
    64 bit desktop) I think it's insignificant memory compared to modern computer with a few gigabytes 
    of RAM.
    
    all the trees in cdsl depends on recursion somehow because all of them inherits same base implementaion
    (base_tree) which depends on recursion. 
    
    trees heavily depends on recursion
    - splay tree (spltree)
    - red black tree (nrbtree)
    - hash tree (hashtree)
    - binary heap (heap)
    
    trees using recursion partially
    - plain binary search tree (bstree) 
    
    there is no recursion usage in list family
    
    
	 
## build requirement 
	GNU toolchain / clang / python / pip   
			 
## How-to-build (Ubuntu)
	1. release build (libcdsl.a libcdsl.so)
	$> make config
	$> make release
	
	2. unit testing
	$> make config
	$> make test
	
## LICENSE 
	BSD-2

