/*
 * main.c
 *
 *  Created on: 2015. 5. 10.
 *      Author: innocentevil
 */


#include <stdio.h>
#include "cdsl_heap_test.h"
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
	printf("Heap Test Result : %s\n",RESULT_STRING[cdsl_heapDoTest()]);
	printf("Binary Search Tree Test Result : %s\n",RESULT_STRING[cdsl_bstreeDoTest()]);
	printf("Splay tree Test Result : %s\n",RESULT_STRING[cdsl_spltreeDoTest()]);
	printf("Red Black Tree Test Result : %s\n",RESULT_STRING[cdsl_rbtreeDoTest()]);
	printf("Singly-Linked List Test Result : %s\n",RESULT_STRING[cdsl_slistDoTest()]);
	return 0;
}

