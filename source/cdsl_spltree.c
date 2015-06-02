/*
 * cdsl_spltree.c
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */

#include <stdio.h>
#include "cdsl.h"
#include "cdsl_spltree.h"


#define LEFT	((uint8_t) 0)
#define RIGHT	((uint8_t) 1)
#define ROOT	((uint8_t) 2)

#define ZIGZIG_STEP	((uint8_t) 4)
#define ZIGZAG_STEP ((uint8_t) 8)

static void print_r(spltreeNode_t* current,int depth);
static spltreeNode_t* insert_r(spltreeNode_t* current,spltreeNode_t* item,uint8_t* context);
static spltreeNode_t* lookup_r(spltreeNode_t* current,int key,uint8_t* context,BOOL splay);
static spltreeNode_t* delete_r(spltreeNode_t* current,int key,uint8_t* context,spltreeNode_t** found);
static spltreeNode_t* largest_r(spltreeNode_t* current,uint8_t* context,BOOL splay);
static spltreeNode_t* smallest_r(spltreeNode_t* current,uint8_t* context,BOOL splay);
static spltreeNode_t* rotateLeft(spltreeNode_t* parent);
static spltreeNode_t* rotateRight(spltreeNode_t* parent);


void cdsl_spltreeInitNode(spltreeNode_t* node,int key){
	node->key = key;
	node->left = node->right = NULL;
}

void cdsl_spltreeInsert(spltreeNode_t** root,spltreeNode_t* item){
	if(!root)
		return;
	if(!(*root)){
		*root = item;
		return;
	}
	uint8_t context;
	if((*root)->key < item->key){
		context = RIGHT;
		(*root)->right = insert_r((*root)->right,item,&context);
		if(context == ZIGZIG_STEP){
			*root = rotateLeft(rotateLeft(*root));
			return;
		}
		if(context == ZIGZAG_STEP){
			*root = rotateLeft(rotateRight((*root)->right));
			return;
		}
		*root = rotateLeft(*root);
	}else{
		context = LEFT;
		(*root)->left = insert_r((*root)->left,item,&context);
		if(context == ZIGZIG_STEP){
			*root = rotateRight(rotateRight(*root));
			return;
		}
		if(context == ZIGZAG_STEP){
			*root = rotateRight(rotateLeft((*root)->left));
			return;
		}
		*root = rotateRight(*root);
	}
}

spltreeNode_t* cdsl_spltreeLookup(spltreeNode_t** root,int key,BOOL splay){
	if(!root || !(*root))
		return NULL;
	uint8_t context;
	if((*root)->key < key){
		context = RIGHT;
		(*root)->right = lookup_r((*root)->right,key,&context,splay);
		if(splay)
			*root = rotateLeft(*root);
	}else{
		context = LEFT;
		(*root)->left = lookup_r((*root)->left,key,&context,splay);
		if(splay)
			*root = rotateRight(*root);
	}
	return *root;

}

spltreeNode_t* cdsl_spltreeLookupLargest(spltreeNode_t** root,BOOL splay){
	if(!root || !(*root))
		return NULL;
	uint8_t context = 0;
	return (*root = largest_r((*root),&context,splay));
}

spltreeNode_t* cdsl_spltreeLookupSmallest(spltreeNode_t** root,BOOL splay){
	if(!root || !(*root))
		return NULL;
	uint8_t context = 0;
	return (*root = smallest_r((*root),&context,splay));
}

spltreeNode_t* cdsl_spltreeDelete(spltreeNode_t** root,int key){
	if(!root || !(*root))
		return NULL;
	uint8_t context = 0;
	spltreeNode_t* delNode = NULL;
	if((*root)->key < key){
		context = RIGHT;
		(*root)->right = lookup_r((*root)->right,key,&context,TRUE);
		delNode = rotateLeft(*root);
	}else{
		context = LEFT;
		(*root)->left = lookup_r((*root)->left,key,&context,TRUE);
		delNode = rotateRight(*root);
	}
	return delNode;
}

void cdsl_spltreePrint(spltreeNode_t** root){
	if(!root || !(*root))
		return;
	printf("\n");
	print_r(*root,0);
	printf("\n");
}

static void print_r(spltreeNode_t* current,int depth){
	if(!current)
		return;
	int k = depth;
	print_r(current->left,depth + 1);
	while(k--) printf("\t"); printf("{key : %d }@depth : %d\n",current->key,depth);
	print_r(current->right,depth + 1);
}


static spltreeNode_t* insert_r(spltreeNode_t* current,spltreeNode_t* item,uint8_t* context){
	if(!current){
		*context = 0;
		return item;
	}
	uint8_t direction = *context;
	if(current->key < item->key){
		*context = RIGHT;
		current->right = insert_r(current->right,item,context);
		if(current->right == item){
			*context = direction == RIGHT? ZIGZIG_STEP : ZIGZAG_STEP;
			return current;
		}
		if(*context == ZIGZIG_STEP){
			*context = 0; 	// clear context
			return rotateLeft(rotateLeft(current));
		}
		if(*context == ZIGZAG_STEP){
			*context = 0;
			current->right = rotateRight(current->right);
			return rotateLeft(current);
		}
		return current;
	}else {
		*context = LEFT;
		current->left = insert_r(current->left,item,context);
		if(current->left == item){
			*context = direction == LEFT ? ZIGZIG_STEP : ZIGZAG_STEP;
		}
		if(*context == ZIGZIG_STEP){
			*context = 0;
			return rotateRight(rotateRight(current));
		}
		if(*context == ZIGZAG_STEP){
			*context = 0;
			current->left = rotateLeft(current->left);
			return rotateRight(current);
		}
		return current;
	}
}

static spltreeNode_t* lookup_r(spltreeNode_t* current,int key,uint8_t* context,BOOL splay){
	return NULL;
}

static spltreeNode_t* delete_r(spltreeNode_t* current,int key,uint8_t* context,spltreeNode_t** found){
	return NULL;
}

static spltreeNode_t* largest_r(spltreeNode_t* current,uint8_t* context,BOOL splay){
	if(current->right == NULL){
		*context = RIGHT;
		return current;
	}
	if (splay) {
		current->right = largest_r(current->right, context, splay);
		if ((*context) == RIGHT) {
			*context = ZIGZIG_STEP;
			return current;
		} else if ((*context) == ZIGZIG_STEP) {
			current = rotateLeft(rotateLeft(current));
			*context = RIGHT;
			return current;
		}
	}else{
		return largest_r(current->right,context,splay);
	}
	return NULL;
}

static spltreeNode_t* smallest_r(spltreeNode_t* current,uint8_t* context,BOOL splay){
	if(current->left == NULL){
		*context = LEFT;
		return current;
	}
	if (splay) {
		current->left = smallest_r(current->left, context, splay);
		if ((*context) == LEFT) {
			*context = ZIGZIG_STEP;
			return current;
		} else if ((*context) == ZIGZIG_STEP) {
			current = rotateRight(rotateRight(current));
			*context = LEFT;
			return current;
		}
	}
	else{
		return smallest_r(current->left,context,splay);
	}
	return NULL;
}


static spltreeNode_t* rotateLeft(spltreeNode_t* parent){
/*	if(!parent->right)
		return parent;*/
	spltreeNode_t* nparent = parent->right;
	parent->right = nparent->left;
	nparent->left = parent;
	return nparent;
}

static spltreeNode_t* rotateRight(spltreeNode_t* parent){
/*	if(!parent->left)
		return parent;*/
	spltreeNode_t* nparent = parent->left;
	parent->left = nparent->right;
	nparent->right = parent;
	return nparent;
}
