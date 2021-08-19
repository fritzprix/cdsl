/*
 * cdsl_hashtree.h
 *
 *  Created on: Apr 7, 2016
 *      Author: innocentevil
 */

#ifndef INCLUDE_CDSL_HASHTREE_H_
#define INCLUDE_CDSL_HASHTREE_H_

#include "cdsl_defs.h"
#include "cdsl_rbtree.h"
#include "cdsl_slist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define cdsl_hashtreeForEach(root, cb, order,arg)       tree_for_each((base_treeRoot_t*) root, (base_tree_callback_t) cb, order,arg)
#define cdsl_hashtreeSize(root)                         tree_size((base_treeRoot_t*) root)
#define cdsl_hashtreePrint(root, print)                 tree_print((base_treeRoot_t*) root, print)
#define cdsl_hashtreeIsEmpty(root)                      tree_is_empty((base_treeRoot_t*) root)

typedef struct hashtree_node hashNode_t;
typedef enum {
	HashCollisionPolicy_DISCARD_OLD,
	HashCollisionPolicy_BUILD_COL_LIST,
} HashCollisionPolicy_t;

struct hashtree_node {
	union {
		rbtreeNode_t     _base;
		struct {
			hashNode_t*    right;
			hashNode_t*    left;
		};
	};
	__cdsl_uaddr_t         key;                       // converted hash value from the char string
	const char*            _str_key;                  // char string used as a key
	slistEntry_t           col_lentry;                // single-linked list to handle hash collision
};

typedef struct hashtree_root {
	union {
		rbtreeRoot_t      _base;
		hashNode_t*        entry;
	};
	HashCollisionPolicy_t policy;
}hashRoot_t;

extern void cdsl_hashtreeNodeInit(hashNode_t* node, const char* key);
extern void cdsl_hashtreeRootInit(hashRoot_t* root, HashCollisionPolicy_t col_policy);
extern hashNode_t* cdsl_hashtreeInsert(hashRoot_t* root, hashNode_t* node);
extern hashNode_t* cdsl_hashtreeLookup(hashRoot_t* root, const char* key);
extern hashNode_t* cdsl_hashtreeRemove(hashRoot_t* root, const char* key);



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CDSL_HASHTREE_H_ */
