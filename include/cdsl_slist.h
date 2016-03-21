/*
 * cdsl_slist.h
 *	\brief singly linked list library
 *  Created on: 2015. 5. 31.
 *      Author: innocentevil
 */

#ifndef CDSL_SLIST_H_
#define CDSL_SLIST_H_

#include "cdsl.h"

#if defined(__cplusplus)
extern "C" {
#endif


#define cdsl_slistIsEmpty(node) (((cdsl_slistNode_t*) node)->next == NULL)

typedef struct cdsl_slnode slistNode_t;
typedef struct cdsl_slentry slistEntry_t;


struct cdsl_slnode {
	slistNode_t* next;
};

struct cdsl_slentry {
	slistNode_t* next;
};

extern void cdsl_slistEntryInit(slistEntry_t* entry);
extern void cdsl_slistNodeInit(slistNode_t* node);


extern void cdsl_slistEnqueuePriority(slistEntry_t* lentry,slistNode_t* item,cdsl_generic_compare_t rule);
extern void cdsl_slistInsertAfter(slistNode_t* ahead,slistNode_t* item);
extern slistNode_t* cdsl_slistDequeue(slistEntry_t* lentry);

extern int cdsl_slistPutHead(slistEntry_t* lentry,slistNode_t* item);
extern int cdsl_slistPutTail(slistEntry_t* lentry,slistNode_t* item);
extern slistNode_t* cdsl_slistRemoveHead(slistEntry_t* lentry);
extern slistNode_t* cdsl_slistRemoveTail(slistEntry_t* lentry);
extern slistNode_t* cdsl_slistRemoveAt(slistEntry_t* entry,int idx);
extern BOOL cdsl_slistRemove(slistEntry_t* lentry,slistNode_t* item);


extern int cdsl_slistSize(slistEntry_t* lentry);
extern BOOL cdsl_slistContain(slistEntry_t* lentry,slistNode_t* item);
extern void cdsl_slistPrint(slistEntry_t* lentry,cdsl_generic_printer_t prt);

#if defined(__cplusplus)
}
#endif


#endif /* CDSL_SLIST_H_ */
