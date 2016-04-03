/*
 * cdsl_nrbtree.h
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#ifndef INCLUDE_CDSL_NRBTREE_H_
#define INCLUDE_CDSL_NRBTREE_H_

#include "base_tree.h"
#include "cdsl.h"
#include "arch.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef __cdsl_uaddr_t rb_key_t;

#define cdsl_nrbtreeMaxDepth(root)                 tree_max_depth((base_treeRoot_t*) root)
#define cdsl_nrbtreeTraverse(root, cb, order)      tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, order)
#define cdsl_nrbtreeSize(root)                     tree_size((base_treeRoot_t*) root)
#define cdsl_nrbtreePrint(root, print)             tree_print((base_treeRoot_t*) root, print)
#define cdsl_nrbtreeIsEmpty(root)                  tree_is_empty((base_treeRoot_t*) root)
#define cdsl_nrbtreeGoLeft(cur)                    (nrbtreeNode_t*) tree_go_left((base_treeNode_t*) cur)
#define cdsl_nrbtreeGoRight(cur)                   (nrbtreeNode_t*) tree_go_right((base_treeNode_t*) cur)


typedef struct cdsl_nrbtree nrbtreeNode_t;
typedef struct cdsl_nrbroot {
	union {
		struct base_tree_root 	base_root;
		nrbtreeNode_t* entry;
	};
}nrbtreeRoot_t;

struct cdsl_nrbtree {
	union {
		struct base_tree_node 	base_node;
		struct {
			nrbtreeNode_t* left;
			nrbtreeNode_t* right;
		};

	};
	rb_key_t key;
};



extern void cdsl_nrbtreeRootInit(nrbtreeRoot_t* rootp);
extern void cdsl_nrbtreeNodeInit(nrbtreeNode_t* node, rb_key_t key);
extern nrbtreeNode_t* cdsl_nrbtreeInsert(nrbtreeRoot_t* rootp,nrbtreeNode_t* item);
extern nrbtreeNode_t* cdsl_nrbtreeLookup(nrbtreeRoot_t* rootp,rb_key_t key);
extern nrbtreeNode_t* cdsl_nrbtreeDelete(nrbtreeRoot_t* rootp,rb_key_t key);

#ifdef __DBG
extern void cdsl_nrbtreePrint_dev(nrbtreeRoot_t* root);
#else
#define cdsl_nrbtreePrint_dev(root)
#endif


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CDSL_NRBTREE_H_ */
