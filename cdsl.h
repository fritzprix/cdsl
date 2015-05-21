/*
 * cdsl.h
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */

#ifndef CDSL_H_
#define CDSL_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "stdint.h"

#ifndef TEST_SIZE
#define TEST_SIZE 100
#endif


typedef enum { TRUE = ((uint8_t) 1 > 0), FALSE = ((uint8_t) 1 < 0) } BOOL;
typedef void (*cdsl_generic_printer_t) (void*);
#ifndef NULL
#define NULL 	((void*) 0)
#endif


#if defined(__cplusplus)
}
#endif



#endif /* CDSL_H_ */
