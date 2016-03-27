/*
 * cdsl_nrbtree_test.c
 *
 *  Created on: Mar 26, 2016
 *      Author: innocentevil
 */


#include "cdsl_nrbtree.h"
#include "cdsl_nrbtree_test.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#undef TEST_SIZE
#define TEST_SIZE 20


BOOL cdsl_nrbtreeDoTest(void)
{
	nrbtreeRoot_t root;
	nrbtreeNode_t node_pool[TEST_SIZE];
	int keys[TEST_SIZE];

	cdsl_nrbtreeRootInit(&root);
	int i = 0;
	int depth,depth_temp;
	depth = 0;
	depth_temp = 0;

	for(i = 0;i < TEST_SIZE;i++){
		keys[i] = rand() % TEST_SIZE;
		cdsl_nrbtreeNodeInit(&node_pool[i],keys[i]);
		__dev_log("%d inserted!! \n", keys[i]);
		cdsl_nrbtreeInsert(&root,&node_pool[i]);
		cdsl_nrbtreePrint_dev(&root);
		depth_temp = cdsl_nrbtreeMaxDepth(&root);
		if(depth != depth_temp){
			__dev_log("Max Depth of Tree : %d @ N : %d\n",depth_temp,i);
			depth = depth_temp;
		}
	}
	if(cdsl_nrbtreeSize(&root) != TEST_SIZE)
		return FALSE;

	cdsl_nrbtreePrint_dev(&root);
	nrbtreeNode_t* delete_node;
	for(i = 0;i < TEST_SIZE;i++)
	{
		delete_node = NULL;
		__dev_log("\nkey : %d will be deleted!! \n\n",keys[i]);
		delete_node = cdsl_nrbtreeDelete(&root,keys[i]);
		cdsl_nrbtreePrint_dev(&root);
		if(!delete_node)
			return FALSE;
		if(delete_node->key != keys[i])
			return FALSE;
//		keys[i] = rand() % TEST_SIZE;
//		cdsl_nrbtreeNodeInit(delete_node,keys[i]);
//		cdsl_nrbtreeInsert(&root,delete_node);
	}

	return TRUE;
}
