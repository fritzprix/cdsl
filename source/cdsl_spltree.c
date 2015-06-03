/*
 * cdsl_spltree.c
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */

#include <stdio.h>
#include "cdsl.h"
#include "cdsl_spltree.h"


#define LEFT		((uint8_t) 0)
#define RIGHT		((uint8_t) 1)
#define ROOT		((uint8_t) 2)
#define ZIGZIG_STEP	((uint8_t) 3)
#define ZIGZAG_STEP ((uint8_t) 4)
#define NOT_FOUND	((uint8_t) 5)
#define FOUND		((uint8_t) 6)

static void print_r(spltreeNode_t* current,int depth);
static spltreeNode_t* insert_r(spltreeNode_t* current,spltreeNode_t* item,uint8_t* context);
static spltreeNode_t* lookup_r(spltreeNode_t* current,int key,uint8_t* context,BOOL splay);
static spltreeNode_t* delete_r(spltreeNode_t* current,int key,uint8_t* context,spltreeNode_t** found);
static spltreeNode_t* largest_r(spltreeNode_t* current,uint8_t* context,BOOL splay,BOOL delete);
static spltreeNode_t* smallest_r(spltreeNode_t* current,uint8_t* context,BOOL splay,BOOL delete);
static spltreeNode_t* rotateLeft(spltreeNode_t* parent);
static spltreeNode_t* rotateRight(spltreeNode_t* parent);


void cdsl_spltreeNodeInit(spltreeNode_t* node,int key){
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
	context = ROOT;
	*root = insert_r(*root,item,&context);
}

spltreeNode_t* cdsl_spltreeLookup(spltreeNode_t** root,int key,BOOL splay){
	if(!root || !(*root))
		return NULL;
	uint8_t context;
	context = ROOT;
	return *root = lookup_r(*root,key,&context,splay);
}

spltreeNode_t* cdsl_spltreeLookupLargest(spltreeNode_t** root,BOOL splay){
	if(!root || !(*root))
		return NULL;
	uint8_t context = 0;
	return (*root = largest_r((*root),&context,splay,FALSE));
}

spltreeNode_t* cdsl_spltreeLookupSmallest(spltreeNode_t** root,BOOL splay){
	if(!root || !(*root))
		return NULL;
	uint8_t context = 0;
	return (*root = smallest_r((*root),&context,splay,FALSE));
}

spltreeNode_t* cdsl_spltreeDelete(spltreeNode_t** root,int key){
	if(!root || !(*root))
		return NULL;
	uint8_t context = ROOT;
	spltreeNode_t* delNode = NULL;
	*root = delete_r(*root,key,&context,&delNode);
	return delNode;
}

int cdsl_spltreeSize(spltreeNode_t** root){
	if(!root || !(*root))
		return 0;
	return cdsl_spltreeSize(&(*root)->right) + 1 + cdsl_spltreeSize(&(*root)->left);
}


int cdsl_spltreeMaxDepth(spltreeNode_t** root){
	if(!root || !(*root))
		return 0;
	int max = 0;
	int temp = 0;
	if(max < (temp = cdsl_spltreeMaxDepth(&(*root)->left)))
		max = temp;
	if(max < (temp = cdsl_spltreeMaxDepth(&(*root)->right)))
		max = temp;
	return max + 1;
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
	print_r(current->right,depth + 1);
	while(k--) printf("\t"); printf("{key : %d }@depth : %d\n",current->key,depth);
	print_r(current->left,depth + 1);
}


static spltreeNode_t* insert_r(spltreeNode_t* current,spltreeNode_t* item,uint8_t* context){
	if(!current){
		*context = 0;
		return item;
	}
	uint8_t direction = *context;
	if(current->key <= item->key){
		*context = RIGHT;
		current->right = insert_r(current->right,item,context);

		if(*context == ZIGZIG_STEP){
			*context = direction;
			return rotateLeft(rotateLeft(current));
		}
		if(*context == ZIGZAG_STEP){
			*context = direction;
			current->right = rotateRight(current->right);
			return rotateLeft(current);
		}
		if(direction == ROOT)
			return rotateLeft(current);
		if(current->right == item){
			*context = direction == RIGHT? ZIGZIG_STEP : ZIGZAG_STEP;
			return current;
		}

		return current;
	}else {
		*context = LEFT;
		current->left = insert_r(current->left,item,context);

		if(*context == ZIGZIG_STEP){
			*context = direction;
			return rotateRight(rotateRight(current));
		}
		if(*context == ZIGZAG_STEP){
			*context = direction;
			current->left = rotateLeft(current->left);
			return rotateRight(current);
		}
		if(direction == ROOT)
			return rotateRight(current);
		if(current->left == item){
			*context = direction == LEFT ? ZIGZIG_STEP : ZIGZAG_STEP;
			return current;
		}

		return current;
	}
}

