/*
 * serializer.c
 *
 *  Created on: 2018. 2. 15.
 *      Author: innocentevil
 */

#include "serializer.h"


uint32_t serializer_calcChecksum(uint32_t current_chks, const void* data, size_t sz) {
	if(!data || !sz) {
		return current_chks;
	}
	size_t sz_in_32b = (sz >> 2);
	uint8_t res_sz = sz - (sz_in_32b << 2);
	uint32_t* data_in_32b = (uint32_t*) data;

	while(sz_in_32b--) {
		current_chks += *(data_in_32b++);
	}
	if(res_sz) {
		uint8_t* res_data = (uint8_t*) data_in_32b;
		uint32_t res_chks = 0;
		while (res_sz--) {
			res_chks |= *(res_data++);
			res_chks <<= 8;
		}
		current_chks += res_chks;
	}
	return current_chks;
}
