/*
 * fserializer.c
 *
 *  Created on: 2018. 1. 28.
 *      Author: innocentevil
 */

#include "file_serializer.h"
#include "cdsl_defs.h"


#define END_OF_ITEMS        ((uint8_t) 0xf0)
#define HAS_NEXT            ((uint8_t) 0x1f)


struct serializer_delim {
	uint16_t                 node_chs; // cyclic checksum
	uint16_t                 delim;    // SERIALIZER_DELIM
};

struct deserializer_header {
	cdsl_serializeHeader_t  ser_header;
	uint8_t has_next;
} __attribute__((packed));

struct deserializer_delim {
	struct serializer_delim ser_delim;
	uint8_t has_next;
} __attribute__((packed));

static int ser_on_head(const cdsl_serializer_t* self, const cdsl_serializeHeader_t* head);
static int ser_on_next(const cdsl_serializer_t* self, const cdsl_serializeNode_t* node, size_t nsz, const uint8_t* data);
static int ser_on_tail(const cdsl_serializer_t* self, const cdsl_serializeTail_t* tail);
static int ser_close(const cdsl_serializer_t* self);


static int desr_read_header(const cdsl_deserializer_t* self, cdsl_serializeHeader_t* header);
static void* desr_get_next(const cdsl_deserializer_t* self, cdsl_serializeNode_t* node, size_t nsz, const cdsl_memoryMngt_t* m_mngt);
static BOOL desr_has_next(const cdsl_deserializer_t* self);
static int desr_read_tail(const cdsl_deserializer_t* self, cdsl_serializeTail_t* tailp);
static int desr_close(const cdsl_deserializer_t* self);

int file_serializerOpen(file_serializer_t* serializer, const char* filename) {

	if((serializer == NULL) || (filename == NULL)) {
		return ERR_INV_PARAM;
	}

	serializer->handle.on_head = ser_on_head;
	serializer->handle.on_next = ser_on_next;
	serializer->handle.on_tail = ser_on_tail;
	serializer->handle.close = ser_close;
	serializer->fd = -1;

	const int fd = F_OPEN(filename, FO_RDWR);
	if(fd < 0) {
		PRINT("fail to open\n");
		return ERR_OPEN_FAIL;
	}
	serializer->fd = fd;
	return OK;
}

void file_serializerClose(file_serializer_t* serializer) {
	if((serializer == NULL) ||
			(serializer->fd < 0)) {
		return;
	}
	F_CLOSE(serializer->fd);
}

int file_deserializerOpen(file_deserializer_t* deserializer, const char* filename) {
	if((deserializer == NULL) || (filename == NULL)) {
		return ERR_INV_PARAM;
	}

	deserializer->handle.read_head = desr_read_header;
	deserializer->handle.get_next = desr_get_next;
	deserializer->handle.read_tail = desr_read_tail;
	deserializer->handle.has_next = desr_has_next;
	deserializer->handle.close = desr_close;

	deserializer->fd = F_OPEN(filename, FO_RDONLY);
	if(deserializer->fd < 0) {
		return ERR_OPEN_FAIL;
	}
	deserializer->is_eos_reached = FALSE;

	return OK;
}

void file_deserializerClose(file_deserializer_t* deserializer) {
	if((deserializer == NULL) || (deserializer->fd < 0) ) {
		return;
	}
	F_CLOSE(deserializer->fd);
}



