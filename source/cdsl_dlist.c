/*
 * tch_list.c
 *
 *  Created on: 2014. 7. 25.
 *      Author: innocentevil
 */


#include <stddef.h>
#include <stdio.h>

#include "cdsl_list.h"



void cdsl_listInit(cdsl_dlistNode_t* lentry){
	lentry->next = NULL;
	lentry->prev = NULL;
}

void cdsl_listEnqueuePriority(cdsl_dlistNode_t* lentry,cdsl_dlistNode_t* item,cdsl_listPriorityRule rule){
	cdsl_dlistNode_t* cnode = lentry;
	if(!item)
		return;
	while(cnode->next != NULL){
		cnode = cnode->next;
		if(rule(cnode,item) == item){
			((cdsl_dlistNode_t*)cnode->prev)->next = item;
			item->prev = cnode->prev;
			item->next = cnode;
			cnode->prev = item;
			return;
		}
	}
	cnode->next = item;
	item->prev = cnode;
	item->next = NULL;
	if(cnode != lentry)
		lentry->prev = item;
}

cdsl_dlistNode_t* cdsl_listDequeue(cdsl_dlistNode_t* lentry){
	cdsl_dlistNode_t* cnode = lentry->next;
	if(cdsl_listIsEmpty(lentry))
		return NULL;
	lentry->next = cnode->next;
	if(cnode->next)
		((cdsl_dlistNode_t*)cnode->next)->prev = lentry;
	else
		lentry->prev = NULL;
	return cnode;
}

void cdsl_listPutHead(cdsl_dlistNode_t* lentry,cdsl_dlistNode_t* item){
	if(!lentry)
			return;
	item->next = lentry->next;
	if(lentry->next)
		((cdsl_dlistNode_t*)lentry->next)->prev = item;
	else
		lentry->prev = item;
	item->prev = lentry;
	lentry->next = item;
}

void cdsl_listInsertAfter(cdsl_dlistNode_t* ahead,cdsl_dlistNode_t* item){
	if(!ahead || !item)
		return;

	item->next = ahead->next;
	item->prev = ahead;
	ahead->next = item;
	if(item->next)
		item->next->prev = item;
}



void cdsl_listPutTail(cdsl_dlistNode_t* lentry,cdsl_dlistNode_t* item){
	if(!lentry || !item)
		return;
	if(lentry->next){
		((cdsl_dlistNode_t*)lentry->prev)->next = item;
		item->prev = lentry->prev;
	} else{
		lentry->next = item;
		lentry->prev = item;
		item->prev = lentry;
	}
	item->next = NULL;
	lentry->prev = item;
}


cdsl_dlistNode_t* cdsl_listGetHead(cdsl_dlistNode_t* lentry){
	if(!lentry)
		return NULL;
	return lentry->next;
}

cdsl_dlistNode_t* cdsl_listGetTail(cdsl_dlistNode_t* lentry){
	cdsl_dlistNode_t* last = NULL;
	if(!lentry)
		return NULL;
	if(!lentry->prev)
		return NULL;
	last = lentry->prev;
	lentry->prev = last->prev;
	((cdsl_dlistNode_t*)lentry->prev)->next = NULL;
	return last;
}

int cdsl_listRemove(cdsl_dlistNode_t* item){
	if(!item)
		return (1 < 0);
	cdsl_dlistNode_t* prev = item->prev;
	if(prev)
		prev->next = item->next;
	if(item->next)
		item->next->prev = prev;
	return (1 > 0);
}

int cdsl_listSize(cdsl_dlistNode_t* lentry){
	int cnt = 0;
	cdsl_dlistNode_t* cnode = lentry;
	while(cnode->next != NULL){
		cnode = cnode->next;
		cnt++;
	}
	return cnt;
}

int cdsl_listContain(cdsl_dlistNode_t* lentry,cdsl_dlistNode_t* item){
	cdsl_dlistNode_t* cnode = lentry;
	while(cnode->next != NULL){
		cnode = cnode->next;
		if(cnode == item){
			return (1 > 0);
		}
	}
	return (1 < 0);
}

void cdsl_listPrint(cdsl_dlistNode_t* lentry,void (*printitem)(void* item)){
	cdsl_dlistNode_t* cnode = lentry;
	while(cnode->next != NULL){
		cnode = cnode->next;
		printitem(cnode);
	}
}
