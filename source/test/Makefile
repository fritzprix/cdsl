
TEST_GEN_DIR=$(OBJGEN_DIR)/test
TEST_SRC_DIR=$(SOURCE_ROOT)/test
TEST_HEADER_DIR=$(HEADER_ROOT)/test

TEST_SRCS=cdsl_bstree_test.c\
	cdsl_heap_test.c\
	cdsl_list_test.c\
	cdsl_rbtree_test.c\
	cdsl_spltree_test.c

TEST_OBJS=$(TEST_SRCS:%.c=$(TEST_GEN_DIR)/%.o)

INC+=-I$(TEST_HEADER_DIR)
DIRS+=$(TEST_GEN_DIR)
OBJS+=$(TEST_OBJS)



$(TEST_GEN_DIR)/%.o : $(TEST_SRC_DIR)/%.c
	$(CC) $(CFLAG) -c -o $@ $<  $(INC) 

$(TEST_GEN_DIR) :
	mkdir $@
	
