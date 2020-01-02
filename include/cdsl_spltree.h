/*
 * cdsl_spltree.h
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */

#ifndef CDSL_SPLTREE_H_
#define CDSL_SPLTREE_H_

#include "base_tree.h"

#if defined(__cplusplus)
extern "C" {
#endif


#define cdsl_spltreeMaxDepth(root)                      tree_max_depth((base_treeRoot_t*) root)
#define cdsl_spltreeTraverse(root, cb, order, arg)      tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, order)
#define cdsl_spltreeTraverseTarget(root, cb, key, arg)  tree_traverse_target((base_treeRoot_t*) root, (base_tree_callback_t) cb, key)
#define cdsl_spltreeSize(root)                          tree_size((base_treeRoot_t*) root)
#define cdsl_spltreePrint(root, print)                  tree_print((base_treeRoot_t*) root, print)
#define cdsl_spltreeIsEmpty(root)                       tree_is_empty((base_treeRoot_t*) root)
#define cdsl_spltreeTop(root)                           (spltreeNode_t*) tree_top((base_treeRoot_t*) root)
#define cdsl_spltreeGoLeft(cur)                         (spltreeNode_t*) tree_go_left((base_treeNode_t*) cur)
#define cdsl_spltreeGoRight(cur)                        (spltreeNode_t*) tree_go_right((base_treeNode_t*) cur)
#define cdsl_spltreeMin(root)                           (spltreeNode_t*) tree_min((base_treeRoot_t*) root)
#define cdsl_spltreeMax(root)                           (spltreeNode_t*) tree_max((base_treeRoot_t*) root)
#define cdsl_spltreeUpdate(root,nitem)                  (spltreeNode_t*) tree_update((base_treeRoot_t*) root, (base_treeNode_t*) nitem)


typedef struct spltree_node spltreeNode_t;
typedef struct spltree_root spltreeRoot_t;

struct spltree_root {
	union {
		base_treeRoot_t	root_base;
		spltreeNode_t* entry;
	};
};

struct spltree_node {
	union {
		base_treeNode_t node_base;
		struct {
			spltreeNode_t* left,*right;
			trkey_t          key;
		};
	};
};


extern void cdsl_spltreeRootInit(spltreeRoot_t* root);
extern void cdsl_spltreeNodeInit(spltreeNode_t* node,trkey_t key);
extern void cdsl_spltreeInsert(spltreeRoot_t* root,spltreeNode_t* item);
extern spltreeNode_t* cdsl_spltreeLookup(spltreeRoot_t* root,trkey_t key,BOOL splay);
extern spltreeNode_t* cdsl_spltreeLookupLargest(spltreeRoot_t* root,BOOL splay);
extern spltreeNode_t* cdsl_spltreeLookupSmallest(spltreeRoot_t* root,BOOL splay);
extern spltreeNode_t* cdsl_spltreeDelete(spltreeRoot_t* root,trkey_t key);
extern spltreeNode_t* cdsl_spltreeDeleteMin(spltreeRoot_t* root);
extern spltreeNode_t* cdsl_spltreeDeleteMax(spltreeRoot_t* root);



#if defined(__cplusplus)
}
#endif


#endif /* CDSL_SPLTREE_H_ */
