/*
 * serializer.h
 *
 *  Created on: 2018. 1. 28.
 *      Author: innocentevil
 */

#ifndef INCLUDE_SERIALIZER_SERIALIZER_H_
#define INCLUDE_SERIALIZER_SERIALIZER_H_

#include "cdsl_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SER_MAGIC  0x1C

#ifndef __MAJOR__
#define __MAJOR__   1
#endif

#ifndef __MINOR__
#define __MINOR__   0
#endif

#define GET_SER_VERSION()  (((__MAJOR__ * 10) + (__MINOR__)) | (SER_MAGIC << 8))

typedef struct serializable_handler serializable_handler_t;
typedef struct serializable serializable_t;
typedef struct serializable_callback serializable_callback_t;
typedef struct serialize_header serialize_header_t;


struct serializable_callback {
	/*!
	 * \brief this callback function can be invoked (or used) in serializable implementation. to get the pointer and size of the actual data to be copied in serialization process
	 * \param[in] node pointer to the node of the structure, which can be either tree or list
	 * \param[in] size user must write actual size of the data given size parameter. if the value is less than or equal to 0, only the node will be serialized
	 */
	void* (*get_data) (void* node, size_t* size);
};

struct serialize_header {
	uint16_t ver;         //<   lower byte : version | upper byte : magic
#define BASE_TREE           ((uint16_t) 0x01)
#define BASE_LIST           ((uint16_t) 0x02)
#define DOUBLE_LIST         ((uint16_t) 0x03)
	uint16_t type;
};

/*!
 *  \brief serializable function should be called
 */
struct serializable_handler {
	/*!
	 * \brief write head into byte stream
	 * \param[in] buffer write buffer
	 * \param[in] sz buffer size
	 * \return size of unwritten part of head (sizeof(head) - sizeof(buffer)), so for example, if the size of given buffer is larger than the size of head to be written, negative value would be returned
	 *
	 */
	void (*on_head)(const serializable_handler_t* self, const serialize_header_t* head);
	void (*on_next)(const serializable_handler_t* self, const uint8_t* next, size_t sz);
	void (*on_tail)(const serializable_handler_t* self);
};


struct serializable {
	void (*serialize)(const serializable_t* self, const serializable_handler_t* handler, const serializable_callback_t* cb);
	void* target;
};


/**
 *  writing list into writer
 *  cdsl_rbtree_serializer(
 *
 */


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERIALIZER_SERIALIZER_H_ */
