/*
 * heap.h
 *
 *  Created on: 2015. 5. 3.
 *      Author: innocentevil
 */

#ifndef CDSL_HEAP_H_
#define CDSL_HEAP_H_

#include "base_tree.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define cdsl_heapMaxDepth(root)                 tree_max_depth((base_treeRoot_t*) root)
#define cdsl_heapForEach(root, cb, order)       tree_for_each((base_treeRoot_t*) root, (base_tree_callback_t) cb, order)
#define cdsl_heapSize(root)                     tree_size((base_treeRoot_t*) root)
#define cdsl_heapTop(root)                      (heapNode_t*) tree_top((base_treeRoot*) root)
#define cdsl_heapPrint(root, print)             tree_print((base_treeRoot_t*) root, print)
#define cdsl_heapIsEmpty(root)                  tree_is_empty((base_treeRoot_t*) root)
#define cdsl_heapGoLeft(cur)                    (heapNode_t*) tree_go_left((base_treeNode_t*) cur)
#define cdsl_heapGoRight(cur)                   (heapNode_t*) tree_go_right((base_treeNode_t*) cur)

typedef struct heap_node heapNode_t;
typedef struct heap_root heapRoot_t;

#define MAX_HEAP      ((char) 0)
#define MIN_HEAP      ((char) 1)
typedef char heapType_t;

typedef heapNode_t* (*heapEvaluate)(heapNode_t* a, heapNode_t* b);

struct heap_node {
	union {
		struct {
			heapNode_t*	left, *right;
			trkey_t       key;
		};
		base_treeNode_t node;
	};
};

struct heap_root {
	union {
		heapNode_t* entry;
		base_treeRoot_t root;
	};
	int dir;
	heapType_t type;
};

extern void cdsl_heapRootInit(heapRoot_t* rootp,heapType_t type);
extern void cdsl_heapNodeInit(heapNode_t* node,trkey_t key);
extern BOOL cdsl_heapEnqueue(heapRoot_t* rootp,heapNode_t* item);
extern heapNode_t* cdsl_heapDeqeue(heapRoot_t* rootp);

#if defined(__cplusplus)
}
#endif

#endif /* HEAP_H_ */
