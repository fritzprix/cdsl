

#ifndef __CDSL_SORT_C
#define __CDSL_SORT_C

#ifdef __cplusplus
extern "C" {
#endif

#include "base_list.h"

typedef enum {
	ASC,
	DESC
}sortOrder_t;
/**
 * @return positive if one is larger than the_other, or negative otherwise. 0 when the two are exactly same
 */
typedef int (*cdsl_comparator_t)(void* one, void* the_other);

extern void cdsl_listMergeSort(listEntry_t* entry, sortOrder_t order, cdsl_comparator_t comp, BOOL isDoubleLink);

#ifdef __cplusplus
}
#endif



#endif
