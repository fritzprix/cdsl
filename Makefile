## makefile for cdsl


CC=gcc
CPP=g++
AR=ar

CFLAG=
INC=
OBJS=
SRCS=
DIRS=



ifeq ($(BUILD),) 
	BUILD=DEBUG
endif



ifeq ($(BUILD),DEBUG)
	CFLAG +=	-g3\
				-O0
else
	CFLAG +=	-O2
endif




PROJECT_ROOT_DIR=$(CURDIR)
HEADER_ROOT=$(PROJECT_ROOT_DIR)/include
SOURCE_ROOT=$(PROJECT_ROOT_DIR)/source


TARGET=cdsl
ARCHIVE=libcdsl.a

include $(SOURCE_ROOT)/Makefile

.SILENT : $(TARGET) $(ARCHIVE) clean 

PHONY += all

all : $(DIRS) $(TARGET) $(ARCHIVE)



	
$(TARGET) : main.o $(OBJS)
	$(CC) -o $@ $(CFLAG) $(INC) $< $(OBJS)
	
$(ARCHIVE) : $(OBJS)
	$(AR) rcs $@ $<
	
	
main.o  : main.c 
	$(CC) $(CFLAG) -c -o $@ $< $(INC)



PHONY += clean

clean :
	rm -rf $(OBJS) $(TARGET) $(DIRS) $(ARCHIVE) main.o

.PHONY = $(PHONY)

 
	
