/*
 * fserializer.c
 *
 *  Created on: 2018. 1. 28.
 *      Author: innocentevil
 */

#include "cdsl_defs.h"
#include "fserializer.h"
static int on_head(const cdsl_serializer_t* self, const cdsl_serializeHeader_t* head);
static int on_next(const cdsl_serializer_t* self, const cdsl_serializeNode_t* node, size_t nsz, const uint8_t* data);
static int on_tail(const cdsl_serializer_t* self, const cdsl_serializeTail_t* tail);

void file_serializerInit(file_serialzer_t* serializer, const char* filename) {

	if((serializer == NULL) || (filename == NULL)) {
		return;
	}

	//TODO : set mode for file open
	const int fd = F_OPEN(filename, FO_RDWR);
	if(fd < 0) {
		PRINT("fail to open\n");
		return;
	}

	serializer->fd = fd;

	serializer->handle.on_head = on_head;
	serializer->handle.on_next = on_next;
	serializer->handle.on_tail = on_tail;
}

void file_serializerExit(file_serialzer_t* serializer) {
	if((serializer == NULL) ||
			(serializer->fd < 0)) {
		return;
	}
	F_CLOSE(serializer->fd);
}



static int on_head(const cdsl_serializer_t* self, const cdsl_serializeHeader_t* head) {
	if((self == NULL) ||
			(head == NULL)) {
		return ERR_INV_PARAM;
	}
	uint16_t ser_starter = SERIALIZER_START_OF_FILE;
	const file_serialzer_t* serializer = container_of(self, file_serialzer_t, handle);
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

static int on_next(const cdsl_serializer_t* self, const cdsl_serializeNode_t* node, size_t nsz,const uint8_t* data){
	if((self == NULL) ||
			(node == NULL)) {
		return ERR_INV_PARAM;
	}
	const file_serialzer_t* serializer = container_of(self, file_serialzer_t, handle);
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

static int on_tail(const cdsl_serializer_t* self, const cdsl_serializeTail_t* tail) {
	if((self == NULL) || (tail == NULL)) {
		return ERR_INV_PARAM;
	}
	uint16_t eof = SERIALIZER_END_OF_FILE;
	const file_serialzer_t* serializer = container_of(self, file_serialzer_t, handle);
	if(F_WRITE(serializer->fd, tail, sizeof(cdsl_serializeTail_t)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	if(F_WRITE(serializer->fd, &eof, sizeof(uint16_t)) < 0) {
		return ERR_WR_OP_FAIL;
	}
	return OK;

}
