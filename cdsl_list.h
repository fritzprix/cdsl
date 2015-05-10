/*
 * tch_list.h
 *
 *  Created on: 2014. 7. 25.
 *      Author: innocentevil
 */

#ifndef TCH_LIST_H_
#define TCH_LIST_H_


#if defined(__cplusplus)
extern "C" {
#endif

#define INIT_LIST   {NULL,NULL}
#define DECLARE_COMPARE_FN(fn) void* fn(void* a,void* b)
#define cdsl_listIsEmpty(lhead)   (((cdsl_dlistNode_t*) lhead)->next == NULL)
typedef void* (*cdsl_listPriorityRule)(void* a,void* b);		// should return larger or prior one between two
typedef struct cdsl_dlnode cdsl_dlistNode_t;
 struct cdsl_dlnode {
	cdsl_dlistNode_t* prev;
	cdsl_dlistNode_t* next;
};

extern void cdsl_listInit(cdsl_dlistNode_t* lentry);
extern void cdsl_listEnqueuePriority(cdsl_dlistNode_t* lentry,cdsl_dlistNode_t* item,cdsl_listPriorityRule rule);
extern cdsl_dlistNode_t* cdsl_listDequeue(cdsl_dlistNode_t* lentry);
extern void cdsl_listPutHead(cdsl_dlistNode_t* lentry,cdsl_dlistNode_t* item);
extern void cdsl_listPutTail(cdsl_dlistNode_t* lentry,cdsl_dlistNode_t* item);
extern void cdsl_listInsertAfter(cdsl_dlistNode_t* ahead,cdsl_dlistNode_t* item);
extern cdsl_dlistNode_t* cdsl_listGetHead(cdsl_dlistNode_t* lentry);
extern cdsl_dlistNode_t* cdsl_listGetTail(cdsl_dlistNode_t* lentry);
extern int cdsl_listRemove(cdsl_dlistNode_t* item);
extern int cdsl_listSize(cdsl_dlistNode_t* lentry);
extern int cdsl_listContain(cdsl_dlistNode_t* lentry,cdsl_dlistNode_t* item);
extern void cdsl_listPrint(cdsl_dlistNode_t* lentry,void (*printitem)(void* item));

#if defined(__cplusplus)
}
#endif


#endif /* TCH_LIST_H_ */
