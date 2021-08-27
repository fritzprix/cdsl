/*
 * cdsl_list_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */

#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include "cdsl_dlist.h"
#include "cdsl_list_test.h"
#include "sort.h"
struct card {
	dlistNode_t list_node;
	int num;
};

static DECLARE_COMPARE_FN(compare);
static int compare_sort(void* one, void* the_other);
static struct card cards[TEST_SIZE];

BOOL cdsl_listDoTest(void)
{
	dlistEntry_t list_entry;
	cdsl_dlistEntryInit(&list_entry);
	int idx = 0;

	for(; idx < TEST_SIZE; idx++)
	{
		cdsl_dlistNodeInit(&cards[idx].list_node);
		cards[idx].num = idx;
		cdsl_dlistEnqueuePriority(&list_entry,&cards[idx].list_node,compare);
	}

	struct card *c = NULL;
	int last_num = TEST_SIZE;
	while(!cdsl_dlistIsEmpty(&list_entry))
	{
		c = (struct card*) cdsl_dlistDequeue(&list_entry);
		if(c)
		{
			c = container_of(c, struct card, list_node);
			assert(last_num > c->num);
			last_num = c->num;
		}
	}

	// test sort
	for(idx = 0; idx < TEST_SIZE; idx++) {
		cdsl_dlistNodeInit(&cards[idx].list_node);
		cards[idx].num = idx;
		cdsl_dlistPutHead(&list_entry, &(cards[idx].list_node));
	}

	cdsl_dlistSort(&list_entry, DESC, compare_sort);
	listIter_t iter;
	cdsl_iterInit((listEntry_t*) &list_entry, &iter);
	int prev_num = TEST_SIZE;
	int count = 0;
	for(;cdsl_iterHasNext(&iter);count++) {
		void* node = (void*) cdsl_iterNext(&iter);
		struct card* cp = container_of(node, struct card, list_node);
		if(prev_num < cp->num) {
			return FALSE;
		}
		prev_num = cp->num;
	}
	assert(count == TEST_SIZE);
	cdsl_dlistIterInit(&list_entry, &iter);
	for(;cdsl_dlistIterHasPrev(&iter);count--)
	{
		dlistNode_t* ln = cdsl_dlistIterPrev(&iter);
		assert(ln != NULL);
	}
	assert(count == 0);
	return count == 0;
}

static int compare_sort(void* one, void* the_other) {
	struct card* this_card = container_of(one, struct card, list_node);
	struct card* that_card = container_of(the_other, struct card, list_node);
	return this_card->num - that_card->num;
}


static DECLARE_COMPARE_FN(compare)
{
	if(container_of(a,struct card,list_node)->num < container_of(b, struct card,list_node)->num)
		return b;
	return a;
}
