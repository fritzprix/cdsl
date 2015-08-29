/*
 * cdsl_hashtable.c
 *
 *  Created on: 2015. 8. 29.
 *      Author: innocentevil
 */

#include "cdsl_hash.h"


#define DJB2_MAGIC		((unsigned long) 5381)

unsigned long sbdm_hash(unsigned char* str){
	unsigned long hash = 0;
	int c;
	while ((c = *str++))
		hash = c + (hash << 6) + (hash << 16) - hash;
	return hash;
}

unsigned long djb2_hash(unsigned char* str){
	unsigned long hash = DJB2_MAGIC;
	char c;
	while((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}
