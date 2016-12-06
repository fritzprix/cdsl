/*
 * base_tree.h
 *
 *  Created on: Nov 15, 2015
 *      Author: innocentevil
 */

#ifndef BASE_TREE_H_
#define BASE_TREE_H_

#include "arch.h"
#include "cdsl_defs.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define ORDER_INC			(int) 0
#define ORDER_DEC			(int) 1

#define TRAVERSE_OK         (int) 0
#define TRAVERSE_BREAK      (int) 1

#define DECLARE_TRAVERSE_CALLBACK(fn) int fn(int order, base_treeNode_t* node,void* arg)

typedef __cdsl_uaddr_t  trkey_t;
typedef struct base_tree_node base_treeNode_t;
typedef struct base_tree_root base_treeRoot_t;
typedef int (*base_tree_callback_t)(int,base_treeNode_t*,void*);

/*! replacer callback function type to override default hole resolution mechanism of the given tree
 *  replacer result shall be one among the three cases below
 *  1. return false, pointer updated or not -> this will perform default hole resolve mechanism of given tree structure
 *  2. return true, pointer updated -> this will override default hole resolve mechanism, simply saying replace hole with element of updated pointer
 *  3. return true, pointer is not updated -> this will be assumed, the element not found. nothing happen
 */
typedef int (*base_tree_replacer_t)(base_treeNode_t** node, void* ctx);

struct base_tree_root {
	base_treeNode_t* entry;
};

struct base_tree_node {
	base_treeNode_t *left,*right;
	trkey_t          key;
};

extern void tree_traverse(base_treeRoot_t* rootp, base_tree_callback_t cb,int order, void* arg);
extern void tree_traverse_target(base_treeRoot_t* rootp, base_tree_callback_t cb, trkey_t key,void* arg);
extern base_treeNode_t* tree_go_left(base_treeNode_t* cur);
extern base_treeNode_t* tree_go_right(base_treeNode_t* cur);
extern base_treeNode_t* tree_top(base_treeRoot_t* rootp);
extern int tree_size(base_treeRoot_t* rootp);
extern void tree_print(base_treeRoot_t* rootp,cdsl_generic_printer_t prt);
extern int tree_max_depth(base_treeRoot_t* rootp);
extern BOOL tree_is_empty(base_treeRoot_t* rootp);
extern base_treeNode_t* tree_min(base_treeRoot_t* rootp);
extern base_treeNode_t* tree_max(base_treeRoot_t* rootp);
extern base_treeNode_t* tree_update(base_treeRoot_t* rootp, base_treeNode_t* nitem);


#if defined(__cplusplus)
}
#endif

#endif /* BASE_TREE_H_ */
