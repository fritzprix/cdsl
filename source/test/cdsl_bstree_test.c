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


static int bstree_cb(int order,void* bst);
static int cb_count;
BOOL cdsl_bstreeDoTest(void){
	bs_treeNode_t bst_nodepool[TEST_SIZE];
	int keys[TEST_SIZE];
	bs_treeRoot_t root;
	cb_count = 0;
	bstree_root_init(&root);

	int i = 0;
	int depth,depth_temp;
	depth = 0;
	depth_temp = 0;

	for(i = 0;i < TEST_SIZE;i++){
		keys[i] = rand() % TEST_SIZE;
		bstree_node_init(&bst_nodepool[i],keys[i]);
		bstree_insert(&root,&bst_nodepool[i]);
		depth_temp = bstree_max_depth(&root);
		if(depth != depth_temp){
			depth = depth_temp;
		}
	}

	if(bstree_size(&root) != TEST_SIZE)
		return FALSE;

	bs_treeNode_t* delete_node;
	for(i = 0;i < TEST_SIZE;i++){
		delete_node = NULL;
		delete_node = bstree_delete(&root,keys[i]);
		if(!delete_node)
			return FALSE;
		if(delete_node->key != keys[i])
			return FALSE;
		keys[i] = rand() % TEST_SIZE;
		bstree_node_init(delete_node,keys[i]);
		bstree_insert(&root,delete_node);
	}

	bstree_traverse(&root,bstree_cb,ORDER_INC);
	if(cb_count != 2000)
		return FALSE;

	for(i = 0;i < TEST_SIZE;i++){
		delete_node = NULL;
		delete_node = bstree_delete(&root,keys[i]);
		if(!delete_node)
			return FALSE;
		if(delete_node->key != keys[i])
			return FALSE;
	}

	if(bstree_size(&root) > 0)		// size should be zero
		return FALSE;

	return TRUE;
}

static int bstree_cb(int order,void* bst)
{
	cb_count++;
	if(order == 2000)
	{
		return BREAK_TRAVERS;
	}
	return (1 > 0);
}

