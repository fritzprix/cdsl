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
#define __dev_log(...)       PRINT(__VA_ARGS__)
#else
#define __dev_log(...)
#endif


/*
 *
 * for baremetal application, you can provide your own implementation of dependencies
 */
#ifdef BAREMETAL
#include "baremetal.h"
#define PRINT(...)
#define PRINT_ERR(...)
#define EXIT(n)
#define STRCMP(s1,s2)        baremetal_strcmp(s1,s2)
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PRINT(...)           printf(__VA_ARGS__)
#define PRINT_ERR(...)       fprintf(stderr, __VA_ARGS__)
#define EXIT(n)              exit(n)
#define STRCMP(s1,s2)        strcmp(s1,s2)
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
