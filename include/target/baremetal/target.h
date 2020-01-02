
#ifndef __CDSL_TARGET_H
#define __CDSL_TARGET_H
/*
 *
 * for baremetal application, you can provide your own implementation of dependencies
 */
extern int strcmp(const char* s1, const char* s2);

#define FO_RDWR                    02
#define FO_RDONLY                  03
#define FO_WRONLY                  04
#define PRINT(...)
#define PRINT_ERR(...)
#define EXIT(n)
#define STRCMP(s1,s2)              baremetal_strcmp(s1,s2)
#define F_OPEN(filename, mode)     ((int) -1)
#define F_WRITE(fd, buffer, size)  ((int) -1)
#define F_READ(fd, buffer, size)   ((int) -1)
#define MEMCPY(dest,src,sz)         {}
#define MALLOC                     NULL
#define FREE
#define GET_DEFAULT_MMNGT()          {}
#define F_CLOSE(fd)                ((int) -1)
#define F_FDOPEN(fd)               NULL
#define F_FCLOSE(fp)               ((int) -1)

#endif