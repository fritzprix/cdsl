# makefile for cdsl

CC=clang-3.6 
CXX=g++
AR=llvm-ar-3.6
PYTHON=python
PIP=pip
MKDIR=mkdir

DBG_CFLAG = -O0 -g3 -fmessage-length=0  $(CFLAG) -D__DBG
REL_CFLAG = -O2 -g0 -fmessage-length=0  $(CFLAG)
DYNAMIC_FLAG = -fPIC


PROJECT_ROOT_DIR=$(CURDIR)
HEADER_ROOT=$(PROJECT_ROOT_DIR)/include
SOURCE_ROOT=$(PROJECT_ROOT_DIR)/source
TOOL_DIR=$(PROJECT_ROOT_DIR)/tools

CONFIG_PY=$(TOOL_DIR)/jconfigpy/jconfigpy.py

TEST_TARGET=cdsl
DEV_TEST_TARGET=cdsl_dev

DBG_STATIC_TARGET=libcdsld.a
DBG_DYNAMIC_TARGET=libcdsld.so

REL_STATIC_TARGET=libcdsl.a
REL_DYNAMIC_TARGET=libcdsl.so

-include .config

VPATH=$(SRC-y)
INCS=$(INC-y:%=-I%)
LIBS=$(LIB-y:%=-l%)

DBG_OBJS=$(OBJ-y:%=$(DBG_CACHE_DIR)/%.do)
REL_OBJS=$(OBJ-y:%=$(REL_CACHE_DIR)/%.o)

DBG_SH_OBJS=$(OBJ-y:%=$(DBG_CACHE_DIR)/%.s.do)
REL_SH_OBJS=$(OBJ-y:%=$(REL_CACHE_DIR)/%.s.o)

DBG_CACHE_DIR=Debug
REL_CACHE_DIR=Release

CONFIG_DIR=./source/arch/$(ARCH)/configs

SILENT+= $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET) $(DBG_OBJS)
SILENT+= $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET) $(REL_OBJS)
SILENT+= $(DBG_SH_OBJS) $(REL_SH_OBJS)
SILENT+= $(TEST_TARGET) $(REL_CACHE_DIR)/main.o  $(DEV_TEST_TARGET) $(DBG_CACHE_DIR)/main.do


.SILENT :  $(SILENT) clean 

PHONY+= all debug release clean test

all : debug 

debug : $(DBG_CACHE_DIR) $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET)

release : $(REL_CACHE_DIR) $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET)

test : $(REL_CACHE_DIR) $(DBG_CACHE_DIR) $(TEST_TARGET) $(DEV_TEST_TARGET)

defconf : $(CONFIG_DIR)
	cp -rf .config $(CONFIG_DIR)/$(SUB_ARCH)_config


ifeq ($(DEFCONF),)
config : $(CONFIG_PY)
	$(PYTHON) $(CONFIG_PY) -c -i config.json
else
ifeq ($(ARCH),)
config :
	$(error "ARCH must be specified!!")	
endif
config : $(CONFIG_PY)
	@echo 'config path $(CONFIG_DIR)'
	$(PYTHON) $(CONFIG_PY) -s -i $(CONFIG_DIR)/$(DEFCONF)_config -t ./config.json -o ./.config 
endif

$(CONFIG_PY):
	$(PIP) install jconfigpy -t $(TOOL_DIR)

$(DBG_CACHE_DIR) $(REL_CACHE_DIR) $(CONFIG_DIR) :
	$(MKDIR) $@
	
$(DBG_STATIC_TARGET) : $(DBG_OBJS)
	@echo 'Generating Archive File for $(ARCH) ....$@'
	$(AR) rcs  $@  $(DBG_OBJS)

ifneq ($(BAREMETAL),y)
$(DBG_DYNAMIC_TARGET) : $(DBG_SH_OBJS)
	@echo 'Generating Share Library File for $(ARCH) .... $@'
	$(CC) -o $@ -shared $(DBG_CFLAG) $(DYNAMIC_FLAG) $(DBG_SH_OBJS)
