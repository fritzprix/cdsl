/*
 * fserializer.h
 *
 *  Created on: 2018. 1. 28.
 *      Author: innocentevil
 */

#ifndef INCLUDE_SERIALIZER_FSERIALIZER_H_
#define INCLUDE_SERIALIZER_FSERIALIZER_H_

#include "serializer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct file_serializer file_serialzer_t;

struct file_serializer {
	serializable_handler_t handle;
	int fd;
	uint32_t count;
	uint32_t chksum;
};

extern void file_serializerInit(file_serialzer_t* serializer, const char* filename);
extern void file_serializerExit(file_serialzer_t* serializer);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERIALIZER_FSERIALIZER_H_ */