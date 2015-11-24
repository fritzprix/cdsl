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


typedef struct bs_treenode bs_treeNode_t;
typedef struct bs_treeroot bs_treeRoot_t;

#define ORDER_INC			(int) 0
#define ORDER_DEC			(int) 1

#define BREAK_TRAVERSE		(int) -1

typedef int (*bs_tree_callback_t) (int order,void*);

struct bs_treenode {
	bs_treeNode_t* right;
	bs_treeNode_t* left;
	int key;
};

struct bs_treeroot {
	bs_treeNode_t* entry;
};


extern void bstree_root_init(bs_treeRoot_t* rootp);
extern void bstree_node_init(bs_treeNode_t* node,int key);
extern bs_treeNode_t* bstree_insert(bs_treeRoot_t* rootp,bs_treeNode_t* item);
extern bs_treeNode_t* bstree_lookup(bs_treeRoot_t* rootp,int key);
extern bs_treeNode_t* bstree_delete(bs_treeRoot_t* rootp,int key);
extern void bstree_traverse(bs_treeRoot_t* rootp,base_tree_callback_t cb,int order);
extern int bstree_size(bs_treeRoot_t* rootp);
extern void bstree_print(bs_treeRoot_t* rootp,cdsl_generic_printer_t prt);
extern int bstree_max_depth(bs_treeRoot_t* rootp);


#if defined(__cplusplus)
}
#endif


#endif /* TCH_BTREE_H_ */
