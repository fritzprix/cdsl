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


static const cdsl_serializeHeader_t* desr_get_header(const cdsl_deserializer_t* self, cdsl_alloc_t* alloc);
static const cdsl_serializeNode_t* desr_get_next(const cdsl_deserializer_t* self, cdsl_alloc_t* malloc);
static BOOL desr_has_next(const cdsl_deserializer_t* self);
static const cdsl_serializeTail_t* desr_get_tail(const cdsl_deserializer_t* self, cdsl_alloc_t* alloc);
static int desr_close(const cdsl_deserializer_t* self);

void file_serializerOpen(file_serializer_t* serializer, const char* filename) {

	if((serializer == NULL) || (filename == NULL)) {
		return;
	}

	serializer->handle.on_head = ser_on_head;
	serializer->handle.on_next = ser_on_next;
	serializer->handle.on_tail = ser_on_tail;
	serializer->handle.close = ser_close;

	const int fd = F_OPEN(filename, FO_RDWR);
	if(fd < 0) {
		PRINT("fail to open\n");
		return;
	}

	serializer->fd = fd;
}

void file_serializerClose(file_serializer_t* serializer) {
	if((serializer == NULL) ||
			(serializer->fd < 0)) {
		return;
	}
	F_CLOSE(serializer->fd);
}

void file_deserializerOpen(file_deserializer_t* deserializer, const char* filename) {
	if((deserializer == NULL) || (filename == NULL)) {
		return;
	}

	deserializer->handle.get_head = desr_get_header;
	deserializer->handle.get_next = desr_get_next;
	deserializer->handle.get_tail = desr_get_tail;
	deserializer->handle.has_next = desr_has_next;
	deserializer->handle.close = desr_close;
}

void file_deserializerClose(file_deserializer_t* deserializer) {

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
	if(F_WRITE(serializer->fd, head, sizeof(head)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	return OK;
}

static int ser_on_next(const cdsl_serializer_t* self, const cdsl_serializeNode_t* node, size_t nsz,const uint8_t* data){
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
	uint16_t node_term[2] = {0};
	if(node->d_size > 0) {
		node_term[0] = serializer_calcNodeChecksum(node, data);
		if (F_WRITE(serializer->fd, data, node->d_size) < 0) {
			return ERR_WR_OP_FAIL;
		}
	}
	node_term[1] = SERIALIZER_DELIM;
	if(F_WRITE(serializer->fd, node_term, sizeof(uint32_t)) < 0) {
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


static const cdsl_serializeHeader_t* desr_get_header(const cdsl_deserializer_t* self, cdsl_alloc_t* alloc) {
	if(self == NULL) {
		return NULL;
	}

	file_deserializer_t* desr = container_of(self, file_deserializer_t, handle);
	uint16_t sof = 0;
	if(desr->fd <= 0) {
		return NULL;
	}
	cdsl_serializeHeader_t header;
	do {
		if(F_READ(desr->fd, &sof, sizeof(uint16_t)) <= 0) {
			// fail to find start of file
			return NULL;
		}
	} while(sof != SERIALIZER_START_OF_FILE);

	if(F_READ(desr->fd, &header, sizeof(header)) <= 0) {
		// fail to read header
		return NULL;
	}

	return (const cdsl_serializeHeader_t*) &header;
}

static const cdsl_serializeNode_t* desr_get_next(const cdsl_deserializer_t* self, cdsl_alloc_t* alloc) {
	if(self == NULL || alloc == NULL) {
		return NULL;
	}
	return NULL;
}

static BOOL desr_has_next(const cdsl_deserializer_t* self) {
	return TRUE;
}

static const cdsl_serializeTail_t* desr_get_tail(const cdsl_deserializer_t* self, cdsl_alloc_t* alloc) {
	return NULL;
}

static int desr_close(const cdsl_deserializer_t* self) {
	return OK;
}
