/*
 * base_list.c
 *
 *  Created on: Mar 23, 2016
 *      Author: innocentevil
 */

#include "base_list.h"


int cdsl_listSize(listEntry_t* lentry)
{
	if(!lentry)
		return 0;
	if(!lentry->head)
		return 0;
	int cnt = 0;
	listNode_t* cnode = (listNode_t*) lentry;
	while(cnode->next != NULL){
		cnode = cnode->next;
		cnt++;
	}
	return cnt;
}

BOOL cdsl_listContain(listEntry_t* lentry,listNode_t* item)
{
	if(!lentry)
		return FALSE;
	if(!lentry->head)
		return FALSE;
	listNode_t* cnode = (listNode_t*) lentry;
	while(cnode->next != NULL){
		cnode = cnode->next;
		if(cnode == item){
			return TRUE;
		}
	}
	return FALSE;
}

void cdsl_listPrint(listEntry_t* lentry,cdsl_generic_printer_t prt)
{
	if(!lentry)
		return;
	if(!lentry->head)
		return;
	listNode_t* cnode = (listNode_t*) lentry;
	if(!prt)
		return;
	while(cnode->next != NULL)
	{
		cnode = cnode->next;
		prt(cnode);
	}
}

void cdsl_iterInit(listEntry_t* lentry, listIter_t* iter)
{
	iter->entry = lentry;
	iter->prev = NULL;
}

BOOL cdsl_iterHasNext(listIter_t* iter)
{
	if(!iter->prev)					// at the first position, test entry
		return (iter->entry->head != NULL);
	else if(!iter->prev->next)		// current is null, return false
		return FALSE;
	return (iter->prev->next->next != NULL);  // test if next of current is null
}

listNode_t* cdsl_iterNext(listIter_t* iter)
{
	if(!iter)
		return NULL;
	if(!iter->prev)
		iter->prev = (listNode_t*) iter->entry;
	return iter->prev->next;
}

listNode_t* cdsl_iterRemove(listIter_t* iter)
{
	if(!iter)
		return NULL;
	listNode_t* cur;
	if(!iter->prev)
	{
		iter->prev = (listNode_t) iter->entry;
	}
	cur = iter->prev->next;
	iter->prev->next = cur->next;
	cur->next = NULL;
	return cur;
}

