/*
 * base_list.h
 *
 *  Created on: Mar 23, 2016
 *      Author: innocentevil
 */

#ifndef INCLUDE_TEST_BASE_LIST_H_
#define INCLUDE_TEST_BASE_LIST_H_

#include "cdsl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct list_node listNode_t;
struct list_node {
	listNode_t* next;
};

typedef struct {
	listNode_t* head;
}listEntry_t;

typedef struct {
	listEntry_t* entry;
	listNode_t* prev;
} cdsl_iterator_t;

extern void cdsl_iterInit(listEntry_t* lentry, cdsl_iterator_t* iter);
extern BOOL cdsl_iterHasNext(cdsl_iterator_t* iter);
extern listNode_t* cdsl_iterNext(cdsl_iterator_t* iter);


extern int cdsl_listSize(listEntry_t* lentry);
extern BOOL cdsl_listContain(listEntry_t* lentry,listNode_t* item);
extern void cdsl_listPrint(listEntry_t* lentry,cdsl_generic_printer_t prt);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_TEST_BASE_LIST_H_ */
