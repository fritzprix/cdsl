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
#include <pthread.h>

static nrbtreeNode_t node_pool[TEST_SIZE];
static nrbtreeNode_t replace;
static int keys[TEST_SIZE];

struct compete_req {
	nrbtreeRoot_t* root;
	trkey_t base;
	pthread_mutex_t* lock;
};

BOOL cdsl_nrbtreeDoTest(void) {
	nrbtreeRoot_t root, aroot;
	cdsl_nrbtreeRootInit(&root);
	cdsl_nrbtreeRootInit(&aroot);

	keys[TEST_SIZE - 1] = TEST_SIZE + 1;
	cdsl_nrbtreeNodeInit(&replace, keys[TEST_SIZE - 1]);
	int i = 0;
	int depth;
	depth = 0;
	for (i = 0; i < TEST_SIZE; i++) {
		if (i != TEST_SIZE - 1)
			keys[i] = i;
		cdsl_nrbtreeNodeInit(&node_pool[i], keys[i]);
		cdsl_nrbtreeInsert(&root, &node_pool[i]);
	}
	depth = cdsl_nrbtreeMaxDepth(&root);
	__dev_log("Max Depth of New Red-Black Tree : %d @ N : %d\n",depth,i);
	cdsl_nrbtreeUpdate(&root, &replace);
	if (cdsl_nrbtreeLookup(&root, keys[TEST_SIZE - 1]) != &replace)
		return FALSE;
	if (cdsl_nrbtreeSize(&root) != TEST_SIZE)
		return FALSE;
	nrbtreeNode_t* delete_node;
	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = NULL;
		__dev_log("\nkey : %d will be deleted!! \n\n",keys[i]);
		delete_node = cdsl_nrbtreeDelete(&root, keys[i]);
		if (!delete_node) {
			__dev_log("null node detected !!\n");
			return FALSE;
		}
		if (delete_node->key != keys[i]) {
			__dev_log("key corrupted !!\n");
			return FALSE;
		}
		keys[i] = rand() % TEST_SIZE;
		cdsl_nrbtreeNodeInit(delete_node, keys[i]);
		cdsl_nrbtreeInsert(&root, delete_node);
	}

	trkey_t key = 0;
	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = NULL;
		delete_node = cdsl_nrbtreeDeleteMin(&root);
		if (!delete_node) {
			__dev_log("null node detected !!\n");
			return FALSE;
		}
		if (key > delete_node->key) {
			__dev_log("not increasing order\n");
			return FALSE;
		}
		key = delete_node->key;
		cdsl_nrbtreeNodeInit(delete_node,key);
		cdsl_nrbtreeInsert(&aroot, delete_node);
	}

	if (cdsl_nrbtreeSize(&aroot) != TEST_SIZE) {
		__dev_log("some node is missed\n");
		return FALSE;
	}

	key = 0xffffffff;
	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = NULL;
		delete_node = cdsl_nrbtreeDeleteMax(&aroot);
		if(!delete_node) {
			__dev_log("null node detected !! \n");
			return FALSE;
		}
		if(key < delete_node->key) {
			__dev_log("not decreasing order\n");
			return FALSE;
		}
		key = delete_node->key;
	}

	if(cdsl_nrbtreeTop(&aroot) != NULL)
		return FALSE;

	if (cdsl_nrbtreeSize(&root) > 0)
		return FALSE;

	return TRUE;
}
