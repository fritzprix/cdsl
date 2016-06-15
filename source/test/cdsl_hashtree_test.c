/*
 * cdsl_hashtree_test.c
 *
 *  Created on: Apr 8, 2016
 *      Author: innocentevil
 */

#include "cdsl_hashtree.h"
#include "cdsl_hashtree_test.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef struct {
	hashNode_t     _base;
	char name[20];
	int age;
} person_t;

#define TEST_CNT 1000
static person_t people[TEST_CNT];

BOOL cdsl_hashtreeDoTest(void) {

	person_t* p;
	hashRoot_t root;
	hashNode_t node;

	cdsl_hashtreeRootInit(&root);

	FILE* fp = fopen("hash_inputsample.txt","r");

	int cnt;
	for(cnt = 0;cnt < TEST_CNT; cnt++) {
		fscanf(fp, "%s",people[cnt].name);
		cdsl_hashtreeNodeInit(&people[cnt]._base, people[cnt].name);
		cdsl_hashtreeInsert(&root, &people[cnt]._base);
	}
	if(cdsl_hashtreeSize(&root) != TEST_CNT)
		goto FAIL;
	__dev_log("insertion complete!! \n");

	fseek(fp, 0, SEEK_SET);
	char key_buf[20];
	for(cnt = 0;cnt < TEST_CNT; cnt++) {
		fscanf(fp, "%s",key_buf);
		p = (person_t*) cdsl_hashtreeLookup(&root, key_buf);
		if(!p)
			goto FAIL;
		p = container_of(p, person_t, _base);
		if(strcmp(p->name, key_buf) != EXIT_SUCCESS)
			goto FAIL;
	}

	__dev_log("lookup complete!! \n");

	fseek(fp,0, SEEK_SET);
	for(cnt = 0; cnt < TEST_CNT;cnt++) {
		fscanf(fp, "%s", key_buf);
		p = (person_t*) cdsl_hashtreeRemove(&root, key_buf);
		if(!p)
			goto FAIL;
		p = container_of(p, person_t, _base);
		if(strcmp(p->name, key_buf) != EXIT_SUCCESS)
			goto FAIL;
	}

	__dev_log("delete complete!! \n");

	if(cdsl_hashtreeSize(&root) != 0)
		goto FAIL;

	fclose(fp);
	return TRUE;

FAIL:
	fclose(fp);
	return FALSE;

}
