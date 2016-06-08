# cdsl 
[![Build Status](https://travis-ci.org/fritzprix/cdsl.svg?branch=master)](https://travis-ci.org/fritzprix/cdsl)

	Data Structure library for C / C++.   
	
## About
	simple C implementations of various data structures, including sigle / double linked list,
	binary search tree, some balanced tree (red-black tree, splay tree), binary heap.

## Note
    all the tree implementations depends on recursion which is typically considered impractical 
    due to its stack usage. however, many balanced tree keeps recursion depth in reasonable level 
    so recursion can be practical for those types of data structures. 
    for example, red black tree requires 2kbytes maximum when handling 2,000,000 nodes.    
     
    trees heavily depends on recursion
    - splay tree (spltree)
    - red black tree (nrbtree)
    - hash tree (hashtree)
    - binary heap (heap)
    
    trees using recursion partially
    - plain binary search tree (bstree) 
    
    there is no recursion usage in list family
    
## Build requirement 
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

