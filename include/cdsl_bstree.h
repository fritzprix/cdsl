/*
 * cdsl_bst.h
 *
 *  Created on: 2014. 9. 12.
 *      Author: innocentevil
 */

#ifndef CDSL_BSTREE_H_
#define CDSL_BSTREE_H_

#include "base_tree.h"
#include "serialization/serializer.h"

#if defined(__cplusplus)
extern "C"{
#endif


typedef struct bs_treenode bstreeNode_t;
typedef struct bs_treeroot bstreeRoot_t;

#define cdsl_bstreeMaxDepth(root)                      tree_max_depth((base_treeRoot_t*) root)
#define cdsl_bstreeSerialize(root, serializer, cb)     tree_serialize((base_treeRoot_t*) root, (cdsl_serializer_t*) serializer, cb)
#define cdsl_bstreeForEach(root, cb, order,arg)        tree_for_each((base_treeRoot_t*) root, (base_tree_callback_t) cb, order,arg)
#define cdsl_bstreeForEachToTarget(root, cb, key,arg)  tree_for_each_to_target((baes_treeRoot_t*) root, (base_tree_callback_t) cb, order,arg)
#define cdsl_bstreeSize(root)                          tree_size((base_treeRoot_t*) root)
#define cdsl_bstreePrint(root, print)                  tree_print((base_treeRoot_t*) root, print)
#define cdsl_bstreeIsEmpty(root)                       tree_is_empty((base_treeRoot_t*) root)
#define cdsl_bstreeTop(root)                           (bstreeNode_t*) tree_top((base_treeRoot_t*) root)
#define cdsl_bstreeGoLeft(cur)                         (bstreeNode_t*) tree_go_left((base_treeNode_t*) cur)
#define cdsl_bstreeGoRight(cur)                        (bstreeNode_t*) tree_go_right((base_treeNode_t*) cur)
#define cdsl_bstreeMin(root)                           (bstreeNode_t*) tree_min((base_treeRoot_t*) root)
#define cdsl_bstreeMax(root)                           (bstreeNode_t*) tree_max((base_treeRoot_t*) root)
#define cdsl_bstreeUpdate(root,nitem)                  (bstreeNode_t*) tree_update((base_treeRoot_t*) root, (base_treeNode_t*) nitem)


#define ORDER_INC			(int) 0
#define ORDER_DEC			(int) 1

#define BREAK_TRAVERSE		(int) -1

typedef int (*bs_tree_callback_t) (int order,void*);

struct bs_treenode {
	union {
		struct {
			bstreeNode_t *left, *right;
			trkey_t        key;
		};
		base_treeNode_t node;
	};
};

struct bs_treeroot {
	union {
		bstreeNode_t* entry;
		base_treeRoot_t root;
	};
};


extern void cdsl_bstreeRootInit(bstreeRoot_t* rootp);
extern void cdsl_bstreeNodeInit(bstreeNode_t* node,trkey_t key);
extern bstreeNode_t* cdsl_bstreeInsert(bstreeRoot_t* rootp,bstreeNode_t* item);
extern bstreeNode_t* cdsl_bstreeLookup(bstreeRoot_t* rootp,trkey_t key);
extern bstreeNode_t* cdsl_bstreeConidtionalLookup(bstreeRoot_t* rootp, trkey_t key, condition_t match);
extern bstreeNode_t* cdsl_bstreeDelete(bstreeRoot_t* rootp,trkey_t key);
extern bstreeNode_t* cdsl_bstreeDeleteMin(bstreeRoot_t* rootp);
extern bstreeNode_t* cdsl_bstreeDeleteMax(bstreeRoot_t* rootp);


#if defined(__cplusplus)
}
#endif


#endif /* TCH_BTREE_H_ */
