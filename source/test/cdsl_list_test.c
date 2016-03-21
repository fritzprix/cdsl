/*
 * cdsl_list_test.c
 *
 *  Created on: May 21, 2015
 *      Author: root
 */

#include <stddef.h>
#include <stdio.h>
#include "cdsl.h"
#include "cdsl_dlist.h"
#include "cdsl_list_test.h"

struct card {
	dlistNode_t list_node;
	int num;
};

static DECLARE_COMPARE_FN(compare);

BOOL cdsl_listDoTest(void)
{
	dlistEntry_t list_entry;
	cdsl_dlistEntryInit(&list_entry);
	struct card cards[TEST_SIZE];
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
			if(last_num < c->num)
				return FALSE;
			last_num = c->num;
		}
	}

	return TRUE;
}


static DECLARE_COMPARE_FN(compare)
{
	if(container_of(a,struct card,list_node)->num < container_of(b, struct card,list_node)->num)
		return b;
	return a;
}
