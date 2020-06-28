/*
 * base_list.h
 *
 *  Created on: Mar 23, 2016
 *      Author: innocentevil
 */

#ifndef INCLUDE_TEST_BASE_LIST_H_
#define INCLUDE_TEST_BASE_LIST_H_

#include "cdsl_defs.h"

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
} listIter_t;

extern void cdsl_iterInit(listEntry_t* lentry, listIter_t* iter);
extern BOOL cdsl_iterHasNext(listIter_t* iter);
extern listNode_t* cdsl_iterNext(listIter_t* iter);
extern listNode_t* cdsl_iterRemove(listIter_t* iter);


extern int cdsl_listSize(listEntry_t* lentry);
extern BOOL cdsl_listContain(listEntry_t* lentry,listNode_t* item);
extern void cdsl_listPrint(listEntry_t* lentry,cdsl_generic_printer_t prt);

extern listNode_t* cdsl_listGetHead(listEntry_t* lentry);
extern listNode_t* cdsl_listGetLast(listEntry_t* lentry);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_TEST_BASE_LIST_H_ */
