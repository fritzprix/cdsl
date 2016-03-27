/*
 * cdsl_bst.h
 *
 *  Created on: 2014. 9. 12.
 *      Author: innocentevil
 */

#ifndef CDSL_BSTREE_H_
#define CDSL_BSTREE_H_

#include "cdsl.h"
#include "base_tree.h"

#if defined(__cplusplus)
extern "C"{
#endif


typedef struct bs_treenode bstreeNode_t;
typedef struct bs_treeroot bstreeRoot_t;

#define cdsl_bstreeMaxDepth(root)                 tree_max_depth((base_treeRoot_t*) root)
#define cdsl_bstreeTraverse(root, cb, order)      tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, order)
#define cdsl_bstreeSize(root)                     tree_size((base_treeRoot_t*) root)
#define cdsl_bstreePrint(root, print)             tree_print((base_treeRoot_t*) root, print)

#define ORDER_INC			(int) 0
#define ORDER_DEC			(int) 1

#define BREAK_TRAVERSE		(int) -1

typedef int (*bs_tree_callback_t) (int order,void*);

struct bs_treenode {
	union {
		struct {
			bstreeNode_t *left, *right;
		};
		base_treeNode_t node;
	};
	int key;
};

struct bs_treeroot {
	union {
		bstreeNode_t* entry;
		base_treeRoot_t root;
	};
};


extern void cdsl_bstreeRootInit(bstreeRoot_t* rootp);
extern void cdsl_bstreeNodeInit(bstreeNode_t* node,int key);
extern bstreeNode_t* cdsl_bstreeInsert(bstreeRoot_t* rootp,bstreeNode_t* item);
extern bstreeNode_t* cdsl_bstreeLookup(bstreeRoot_t* rootp,int key);
extern bstreeNode_t* cdsl_bstreeDelete(bstreeRoot_t* rootp,int key);


#if defined(__cplusplus)
}
#endif


#endif /* TCH_BTREE_H_ */
