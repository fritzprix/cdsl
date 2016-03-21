/*
 * cdsl_hash_test.c
 *
 *  Created on: 2015. 8. 29.
 *      Author: innocentevil
 */

#include "cdsl_rbtree.h"
#include "cdsl_hash_test.h"
#include "cdsl_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#ifndef HASH_TEST_FILE
#define HASH_TEST_FILE			"hash_inputsample.txt"
#endif

static rb_treeNode_t* root;
struct hash_result {
	rb_treeNode_t	rbnode;
	unsigned long hash;
	char input[20];
};

extern BOOL cdsl_hashDoTest(void){
	FILE* fp = fopen(HASH_TEST_FILE,"r");

	root = NULL;
	struct hash_result* result;
	int collisionCnt = 0;
	while(!feof(fp)){
		result =  (struct hash_result*) malloc(sizeof(struct hash_result));
		if(fscanf(fp,"%s\n",result->input))
			exit(-1);
		result->hash = sbdm_hash(result->input);
		cdsl_rbtreeNodeInit(&result->rbnode,result->hash);
		cdsl_rbtreeInsert(&root,&result->rbnode);
	}





	return FALSE;
}
