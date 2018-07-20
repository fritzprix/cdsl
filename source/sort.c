


#include "cdsl_dlist.h"
#include "sort.h"


static listNode_t* merge_sort_rc(listNode_t* head, sortOrder_t order, cdsl_comparator_t comp, BOOL isDouble);
static listNode_t* peek_middle(listNode_t* head);

extern void cdsl_listMergeSort(listEntry_t* entry, sortOrder_t order, cdsl_comparator_t comp, BOOL isDoubleLink) {
	entry->head = merge_sort_rc(entry->head, order, comp, isDoubleLink);
}

static listNode_t* merge_sort_rc(listNode_t* head, sortOrder_t order, cdsl_comparator_t comp, BOOL isDouble) {
	if(!(head->next)) {
		return head;
	}
	listNode_t* middle = peek_middle(head);
	if(!middle) {
		return head;
	}
	listNode_t* left_cur, *right_cur;
	if(head->next == middle) {
		head->next = NULL;
		left_cur = head;
		right_cur = merge_sort_rc(middle, order, comp, isDouble);
	} else {
		right_cur = merge_sort_rc(middle->next, order, comp, isDouble);
		middle->next = NULL;
		left_cur = merge_sort_rc(head, order, comp, isDouble);
	}
	head = NULL;
	listNode_t** slot = &head;
	// keep
	// NOTE :  middle used as previous from this point
	middle = NULL;
	while(!((left_cur == NULL) && (right_cur == NULL))) {

		if ((left_cur != NULL) && (right_cur != NULL)) {
			if (comp(left_cur, right_cur) > 0) {
				*slot = (order == ASC)? right_cur : left_cur;
			} else {
				*slot = (order == ASC)? left_cur : right_cur;
			}
		} else if (left_cur == NULL) {
			*slot = right_cur;
		} else {
			*slot = left_cur;
		}
		if(*slot == left_cur) {
			left_cur = left_cur->next;
		} else if(*slot == right_cur) {
			right_cur = right_cur->next;
		}
		if(isDouble) {
			if(middle) {
				((dlistNode_t*) *slot)->prev = (dlistNode_t*) middle;
				middle = *slot;
			} else {
				middle = *slot;
				((dlistNode_t*) middle)->prev = NULL;
			}
		}
		slot = &(*slot)->next;
	}
	return head;
}

/***
 *  head node of list chunk to be searched
 *  @return head if there is only list node in the list chunk, otherwise return middle list node within the list chunk.
 */
static listNode_t* peek_middle(listNode_t* head) {
	listNode_t* mid = head;
	uint32_t l_cnt = 0;
	while(head->next) {
		if((l_cnt % 2) == 0) {
			mid = mid->next;
		}
		l_cnt++;
		head = head->next;
	}
	return mid;
}
