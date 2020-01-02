/*
 * cdsl_types.h
 *
 *  Created on: 2016. 12. 3.
 *      Author: innocentevil
 */

#ifndef INCLUDE_CDSL_DEFS_H_
#define INCLUDE_CDSL_DEFS_H_

#include "autogen.h"
#include "target.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long __cdsl_uaddr_t;

#ifndef TEST_SIZE
#define TEST_SIZE 100
#endif

#ifdef __DBG
#define __dev_log(...)       PRINT(__VA_ARGS__)
#else
#define __dev_log(...)
#endif


#define container_of(ptr, type, elem) 	((type*) ((__cdsl_uaddr_t) ptr - offsetof(type, elem)))

#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef FALSE
#define FALSE ((unsigned char) 0 != 0)
#endif

#ifndef TRUE
#define TRUE  ((unsigned char) 0 == 0)
#endif

#ifndef DECLARE_COMPARE_FN
#define DECLARE_COMPARE_FN(fn) void* fn(void* a,void* b)
#endif

#ifndef DECLARE_PRINTER
#define DECLARE_PRINTER(fn)    void fn(void* node)
#endif

/**!
 *
 */
typedef void* (*cdsl_generic_compare_t)(void*, void*);
typedef void (*cdsl_generic_printer_t) (void*);
typedef void* (*cdsl_alloc_t)(unsigned long sz);
typedef void (*cdsl_free_t) (void* ptr);

typedef struct {
	cdsl_alloc_t     alloc;
	cdsl_free_t      free;
}cdsl_memoryMngt_t;

#ifndef NULL
#define NULL 	((void*) 0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CDSL_DEFS_H_ */
