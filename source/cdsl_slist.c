/*
 * cdsl_slist.c
 *
 *  Created on: 2015. 5. 31.
 *      Author: innocentevil
 */


#include "cdsl_slist.h"

void cdsl_slistEntryInit(slistEntry_t* entry) {
	if (!entry)
		return;
	entry->head = NULL;
}

void cdsl_slistNodeInit(slistNode_t* item) {
	if (!item)
		return;
	item->next = NULL;
}

void cdsl_slistMerge(slistEntry_t* to, slistEntry_t* from) {
	if (!to || !from)
		return;
	slistNode_t* tail = to->head;
	while (tail->next) {
		tail = tail->next;
	}
	tail->next = from->head;
	from->head = NULL;
}

void cdsl_slistEnqueuePriority(slistEntry_t* lentry, slistNode_t* item,	cdsl_generic_compare_t rule) {
	if (!lentry || !item || !rule) {
		return;
	}
	slistNode_t** cnode = (slistNode_t**) &lentry;
	while ((*cnode)->next != NULL) {
		if (rule((*cnode)->next, item) == item) {
			item->next = (*cnode)->next;
			(*cnode)->next = item;
			return;
		}
		cnode = &(*cnode)->next;
	}
	(*cnode)->next = item;
}

void cdsl_slistInsertAfter(slistNode_t* ahead, slistNode_t* item) {
	if (!ahead || !item) {
		return;
	}
	item->next = ahead->next;
	ahead->next = item;
}

slistNode_t* cdsl_slistDequeue(slistEntry_t* lentry) {
	if (!lentry) {
		return NULL;
	}
	slistNode_t* head = lentry->head;
	if (head) {
		lentry->head = head->next;
	}
	return head;
}

int cdsl_slistPutHead(slistEntry_t* lentry, slistNode_t* item) {
	if (!lentry || !item) {
		return -1;
	}
	if (lentry->head) {
		item->next = lentry->head;
	} else {
		item->next = NULL;
	}
	lentry->head = item;
	return 0;
}

int cdsl_slistPutTail(slistEntry_t* lentry, slistNode_t* item) {
	if (!lentry || !item) {
		return -1;
	}
	int idx = 0;
	item->next = NULL;
	slistNode_t* node = (slistNode_t*) lentry;
	while (node->next) {
		node = node->next;
		idx++;
	}
	node->next = item;
	return idx;
}

slistNode_t* cdsl_slistRemoveHead(slistEntry_t* lentry) {
	return cdsl_slistDequeue(lentry);
}

slistNode_t* cdsl_slistRemoveTail(slistEntry_t* lentry) {
	if (!lentry) {
		return NULL;
	}
	if (!lentry->head) {
		return NULL;
	}
	slistNode_t** cnode = (slistNode_t**) &lentry;
	slistNode_t* tail = NULL;
	while ((*cnode)->next != NULL) {
		cnode = &(*cnode)->next;
	}
	tail = (*cnode);
	*cnode = NULL;
	return tail;
}

BOOL cdsl_slistRemove(slistEntry_t* lentry, slistNode_t* item) {
	if (!lentry || !item) {
		return FALSE;
	}
	if (!lentry->head) {
		return FALSE;
	}
	slistNode_t* cnode = (slistNode_t*) lentry;
	while (cnode->next != NULL) {
		if (cnode->next == item) {
			cnode->next = item->next;
			return TRUE;
		}
		cnode = cnode->next;
	}
	return FALSE;
}

slistNode_t* cdsl_slistRemoveAt(slistEntry_t* entry, int idx) {
	if (!entry) {
		return NULL;
	}
	if (!entry->head) {
		return NULL;
	}
	slistNode_t* cnode = (slistNode_t*) entry;
	slistNode_t* found = cnode;
	while (idx--) {
		cnode = cnode->next;
	}
	found = cnode->next;
	cnode->next = found->next;
	return found;
}

void cdsl_slistIterRemove(listIter_t* iter) {
	if (!iter) {
		return;
	}
	if (!iter->entry || !iter->prev) {
		/**
		 * if iter in first position or invalid return immediately
		 */
		return;
	}
	if (!iter->prev->next) {
		/**
		 * if current is null, return immediately
		 */
		return;
	}
	listNode_t* tobrmv = iter->prev->next;
	iter->prev->next = tobrmv->next;
}

