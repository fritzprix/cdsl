/*
 * cdsl_slist_test.c
 *
 *  Created on: 2015. 5. 31.
 *      Author: innocentevil
 */



#include <stdio.h>
#include "cdsl_slist.h"
#include "cdsl_slist_test.h"


typedef struct card {
	slistNode_t	list_head;
	int card_num;
}card_t;


static card_t Cards[TEST_SIZE];
static DECLARE_COMPARE_FN(card_compare);
static slistEntry_t listEntry;
static int compare_sort(void* one, void* the_other);


BOOL cdsl_slistDoTest(void){
	int i = 0;
	cdsl_slistEntryInit(&listEntry);

	for(;i < TEST_SIZE;i++){
		Cards[i].card_num = i;
		cdsl_slistNodeInit(&Cards[i].list_head);
		cdsl_slistEnqueuePriority(&listEntry,(slistNode_t*) &Cards[i],card_compare);
	}
	card_t* card = NULL;
	int last_num = TEST_SIZE;
	if(TEST_SIZE != cdsl_slistSize(&listEntry))
		return FALSE;
	for(i = 0;i < TEST_SIZE;i++)
	{
		card = (card_t*) cdsl_slistDequeue(&listEntry);
		if(last_num < card->card_num)
			return FALSE;
		last_num = card->card_num;
	}

	for(i = 0;i < TEST_SIZE;i++){
		Cards[i].card_num = i;
		cdsl_slistNodeInit(&Cards[i].list_head);
		cdsl_slistPutHead(&listEntry, &Cards[i].list_head);
	}
	if(TEST_SIZE != cdsl_slistSize(&listEntry))
		return FALSE;
	for(i = 0;i < TEST_SIZE;i++)
	{
		card = (card_t*) cdsl_slistDequeue(&listEntry);
		if(!card)
			return FALSE;
	}
	for(i = 0; i < TEST_SIZE; i++) {
			cdsl_slistNodeInit(&Cards[i].list_head);
			Cards[i].card_num = i;
			cdsl_slistPutHead(&listEntry, &Cards[i].list_head);
		}
		cdsl_slistSort(&listEntry, DESC, compare_sort);
		listIter_t iter;
		cdsl_iterInit((listEntry_t*) &listEntry, &iter);
		int prev_num = TEST_SIZE;
		while(cdsl_iterHasNext(&iter)) {
			void* node = (void*) cdsl_iterNext(&iter);
			struct card* cp = container_of(node, struct card, list_head);
			if(prev_num < cp->card_num) {
				return FALSE;
			}
			prev_num = cp->card_num;
		}
	return TRUE;
}


static int compare_sort(void* one, void* the_other) {
	struct card* this_card = container_of(one, struct card, list_head);
	struct card* that_card = container_of(the_other, struct card, list_head);
	return this_card->card_num - that_card->card_num;
}


static DECLARE_COMPARE_FN(card_compare){
	if(!a)
		return b;
	if(!b)
		return a;
	return ((card_t*) b)->card_num > ((card_t*) a)->card_num? b : a;
}

