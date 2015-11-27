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

static void print_heapnode(void*);

BOOL cdsl_heapDoTest(void)
{
	heapRoot_t root;
	heapNode_t nodes[100];
	heap_root_init(&root,MAX_HEAP);
	int i;
	int max = 0,min = 0;
	for(i = 0; i < 100;i++)
	{
		heap_node_init(&nodes[i], rand());
		heap_enqueue(&root, &nodes[i]);
		if(heap_size(&root) != (i + 1))
			return FALSE;
	}

	heapNode_t* node = NULL;
	while((node = heap_deqeue(&root)) != NULL)
	{
		if(max < node->key && max != 0)
			return FALSE;
		else
			max = node->key;
	}
	heap_root_init(&root,MIN_HEAP);
	for (i = 0;i < 10;i++)
	{
		heap_node_init(&nodes[i],rand());
		heap_enqueue(&root, &nodes[i]);
		if(heap_size(&root) != (i + 1))
			return FALSE;
	}
	while((node = heap_deqeue(&root)) != NULL)
	{
		if(min > node->key && min != 0)
			return FALSE;
		else
			min = node->key;
	}
	return TRUE;
}


static void print_heapnode(void* node)
{
	printf("%d\n", ((heapNode_t*)node)->key );
}
