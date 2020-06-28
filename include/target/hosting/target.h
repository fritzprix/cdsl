
#ifndef __CDSL_TARGET_H
#define __CDSL_TARGET_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>

#define FO_RDWR                    O_RDWR | O_CREAT
#define FO_RDONLY                  O_RDONLY
#define FO_WRONLY                  O_WRONLY
#define PRINT(...)                 printf(__VA_ARGS__)
#define PRINT_ERR(...)             fprintf(stderr, __VA_ARGS__)
#define EXIT(n)                    exit(n)
#define STRCMP(s1,s2)              strcmp(s1,s2)
#define F_OPEN(filename, mode)     open(filename, mode, 0666)
#define F_WRITE(fd, buffer, size)  write(fd, buffer, size)
#define F_READ(fd, buffer, size)   read(fd, buffer, size)
#define MEMCPY(dest,src,sz)        memcpy(dest, src, sz)
#define MALLOC                     malloc
#define FREE                       free
#define GET_DEFAULT_MMNGT()        {\
		.alloc       =    MALLOC,\
		.free        =    FREE\
}

#define F_CLOSE(fd)                close(fd)
#define F_FDOPEN(fd)               fdopen(fd)
#define F_FCLOSE(fp)               fclose(fp)

#endif