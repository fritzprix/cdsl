/*
 * cdsl_hashtable.h
 *
 *  Created on: 2015. 8. 29.
 *      Author: innocentevil
 */

#ifndef CDSL_HASHTABLE_H_
#define CDSL_HASHTABLE_H_

#if defined(__cplusplus)
extern "C" {
#endif


extern unsigned long sbdm_hash(unsigned char* str);
extern unsigned long djb2_hash(unsigned char* str);



#if defined(__cplusplus)
}
#endif

#endif /* CDSL_HASHTABLE_H_ */
