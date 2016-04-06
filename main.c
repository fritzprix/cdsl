/*
 * main.c
 *
 *  Created on: 2015. 5. 10.
 *      Author: innocentevil
 */


#include <stdio.h>
#include <stdlib.h>
#include "cdsl_nrbtree_test.h"
#include "cdsl_heap_test.h"
#include "cdsl_list_test.h"
#include "cdsl_slist_test.h"
#include "cdsl_bstree_test.h"
#include "cdsl_spltree_test.h"
#include "cdsl_rbtree_test.h"
#include "cdsl_hash_test.h"

const char* RESULT_STRING[] = {
		"FAIL",
		"PASS"
};

int main(void){
	setbuf(stdout,NULL);
	BOOL result;
	printf("Heap Test Result : %s\n",RESULT_STRING[result = cdsl_heapDoTest()]);
	if(result == FALSE)
	{
		exit(-1);
	}

	printf("Binary Search Tree Test Result : %s\n",RESULT_STRING[result = cdsl_bstreeDoTest()]);
	if(result == FALSE)
	{
		exit(-1);
	}
	printf("Splay tree Test Result : %s\n",RESULT_STRING[result = cdsl_spltreeDoTest()]);
	if(result == FALSE)
	{
		exit(-1);
	}
	printf("Singly-Linked List Test Result : %s\n",RESULT_STRING[result = cdsl_slistDoTest()]);
	if(result == FALSE)
	{
		exit(-1);
	}

	printf("Doubly-Linked List Test Result : %s\n",RESULT_STRING[result = cdsl_listDoTest()]);
	if(result == FALSE)
	{
		exit(-1);
	}
	printf("Red Black Tree Test Result : %s\n",RESULT_STRING[result = cdsl_nrbtreeDoTest()]);
	if(result == FALSE)
	{
		exit(-1);
	}

	return 0;
}

