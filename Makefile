## makefile for cdsl


CC=gcc
CPP=g++

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

include $(SOURCE_ROOT)/Makefile



all : $(DIRS) $(TARGET)



	
$(TARGET) : main.o $(OBJS)
	$(CC) -o $@ $(CFLAG) $(INC) $< $(OBJS)
	
main.o  : main.c 
	$(CC) $(CFLAG) -c -o $@ $< $(INC)



clean :
	rm -rf $(OBJS) $(TARGET) $(DIRS) main.o

 
	
