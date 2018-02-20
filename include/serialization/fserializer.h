/*
 * fserializer.h
 *
 *  Created on: 2018. 1. 28.
 *      Author: innocentevil
 */

#ifndef INCLUDE_SERIALIZATION_FSERIALIZER_H_
#define INCLUDE_SERIALIZATION_FSERIALIZER_H_

#include "../serialization/serializer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct file_serializer file_serializer_t;
typedef struct file_deserializer file_deserializer_t;

struct file_deserializer {
	cdsl_deserializer_t     handle;
	int                     fd;
};

struct file_serializer {
	cdsl_serializer_t        handle;
	int                      fd;
};

extern void file_serializerOpen(file_serializer_t* serializer, const char* filename);
extern void file_serializerClose(file_serializer_t* serializer);

extern void file_deserializerOpen(file_deserializer_t* deserializer, const char* filename);
extern void file_deserializerClose(file_deserializer_t* deserializer);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERIALIZATION_FSERIALIZER_H_ */
