
SRCS=cdsl_bstree.c\
	cdsl_heap.c\
	cdsl_slist.c\
	cdsl_dlist.c\
	cdsl_rbtree.c\
	cdsl_spltree.c

	
OBJGEN_DIR=$(PROJECT_ROOT_DIR)/$(BUILD)
	

INC +=-I$(HEADER_ROOT)\
	  -I$(PROJECT_ROOT_DIR)


DIRS+=$(OBJGEN_DIR)
OBJS+=$(SRCS:%.c=$(OBJGEN_DIR)/%.o)


include $(SOURCE_ROOT)/test/Makefile


$(OBJGEN_DIR)/%.o : $(SOURCE_ROOT)/%.c
	$(CC) $(CFLAG) -c -o $@ $<  $(INC) 

	
$(OBJGEN_DIR) : 
	mkdir $@
 
 	
 


