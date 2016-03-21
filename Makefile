## makefile for cdsl


CC=gcc
CXX=clang++-3.6
AR=ar
PYTHON=python
PIP=pip
MKDIR=mkdir

DBG_CFLAG = -O0 -g3 -fmessage-length=0 
REL_CFLAG = -O2 -g0 -fmessage-length=0
DYNAMIC_FLAG = -shared -fPIC


PROJECT_ROOT_DIR=$(CURDIR)
HEADER_ROOT=$(PROJECT_ROOT_DIR)/include
SOURCE_ROOT=$(PROJECT_ROOT_DIR)/source
TOOL_DIR=$(PROJECT_ROOT_DIR)/tools

CONFIG_PY=$(TOOL_DIR)/jconfigpy/jconfigpy.py

TEST_TARGET=cdsl

DBG_STATIC_TARGET=libcdsld.a
DBG_DYNAMIC_TARGET=libcdsld.so

REL_STATIC_TARGET=libcdsl.a
REL_DYNAMIC_TARGET=libcdsl.so

-include .config

VPATH=$(SRC-y)
INCS=$(INC-y:%=-I%)

DBG_OBJS=$(OBJ-y:%=$(DBG_CACHE_DIR)/%.do)
REL_OBJS=$(OBJ-y:%=$(REL_CACHE_DIR)/%.o)

DBG_CACHE_DIR=Debug
REL_CACHE_DIR=Release

SILENT+= $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET) $(DBG_OBJS)
SILENT+= $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET) $(REL_OBJS)
SILENT+= $(TEST_TARGET) $(REL_CACHE_DIR)/main.o 


.SILENT :  $(SILENT) clean 

PHONY+= all debug release clean test

all : debug 

debug : $(DBG_CACHE_DIR) $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET)

release : $(REL_CACHE_DIR) $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET)

test : $(REL_CACHE_DIR)  $(TEST_TARGET) 

config : $(CONFIG_PY)
	$(PYTHON) $(CONFIG_PY) -c -i config.json

$(CONFIG_PY):
	$(PIP) install jconfigpy -t $(TOOL_DIR)

$(DBG_CACHE_DIR) $(REL_CACHE_DIR) :
	$(MKDIR) $@
	
$(DBG_STATIC_TARGET) : $(DBG_OBJS)
	@echo 'Generating Archive File ....$@'
	$(AR) rcs -o $@  $(DBG_OBJS)
	
$(DBG_DYNAMIC_TARGET) : $(DBG_OBJS)
	@echo 'Generating Share Library File .... $@'
	$(CC) -o $@ $(DBG_CFLAG) $(DYNAMIC_FLAG) $(DBG_OBJS)

$(REL_STATIC_TARGET) : $(REL_OBJS)
	@echo 'Generating Archive File ....$@'
	$(AR) rcs -o $@ $(REL_OBJS)
	
$(REL_DYNAMIC_TARGET) : $(REL_OBJS)
	@echo 'Generating Share Library File .... $@'
	$(CC) -o $@ $(REL_CFLAG) $(DYNAMIC_FLAG) $(REL_OBJS)
	
$(TEST_TARGET) : $(REL_CACHE_DIR)/main.o $(REL_OBJS) 
	@echo 'Building unit-test executable... $@'
	$(CC) -o $@ $(REL_CFLAG) $< $(REL_OBJS)
	
$(DBG_CACHE_DIR)/%.do : %.c
	@echo 'compile...$@'
	$(CC) -c -o $@ $(DBG_CFLAG) $(DYNAMIC_FLAG) $< $(INCS)
	
$(REL_CACHE_DIR)/%.o : %.c
	@echo 'compile...$@'
	$(CC) -c -o $@ $(REL_CFLAG) $(DYNAMIC_FLAG) $< $(INCS)
	
PHONY += clean

clean : 
	rm -rf $(DBG_CACHE_DIR) $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET)\
			$(REL_CACHE_DIR) $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET)\
			$(TEST_TARGET)

.PHONY = $(PHONY)

 
	
