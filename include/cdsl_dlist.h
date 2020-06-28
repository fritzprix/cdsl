/*
 * tch_dlist.h
 *
 * \brief doubly linked list library (can be used as stack or queue)
 *
 *
 *  Created on: 2014. 7. 25.
 *      Author: innocentevil
 */

#ifndef CDSL_DLIST_H_
#define CDSL_DLIST_H_

#include "base_list.h"
#include "sort.h"

#if defined(__cplusplus)
extern "C" {
#endif


#define cdsl_dlistIsEmpty(lhead)                      (((dlistEntry_t*) lhead)->head== NULL)

#define cdsl_dlistSize(lentry)                        cdsl_listSize((listEntry_t*) lentry)
#define cdsl_dlistContain(lentry, item)               cdsl_listContain((listEntry_t*) lentry, (listNode_t*) item)
#define cdsl_dlistPrint(lentry,print)                 cdsl_listPrint((listEntry_t*) lentry, print)


#define cdsl_dlistIterInit(lentry, iter)              cdsl_iterInit((listEntry_t*) lentry, iter)
#define cdsl_dlistIterHasNext(iter)                   cdsl_iterHasNext(iter)
#define cdsl_dlistIterNext(iter)                      cdsl_iterNext(iter)

#define cdsl_dlistSort(lentry, order, compare)        cdsl_listMergeSort((listEntry_t*) lentry, order, compare, TRUE)
#define cdsl_dlistGetHead(lentry)                     cdsl_listGetHead((listEntry_t*) lentry)

typedef struct cdsl_dlnode dlistNode_t;
typedef struct cdsl_dlentry dlistEntry_t;

struct cdsl_dlnode {
	union {
		listNode_t	 __base;
		dlistNode_t* next;
	};
	dlistNode_t* prev;
};

struct cdsl_dlentry {
	union {
		listEntry_t  base_entry;
		dlistNode_t* head;
	};
	dlistNode_t* tail;
};

extern void cdsl_dlistEntryInit(dlistEntry_t* lentry);
extern void cdsl_dlistNodeInit(dlistNode_t* node);

extern void cdsl_dlistEnqueuePriority(dlistEntry_t* lentry,dlistNode_t* item,cdsl_generic_compare_t rule);
extern void cdsl_dlistInsertAfter(dlistNode_t* ahead,dlistNode_t* item);
extern dlistNode_t* cdsl_dlistDequeue(dlistEntry_t* lentry);

extern void cdsl_dlistPutHead(dlistEntry_t* lentry,dlistNode_t* item);
extern void cdsl_dlistPutTail(dlistEntry_t* lentry,dlistNode_t* item);
extern dlistNode_t* cdsl_dlistRemoveHead(dlistEntry_t* lentry);
extern dlistNode_t* cdsl_dlistRemoveTail(dlistEntry_t* lentry);
extern BOOL cdsl_dlistRemove(dlistNode_t* item);
extern void cdsl_dlistReplace(dlistNode_t* old, dlistNode_t* nu);
extern void cdsl_dlistIterRemove(listIter_t* iter);

extern dlistNode_t* cdsl_dlistGetLast(dlistEntry_t* lentry);


#if defined(__cplusplus)
}
#endif


#endif /* TCH_LIST_H_ */
