/*
 * cdsl_spltree_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */


#include "cdsl.h"
#include "cdsl_spltree.h"
#include "cdsl_spltree_test.h"
#include <stdio.h>

static spltreeNode_t nodes[TEST_SIZE];
static int keys[TEST_SIZE];
static spltreeNode_t* root;

BOOL cdsl_spltreeDoTest(void){
	int i = 0;
	root = NULL;

	int depth,depth_temp;
	depth = 0;
	depth_temp = 0;

	for(;i < TEST_SIZE;i++){
		keys[i] = rand() % TEST_SIZE;
		cdsl_spltreeNodeInit(&nodes[i],keys[i]);
		cdsl_spltreeInsert(&root,&nodes[i]);
		depth_temp = cdsl_spltreeMaxDepth(&root);
		if(depth != depth_temp){
//			log("Max Depth of Tree : %d @ N : %d\n",depth_temp,i);
			depth = depth_temp;
		}
	}

	if(cdsl_spltreeSize(&root) != TEST_SIZE)
		return FALSE;

	spltreeNode_t* del = NULL;
	for(i = 0;i < TEST_SIZE;i++){
		del = cdsl_spltreeDelete(&root,keys[i]);
		if(!del)
			return FALSE;
		if(del->key != keys[i])
			return FALSE;
		// insert new random value to the tree
		keys[i] = rand() % TEST_SIZE;
		cdsl_spltreeNodeInit(del,keys[i]);
		cdsl_spltreeInsert(&root,del);
	}

	for(i = 0;i < TEST_SIZE;i++){
		del = NULL;
		del = cdsl_spltreeDelete(&root,keys[i]);
		if(!del)
			return FALSE;
		if(del->key != keys[i])
			return FALSE;
	}
	if(cdsl_spltreeSize(&root) > 0)
		return FALSE;
	return TRUE;
}
