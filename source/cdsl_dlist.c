/*
 * tch_list.c
 *
 *  Created on: 2014. 7. 25.
 *      Author: innocentevil
 */

#include "cdsl_dlist.h"

void cdsl_dlistNodeInit(dlistNode_t* node){
	if(!node)
		return;
	node->next = NULL;
	node->prev = NULL;
}

void cdsl_dlistEntryInit(dlistEntry_t* lentry){
	if(!lentry)
		return;
	lentry->head = NULL;
	lentry->tail = NULL;
}



void cdsl_dlistEnqueuePriority(dlistEntry_t* lentry,dlistNode_t* item,cdsl_generic_compare_t  rule){
	dlistNode_t* cnode = (dlistNode_t*) lentry;
	if(!item)
		return;
	while(cnode->next != NULL){
		cnode = cnode->next;
		if(rule(cnode,item) == item){
			((dlistNode_t*)cnode->prev)->next = item;
			item->prev = cnode->prev;
			item->next = cnode;
			cnode->prev = item;
			return;
		}
	}
	cnode->next = item;
	item->prev = cnode;
	item->next = NULL;
	if(cnode != (dlistNode_t*) lentry)
		lentry->tail = item;
}

dlistNode_t* cdsl_dlistDequeue(dlistEntry_t* lentry){
	dlistNode_t* cnode = lentry->head;
	if(cdsl_dlistIsEmpty(lentry))
		return NULL;
	lentry->head = cnode->next;
	if(cnode->next)
		((dlistNode_t*)cnode->next)->prev = (dlistNode_t*) lentry;
	else
		lentry->head = NULL;
	return cnode;
}

void cdsl_dlistPutHead(dlistEntry_t* lentry,dlistNode_t* item){
	if(!lentry)
		return;
	item->next = lentry->head;
	if(lentry->head)
		((dlistNode_t*)lentry->head)->prev = item;
	else
		lentry->tail = item;
	item->prev = (dlistNode_t*) lentry;
	lentry->head = item;
}

void cdsl_dlistInsertAfter(dlistNode_t* ahead,dlistNode_t* item){
	if(!ahead || !item)
		return;

	item->next = ahead->next;
	item->prev = ahead;
	ahead->next = item;
	if(item->next)
		item->next->prev = item;
}



void cdsl_dlistPutTail(dlistEntry_t* lentry,dlistNode_t* item){
	if(!lentry || !item)
		return;
	if(lentry->head){
		((dlistNode_t*)lentry->tail)->next = item;
		item->prev = lentry->tail;
	} else{
		lentry->head = item;
		lentry->tail = item;
		item->prev = (dlistNode_t*) lentry;
	}
	item->next = NULL;
	lentry->tail = item;
}


dlistNode_t* cdsl_dlistRemoveHead(dlistEntry_t* lentry){
	if(!lentry)
		return NULL;
	if(!lentry->head)
		return NULL;
	dlistNode_t* head = lentry->head;
	if(head->next)
	{
		lentry->head = head->next;
		lentry->head->prev = (dlistNode_t*) lentry;
	}
	else
	{
		lentry->head = NULL;
		lentry->tail = NULL;
	}
	return head;
}

dlistNode_t* cdsl_dlistRemoveTail(dlistEntry_t* lentry){
	if(!lentry)
		return NULL;
	if(!lentry->tail)
		return NULL;
	dlistNode_t* tail = lentry->tail;
	if(tail->prev != (dlistNode_t*) lentry)
	{
		lentry->tail = tail->prev;
		lentry->tail->next = NULL;
	}
	else
	{
		lentry->tail = NULL;
		lentry->head = NULL;
	}
	return tail;
}

BOOL cdsl_dlistRemove(dlistNode_t* item){
	if(!item)
		return FALSE;
	dlistNode_t* prev = item->prev;
	if(prev)
		prev->next = item->next;
	if(item->next)
		item->next->prev = prev;
	return TRUE;
}

void cdsl_dlistReplace(dlistNode_t* old, dlistNode_t* nu)
{
	if(!old || !nu)
		return;
	nu->next = old->next;
	nu->prev = old->prev;
	if(nu->next)
		nu->next->prev = nu;
	if(nu->prev)
		nu->prev->next = nu;
	old->next = old->prev = NULL;
}

dlistNode_t* cdsl_dlistGetLast(dlistEntry_t* lentry)
{
	if(!lentry)
	{
		return NULL;
	}
	return lentry->tail;
}

void cdsl_dlistIterRemove(listIter_t* iter)
{
	if(!iter)
		return;
	if(!iter->entry || !iter->prev)
	{
		/**
		 * if iter in first position or invalid return immediately
		 */
		return;
	}
	if(!iter->prev->next)
	{
		/**
		 * if current is null, return immediately
		 */
		return;
	}
	dlistNode_t* tobrmv = (dlistNode_t*) iter->prev->next;
	iter->prev->next = &tobrmv->__base;
	tobrmv->prev = (dlistNode_t*) iter->prev;
}


