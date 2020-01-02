/*
 * cdsl_slist.h
 *	\brief singly linked list library
 *  Created on: 2015. 5. 31.
 *      Author: innocentevil
 */

#ifndef CDSL_SLIST_H_
#define CDSL_SLIST_H_

#include "base_list.h"
#include "sort.h"

#if defined(__cplusplus)
extern "C" {
#endif


#define cdsl_slistIsEmpty(node)                          (((slistEntry_t*) node)->head == NULL)

#define cdsl_slistSize(lentry)                           cdsl_listSize((listEntry_t*) lentry)
#define cdsl_slistContain(lentry, item)                  cdsl_listContain((listEntry_t*) lentry, (listNode_t*) item)
#define cdsl_slistPrint(lentry,print)                    cdsl_listPrint((listEntry_t*) lentry, print)

#define cdsl_slistIterInit(lentry, iter)                 cdsl_iterInit((listEntry_t*) lentry, iter)
#define cdsl_slistIterHasNext(iter)                      cdsl_iterHasNext(iter)
#define cdsl_slistIterNext(iter)                         cdsl_iterNext(iter)
#define cdsl_slistSort(lentry, order, compare)           cdsl_listMergeSort((listEntry_t*) lentry, order, compare, FALSE)


typedef struct cdsl_slnode slistNode_t;
typedef struct cdsl_slentry slistEntry_t;


struct cdsl_slnode {
	slistNode_t* next;
};

struct cdsl_slentry {
	union {
		slistNode_t* head;     // head
		listEntry_t  __base;
	};
};

extern void cdsl_slistEntryInit(slistEntry_t* entry);
extern void cdsl_slistNodeInit(slistNode_t* node);

extern void cdsl_slistMerge(slistEntry_t* to, slistEntry_t* from);
extern void cdsl_slistEnqueuePriority(slistEntry_t* lentry,slistNode_t* item,cdsl_generic_compare_t rule);
extern void cdsl_slistInsertAfter(slistNode_t* ahead,slistNode_t* item);
extern slistNode_t* cdsl_slistDequeue(slistEntry_t* lentry);

extern int cdsl_slistPutHead(slistEntry_t* lentry,slistNode_t* item);
extern int cdsl_slistPutTail(slistEntry_t* lentry,slistNode_t* item);
extern slistNode_t* cdsl_slistRemoveHead(slistEntry_t* lentry);
extern slistNode_t* cdsl_slistRemoveTail(slistEntry_t* lentry);
extern slistNode_t* cdsl_slistRemoveAt(slistEntry_t* entry,int pos);
extern BOOL cdsl_slistRemove(slistEntry_t* lentry,slistNode_t* item);
extern void cdsl_slistIterRemove(listIter_t* iter);


#if defined(__cplusplus)
}
#endif


#endif /* CDSL_SLIST_H_ */
