/*
 * cdsl_bstree_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */

#include "cdsl_bstree.h"
#include "cdsl_bstree_test.h"
#include "cdsl_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../include/serialization/fserializer.h"



static bstreeNode_t bst_nodepool[TEST_SIZE];
static bstreeNode_t replace;
static trkey_t repl_key;
static trkey_t keys[TEST_SIZE];

static int cb_count;
static DECLARE_FOREACH_CALLBACK(onTreeNode);

static DECLARE_SERIALIZER_ON_ERROR(onSerError);
static DECLARE_SERIALIZER_ON_NODE(onSerNode);
static DECLARE_SERIALIZER_ON_COMPLETE(onSerDone);

static cdsl_serializerUsrCallback_t callback = {
		.on_complete = onSerDone,
		.on_node_to_data = onSerNode,
		.on_error = onSerError
};

#define SER_SIZE     5

typedef struct person {
	bstreeNode_t node;
	char         name[20];
}person_t;

const char* names[SER_SIZE] = {
		"David",
		"John",
		"Helen",
		"Joseph",
		"Arthur"
};

static person_t people[SER_SIZE];



BOOL cdsl_bstreeDoTest(void)
{
	bstreeRoot_t root;
	bstreeRoot_t aroot;
	cb_count = 0;
	cdsl_bstreeRootInit(&root);
	cdsl_bstreeRootInit(&aroot);

	int i = 0;
	int depth,depth_temp;
	depth = 0;
	depth_temp = 0;

	keys[TEST_SIZE - 1] = repl_key =TEST_SIZE + 10;
	cdsl_bstreeNodeInit(&replace, keys[TEST_SIZE - 1]);

	for(i = 0;i < TEST_SIZE;i++)
	{
		if(i != (TEST_SIZE - 1))
			keys[i] = rand() % TEST_SIZE;
		cdsl_bstreeNodeInit(&bst_nodepool[i],keys[i]);
		cdsl_bstreeInsert(&root,&bst_nodepool[i]);
	}
	cdsl_bstreeUpdate(&root, &replace);
	if(cdsl_bstreeLookup(&root,repl_key) != &replace)
		return FALSE;

	depth = cdsl_bstreeMaxDepth(&root);
	__dev_log("Max Depth of binary search Tree : %d @ N : %d\n",depth,i);

	if(cdsl_bstreeSize(&root) != TEST_SIZE)
		return FALSE;

	bstreeNode_t* delete_node;
	for(i = 0;i < TEST_SIZE;i++)
	{
		delete_node = NULL;
		delete_node = cdsl_bstreeDelete(&root,keys[i]);
		if(!delete_node)
			return FALSE;
		if(delete_node->key != keys[i])
			return FALSE;
		keys[i] = rand() % TEST_SIZE;
		cdsl_bstreeNodeInit(delete_node,keys[i]);
		cdsl_bstreeInsert(&root,delete_node);
	}

	trkey_t key = 0;
	for(i = 0;i < TEST_SIZE;i++)
	{
		delete_node = NULL;
		delete_node = cdsl_bstreeDeleteMin(&root);
		if(!delete_node)
			return FALSE;
		if(key > delete_node->key)
			return FALSE;
		key = delete_node->key;
		cdsl_bstreeNodeInit(delete_node,key);
		cdsl_bstreeInsert(&aroot, delete_node);
	}

	if(cdsl_bstreeSize(&root) > 0)		// size should be zero
		return FALSE;
	if(cdsl_bstreeSize(&aroot) != TEST_SIZE)
		return FALSE;

	int traverse_count = 0;
	cdsl_bstreeForEach(&aroot, onTreeNode,ORDER_INC, &traverse_count);
	if(traverse_count != TEST_SIZE)
		return FALSE;

	key = 0xffffffff;
	for(i = 0;i < TEST_SIZE;i++)
	{
		delete_node = NULL;
		delete_node = cdsl_bstreeDeleteMax(&aroot);
		if(!delete_node)
			return FALSE;
		if(key < delete_node->key)
			return FALSE;
		key = delete_node->key;
	}
	if(cdsl_bstreeTop(&aroot) != NULL)
		return FALSE;


	for(i = 0;i < SER_SIZE;i++) {
		cdsl_bstreeNodeInit(&people[i].node, sbdm_hash((unsigned char*) names[i]));
		strcpy(people[i].name, names[i]);
		cdsl_bstreeInsert(&aroot, &people[i].node);
	}

	if(cdsl_bstreeSize(&aroot) != SER_SIZE) {
		return FALSE;
	}
	file_serializer_t fser;
	file_serializerOpen(&fser, "serialize_tree.bin");
	cdsl_bstreeSerialize(&aroot, &fser, &callback);
	file_serializerClose(&fser);

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


static DECLARE_FOREACH_CALLBACK(onTreeNode) {
	int* cnt = (int*) arg;
	if(!node) {
		*cnt = 0;
	}
	(*cnt)++;
	return FOREACH_CONTINUE;
}