else
PHONY+=$(DBG_DYNMAIC_TARGET)
$(DBG_DYNAMIC_TARGET) : 
	@echo 'Shared Object is skipped for baremetal'
endif

$(REL_STATIC_TARGET) : $(REL_OBJS)
	@echo 'Generating Archive File for $(ARCH) ....$@'
	$(AR) rcs  $@ $(REL_OBJS)

ifneq ($(BAREMETAL),y)
$(REL_DYNAMIC_TARGET) : $(REL_SH_OBJS)
	@echo 'Generating Share Library File for $(ARCH) .... $@'
	$(CC) -o $@ -shared $(REL_CFLAG) $(DYNAMIC_FLAG) $(REL_SH_OBJS)
else
PHONY+=$(REL_DYNAMIC_TARGET)
$(REL_DYNAMIC_TARGET) :
	@echo 'Shared Object is skipped for baremetal'
endif
	
$(TEST_TARGET) : $(REL_CACHE_DIR)/main.o $(REL_OBJS) 
	@echo 'Building unit-test executable... for $(ARCH) $@'
	$(CXX) -o $@ $(REL_CFLAG) $< $(REL_OBJS) $(LIBS)
	

$(DEV_TEST_TARGET) : $(DBG_CACHE_DIR)/main.do $(DBG_OBJS) 
	@echo 'Building unit-test executable... for $(ARCH) $@'
	$(CXX) -o $@ $(DBG_CFLAG) $< $(DBG_OBJS) $(LIBS)
	
$(DBG_CACHE_DIR)/%.do : %.c
	@echo '$(ARCH) compile...$@'
	$(CC) -c -o $@ $(DBG_CFLAG)  $< $(INCS) 
	
$(REL_CACHE_DIR)/%.o : %.c
	@echo '$(ARCH) compile...$@'
	$(CC) -c -o $@ $(REL_CFLAG)  $< $(INCS)
	
$(DBG_CACHE_DIR)/%.s.do : %.c
	@echo '$(ARCH) compile...$@'
	$(CC) -c -o $@ $(DBG_CFLAG)  $< $(INCS) $(DYNAMIC_FLAG)
	
$(REL_CACHE_DIR)/%.s.o : %.c
	@echo '$(ARCH) compile...$@'
	$(CC) -c -o $@ $(REL_CFLAG)  $< $(INCS) $(DYNAMIC_FLAG)
	
$(DBG_CACHE_DIR)/%.do : %.cpp
	@echo '$(ARCH) compile...$@'
	$(CXX) -c -o $@ $(DBG_CFLAG)  $< $(INCS)
	
$(REL_CACHE_DIR)/%.o : %.cpp
	@echo '$(ARCH) compile...$@'
	$(CXX) -c -o $@ $(REL_CFLAG)  $< $(INCS)
	
$(DBG_CACHE_DIR)/%.s.do : %.cpp
	@echo '$(ARCH) compile...$@'
	$(CXX) -c -o $@ $(DBG_CFLAG)  $< $(INCS) $(DYNAMIC_FLAG)
	
$(REL_CACHE_DIR)/%.s.o : %.cpp
	@echo '$(ARCH) compile...$@'
	$(CXX) -c -o $@ $(REL_CFLAG)  $< $(INCS) $(DYNAMIC_FLAG)
	
PHONY += clean

clean : 
	rm -rf $(DBG_CACHE_DIR) $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET)\
			$(REL_CACHE_DIR) $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET)\
			$(TEST_TARGET) $(REL_SH_OBJS) $(DBG_SH_OBJS) $(DEV_TEST_TARGET)
			
config_clean:
	rm -rf $(CONFIG_TARGET) $(CONFIG_AUTOGEN) $(AUTOGEN_DIR) $(REPO-y) $(LDIR-y) .config 

.PHONY = $(PHONY)

 
	
