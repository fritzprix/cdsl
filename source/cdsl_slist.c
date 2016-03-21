/*
 * cdsl_slist.c
 *
 *  Created on: 2015. 5. 31.
 *      Author: innocentevil
 */


#include "cdsl.h"
#include "cdsl_slist.h"


void cdsl_slistEntryInit(slistEntry_t* entry)
{
	if(!entry)
		return;
	entry->next = NULL;
}


void cdsl_slistNodeInit(slistNode_t* item)
{
	if(!item)
		return;
	item->next = NULL;
}


void cdsl_slistEnqueuePriority(slistEntry_t* lentry,slistNode_t* item,cdsl_generic_compare_t rule){
	if(!lentry || !item || !rule)
		return;
	slistNode_t** cnode = (slistNode_t**) &lentry;
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

slistNode_t* cdsl_slistDequeue(slistEntry_t* lentry){
	if(!lentry)
		return NULL;
	slistNode_t* head = lentry->next;
	if(head)
		lentry->next = head->next;
	return head;
}


int cdsl_slistPutHead(slistEntry_t* lentry,slistNode_t* item){
	if(!lentry || !item)
		return -1;
	if(lentry->next)
		item->next = lentry->next->next;
	else
		item->next = NULL;
	lentry->next = item;
	return 0;
}

int cdsl_slistPutTail(slistEntry_t* lentry,slistNode_t* item){
	if(!lentry || !item)
		return -1;
	int idx = 0;
	item->next = NULL;
	slistNode_t* cnode = (slistNode_t*)lentry;
	while(cnode->next != NULL){
		cnode = cnode->next;
		idx++;
	}
	cnode->next = item;
	return idx;
}

slistNode_t* cdsl_slistRemoveHead(slistEntry_t* lentry){
	return cdsl_slistDequeue(lentry);
}

slistNode_t* cdsl_slistRemoveTail(slistEntry_t* lentry){
	if(!lentry)
		return NULL;
	if(!lentry->next)
		return NULL;
	slistNode_t* tail = NULL;
	slistNode_t** cnode = (slistNode_t**)&lentry;
	while((*cnode)->next != NULL){
		cnode = &(*cnode)->next;
	}
	tail = (*cnode);
	*cnode = NULL;
	return tail;
}

BOOL cdsl_slistRemove(slistEntry_t* lentry,slistNode_t* item){
	if(!lentry || !item)
		return FALSE;
	if(!lentry->next)
		return FALSE;
	slistNode_t* cnode = (slistNode_t*) lentry;
	while(cnode->next != NULL){
		if(cnode->next == item){
			cnode->next = item->next;
			return TRUE;
		}
		cnode = cnode->next;
	}
	return FALSE;
}

slistNode_t* cdsl_slistRemoveAt(slistEntry_t* entry, int idx)
{
	if(!entry)
		return NULL;
	if(!entry->next)
		return NULL;
	slistNode_t* cnode = (slistNode_t*)entry;
	slistNode_t* found = cnode;
	while(idx)
	{
		cnode = cnode->next;
	}
	found = cnode->next;
	cnode->next = found->next;
	return found;
}


int cdsl_slistSize(slistEntry_t* lentry){
	if(!lentry)
		return 0;
	if(!lentry->next)
		return 0;
	int cnt = 0;
	slistNode_t* cnode = (slistNode_t*) lentry;
	while(cnode->next){
		cnode = cnode->next;
		cnt++;
	}
	return cnt;
}

BOOL cdsl_slistContain(slistEntry_t* lentry,slistNode_t* item){
	if(!lentry)
		return FALSE;
	if(!lentry->next)
		return FALSE;
	slistNode_t* cnode =  (slistNode_t*) lentry;
	while(cnode->next){
		if(cnode->next == item)
			return TRUE;
		cnode = cnode->next;
	}
	return FALSE;
}

void cdsl_slistPrint(slistEntry_t* lentry,cdsl_generic_printer_t prt){
	if(!lentry || !prt)
		return;
	slistNode_t* cnode = (slistNode_t*) lentry;
	while(cnode->next){
		cnode = cnode->next;
		prt(cnode->next);
	}
}
