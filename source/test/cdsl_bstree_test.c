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
#include <stdlib.h>
#include <pthread.h>

struct compete_req {
	bstreeRoot_t* root;
	trkey_t        base;
	pthread_mutex_t* lock;
};


static bstreeNode_t bst_nodepool[TEST_SIZE];
static bstreeNode_t replace;
static trkey_t repl_key;
static trkey_t keys[TEST_SIZE];
static pthread_t threads[100];

static int cb_count;
static void* compete_rbtree(void* com_req);


BOOL cdsl_bstreeDoTest(void)
{
	bstreeRoot_t root;
	cb_count = 0;
	cdsl_bstreeRootInit(&root);

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
		if(key > delete_node->key)
			return FALSE;
		key = delete_node->key;
		if(!delete_node)
			return FALSE;
	}

	if(cdsl_bstreeSize(&root) > 0)		// size should be zero
		return FALSE;


	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);
	int idx;
	struct compete_req* req = NULL;
	for(idx = 0;idx < 100; idx++) {
		req = malloc(sizeof(struct compete_req));
		req->base = idx * 100;
		req->root = &root;
		req->lock = &lock;
		pthread_create(&threads[idx], NULL, compete_rbtree, req);
	}

	for(idx = 0;idx < 100; idx++) {
		pthread_join(threads[idx], (void**) &req);
		free(req);
	}
	return TRUE;
}

static void* compete_rbtree(void* com_req) {
	struct compete_req* req = (struct compete_req*) com_req;
	printf("Start Job : %lu \n", req->base);
	trkey_t end = req->base + 100;
	trkey_t start = req->base;
	bstreeNode_t* node = NULL;
	for(; start < end; start++) {
		cdsl_bstreeNodeInit(&bst_nodepool[start],start);
		cdsl_bstreeInsert(req->root, &bst_nodepool[start]);
	}
	for(start = req->base; start < end; start++) {
		node = cdsl_bstreeDelete(req->root, start);
		if(!node) {
			fprintf(stderr, "Reentrant Test Delete Fail %lu / %lu\n",start,req->base);
			exit(-1);
		}
		if((node->key < req->base) || (node->key >= end)) {
			fprintf(stderr, "Reentrant Test Fail\n");
			exit(-1);
		}
	}
	return com_req;
}
