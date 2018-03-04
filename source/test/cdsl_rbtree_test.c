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

#include "cdsl_hash.h"
#include "cdsl_rbtree.h"
#include "cdsl_rbtree_test.h"
#include "file_serializer.h"
#include "serializer_test.h"

static rbtreeNode_t node_pool[TEST_SIZE];
static rbtreeNode_t replace;
static int keys[TEST_SIZE];

struct compete_req {
	rbtreeRoot_t* root;
	trkey_t base;
	pthread_mutex_t* lock;
};


typedef struct person {
	rbtreeNode_t node;
	char         name[20];
}person_t;

static person_t people[SER_SIZE];


static BOOL is_match(base_treeNode_t* node, trkey_t key);


static DECLARE_SERIALIZER_ON_ERROR(onSerError);
static DECLARE_SERIALIZER_ON_NODE(onSerNode);
static DECLARE_SERIALIZER_ON_COMPLETE(onSerDone);


static cdsl_serializerUsrCallback_t callback = {
		.on_complete = onSerDone,
		.on_node_to_data = onSerNode,
		.on_error = onSerError
};



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


	for(i = 0;i < SER_SIZE;i++) {
		cdsl_rbtreeNodeInit(&people[i].node, sbdm_hash((unsigned char*) names[i]));
		strcpy(people[i].name, names[i]);
		cdsl_rbtreeInsert(&aroot, &people[i].node, FALSE);
	}


	if(cdsl_rbtreeSize(&aroot) != SER_SIZE) {
		return FALSE;
	}
	file_serializer_t fser;
	file_serializerOpen(&fser, SERIALIZE_FILE_NAME);
	cdsl_rbtreeSerialize(&aroot, &fser, &callback);
	file_serializerClose(&fser);

	rbtreeRoot_t nroot;
	cdsl_rbtreeRootInit(&nroot);


	file_deserializer_t desr;
	if(file_deserializerOpen(&desr, SERIALIZE_FILE_NAME) != OK) {
		PRINT("OPEN Failed \n");
	}
	const cdsl_memoryMngt_t mmngt = GET_DEFAULT_MMNGT();
	cdsl_rbtreeDeserialize(&nroot, &desr, &mmngt);
	file_deserializerClose(&desr);

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
		cdsl_rbtreeNodeInit(&apeople[i].node, sbdm_hash((unsigned char*) another_names[i]));
		cdsl_rbtreeNodeInit(&npeople[i].node, sbdm_hash((unsigned char*) another_names[i]));

		strcpy(apeople[i].name, another_names[i]);
		strcpy(npeople[i].name, another_names[i]);
		cdsl_rbtreeInsert(&aroot, &apeople[i].node, FALSE);
		cdsl_rbtreeInsert(&nroot, &npeople[i].node, FALSE);
	}


	if(cdsl_rbtreeCompare(&nroot, &aroot) != 0) {
		return FALSE;
	}

	cdsl_rbtreeDeleteMin(&aroot);
	if(cdsl_rbtreeCompare(&nroot, &aroot) == 0) {
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



static BOOL is_match(base_treeNode_t* node, trkey_t key) {
	return node->key == key;
}

