/*
 * cdsl.h
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */

#ifndef CDSL_H_
#define CDSL_H_

#include "autogen.h"
#include <stdint.h>
#include <stddef.h>


#if defined(__cplusplus)
extern "C" {
#endif

#ifndef TEST_SIZE
#define TEST_SIZE 10000
#endif

#ifdef __DBG
#define __dev_log(...) 	PRINT(__VA_ARGS__)
#else
#define __dev_log(...)
#endif


/*
 *  baremetal environment assume libc doesn't exist
 *  so PRINT do nothing till any target specific printf equivalent
 */
#ifdef BAREMETAL
#define PRINT(...)
#else
#include <stdio.h>
#define PRINT(...)           printf(__VA_ARGS__)
#endif

#define container_of(ptr, type, elem) 	((type*) ((size_t) ptr - offsetof(type, elem)))

#ifndef BOOL
#define BOOL uint8_t
#define FALSE ((uint8_t) 1 < 0)
#define TRUE  ((uint8_t) 1 > 0)
#endif

#ifndef DECLARE_COMPARE_FN
#define DECLARE_COMPARE_FN(fn) void* fn(void* a,void* b)
#endif

typedef void* (*cdsl_generic_compare_t)(void*, void*);
typedef void (*cdsl_generic_printer_t) (void*);

#ifndef NULL
#define NULL 	((void*) 0)
#endif





#if defined(__cplusplus)
}
#endif



#endif /* CDSL_H_ */
