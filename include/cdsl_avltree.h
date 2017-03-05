/*
 * cdsl_avltree.h
 *
 *  Created on: Jun 3, 2015
 *      Author: innocentevil
 */

#ifndef CDSL_AVLTREE_H_
#define CDSL_AVLTREE_H_

#include "base_tree.h"

#if defined(__cplusplus)
extern "C" {
#endif


#define cdsl_avltreeMaxDepth(root)                      tree_max_depth((base_treeRoot_t*) root)
#define cdsl_avltreeTraverse(root, cb, order, arg)      tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, order, arg)
#define cdsl_avltreeTraverseTarget(root,cb,key, arg)    tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, key, arg)
#define cdsl_avltreeSize(root)                          tree_size((base_treeRoot_t*) root)
#define cdsl_avltreePrint(root, print)                  tree_print((base_treeRoot_t*) root, print)
#define cdsl_avltreeIsEmpty(root)                       tree_is_empty((base_treeRoot_t*) root)
#define cdsl_avltreeTop(root)                           (avltreeNode_t*) tree_top((base_treeRoot_t*) root)
#define cdsl_avltreeGoLeft(cur)                         (avltreeNode_t*) tree_go_left((base_treeNode_t*) cur)
#define cdsl_avltreeGoRight(cur)                        (avltreeNode_t*) tree_go_right((base_treeNode_t*) cur)
#define cdsl_avltreeMin(root)                           (avltreeNode_t*) tree_min((base_treeRoot_t*) root)
#define cdsl_avltreeMax(root)                           (avltreeNode_t*) tree_max((base_treeRoot_t*) root)
#define cdsl_avltreeUpdate(root,nitem)                  (avltreeNode_t*) tree_update((base_treeRoot_t*) root, (base_treeNode_t*) nitem)
#define cdsl_avltreeDelete(root, key)                   cdsl_avltreeDeleteReplace(root, key, NULL, NULL)
#define cdsl_avltreeDeleteMin(root)                     cdsl_avltreeDeleteMinReplace(root, NULL, NULL)
#define cdsl_avltreeDeleteMax(root)                     cdsl_avltreeDeleteMaxReplace(root, NULL, NULL)


typedef struct avltree_node avltreeNode_t;
typedef struct avltree_root {
	union {
		base_treeRoot_t  base_root;
		avltreeNode_t *entry;
	};
	int balance_factor;
} avltreeRoot_t;

struct avltree_node {
	union {
		base_treeNode_t base_node;
		struct {
			avltreeNode_t* left, right;
			trkey_t key;
		};
	};
	int depth;
};

extern void cdsl_avltreeRootInit(avltreeRoot_t* rootp, int bal);
extern void cdsl_avltreeNodeInit(avltreeNode_t* node, trkey_t key);

/*!
 * insert element to tree
 *
 */
extern avltreeNode_t* cdsl_avltreeInsert(avltreeRoot_t* rootp,avltreeNode_t* item, BOOL is_set);
extern avltreeNode_t* cdsl_avltreeLookup(avltreeRoot_t* rootp,trkey_t key);
extern avltreeNode_t* cdsl_avltreeDeleteReplace(avltreeRoot_t* rootp,trkey_t key, base_tree_replacer_t replacer, void* cb_arg);
extern avltreeNode_t* cdsl_avltreeDeleteMinReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg);
extern avltreeNode_t* cdsl_avltreeDeleteMaxReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg);

#if defined(__cplusplus)
}
#endif

#endif /* CDSL_AVLTREE_H_ */
