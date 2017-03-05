/*
 * cdsl_avltree.c
 *
 *  Created on: Jun 3, 2015
 *      Author: innocentevil
 */

#include "cdsl_avltree.h"

void cdsl_avltreeRootInit(avltreeRoot_t* rootp, int bal) {
	if(!rootp) {
		return;
	}
	rootp->balance_factor = bal;
	rootp->entry = NULL;
}

void cdsl_avltreeNodeInit(avltreeNode_t* node, trkey_t key) {
	if(!node) {
		return;
	}
	node->depth = 0;
	node->key = key;
	node->left = node->right = NULL;
}

/*!
 * insert element to tree
 *
 */
avltreeNode_t* cdsl_avltreeInsert(avltreeRoot_t* rootp,avltreeNode_t* item, BOOL is_set) {
	if(!rootp || !item) {
		return NULL;
	}
}

avltreeNode_t* cdsl_avltreeLookup(avltreeRoot_t* rootp,trkey_t key) {

}

avltreeNode_t* cdsl_avltreeDeleteReplace(avltreeRoot_t* rootp,trkey_t key, base_tree_replacer_t replacer, void* cb_arg) {

}

avltreeNode_t* cdsl_avltreeDeleteMinReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg) {

}

avltreeNode_t* cdsl_avltreeDeleteMaxReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg) {

}