static spltreeNode_t* lookup_r(spltreeNode_t* current,int key,uint8_t* context,BOOL splay){
	if(!current){
		*context = NOT_FOUND;
		return NULL;
	}
	uint8_t direction = *context;
	if(current->key == key){
		*context = FOUND;
		return current;
	}
	if(current->key < key){
		if(splay){
			*context = RIGHT;
			current->right = lookup_r(current->right, key, context, splay);
			if (*context == NOT_FOUND)
				return current;
			if (*context == ZIGZIG_STEP) {
				*context = direction;
				return rotateLeft(rotateLeft(current));
			}
			if (*context == ZIGZAG_STEP) {
				*context = direction;
				current->right = rotateRight(current->right);
				return rotateLeft(current);
			}
			if (direction == ROOT)
				return rotateLeft(current);
			if (current->right->key == key) {
				*context = direction == RIGHT ? ZIGZIG_STEP : ZIGZAG_STEP;
				return current;
			}
		}else{
			return lookup_r(current->right,key,context,splay);
		}
	}else {
		if(splay){
			*context = LEFT;
			current->left = lookup_r(current->left, key, context, splay);
			if (*context == NOT_FOUND)
				return current;
			if (*context == ZIGZIG_STEP) {
				*context = direction;
				return rotateRight(rotateRight(current));
			}
			if (*context == ZIGZAG_STEP) {
				*context = direction;
				current->left = rotateLeft(current->left);
				return rotateRight(current);
			}
			if (direction == ROOT)
				return rotateRight(current);
			if (current->left->key == key) {
				*context = direction == LEFT ? ZIGZIG_STEP : ZIGZAG_STEP;
				return current;
			}
		}else{
			return lookup_r(current->left,key,context,splay);
		}
	}
	return NULL;
}

static spltreeNode_t* delete_r(spltreeNode_t* current,int key,uint8_t* context,spltreeNode_t** found){
	if(!current){
		*context = NOT_FOUND;
		*found = NULL;
		return NULL;
	}
	if(current->key == key){
		*found = current;
		if(current->left){
			current = largest_r(current->left,context,FALSE,TRUE);
			*context = FOUND;
			if(current == (*found)->left){
				current->right = (*found)->right;
				return current;
			}
		}else if(current->right){
			current = smallest_r(current->right,context,FALSE,TRUE);
			*context = FOUND;
			if(current == (*found)->right){
				current->left = (*found)->left;
				return current;
			}
		}
		else{
			*context = FOUND;
			return NULL;
		}
		current->left = (*found)->left;
		current->right = (*found)->right;
		return current;
	}
	uint8_t direction = *context;
	if(current->key < key){
		*context = RIGHT;
		current->right = delete_r(current->right,key,context,found);
		if(!current->right)
			return current;
		if(*context == NOT_FOUND)
			return current;
		if(*context == ZIGZIG_STEP){
			*context = FOUND;
			return rotateLeft(rotateLeft(current));
		}
		if(*context == ZIGZAG_STEP){
			*context = FOUND;
			current->right = rotateRight(current->right);
			return rotateLeft(current);
		}
		if(direction == ROOT)
			return rotateLeft(current);
		if(*context == FOUND) {
			*context = (direction == RIGHT ? ZIGZIG_STEP : ZIGZAG_STEP);
			return current;
		}
	}else {
		*context = LEFT;
		current->left = delete_r(current->left,key,context,found);
		if(!current->left)
			return current;
		if(*context == NOT_FOUND)
			return current;
		if(*context == ZIGZIG_STEP){
			*context = FOUND;
			return rotateRight(rotateRight(current));
		}
		if(*context == ZIGZAG_STEP){
			*context = FOUND;
			current->left = rotateLeft(current->left);
			return rotateRight(current);
		}
		if(direction == ROOT)
			return rotateRight(current);
		if(*context == FOUND) {
			*context = (direction == LEFT ? ZIGZIG_STEP : ZIGZAG_STEP);
			return current;
		}
	}
	return NULL;
}

static spltreeNode_t* largest_r(spltreeNode_t* current,uint8_t* context,BOOL splay,BOOL delete){
	if(current->right == NULL){
		*context = FOUND;
		return current;
	}
	if (splay) {
		current->right = largest_r(current->right, context, splay,delete);
		if ((*context) == FOUND) {
			*context = ZIGZIG_STEP;
			return current;
		} else if ((*context) == ZIGZIG_STEP) {
			current = rotateLeft(rotateLeft(current));
			*context = FOUND;
			return current;
		}
	}else{
		spltreeNode_t* largest = largest_r(current->right,context,splay,delete);
		if(current->right == largest) {
			current->right = largest->left;
		}
		return largest;
	}
	return NULL;
}

static spltreeNode_t* smallest_r(spltreeNode_t* current,uint8_t* context,BOOL splay,BOOL delete){
	if(current->left == NULL){
		*context = FOUND;
		return current;
	}
	if (splay) {
		current->left = smallest_r(current->left, context, splay,delete);
		if ((*context) == FOUND) {
			*context = ZIGZIG_STEP;
			return current;
		} else if ((*context) == ZIGZIG_STEP) {
			current = rotateRight(rotateRight(current));
			*context = FOUND;
			return current;
		}
	}
	else{
		spltreeNode_t* smallest = smallest_r(current->left,context,splay,delete);
		if(current->left == smallest) {
			current->left = smallest->right;
		}
		return smallest;
	}
	return NULL;
}


static spltreeNode_t* rotateLeft(spltreeNode_t* parent){
	spltreeNode_t* nparent = parent->right;
	parent->right = nparent->left;
	nparent->left = parent;
	return nparent;
}

static spltreeNode_t* rotateRight(spltreeNode_t* parent){
	spltreeNode_t* nparent = parent->left;
	parent->left = nparent->right;
	nparent->right = parent;
	return nparent;
}
