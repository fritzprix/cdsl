/*
 * fserializer.c
 *
 *  Created on: 2018. 1. 28.
 *      Author: innocentevil
 */

#include "fserializer.h"
#include "cdsl_defs.h"


static int ser_on_head(const cdsl_serializer_t* self, const cdsl_serializeHeader_t* head);
static int ser_on_next(const cdsl_serializer_t* self, const cdsl_serializeNode_t* node, size_t nsz, const uint8_t* data);
static int ser_on_tail(const cdsl_serializer_t* self, const cdsl_serializeTail_t* tail);
static int ser_close(const cdsl_serializer_t* self);


static int desr_get_header(const cdsl_deserializer_t* self, cdsl_serializeHeader_t* header);
static const cdsl_serializeNode_t* desr_get_next(const cdsl_deserializer_t* self, cdsl_alloc_t alloc);
static BOOL desr_has_next(const cdsl_deserializer_t* self);
static int desr_get_tail(const cdsl_deserializer_t* self, cdsl_serializeTail_t* tailp);
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

	deserializer->handle.get_head = desr_get_header;
	deserializer->handle.get_next = desr_get_next;
	deserializer->handle.get_tail = desr_get_tail;
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

struct serializer_delim {
	uint16_t                 node_chs; // cyclic checksum
	uint16_t                 delim;    // SERIALIZER_DELIM
};

static int ser_on_next(const cdsl_serializer_t* self, const cdsl_serializeNode_t* node, size_t nsz, const uint8_t* data){
	if((self == NULL) ||
			(node == NULL)) {
		return ERR_INV_PARAM;
	}
	const file_serializer_t* serializer = container_of(self, file_serializer_t, handle);
	if(serializer->fd < 0) {
		return ERR_INV_PARAM;
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
static int desr_get_header(const cdsl_deserializer_t* self, cdsl_serializeHeader_t* header) {
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

	if(F_READ(desr->fd, header, sizeof(cdsl_serializeHeader_t)) <= 0) {
		// fail to read header
		desr->is_eos_reached = TRUE;
		return ERR_RD_OP_FAIL;
	}

	PRINT("Valid Header!!\n");
	return OK;
}

static const cdsl_serializeNode_t* desr_get_next(const cdsl_deserializer_t* self, cdsl_alloc_t alloc) {
	if(self == NULL || alloc == NULL) {
		return NULL;
	}
	file_deserializer_t* desr = container_of(self, file_deserializer_t, handle);
	if(desr->fd < 0) {
		desr->is_eos_reached = TRUE;
		return NULL;
	}
	cdsl_serializeNode_t node_head = {0};
	if(F_READ(desr->fd, &node_head, sizeof(cdsl_serializeNode_t)) < 0) {
		desr->is_eos_reached = TRUE;
		return NULL;
	}

	size_t node_sz = node_head.d_size + node_head.d_offset + offsetof(cdsl_serializeNode_t, flags);
	cdsl_serializeNode_t* ptr_node = alloc(node_sz);
	uint8_t* cursor = ((uint8_t*) (&ptr_node[1]));
	MEMCPY(ptr_node, &node_head, sizeof(cdsl_serializeNode_t));
	PRINT("NODE Header (E_OFFSET : %u / DSIZE : %u /  D_OFFSET : %u)\n", ptr_node->e_offset, ptr_node->d_size, ptr_node->d_offset);
	size_t ext_sz = ptr_node->d_offset - sizeof(ptr_node->flags);
	if(ext_sz > sizeof(ptr_node->flags)) {
		if(F_READ(desr->fd, cursor, sizeof(ext_sz)) < 0) {
			desr->is_eos_reached = TRUE;
			return NULL;
		}
		cursor = &cursor[ext_sz];
	}
	int res;
	if((res = F_READ(desr->fd, cursor, ptr_node->d_size)) < 0) {
		desr->is_eos_reached = TRUE;
		return NULL;
	}
	struct serializer_delim node_delim;
	if(F_READ(desr->fd, &node_delim, sizeof(struct serializer_delim)) < 0) {
		desr->is_eos_reached = TRUE;
		return NULL;
	}
	if(node_delim.delim != SERIALIZER_DELIM) {
		PRINT("DELIM NOK %d \n", node_delim.delim);
		return NULL;
	}
	PRINT("DELIM OK!\n");
	void* data = SER_GET_DATA(ptr_node);
	const uint16_t chs = serializer_calcNodeChecksum(ptr_node, data);
	if(node_delim.node_chs != chs) {
		return NULL;
	}
	return ptr_node;
}

static BOOL desr_has_next(const cdsl_deserializer_t* self) {
	if(self == NULL) {
		return FALSE;
	}
	file_deserializer_t* desr = container_of(self, file_deserializer_t, handle);
	if(desr->fd < 0) {
		return FALSE;
	}
	return !desr->is_eos_reached;
}

static int desr_get_tail(const cdsl_deserializer_t* self, cdsl_serializeTail_t* tailp) {
	return OK;
}

static int desr_close(const cdsl_deserializer_t* self) {
	return OK;
}
