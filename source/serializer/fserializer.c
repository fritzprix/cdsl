/*
 * fserializer.c
 *
 *  Created on: 2018. 1. 28.
 *      Author: innocentevil
 */

#include "cdsl_defs.h"
#include "fserializer.h"

static int on_head(const serializer_t* self, const serializable_header_t* head);
static int on_next(const serializer_t* self, const uint8_t* next, size_t sz);
static void on_tail(const serializer_t* self, int res);

void file_serializerInit(file_serialzer_t* serializer, const char* filename) {

	if((serializer == NULL) || (filename == NULL)) {
		return;
	}

	//TODO : set mode for file open
	const int fd = F_OPEN(filename,1);
	if(fd < 0) {
		return;
	}

	serializer->fd = fd;
	serializer->header.chksum = 0;
	serializer->header.count = 0;
	serializer->header.size = 0;

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



static int on_head(const serializer_t* self, const serializable_header_t* head) {
	if((self == NULL) ||
			(head == NULL)) {
		return ERR_INV_PARAM;
	}
	int res;
	const file_serialzer_t* serializer = container_of(self, file_serialzer_t, handle);
	if(serializer->fd < 0) {
		return ERR_INV_PARAM;
	}
	if((res = F_WRITE(serializer->fd, head, sizeof(head))) < 0) {
		return ERR_WR_OP_FAIL;
	}
	// write unfilled serializer header into file to reserve space for update at the end of serialization
	if((res = F_WRITE(serializer->fd, &serializer->header, sizeof(serializable_header_t))) < 0) {
		return ERR_WR_OP_FAIL;
	}
	return 0;
}

static int on_next(const serializer_t* self, const uint8_t* next, size_t sz) {
	if((self == NULL) ||
			(next == NULL) ||
			(sz > 0)) {
		return ERR_INV_PARAM;
	}
	int res;
	const file_serialzer_t* serializer = container_of(self, file_serialzer_t, handle);
	if(serializer->fd < 0) {
		return ERR_INV_PARAM;
	}
	return 0;
}

static void on_tail(const serializer_t* self, int result) {

}
