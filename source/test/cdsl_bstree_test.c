/*
 * cdsl_bstree_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */

#include "cdsl.h"
#include "cdsl_bstree.h"
#include "cdsl_bstree_test.h"
#include <stdio.h>
#include <stdlib.h>

static bstreeNode_t bst_nodepool[TEST_SIZE];
static bstreeNode_t replace;
static trkey_t repl_key;
static trkey_t keys[TEST_SIZE];

static int cb_count;
BOOL cdsl_bstreeDoTest(void)
{
	bstreeRoot_t root;
	cb_count = 0;
	cdsl_bstreeRootInit(&root);

	int i = 0;
	int depth,depth_temp;
	depth = 0;
	depth_temp = 0;

	keys[TEST_SIZE - 1] = repl_key =TEST_SIZE + 10;
	cdsl_bstreeNodeInit(&replace, keys[TEST_SIZE - 1]);

	for(i = 0;i < TEST_SIZE;i++)
	{
		if(i != (TEST_SIZE - 1))
			keys[i] = rand() % TEST_SIZE;
		cdsl_bstreeNodeInit(&bst_nodepool[i],keys[i]);
		cdsl_bstreeInsert(&root,&bst_nodepool[i]);
	}
	cdsl_bstreeUpdate(&root, &replace);
	if(cdsl_bstreeLookup(&root,repl_key) != &replace)
		return FALSE;

	depth = cdsl_bstreeMaxDepth(&root);
	__dev_log("Max Depth of binary search Tree : %d @ N : %d\n",depth,i);

	if(cdsl_bstreeSize(&root) != TEST_SIZE)
		return FALSE;

	bstreeNode_t* delete_node;
	for(i = 0;i < TEST_SIZE;i++)
	{
		delete_node = NULL;
		delete_node = cdsl_bstreeDelete(&root,keys[i]);
		if(!delete_node)
			return FALSE;
		if(delete_node->key != keys[i])
			return FALSE;
		keys[i] = rand() % TEST_SIZE;
		cdsl_bstreeNodeInit(delete_node,keys[i]);
		cdsl_bstreeInsert(&root,delete_node);
	}


	for(i = 0;i < TEST_SIZE;i++)
	{
		delete_node = NULL;
		delete_node = cdsl_bstreeDelete(&root,keys[i]);
		if(!delete_node)
			return FALSE;
		if(delete_node->key != keys[i])
			return FALSE;
	}

	if(cdsl_bstreeSize(&root) > 0)		// size should be zero
		return FALSE;
	return TRUE;
}


