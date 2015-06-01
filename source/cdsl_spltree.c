/*
 * cdsl_spltree.c
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */


#include "cdsl.h"
#include "cdsl_spltree.h"


#define LEFT	((uint8_t) 0)
#define RIGHT	((uint8_t) 1)
#define ROOT	((uint8_t) 2)

static spltreeNode_t* insert_r(spltreeNode_t* current,spltreeNode_t* item,uint8_t* context);
static spltreeNode_t* lookup_r(spltreeNode_t* current,int key,uint8_t* context,BOOL splay);
static spltreeNode_t* rotateLeft(spltreeNode_t* parent);
static spltreeNode_t* rotateRight(spltreeNode_t* parent);


void cdsl_spltreeInitNode(spltreeNode_t* node,int key){
	node->key = key;
	node->left = node->right = NULL;
}

void cdsl_spltreeInsert(spltreeNode_t** root,spltreeNode_t* item){
	if(!root)
		return;
	if(!(*root))
		*root = item;
	uint8_t context;
	if(root->key < item->key){
		context = RIGHT;
		(*root)->right = insert_r((*root)->right,item,&context);
		*root = rotateLeft(*root);
	}else{
		context = LEFT;
		(*root)->left = insert_r((*root)->left,item,&context);
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

}

spltreeNode_t* cdsl_spltreeLookupSmallest(spltreeNode_t** root,BOOL splay){

}

spltreeNode_t* cdsl_spltreeDelete(spltreeNode_t** root,int key){
	if(!root || !(*root))
		return NULL;
	uint8_t context;
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
}

static spltreeNode_t* insert_r(spltreeNode_t* current,spltreeNode_t* item,uint8_t* context){

}

static spltreeNode_t* lookup_r(spltreeNode_t* current,int key,uint8_t* context,BOOL splay){

}


static spltreeNode_t* rotateLeft(spltreeNode_t* parent){

}

static spltreeNode_t* rotateRight(spltreeNode_t* parent){

}
