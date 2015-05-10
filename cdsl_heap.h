/*
 * heap.h
 *
 *  Created on: 2015. 5. 3.
 *      Author: innocentevil
 */

#ifndef HEAP_H_
#define HEAP_H_

#include <stdint-gcc.h>
typedef struct heap_node cdsl_heapNode_t;
typedef cdsl_heapNode_t* (*heapEvaluate)(cdsl_heapNode_t* a, cdsl_heapNode_t* b);
typedef void (*heapPrint)(cdsl_heapNode_t* item);

struct heap_node{
	cdsl_heapNode_t*	right;
	cdsl_heapNode_t*	left;
	uint8_t 	flipper;
};

extern int cdsl_heapEnqueue(cdsl_heapNode_t** heap,cdsl_heapNode_t* item,heapEvaluate eval);
extern cdsl_heapNode_t* cdsl_heapDeqeue(cdsl_heapNode_t** heap,heapEvaluate eval);
extern void cdsl_heapPrint(cdsl_heapNode_t** heap,heapPrint prt);

#endif /* HEAP_H_ */
