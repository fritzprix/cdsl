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

typedef struct cdsl_serializer_interface cdsl_serializer_t;
typedef struct cdsl_serializable cdsl_serializable_t;
typedef struct cdsl_serializer_usr_callback cdsl_serializerUsrCallback_t;
typedef struct cdsl_serialize_header cdsl_serializeHeader_t;
typedef struct cdsl_serialize_node cdsl_serializeNode_t;
typedef struct cdsl_serialize_tail cdsl_serializeTail_t;

#define OK               ((int) 0)
#define ERR_INV_PARAM    ((int) -0x0001)
#define ERR_WR_OP_FAIL   ((int) -0x0002)
#define ERR_RD_OP_FAIL   ((int) -0x0003)

#define SERIALIZER_START_OF_FILE ((uint32_t) 0x0F0F)
#define SERIALIZER_DELIM         ((uint16_t) 0x00FF)
#define SERIALIZER_END_OF_FILE   ((uint16_t) 0xC1C1)


struct cdsl_serialize_header {
	uint16_t ver;         //<   lower byte : version | upper byte : magic
#define TYPE_TREE           ((uint16_t) (1 << 8))
#define SUB_TYPE_PLAIN      ((uint16_t) 0)
#define SUB_TYPE_REDBLACK   ((uint16_t) 1)
#define TYPE_LIST           ((uint16_t) (2 << 8))
	uint16_t type;
};

// serialize_header  |  serialize_node #1 | payload #1 | serialize_node #2 | payload #2 ... | serialize_tail
//                  /                                  \
//                 /                                    \
//                /                                      \
//               /                                        \
//              /                                          \
//              | d_offset | e_offset | d_size | flags | optional_header_ext | payload........[embedded node] ... payload |
//                d_offset                     |<-- from               to -->|
//                           e_offset                                        |<-- from  to -->|
//                                      d_size                               |<-- from                              to -->|

struct cdsl_serialize_node {
	//  MSB -------------------/ -------------------/  -------------------/ -------------------/
	// reserved for future use / type specific flag /         type        /       sub type     /
	uint32_t   d_offset;     ///< bytes offset of the payload from the beginning of serialize_node in bytes from the beginning of the data
	uint32_t   e_offset;     ///< offset of the
	uint32_t   d_size;       ///< size in bytes of the node data
#define NODE_MSK                ((uint16_t) 1)          ///< null node used to mark terminating node of the branch with given traversal order
#define NODE_NULL               ((uint16_t) 0)
#define NODE_NORMAL             ((uint16_t) 1)
#define IS_NULL_NODE(nodep)     ((nodep->flags & NODE_MSK) == NODE_NORMAL)
#define EMBEDDED_MSK            ((uint16_t) (1 << 1))
#define IS_EMBEDDED_NODE(nodep) ((nodep->flags & EMBEDDED_MSK) == EMBEDDED_MSK)
#define SPEC_MSK                ((uint16_t) (7 << 2))   ///< Type Specific value (ex. red / black for red-black tree)
	uint32_t flags;
	// there can be extension fields after the end of the struct
};

struct cdsl_serialize_tail {
	uint32_t     count;
	uint32_t     chksum;
};


struct cdsl_serializer_usr_callback {

#define DECLARE_SERIALIZER_ON_ERROR(fn) void fn(const cdsl_serializerUsrCallback_t* self, int error)
#define DECLARE_SERIALIZER_ON_NODE(fn) void* fn(const cdsl_serializerUsrCallback_t* self, const void* node, uint32_t* size)
#define DECLARE_SERIALIZER_ON_COMPLETE(fn) void fn(const cdsl_serializerUsrCallback_t* self)
	void (*on_error)(const cdsl_serializerUsrCallback_t* self, int error);

	/*!
	 * \brief this callback function will be invoked (or used) to get pointer to data to be written by serializer
	 * \param[in] node pointer to the node of the structure, which can be either tree or list
	 * \param[in] size user must write actual size of the data given size parameter. if the value is less than or equal to 0, only the node will be serialized
	 * \return ptr_data pointer to data to be written
	 */
	void* (*on_node_to_data) (const cdsl_serializerUsrCallback_t* self, const void* node, uint32_t* size);
	void (*on_complete) (const cdsl_serializerUsrCallback_t* self);
};


/*!
 *  \brief serializer should implement these callback
 *
 */
struct cdsl_serializer_interface {
	/*!
	 * \brief write head into byte stream
	 * \param[in] buffer write buffer
	 * \param[in] sz buffer size
	 * \return size of unwritten part of head (sizeof(head) - sizeof(buffer)), so for example, if the size of given buffer is larger than the size of head to be written, negative value would be returned
	 *
	 */
	int (*on_head)(const cdsl_serializer_t* self, const cdsl_serializeHeader_t* head);
	int (*on_next)(const cdsl_serializer_t* self, const cdsl_serializeNode_t* node, size_t nsz, const uint8_t* data);
	int (*on_tail)(const cdsl_serializer_t* self, const cdsl_serializeTail_t* tail);
};



extern uint16_t serializer_calcNodeChecksum(const cdsl_serializeNode_t* node, const void* data);



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERIALIZER_SERIALIZER_H_ */
