/*
 * heap.h
 *
 *  Created on: 2015. 5. 3.
 *      Author: innocentevil
 */

#ifndef CDSL_HEAP_H_
#define CDSL_HEAP_H_

#include "base_tree.h"
#include "cdsl.h"

#if defined(__cplusplus)
extern "C" {
#endif




typedef struct heap_node heapNode_t;
typedef struct heap_root heapRoot_t;
typedef enum { MAX_HEAP, MIN_HEAP } heapType_t;
typedef heapNode_t* (*heapEvaluate)(heapNode_t* a, heapNode_t* b);

struct heap_node {
	union {
		struct {
			heapNode_t*	left, *right;
		};
		base_treeNode_t _node;
	};
	uint32_t 	key;
};

struct heap_root {
	union {
		heapNode_t* entry;
		base_treeRoot_t _root;
	};
	int dir;
	heapType_t type;
};

extern void heap_root_init(heapRoot_t* rootp,heapType_t type);
extern void heap_node_init(heapNode_t* node,int key);
extern int heap_enqueue(heapRoot_t* rootp,heapNode_t* item);
extern heapNode_t* heap_deqeue(heapRoot_t* rootp);
extern void heap_print(heapRoot_t* rootp,cdsl_generic_printer_t print);
extern int heap_size(heapRoot_t* rootp);
extern int heap_max_depth(heapRoot_t* rootp);

#if defined(__cplusplus)
}
#endif

#endif /* HEAP_H_ */
