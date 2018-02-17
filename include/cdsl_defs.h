/*
 * cdsl_types.h
 *
 *  Created on: 2016. 12. 3.
 *      Author: innocentevil
 */

#ifndef INCLUDE_CDSL_DEFS_H_
#define INCLUDE_CDSL_DEFS_H_

#include "autogen.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TEST_SIZE
#define TEST_SIZE 100
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
#include <stddef.h>
#define PRINT(...)
#define PRINT_ERR(...)
#define EXIT(n)
#define STRCMP(s1,s2)              baremetal_strcmp(s1,s2)
#define F_OPEN(filename, mode)     ((int) -1)
#define F_WRITE(fd, buffer, size)  ((int) -1)
#define F_CLOSE(fd)                ((int) -1)
#define F_FDOPEN(fd)               NULL
#define F_FCLOSE(fp)               ((int) -1)
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#define PRINT(...)                 printf(__VA_ARGS__)
#define PRINT_ERR(...)             fprintf(stderr, __VA_ARGS__)
#define EXIT(n)                    exit(n)
#define STRCMP(s1,s2)              strcmp(s1,s2)
#define F_OPEN(filename, mode)     open(filename, mode)
#define F_WRITE(fd, buffer, size)  write(fd, buffer, size)
#define F_CLOSE(fd)                close(fd)
#define F_FDOPEN(fd)               fdopen(fd)
#define F_FCLOSE(fp)               fclose(fp)
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

#ifndef DECLARE_PRINTER
#define DECLARE_PRINTER(fn)    void fn(void* node)
#endif

typedef void* (*cdsl_generic_compare_t)(void*, void*);
typedef void (*cdsl_generic_printer_t) (void*);
typedef void* (*cdsl_alloc_t)(size_t);

#ifndef NULL
#define NULL 	((void*) 0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CDSL_DEFS_H_ */
