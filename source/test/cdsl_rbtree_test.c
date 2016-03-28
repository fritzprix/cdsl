/*
 * cdsl_rbtree_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */

#include "cdsl.h"
#include "cdsl_rbtree.h"
#include <stdlib.h>
#include <stdio.h>

static rb_treeNode_t node_pool[TEST_SIZE];
static int keys[TEST_SIZE];

BOOL cdsl_rbtreeDoTest(void){
	rb_treeNode_t* root;
	root = NULL;
	int i = 0;
	int depth,depth_temp;
	depth = 0;
	depth_temp = 0;

	for(i = 0;i < TEST_SIZE;i++){
		keys[i] = rand() % TEST_SIZE;
		cdsl_rbtreeNodeInit(&node_pool[i],keys[i]);
		cdsl_rbtreeInsert(&root,&node_pool[i]);
		/*
		*/
	}
	depth = cdsl_rbtreeMaxDepth(&root);
	__dev_log("Max Depth of Red-Black Tree : %d @ N : %d\n",depth,i);
	if(TEST_SIZE != cdsl_rbtreeSize(&root))
		return FALSE;
	rb_treeNode_t* del = NULL;
	for(i = 0;i < TEST_SIZE;i++){
		del = NULL;
		del = cdsl_rbtreeDelete(&root,keys[i]);
		if(!del)
			return FALSE;

		if(del->key != keys[i])
			return FALSE;
		// insert new random value to the tree
		keys[i] = rand() % TEST_SIZE;
		cdsl_rbtreeNodeInit(del,keys[i]);
		cdsl_rbtreeInsert(&root,del);
	}

	for(i = 0;i < TEST_SIZE;i++){
		del = NULL;
		del = cdsl_rbtreeDelete(&root,keys[i]);
		if(!del)
			return FALSE;
		if(del->key != keys[i])
			return FALSE;
	}
	if(cdsl_rbtreeSize(&root) > 0)
		return FALSE;
	return TRUE;

}
