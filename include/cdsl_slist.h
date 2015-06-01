/*
 * cdsl_slist.h
 *	\brief singly linked list library
 *  Created on: 2015. 5. 31.
 *      Author: innocentevil
 */

#ifndef CDSL_SLIST_H_
#define CDSL_SLIST_H_

#include "cdsl.h"



#ifndef DECLARE_COMPARE_FN
#define DECLARE_COMPARE_FN(fn) void* fn(void* a,void* b)
#endif

#define cdsl_slistIsEmpty(node) (((cdsl_slistNode_t*) node)->next == NULL)

typedef struct cdsl_slnode slistNode_t;
typedef void* (*cdsl_slistPriorityRule)(void* a, void* b);
struct cdsl_slnode {
	slistNode_t* next;
};

extern void cdsl_slistInit(slistNode_t* lentry);


extern void cdsl_slistEnqueuePriority(slistNode_t* lentry,slistNode_t* item,cdsl_slistPriorityRule rule);
extern void cdsl_slistInsertAfter(slistNode_t* ahead,slistNode_t* item);
extern slistNode_t* cdsl_slistDequeue(slistNode_t* lentry);

extern void cdsl_slistPutHead(slistNode_t* lentry,slistNode_t* item);
extern void cdsl_slistPutTail(slistNode_t* lentry,slistNode_t* item);
extern slistNode_t* cdsl_slistGetHead(slistNode_t* lentry);
extern slistNode_t* cdsl_slistGetTail(slistNode_t* lentry);

extern BOOL cdsl_slistRemove(slistNode_t* lentry,slistNode_t* item);
extern int cdsl_slistSize(slistNode_t* lentry);
extern BOOL cdsl_slistContain(slistNode_t* lentry,slistNode_t* item);
extern void cdsl_slistPrint(slistNode_t* lentry,cdsl_generic_printer_t prt);



#endif /* CDSL_SLIST_H_ */
