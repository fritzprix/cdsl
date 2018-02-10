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

typedef struct serializer_interface serializer_t;
typedef struct serializable serializable_t;
typedef struct serializable_callback serializable_callback_t;
typedef struct serializable_header serializable_header_t;
typedef struct serializable_node serializable_node_t;
typedef struct serializable_tail serializer_tail_t;

#define ERR_INV_PARAM    ((int) -0x0001)
#define ERR_WR_OP_FAIL   ((int) -0x0002)
#define ERR_RD_OP_FAIL   ((int) -0x0003)



struct serializable_callback {
	/*!
	 * \brief this callback function can be invoked (or used) in serializable implementation. to get the pointer and size of the actual data to be copied in serialization process
	 * \param[in] node pointer to the node of the structure, which can be either tree or list
	 * \param[in] size user must write actual size of the data given size parameter. if the value is less than or equal to 0, only the node will be serialized
	 */
	void* (*get_data) (const void* node, size_t* size);
};

struct serializable_header {
	uint16_t ver;         //<   lower byte : version | upper byte : magic
#define TYPE_TREE           ((uint16_t) (1 << 8))
#define SUB_TYPE_PLAIN      ((uint16_t) 0)
#define SUB_TYPE_REDBLACK   ((uint16_t) 1)
#define TYPE_LIST           ((uint16_t) (2 << 8))
	uint16_t type;
};

struct serializable_node {
	//  MSB -------------------/ -------------------/  -------------------/ -------------------/
	// reserved for future use / type specific flag /         type        /       sub type     /
	union {
		uint32_t flags;
		struct {
			uint8_t _;
			// spec |  |  |  |  |  |  |  |  |
			union {
				uint8_t spec;
				struct {
					unsigned is_embedded:1;
					unsigned sub_type_spec:7;
				};
			};
#define NODE_VALID        ((uint8_t) 0x1)
#define NODE_NULL         ((uint8_t) 0x0)
			uint8_t type;
			uint8_t sub_type;
		} __attribute__((packed));
	};
	size_t size;   ///< size in bytes of the node data
	size_t offset; ///< offset in bytes from the end of this struct to embedded tree node struct
};

struct serializable_tail {
	uint32_t     count;
	size_t       size;
	uint32_t     chksum;
};


/*!
 *  \brief serializer interface
 */
struct serializer_interface {
	/*!
	 * \brief write head into byte stream
	 * \param[in] buffer write buffer
	 * \param[in] sz buffer size
	 * \return size of unwritten part of head (sizeof(head) - sizeof(buffer)), so for example, if the size of given buffer is larger than the size of head to be written, negative value would be returned
	 *
	 */


	int (*on_head)(const serializer_t* self, const serializable_header_t* head);
	int (*on_next)(const serializer_t* self, serializable_node_t* node, const uint8_t* data, size_t sz);
	void (*on_tail)(const serializer_t* self, int res);
};


struct serializable {
	void (*serialize)(const serializable_t* self, const serializer_t* handler, const serializable_callback_t* cb);
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
