/*
 * cdsl_rbtree_test.c
 *
 *  Created on: Mar 26, 2016
 *      Author: innocentevil
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "../../include/cdsl_rbtree.h"
#include "../../include/test/cdsl_rbtree_test.h"

static rbtreeNode_t node_pool[TEST_SIZE];
static rbtreeNode_t replace;
static int keys[TEST_SIZE];

struct compete_req {
	rbtreeRoot_t* root;
	trkey_t base;
	pthread_mutex_t* lock;
};

static BOOL is_match(base_treeNode_t* node, trkey_t key);

BOOL cdsl_rbtreeDoTest(void) {
	rbtreeRoot_t root, aroot;
	cdsl_rbtreeRootInit(&root);
	cdsl_rbtreeRootInit(&aroot);

	keys[TEST_SIZE - 1] = TEST_SIZE + 1;
	cdsl_rbtreeNodeInit(&replace, keys[TEST_SIZE - 1]);
	int i = 0;
	int depth;
	depth = 0;
	for (i = 0; i < TEST_SIZE; i++) {
		if (i != TEST_SIZE - 1)
			keys[i] = i;
		cdsl_rbtreeNodeInit(&node_pool[i], keys[i]);
		cdsl_rbtreeInsert(&root, &node_pool[i],FALSE);
	}
	depth = cdsl_rbtreeMaxDepth(&root);
	__dev_log("Max Depth of New Red-Black Tree : %d @ N : %d\n",depth,i);
	cdsl_rbtreeUpdate(&root, &replace);
	if (cdsl_rbtreeLookup(&root, keys[TEST_SIZE - 1]) != &replace)
		return FALSE;
	if (cdsl_rbtreeSize(&root) != TEST_SIZE)
		return FALSE;
	rbtreeNode_t* delete_node;
	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = NULL;
		__dev_log("\nkey : %d will be deleted!! \n\n",keys[i]);
		delete_node = cdsl_rbtreeDelete(&root, keys[i]);
		if (!delete_node) {
			__dev_log("null node detected !!\n");
			return FALSE;
		}
		if (delete_node->key != keys[i]) {
			__dev_log("key corrupted !!\n");
			return FALSE;
		}
		keys[i] = rand() % TEST_SIZE;
		cdsl_rbtreeNodeInit(delete_node, keys[i]);
		cdsl_rbtreeInsert(&root, delete_node,FALSE);
	}

	trkey_t key = 0;
	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = NULL;
		delete_node = cdsl_rbtreeDeleteMin(&root);
		if (!delete_node) {
			__dev_log("null node detected !!\n");
			return FALSE;
		}
		if (key > delete_node->key) {
			__dev_log("not increasing order\n");
			return FALSE;
		}
		key = delete_node->key;
		cdsl_rbtreeNodeInit(delete_node, key);
		cdsl_rbtreeInsert(&aroot, delete_node, FALSE);
	}

	if (cdsl_rbtreeSize(&aroot) != TEST_SIZE) {
		__dev_log("some node is missed\n");
		return FALSE;
	}

	key = 0xffffffff;
	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = NULL;
		delete_node = cdsl_rbtreeDeleteMax(&aroot);
		if(!delete_node) {
			__dev_log("unexptected null node !! \n");
			return FALSE;
		}
		if(key < delete_node->key) {
			__dev_log("not decreasing order\n");
			return FALSE;
		}
		key = delete_node->key;
	}

	if(cdsl_rbtreeTop(&aroot) != NULL)
		return FALSE;



	for (i = 0;i < TEST_SIZE; i++) {
		cdsl_rbtreeNodeInit(&node_pool[i], rand());
		cdsl_rbtreeInsert(&root, &node_pool[i], FALSE);
	}
	if (cdsl_rbtreeMaxDepth(&root) > 25) {
		PRINT_ERR("abnormal balancing failure %d\n", cdsl_rbtreeMaxDepth(&root));
		return FALSE;
	}

	for (i = 0;i < TEST_SIZE; i++) {
		delete_node = cdsl_rbtreeDeleteMin(&root);
		if(!delete_node) {
			PRINT_ERR("unexpected null node\n");
			return FALSE;
		}
	}

	if (cdsl_rbtreeSize(&root) > 0) {
		PRINT_ERR("unexpected non-empty tree\n");
		return FALSE;
	}

	// key collision test
	rbtreeNode_t* replace = NULL;
	cdsl_rbtreeNodeInit(&node_pool[0], 1);
	cdsl_rbtreeNodeInit(&node_pool[1], 1);
	if((replace = cdsl_rbtreeInsert(&root, &node_pool[0], TRUE))) {
		PRINT_ERR("unexpected replacement in set operation\n");
		return FALSE;
	}
	if(!(replace = cdsl_rbtreeInsert(&root, &node_pool[1], TRUE))) {
		PRINT_ERR("no replacement happens\n");
		return FALSE;
	}
	if(replace != &node_pool[0]) {
		PRINT_ERR("wrong replacement\n");
		return FALSE;
	}

	if(!cdsl_rbtreeConditionalLookup(&root, 1, is_match)) {
		PRINT_ERR("fail to condition match\n");
		return FALSE;
	}

	return TRUE;
}


static BOOL is_match(base_treeNode_t* node, trkey_t key) {
	return node->key == key;
}