static int ser_on_head(const cdsl_serializer_t* self, const cdsl_serializeHeader_t* head) {
	if((self == NULL) ||
			(head == NULL)) {
		return ERR_INV_PARAM;
	}
	uint16_t ser_starter = SERIALIZER_START_OF_FILE;
	const file_serializer_t* serializer = container_of(self, file_serializer_t, handle);
	if(serializer->fd < 0) {
		return ERR_INV_PARAM;
	}
	if(F_WRITE(serializer->fd, &ser_starter, sizeof(uint16_t)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	if(F_WRITE(serializer->fd, head, sizeof(cdsl_serializeHeader_t)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	return OK;
}


static int ser_on_next(const cdsl_serializer_t* self, const cdsl_serializeNode_t* node, size_t nsz, const uint8_t* data){
	if((self == NULL) ||
			(node == NULL)) {
		return ERR_INV_PARAM;
	}
	const file_serializer_t* serializer = container_of(self, file_serializer_t, handle);
	if(serializer->fd < 0) {
		return ERR_INV_PARAM;
	}
	uint8_t has_next = HAS_NEXT;
	if(F_WRITE(serializer->fd, &has_next, sizeof(uint8_t)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	if(F_WRITE(serializer->fd, node, nsz) < 0){
		return ERR_WR_OP_FAIL;
	}
	struct serializer_delim node_term = {0};
	if(node->d_size > 0) {
		node_term.node_chs = serializer_calcNodeChecksum(node, data);
		if (F_WRITE(serializer->fd, data, node->d_size) < 0) {
			return ERR_WR_OP_FAIL;
		}
	}
	node_term.delim = SERIALIZER_DELIM;
	if(F_WRITE(serializer->fd, &node_term, sizeof(struct serializer_delim)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	return OK;
}


static int ser_on_tail(const cdsl_serializer_t* self, const cdsl_serializeTail_t* tail) {
	if((self == NULL) || (tail == NULL)) {
		return ERR_INV_PARAM;
	}
	uint16_t eof = SERIALIZER_END_OF_FILE;
	const file_serializer_t* serializer = container_of(self, file_serializer_t, handle);
	uint8_t has_next = END_OF_ITEMS;
	if(F_WRITE(serializer->fd, &has_next, sizeof(uint8_t)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	if(F_WRITE(serializer->fd, tail, sizeof(cdsl_serializeTail_t)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	if(F_WRITE(serializer->fd, &eof, sizeof(uint16_t)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	return OK;
}

static int ser_close(const cdsl_serializer_t* self) {
	if(self == NULL) {
		return ERR_INV_PARAM;
	}

	const file_serializer_t* serializer = container_of(self, file_serializer_t, handle);
	if(serializer->fd <= 0) {
		return ERR_INV_PARAM;
	}
	F_CLOSE(serializer->fd);
	return OK;
}


// caller should free
static int desr_read_header(const cdsl_deserializer_t* self, cdsl_serializeHeader_t* header) {
	if(self == NULL) {
		return ERR_INV_PARAM;
	}

	file_deserializer_t* desr = container_of(self, file_deserializer_t, handle);
	uint16_t sof = 0;
	if(desr->fd <= 0) {
		return ERR_INV_PARAM;
	}
	do {
		if(F_READ(desr->fd, &sof, sizeof(uint16_t)) <= 0) {
			// fail to find start of file
			desr->is_eos_reached = TRUE;
			return ERR_RD_OP_FAIL;
		}
	} while(sof != SERIALIZER_START_OF_FILE);

	struct deserializer_header desr_header;
	if(F_READ(desr->fd, &desr_header, sizeof(struct deserializer_header)) <= 0) {
		// fail to read header
		desr->is_eos_reached = TRUE;
		return ERR_RD_OP_FAIL;
	}
	desr->has_next = (desr_header.has_next == HAS_NEXT);
	MEMCPY(header, &desr_header, sizeof(cdsl_serializeHeader_t));
	return OK;
}



static void* desr_get_next(const cdsl_deserializer_t* self,
                          cdsl_serializeNode_t* nodep,
                          size_t nsz,
                          const cdsl_memoryMngt_t* m_mngt) {

	if(self == NULL || m_mngt == NULL) {
		return NULL;
	}
	file_deserializer_t* desr = container_of(self, file_deserializer_t, handle);
	if(desr->fd < 0) {
		desr->is_eos_reached = TRUE;
		return NULL;
	}
	if(F_READ(desr->fd, nodep, nsz) < 0) {
		desr->is_eos_reached = TRUE;
		return NULL;
	}
	size_t node_hole_size = 0; //   if serialized data doesn't contain space which tree node struct is inserted into, \
                                    then allocate more size for that purpose

	if(!(nodep->flags & EMBEDDED_MSK)) {
		node_hole_size = nsz;
	}
	void* data = NULL;
	if(nodep->d_size > 0) {
		uint8_t ext_sz = nodep->d_offset + offsetof(cdsl_serializeNode_t, flags) - nsz;
		if(ext_sz > 0) {
			uint8_t _readout_buffer[ext_sz];
			if(F_READ(desr->fd, _readout_buffer, ext_sz) < 0) {
				desr->is_eos_reached = TRUE;
				return NULL;
			}
		}
		data = m_mngt->alloc(nodep->d_size + node_hole_size);
		if (F_READ(desr->fd, &data[node_hole_size], nodep->d_size) < 0) {
			desr->is_eos_reached = TRUE;
			goto FREE_AND_RETURN_NULL;
		}
	}
	struct deserializer_delim node_delim;
	if(F_READ(desr->fd, &node_delim, sizeof(struct deserializer_delim)) < 0) {
		desr->is_eos_reached = TRUE;
		goto FREE_AND_RETURN_NULL;
	}
	if(node_delim.ser_delim.delim != SERIALIZER_DELIM) {
		goto FREE_AND_RETURN_NULL;
	}
	if(node_delim.ser_delim.node_chs != serializer_calcNodeChecksum(nodep, data)) {
		goto FREE_AND_RETURN_NULL;
	}

	desr->has_next = (node_delim.has_next == HAS_NEXT);
	return data;

FREE_AND_RETURN_NULL:
	if(data) m_mngt->free(data);
	return NULL;
}

static BOOL desr_has_next(const cdsl_deserializer_t* self) {
	if(self == NULL) {
		return FALSE;
	}
	file_deserializer_t* desr = container_of(self, file_deserializer_t, handle);
	if(desr->fd < 0) {
		return FALSE;
	}
	return desr->has_next;
}

static int desr_read_tail(const cdsl_deserializer_t* self, cdsl_serializeTail_t* tailp) {
	if(!self || !tailp) {
		return ERR_INV_PARAM;
	}
	file_deserializer_t* desr = container_of(self, file_deserializer_t, handle);
	if(desr->fd <= 0) {
		return ERR_INV_PARAM;
	}
	if(F_READ(desr->fd, tailp, sizeof(cdsl_serializeTail_t))) {
		return ERR_RD_OP_FAIL;
	}
	return OK;
}

static int desr_close(const cdsl_deserializer_t* self) {
	if(!self) {
		return ERR_INV_PARAM;
	}
	file_deserializer_t* desr = container_of(self,file_deserializer_t, handle);
	if(desr->fd <= 0) {
		return ERR_INV_PARAM;
	}
	F_CLOSE(desr->fd);
	return OK;
}
