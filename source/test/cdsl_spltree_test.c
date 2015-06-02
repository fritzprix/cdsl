/*
 * cdsl_spltree_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */


#include "cdsl.h"
#include "cdsl_spltree.h"
#include "cdsl_spltree_test.h"
#include <stdio.h>

static spltreeNode_t nodes[TEST_SIZE];
static int keys[TEST_SIZE];
static spltreeNode_t* root;

BOOL cdsl_spltreeDoTest(void){
	int i = 0;
	root = NULL;
	for(;i < 100;i++){
		keys[i] = rand() % 100;
		log("Inserted : %d\n",keys[i]);
		cdsl_spltreeInitNode(&nodes[i],keys[i]);
		cdsl_spltreeInsert(&root,&nodes[i]);
		cdsl_spltreePrint(&root);
	}
	return TRUE;
}
