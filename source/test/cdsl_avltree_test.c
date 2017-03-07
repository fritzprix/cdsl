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
	cdsl_avltreeRootInit(&root, 2);

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
	if(cdsl_avltreeSize(&root) != TEST_SIZE) {
		return FALSE;
	}
	avltreeNode_t* node = cdsl_avltreeLookup(&root, keys[0]);
	if(node->key != keys[0]) {
		return FALSE;
	}

	if(cdsl_avltreeSize(&root) != TEST_SIZE) {
		return FALSE;
	}


//	cdsl_avltreePrint(&root, avlnode_printer);

	return TRUE;
}

static DECLARE_PRINTER(avlnode_printer) {
	avltreeNode_t* avlnode = (avltreeNode_t*) node;

	PRINT("NODE : %ld @ %d\n", avlnode->key, avlnode->height);

}

