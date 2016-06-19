/*
 * cdsl_hashtree.h
 *
 *  Created on: Apr 7, 2016
 *      Author: innocentevil
 */

#ifndef INCLUDE_CDSL_HASHTREE_H_
#define INCLUDE_CDSL_HASHTREE_H_

#include "cdsl_nrbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

#define cdsl_hashtreeTraverse(root, cb, order,arg)      tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, order,arg)
#define cdsl_hashtreeSize(root)                         tree_size((base_treeRoot_t*) root)
#define cdsl_hashtreePrint(root, print)                 tree_print((base_treeRoot_t*) root, print)
#define cdsl_hashtreeIsEmpty(root)                      tree_is_empty((base_treeRoot_t*) root)

typedef struct hashtree_node hashNode_t;

struct hashtree_node {
	union {
		nrbtreeNode_t     _base;
		struct {
			hashNode_t*    next;
			hashNode_t*    prev;
		};
	};
	__cdsl_uaddr_t         key;
	const char*            _str_key;
};

typedef struct hashtree_root {
	union {
		nrbtreeRoot_t      _base;
		hashNode_t*        entry;
	};
}hashRoot_t;

extern void cdsl_hashtreeNodeInit(hashNode_t* node, const char* key);
extern void cdsl_hashtreeRootInit(hashRoot_t* root);
extern hashNode_t* cdsl_hashtreeInsert(hashRoot_t* root, hashNode_t* node);
extern hashNode_t* cdsl_hashtreeLookup(hashRoot_t* root, const char* key);
extern hashNode_t* cdsl_hashtreeRemove(hashRoot_t* root, const char* key);
extern hashNode_t* cdsl_hashtreeRemoveRandom(hashRoot_t* root);



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CDSL_HASHTREE_H_ */
