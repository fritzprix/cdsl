/*
 * cdsl_avltree_test.c
 *
 *  Created on: 2017. 3. 5.
 *      Author: innocentevil
 */

#include "cdsl_avltree_test.h"
#include "cdsl_avltree.h"
#include "cdsl_hash.h"
#include "base_tree.h"
#include "file_serializer.h"
#include "serializer_test.h"
#include <stdio.h>
#include <stdlib.h>

static avltreeNode_t node_pool[TEST_SIZE];
static avltreeNode_t replace;
static trkey_t keys[TEST_SIZE];

static DECLARE_PRINTER(avlnode_printer);

typedef struct person {
	avltreeNode_t node;
	char          name[20];
}person_t;

static person_t people[SER_SIZE];



static DECLARE_SERIALIZER_ON_ERROR(onSerError);
static DECLARE_SERIALIZER_ON_NODE(onSerNode);
static DECLARE_SERIALIZER_ON_COMPLETE(onSerDone);


static cdsl_serializerUsrCallback_t callback = {
		.on_complete = onSerDone,
		.on_node_to_data = onSerNode,
		.on_error = onSerError
};


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




	for(i = 0;i < SER_SIZE;i++) {
		cdsl_avltreeNodeInit(&people[i].node, sbdm_hash((unsigned char*) names[i]));
		strcpy(people[i].name, names[i]);
		cdsl_avltreeInsert(&aroot, &people[i].node, FALSE);
	}


	if(cdsl_avltreeSize(&aroot) != SER_SIZE) {
		return FALSE;
	}
	file_serializer_t fser;
	file_serializerOpen(&fser, SERIALIZE_FILE_NAME);
	cdsl_avltreeSerialize(&aroot, &fser, &callback);
	file_serializerClose(&fser);

	avltreeRoot_t nroot;
	cdsl_avltreeRootInit(&nroot,2);


	file_deserializer_t desr;
	if(file_deserializerOpen(&desr, SERIALIZE_FILE_NAME) != OK) {
		PRINT("OPEN Failed \n");
	}
	const cdsl_memoryMngt_t mmngt = GET_DEFAULT_MMNGT();
	cdsl_avltreeDeserialize(&nroot, &desr, &mmngt);
	file_deserializerClose(&desr);

	if(cdsl_avltreeCompare(&nroot, &aroot) != 0) {
		return FALSE;
	}

	person_t apeople[5];
	person_t npeople[5];
	const char* another_names[5] = {
			"Alice",
			"Bob",
			"Smith",
			"Sam",
			"Jack"
	};

	for(i = 0;i < 5;i++) {
		cdsl_avltreeNodeInit(&apeople[i].node, sbdm_hash((unsigned char*) another_names[i]));
		cdsl_avltreeNodeInit(&npeople[i].node, sbdm_hash((unsigned char*) another_names[i]));

		strcpy(apeople[i].name, another_names[i]);
		strcpy(npeople[i].name, another_names[i]);

		cdsl_avltreeInsert(&aroot, &apeople[i].node, FALSE);
		cdsl_avltreeInsert(&nroot, &npeople[i].node, FALSE);
	}

	if(cdsl_avltreeCompare(&nroot, &aroot) != 0) {
		return FALSE;
	}

	cdsl_avltreeDeleteMin(&aroot);
	if(cdsl_avltreeCompare(&nroot, &aroot) == 0) {
		return FALSE;
	}

	return TRUE;
}



static DECLARE_SERIALIZER_ON_ERROR(onSerError) {

}

static DECLARE_SERIALIZER_ON_NODE(onSerNode) {
	person_t* person = container_of(node, person_t, node);
	*size = offsetof(person_t, name) + (strlen(person->name) * sizeof(char));
	if((*size % 2) != 0) {
		*size += 1;
	}
	return (void*) node;
}

static DECLARE_SERIALIZER_ON_COMPLETE(onSerDone) {
	// NOP
}

static DECLARE_PRINTER(avlnode_printer) {
	avltreeNode_t* avlnode = (avltreeNode_t*) node;

	PRINT("NODE : %ld @ %d\n", avlnode->key, avlnode->height);

}

