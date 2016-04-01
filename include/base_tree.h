/*
 * base_tree.h
 *
 *  Created on: Nov 15, 2015
 *      Author: innocentevil
 */

#ifndef BASE_TREE_H_
#define BASE_TREE_H_

#include "cdsl.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define ORDER_INC			(int) 0
#define ORDER_DEC			(int) 1

#define MSG_BREAK_TRAVERSE	(int) -1

#define DECLARE_TRAVERSE_CALLBACK(fn) int fn(int order, base_treeNode_t* node)


typedef struct base_tree_node base_treeNode_t;
typedef struct base_tree_root base_treeRoot_t;
typedef int (*base_tree_callback_t)(int order,base_treeNode_t* node);

struct base_tree_root {
	base_treeNode_t* entry;
};

struct base_tree_node {
	base_treeNode_t *left,*right;
};

extern void tree_traverse(base_treeRoot_t* rootp, base_tree_callback_t cb,int order);
extern int tree_size(base_treeRoot_t* rootp);
extern void tree_print(base_treeRoot_t* rootp,cdsl_generic_printer_t prt);
extern int tree_max_depth(base_treeRoot_t* rootp);


#if defined(__cplusplus)
}
#endif

#endif /* BASE_TREE_H_ */
