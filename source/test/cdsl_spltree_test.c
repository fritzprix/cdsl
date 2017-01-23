/*
 * cdsl_spltree_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */


#include "cdsl_spltree.h"
#include "cdsl_spltree_test.h"
#include <stdio.h>
#include <stdlib.h>

static spltreeNode_t nodes[TEST_SIZE];
static int keys[TEST_SIZE];
static spltreeNode_t replace;

BOOL cdsl_spltreeDoTest(void){
	static spltreeRoot_t aroot,root;
	int i = 0;
	int depth,depth_temp;
	depth = 0;
	depth_temp = 0;

	cdsl_spltreeRootInit(&aroot);
	cdsl_spltreeRootInit(&root);

	keys[TEST_SIZE - 1] = TEST_SIZE + 1;
	cdsl_spltreeNodeInit(&replace, keys[TEST_SIZE - 1]);

	for(;i < TEST_SIZE;i++){
		if(i != TEST_SIZE - 1)
			keys[i] = rand() % TEST_SIZE;
		cdsl_spltreeNodeInit(&nodes[i],keys[i]);
		cdsl_spltreeInsert(&root,&nodes[i]);

	}
	depth = cdsl_spltreeMaxDepth(&root);
	__dev_log("Max Depth of Splay Tree : %d @ N : %d\n",depth,i);

	cdsl_spltreeUpdate(&root, &replace);
	if(cdsl_spltreeLookup(&root,keys[TEST_SIZE - 1], FALSE) != &replace)
		return FALSE;

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
		keys[i] = rand() % TEST_SIZE;
		cdsl_spltreeNodeInit(del, keys[i]);
		cdsl_spltreeInsert(&root, del);
	}

	if(cdsl_spltreeSize(&root) != TEST_SIZE) {
		__dev_log("some node is lost in insert op\n");
		return FALSE;
	}

	trkey_t key = 0;
	for (i = 0; i < TEST_SIZE; i++) {
		del = NULL;
		del = cdsl_spltreeDeleteMin(&root);
		if (!del) {
			__dev_log("null node detected !!\n");\
			return FALSE;
		}
		if (key > del->key) {
			__dev_log("not increasing order\n");
			return FALSE;
		}
		key = del->key;
		cdsl_spltreeNodeInit(del,key);
		cdsl_spltreeInsert(&aroot, del);
	}

	if (cdsl_spltreeSize(&aroot) != TEST_SIZE) {
		__dev_log("some node is missed\n");
		return FALSE;
	}

	key = 0xffffffff;
	for (i = 0; i < TEST_SIZE; i++) {
		del = NULL;
		del = cdsl_spltreeDeleteMax(&aroot);
		if(!del) {
			__dev_log("null node detected !! \n");
			return FALSE;
		}
		if(key < del->key) {
			__dev_log("not decreasing order\n");
			return FALSE;
		}
		key = del->key;
	}

	if(cdsl_spltreeTop(&aroot) != NULL)
		return FALSE;

	if (cdsl_spltreeSize(&root) > 0)
		return FALSE;

	return TRUE;
}
