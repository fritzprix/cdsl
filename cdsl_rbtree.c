/*
 * cdsl_rbtree.c
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */


#include <stddef.h>
#include <stdio.h>
#include "cdsl_rbtree.h"


#define BLACK			((unsigned) (1 > 0))
#define RED				((unsigned) (1 < 0))

#define CLEAN			((uint8_t) 0)
#define COLLISION		((uint8_t) 1)
#define DIR_RIGHT		((uint8_t) 2)
#define DIR_LEFT		((uint8_t) 3)


const static rb_treeNode_t NIL_NODE = {
		.left = NULL,
		.right = NULL,
		.key = 0,
		.color = BLACK
};


#define RB_NIL 		((rb_treeNode_t*) &NIL_NODE)

static rb_treeNode_t* rotateLeft(rb_treeNode_t* rot_pivot);
static rb_treeNode_t* rotateRight(rb_treeNode_t* rot_pivot);
static rb_treeNode_t* insert_r(rb_treeNode_t* parent,rb_treeNode_t* item,uint8_t* context);
static rb_treeNode_t* delete_r(rb_treeNode_t* parent,rb_treeNode_t* delete_node,uint8_t* context);
static rb_treeNode_t* getLeftMost(rb_treeNode_t* root);
static rb_treeNode_t* getRightMost(rb_treeNode_t* root);
static void print_r(rb_treeNode_t* current,int depth);
static void print_tab(int cnt);

void cdsl_rbtreeNodeInit(rb_treeNode_t* item,int key){
	item->key = key;
	item->left = item->right = RB_NIL;
	item->color = RED;
}


BOOL cdsl_rbtreeInsert(rb_treeNode_t** root,rb_treeNode_t* item){
	if(!root)
		return FALSE;
	if(!*root){
		*root = item;
		item->color = BLACK;
		return TRUE;
	}
	uint8_t info = 0;

	*root = insert_r(*root,item,&info);
	(*root)->color = BLACK;
	return TRUE;
}

rb_treeNode_t* cdsl_rbtreeDelete(rb_treeNode_t** root,int key){
	if(!root || !(*root))
		return FALSE;
	uint8_t context = 0;
	rb_treeNode_t* replacement = RB_NIL;
	rb_treeNode_t* current_node = *root;
	rb_treeNode_t* parent_node = RB_NIL;
	while((current_node != RB_NIL) && (current_node->key != key)){
		parent_node = current_node;
		if(current_node->key < key)
		{
			context = DIR_RIGHT;
			current_node = current_node->right;
		}
		else
		{
			context = DIR_LEFT;
			current_node = current_node->left;
		}
	}
	if(current_node == RB_NIL)
		return FALSE;
	replacement = delete_r(parent_node,current_node,&context);
}


void cdsl_rbtreePrint(rb_treeNode_t** root){
	if(!root)
		return;
	printf("\n");
	print_r(*root,0);
	printf("\n");
}

static rb_treeNode_t* insert_r(rb_treeNode_t* parent,rb_treeNode_t* item,uint8_t* context){
	if(!item || !parent)
		return RB_NIL;
	if(parent == RB_NIL){
		*context = CLEAN;
		return item;
	}
	uint8_t direction = *context;
	if(parent->key < item->key){
		*context = DIR_RIGHT;
		parent->right = insert_r(parent->right,item,context);
		if((*context) == COLLISION)		// if collision
		{
			if(parent->left->color == parent->right->color){
				parent->right->color = !parent->left->color;
				parent->left->color =!parent->left->color;
				if((parent->left->color == BLACK) && (parent->right->color == BLACK))
					parent->color = RED;
				*context = CLEAN;
				return parent;
			}
			parent = rotateLeft(parent);
			parent->color = parent->left->color;
			parent->left->color = !parent->left->color;
			*context = CLEAN;
			return parent;
		}

		if((parent->color == RED) && (parent->right->color == RED)){
			*context = COLLISION;
			if(direction == DIR_LEFT){
				parent = rotateLeft(parent);
			}
		}
	}else{
		*context = DIR_LEFT;
		parent->left = insert_r(parent->left,item,context);
		if((*context) == COLLISION)
		{
			if(parent->left->color == parent->right->color){
				parent->right->color = !parent->left->color;
				parent->left->color =!parent->left->color;
				if((parent->left->color == BLACK) && (parent->right->color == BLACK))
					parent->color = RED;
				*context = CLEAN;
				return parent;
			}
			parent = rotateRight(parent);
			parent->color = parent->right->color;
			parent->right->color = !parent->right->color;
			*context = CLEAN;
			return parent;
		}

		if((parent->color == RED) && (parent->left->color == RED)){
			*context = COLLISION;
			if(direction == DIR_RIGHT){
				parent = rotateRight(parent);
			}
		}
	}
	return parent;
}

static rb_treeNode_t* rotateLeft(rb_treeNode_t* rot_pivot){
	rb_treeNode_t* nparent = rot_pivot->right;
	rot_pivot->right = nparent->left;
	nparent->left = rot_pivot;
	return nparent;
}

static rb_treeNode_t* rotateRight(rb_treeNode_t* rot_pivot){
	rb_treeNode_t* nparent = rot_pivot->left;
	rot_pivot->left = nparent->right;
	nparent->right = rot_pivot;
	return nparent;
}

static void print_tab(int cnt){
	while(cnt--)
		printf("\t");
}


static void print_r(rb_treeNode_t* current,int depth){
	if(current == RB_NIL){
		print_tab(depth); printf("NIL {Color : BLACK, Key: %d} @Depth %d \n",current->key,depth);
		return;
	}
	print_r(current->right,depth + 1);
	print_tab(depth); printf("{Color : %s, Key: %d} @Depth %d \n",current->color == BLACK? "BLACK" : "RED",current->key,depth);
	print_r(current->left,depth + 1);
}

static rb_treeNode_t* delete_r(rb_treeNode_t* parent,rb_treeNode_t* delete_node,uint8_t* context){
	if(!parent || !delete_node || !context)		// unexpected parameter error
		return RB_NIL;
	uint8_t direction = *context;
	if((delete_node->right == RB_NIL) && (delete_node->left == RB_NIL)){
		if(direction == DIR_RIGHT)
			parent->right = RB_NIL;
		else
			parent->left = RB_NIL;
		return delete_node;
	}


	if(direction == DIR_RIGHT)
	{

	} else
	{

	}

}


static rb_treeNode_t* getLeftMost(rb_treeNode_t* root){

}

static rb_treeNode_t* getRightMost(rb_treeNode_t* root){

}


