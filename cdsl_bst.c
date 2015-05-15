/*
 * tch_btree.c
 *
 *  Created on: 2014. 9. 12.
 *      Author: innocentevil
 */


#include "cdsl_btree.h"
#include <stddef.h>


void cdsl_btreeInit(cdsl_bstNode_t* node,int key){
	node->key = key;
	node->left = NULL;
	node->right = NULL;
}

static cdsl_bstNode_t* tch_btree_get_rightmost(cdsl_bstNode_t* node){
	if(!node) return NULL;
	if(!node->right) return node;
	cdsl_bstNode_t* rightmost = tch_btree_get_rightmost(node->right);
	node->right = rightmost->left;
	rightmost->left = node;
	return rightmost;
}

static cdsl_bstNode_t* tch_btree_get_leftmost(cdsl_bstNode_t* node){
	if(!node) return NULL;
	if(!node->left) return node;
	cdsl_bstNode_t* leftmost = tch_btree_get_leftmost(node->left);
	node->left = leftmost->right;
	leftmost->right = node;
	return leftmost;
}


cdsl_bstNode_t* cdsl_btree_insert(cdsl_bstNode_t** root,cdsl_bstNode_t* item){
	if(!root || !item)
		return NULL;
	if(!*root){
		*root = item;
		return item;
	}
	item->left = NULL;
	item->right = NULL;
	cdsl_bstNode_t* current = *root;
	while(current){
		if(current->key < item->key){
			if(!current->right){
				current->right = item;
				return item;
			}
			current = current->right;
		}else{
			if(!current->left){
				current->left = item;
				return item;
			}
			current = current->left;
		}
	}
	return NULL;
}

cdsl_bstNode_t* cdsl_btree_lookup(cdsl_bstNode_t* root,int key){
	if(!root)
		return NULL;
	while(root && (root->key != key)){
		if(root->key < key)
			root = root->right;
		else
			root = root->left;
	}
	return root;
}

cdsl_bstNode_t* cdsl_btree_delete(cdsl_bstNode_t** root,int key){

	cdsl_bstNode_t* todelete = NULL;
	if(!root || !*root)
		return NULL;
	cdsl_bstNode_t** current = root;
	while((*current) && ((*current)->key != key)){
		if((*current)->key < key){
			current = &(*current)->right;
		}else{
			current = &(*current)->left;
		}
	}
	todelete = *current;
	if (todelete->left) {
		*current = tch_btree_get_rightmost(todelete->left);
		if (*current) {
			(*current)->right = todelete->right;
		}
	}else if(todelete->right){
		*current = tch_btree_get_leftmost(todelete->right);
		(*current)->left = todelete->left;
	}else{
		*current = NULL;
	}
	return todelete;
}

int cdsl_btree_size(cdsl_bstNode_t* root){
	int cnt = 0;
	if(root)
		cnt = 1;
	if(!root->left && !root->right) return cnt;
	if(root->left)
		cnt += cdsl_btree_size(root->left);
	if(root->right)
		cnt += cdsl_btree_size(root->right);
	return cnt;
}


