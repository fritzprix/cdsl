/*
 * cdsl_bstree_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */

#include "cdsl.h"
#include "cdsl_bstree.h"
#include "cdsl_bstree_test.h"

BOOL cdsl_bstreeDoTest(void){
	bs_treeNode_t* root;
	bs_treeNode_t bst_nodepool[TEST_SIZE];
	int keys[TEST_SIZE];

	root = NULL;
	int i = 0;

	for(i = 0;i < TEST_SIZE;i++){
		keys[i] = rand() % TEST_SIZE;
		cdsl_bstreeNodeInit(&bst_nodepool[i],keys[i]);
		cdsl_bstreeInsert(&root,&bst_nodepool[i]);
	}
	if(cdsl_bstreeSize(&root) != TEST_SIZE)
		return FALSE;

	bs_treeNode_t* delete_node;
	for(i = 0;i < TEST_SIZE;i++){
		delete_node = NULL;
		delete_node = cdsl_bstreeDelete(&root,keys[i]);
		if(!delete_node)
			return FALSE;
		if(delete_node->key != keys[i])
			return FALSE;

	}

	return TRUE;
}
