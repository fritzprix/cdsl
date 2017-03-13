/*
 * cdsl_avltree_test.c
 *
 *  Created on: 2017. 3. 5.
 *      Author: innocentevil
 */

#include "cdsl_avltree_test.h"
#include "cdsl_avltree.h"
#include "base_tree.h"
#include <stdio.h>
#include <stdlib.h>

static avltreeNode_t node_pool[TEST_SIZE];
static avltreeNode_t replace;
static trkey_t keys[TEST_SIZE];

static DECLARE_PRINTER(avlnode_printer);

BOOL cdsl_avltreeDoTest(void) {
	avltreeRoot_t root;
	cdsl_avltreeRootInit(&root, 1);

	keys[TEST_SIZE - 1] = TEST_SIZE + 1;
	cdsl_avltreeNodeInit(&replace, keys[TEST_SIZE - 1]);
	int i = 0;
	int depth;
	depth = 0;
	for (i = 0; i < TEST_SIZE; i++) {
		if (i != TEST_SIZE - 1)
			keys[i] = i;
		cdsl_avltreeNodeInit(&node_pool[i], keys[i]);
		cdsl_avltreeInsert(&root, &node_pool[i], FALSE);
	}
	if (cdsl_avltreeSize(&root) != TEST_SIZE) {
		return FALSE;
	}
	avltreeNode_t* node = cdsl_avltreeLookup(&root, keys[0]);
	if (node->key != keys[0]) {
		return FALSE;
	}

	if (cdsl_avltreeSize(&root) != TEST_SIZE) {
		return FALSE;
	}
	for (i = 0; i < TEST_SIZE; i++) {
		node = cdsl_avltreeDeleteReplace(&root, keys[i], NULL, NULL);
		if (!node) {
			return FALSE;
		}
		if (node->key != keys[i]) {
			return FALSE;
		}
	}
	for (i = 0; i < TEST_SIZE; i++) {
		cdsl_avltreeNodeInit(&node_pool[i], keys[i]);
		cdsl_avltreeInsert(&root, &node_pool[i], FALSE);
	}
	if (cdsl_avltreeSize(&root) != TEST_SIZE) {
		return FALSE;
	}
	for (i = 0; i < TEST_SIZE; i++) {
		node = cdsl_avltreeDeleteMinReplace(&root, NULL, NULL);
		if (!node) {
			return FALSE;
		}
	}

	if (cdsl_avltreeSize(&root) != 0) {
		return FALSE;
	}

	for (i = 0; i < TEST_SIZE; i++) {
		cdsl_avltreeNodeInit(&node_pool[i], keys[i]);
		cdsl_avltreeInsert(&root, &node_pool[i], FALSE);
	}
	if (cdsl_avltreeSize(&root) != TEST_SIZE) {
		return FALSE;
	}
	for (i = 0; i < TEST_SIZE; i++) {
		node = cdsl_avltreeDeleteMaxReplace(&root, NULL, NULL);
		if (!node) {
			return FALSE;
		}
	}

	if (cdsl_avltreeSize(&root) != 0) {
		return FALSE;
	}

	avltreeRoot_t aroot;
	cdsl_avltreeRootInit(&aroot, 1);
	for (i = 0; i < TEST_SIZE; i++) {
		cdsl_avltreeNodeInit(&node_pool[i], keys[i]);
		cdsl_avltreeInsert(&root, &node_pool[i], FALSE);
	}

	for (i = 0; i < (TEST_SIZE >> 1); i++) {
		node = cdsl_avltreeDeleteMaxReplace(&root, NULL, NULL);
		if (!node) {
			return FALSE;
		}
		cdsl_avltreeInsert(&aroot, node, FALSE);
	}
	if (cdsl_avltreeSize(&aroot) != (TEST_SIZE >> 1)) {
		return FALSE;
	}

	for (i = 0; i < (TEST_SIZE >> 1); i++) {
		node = cdsl_avltreeDeleteMinReplace(&aroot, NULL, NULL);
		if (!node) {
			return FALSE;
		}
		cdsl_avltreeInsert(&root, node, FALSE);
	}
	if (cdsl_avltreeSize(&aroot) != 0) {
		return FALSE;
	}

	avltreeNode_t* delete_node;
	trkey_t key = 0;
	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = NULL;
		delete_node = cdsl_avltreeDeleteMin(&root);
		if (!delete_node) {
			__dev_log("null node detected !!\n");
			return FALSE;
		}
		if (key > delete_node->key) {
			__dev_log("not increasing order\n");
			return FALSE;
		}
		key = delete_node->key;
		cdsl_avltreeNodeInit(delete_node, key);
		cdsl_avltreeInsert(&aroot, delete_node, FALSE);
	}


	if (cdsl_avltreeSize(&aroot) != TEST_SIZE) {
		__dev_log("some node is missed\n");
		return FALSE;
	}

	key = 0xffffffff;
	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = NULL;
		delete_node = cdsl_avltreeDeleteMax(&aroot);
		if (!delete_node) {
			__dev_log("unexptected null node !! \n");
			return FALSE;
		}
		if (key < delete_node->key) {
			__dev_log("not decreasing order\n");
			return FALSE;
		}
		key = delete_node->key;
	}

	if (cdsl_avltreeTop(&aroot) != NULL) {
		return FALSE;
	}

	for (i = 0; i < TEST_SIZE; i++) {
		cdsl_avltreeNodeInit(&node_pool[i], rand());
		cdsl_avltreeInsert(&root, &node_pool[i], FALSE);
	}

	if (cdsl_avltreeMaxDepth(&root) > 25) {
		PRINT_ERR("abnormal balancing failure %d\n",
				cdsl_avltreeMaxDepth(&root));
		return FALSE;
	}

	for (i = 0; i < TEST_SIZE; i++) {
		delete_node = cdsl_avltreeDeleteMin(&root);
		if (!delete_node) {
			PRINT_ERR("unexpected null node\n");
			return FALSE;
		}
	}

	if (cdsl_avltreeSize(&root) > 0) {
		PRINT_ERR("unexpected non-empty tree\n");
		return FALSE;
	}

	// key collision test
	avltreeNode_t* replace = NULL;
	cdsl_avltreeNodeInit(&node_pool[0], 1);
	cdsl_avltreeNodeInit(&node_pool[1], 1);
	if ((replace = cdsl_avltreeInsert(&root, &node_pool[0], TRUE))) {
		PRINT_ERR("unexpected replacement in set operation\n");
		return FALSE;
	}
	if (!(replace = cdsl_avltreeInsert(&root, &node_pool[1], TRUE))) {
		PRINT_ERR("no replacement happens\n");
		return FALSE;
	}
	if (replace != &node_pool[0]) {
		PRINT_ERR("wrong replacement\n");
		return FALSE;
	}
	return TRUE;
}

static DECLARE_PRINTER(avlnode_printer) {
	avltreeNode_t* avlnode = (avltreeNode_t*) node;

	PRINT("NODE : %ld @ %d\n", avlnode->key, avlnode->height);

}

