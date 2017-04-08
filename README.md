# cdsl 
[![Build Status](https://travis-ci.org/fritzprix/cdsl.svg?branch=master)](https://travis-ci.org/fritzprix/cdsl)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/fb90f6cc7f2a45ac8566c1407ad2635b)](https://www.codacy.com/app/innocentevil0914/cdsl?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=fritzprix/cdsl&amp;utm_campaign=Badge_Grade)

	Data Structure library for C / C++.   
	
## About
	simple C implementations of various data structures, including sigle / double linked list,
	binary search tree, some balanced tree (red-black tree, splay tree), binary heap.
	 

## Note
    all the tree implementations depend on recursion which is typically considered impractical 
    due to its stack usage. however, for balanced tree family (AVL, Red-Black ..), it's possible to keep recursion depth in reasonable level 
    so recursion can be practical for them. for example, red black tree requires only 2kbytes at maximum when handling 2,000,000 nodes.    
    most of them are focused to reduce memory foot print cosidering very constrained system. (typically having only a few killobytes of RAM)
    you can see this kind of effort by investigating node structure in red black tree implmentation. and all the implementation adopt object-oriented concept so you can easily 
    integrate it with your own data structure design. (inheritance / genericity)
         
    
     
    trees heavily depends on recursion
    - splay tree (spltree)
    - red black tree (nrbtree)
    - avl tree (avltree)
    - hash tree (hashtree)
    - binary heap (heap)
    
    trees using recursion partially
    - plain binary search tree (bstree) 
    
    there is no recursion usage in list family
    - single-linked list
    - double-linked list
    
## Build requirement 
	GNU toolchain / clang / python / pip   
			 
## How-to-build (Ubuntu)
	1. release build (libcdsl.a libcdsl.so)
	$> make config
	$> make release
	
	2. unit testing
	$> make config
	$> make test
	
## How-to-install (x86)
	1. release build (libcdsl.a libcdsl.so)
	for 32 bit machine
	$> make config DEFCONF=x86_32
	
	for 64 bit machine 
	$> make config DEFCONF=x86_64
	
	build library
	$> make release
	
	2. install them
	$> sudo make install 

	
## LICENSE 
	BSD-2

