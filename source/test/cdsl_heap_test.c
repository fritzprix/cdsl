/*
 * cdsl_heap_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */

#include <stdio.h>
#include "cdsl.h"
#include "cdsl_heap.h"
#include "cdsl_heap_test.h"
#include <limits.h>
#include <stdlib.h>


BOOL cdsl_heapDoTest(void)
{
	heapRoot_t root;
	heapNode_t nodes[100];
	cdsl_heapRootInit(&root,MAX_HEAP);
	int i;
	int max = 0,min = 0;
	for(i = 0; i < 100;i++)
	{
		cdsl_heapNodeInit(&nodes[i], rand());
		cdsl_heapEnqueue(&root, &nodes[i]);
		if(cdsl_heapSize(&root) != (i + 1))
			return FALSE;
	}

	heapNode_t* node = NULL;
	while((node = cdsl_heapDeqeue(&root)) != NULL)
	{
		if(max < node->key && max != 0)
			return FALSE;
		else
			max = node->key;
	}
	cdsl_heapRootInit(&root,MIN_HEAP);
	for (i = 0;i < 10;i++)
	{
		cdsl_heapNodeInit(&nodes[i],rand());
		cdsl_heapEnqueue(&root, &nodes[i]);
		if(cdsl_heapSize(&root) != (i + 1))
			return FALSE;
	}
	while((node = cdsl_heapDeqeue(&root)) != NULL)
	{
		if(min > node->key && min != 0)
			return FALSE;
		else
			min = node->key;
	}
	return TRUE;
}


