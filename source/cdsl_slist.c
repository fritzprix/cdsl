/*
 * cdsl_slist.c
 *
 *  Created on: 2015. 5. 31.
 *      Author: innocentevil
 */


#include "cdsl.h"
#include "cdsl_slist.h"


void cdsl_slistInit(slistNode_t* lentry){
	lentry->next = NULL;
}

void cdsl_slistEnqueuePriority(slistNode_t* lentry,slistNode_t* item,cdsl_slistPriorityRule rule){
	slistNode_t** cnode = &lentry;
	while((*cnode)->next != NULL){
		if(rule((*cnode)->next,item) == item){
			item->next = (*cnode)->next;
			(*cnode)->next = item;
			return;
		}
		cnode = &(*cnode)->next;
	}
	(*cnode)->next = item;
}

void cdsl_slistInsertAfter(slistNode_t* ahead,slistNode_t* item){
	if(!ahead || !item)
		return;
	item->next = ahead->next;
	ahead->next = item;
}

slistNode_t* cdsl_slistDequeue(slistNode_t* lentry){
	if(!lentry)
		return NULL;
	slistNode_t* head = lentry->next;
	if(head)
		lentry->next = head->next;
	return head;
}


void cdsl_slistPutHead(slistNode_t* lentry,slistNode_t* item){
	if(!lentry || !item)
		return;
	if(lentry->next)
		item->next = lentry->next->next;
	else
		item->next = NULL;
	lentry->next = item;
}

void cdsl_slistPutTail(slistNode_t* lentry,slistNode_t* item){
	if(!lentry || !item)
		return;
	item->next = NULL;
	slistNode_t* cnode = lentry;
	while(cnode->next != NULL){
		cnode = cnode->next;
	}
	cnode->next = item;
}

slistNode_t* cdsl_slistGetHead(slistNode_t* lentry){
	return cdsl_slistDequeue(lentry);
}

slistNode_t* cdsl_slistGetTail(slistNode_t* lentry){
	if(!lentry)
		return NULL;
	slistNode_t* tail = NULL;
	slistNode_t** cnode = &lentry;
	while((*cnode)->next != NULL){
		cnode = &(*cnode)->next;
	}
	tail = (*cnode);
	*cnode = NULL;
	return tail;
}

BOOL cdsl_slistRemove(slistNode_t* lentry,slistNode_t* item){
	if(!lentry || !item)
		return FALSE;
	slistNode_t* cnode = lentry;
	while(cnode->next != NULL){
		if(cnode->next == item){
			cnode->next = item->next;
			return TRUE;
		}
		cnode = cnode->next;
	}
	return FALSE;
}

int cdsl_slistSize(slistNode_t* lentry){
	if(!lentry)
		return FALSE;
	int cnt = 0;
	slistNode_t* cnode = lentry;
	while(cnode->next){
		cnode = cnode->next;
		cnt++;
	}
	return cnt;
}

BOOL cdsl_slistContain(slistNode_t* lentry,slistNode_t* item){
	if(!lentry)
		return FALSE;
	slistNode_t* cnode = lentry;
	while(cnode->next){
		if(cnode->next == item)
			return TRUE;
		cnode = cnode->next;
	}
	return FALSE;
}

void cdsl_slistPrint(slistNode_t* lentry,cdsl_generic_printer_t prt){
	if(!lentry || !prt)
		return;
	slistNode_t* cnode = lentry;
	while(cnode->next){
		cnode = cnode->next;
		prt(cnode->next);
	}
}
