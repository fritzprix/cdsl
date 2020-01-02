/*
 * main.c
 *
 *  Created on: 2015. 5. 10.
 *      Author: innocentevil
 */

#include <stdio.h>
#include <stdlib.h>
#include "autogen.h"
#if TARGET_VAL == 0
#else
	#include "cdsl_heap_test.h"
	#include "cdsl_list_test.h"
	#include "cdsl_slist_test.h"
	#include "cdsl_bstree_test.h"
	#include "cdsl_spltree_test.h"
	#include "cdsl_hashtree_test.h"
	#include "cdsl_avltree_test.h"
	#include "rbtree_benchmark.h"
	#include "avltree_benchmark.h"
	#include "cdsl_rbtree_test.h"
#endif


const char *RESULT_STRING[] = {
	"FAIL",
	"PASS"};

int main(void)
{
#if TARGET_VAL == 0
#else 
	setbuf(stdout, NULL);
	BOOL result;
	printf("Heap Test Result : %s\n", RESULT_STRING[result = cdsl_heapDoTest()]);
	if (result == FALSE)
	{
		exit(-1);
	}

	printf("Binary Search Tree Test Result : %s\n", RESULT_STRING[result = cdsl_bstreeDoTest()]);
	if (result == FALSE)
	{
		exit(-1);
	}
	printf("Splay tree Test Result : %s\n", RESULT_STRING[result = cdsl_spltreeDoTest()]);
	if (result == FALSE)
	{
		exit(-1);
	}
	printf("Singly-Linked List Test Result : %s\n", RESULT_STRING[result = cdsl_slistDoTest()]);
	if (result == FALSE)
	{
		exit(-1);
	}

	printf("Doubly-Linked List Test Result : %s\n", RESULT_STRING[result = cdsl_listDoTest()]);
	if (result == FALSE)
	{
		exit(-1);
	}
	printf("Red Black Tree Test Result : %s\n", RESULT_STRING[result = cdsl_rbtreeDoTest()]);
	if (result == FALSE)
	{
		exit(-1);
	}
	printf("Hash Tree Test Result : %s\n", RESULT_STRING[result = cdsl_hashtreeDoTest()]);
	if (result == FALSE)
	{
		exit(-1);
	}
	printf("AVL Tree Test Result : %s\n", RESULT_STRING[result = cdsl_avltreeDoTest()]);
	if (result == FALSE)
	{
		exit(-1);
	}

	perform_avltree_benchmark();
	perform_rbtree_benchmark();
#endif
	return 0;
}
