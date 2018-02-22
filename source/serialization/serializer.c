/*
 * serializer.c
 *
 *  Created on: Feb 18, 2018
 *      Author: lucifer
 */

#include "../../include/serialization/serializer.h"

uint16_t serializer_calcNodeChecksum(const cdsl_serializeNode_t* node, const void* data) {
	if(data == NULL) {
		return 0;
	}
	size_t sz_in_short = node->d_size >> 1;
	uint8_t res_sz = node->d_size - (sz_in_short << 1);
	uint16_t* dp16 = (uint16_t*) data;
	uint8_t* dp8 = NULL;
	uint16_t res_chk = 0
			, chksum = 0;
	while(sz_in_short--) {
		chksum += *(dp16++);
	}
	dp8 = (uint8_t*) dp16;

	while(res_sz) {
		res_chk += *(dp8++);
		res_chk <<= 8;       // shift right
	}
	return chksum + res_chk;
}






