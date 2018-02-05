/*
 * base_tree.c
 *
 *  Created on: Nov 15, 2015
 *      Author: innocentevil
 */

#include "base_tree.h"
#include "cdsl_defs.h"
#include "arch.h"


#define GET_PTR(ptr)           ((base_treeNode_t*) (((__cdsl_uaddr_t) ptr) & ~1))
#define GET_NPTR(ptr)          (((__cdsl_uaddr_t) ptr) & 1)
#define SET_NPTR(ptr,v)        do { \
	ptr = (base_treeNode_t*) (((__cdsl_uaddr_t) ptr) | v);\
} while(0)

struct serialize_argument {
	const serializable_handler_t* ser_handler;
	const serializable_callback_t* ser_callback;
};

static int calc_max_depth_rc(base_treeNode_t** root);
static int calc_size_rc(base_treeNode_t** root);
static void print_rc(base_treeNode_t* current,cdsl_generic_printer_t prt,int depth);
static int traverse_incremental_rc(base_treeNode_t* current,int* current_order,base_tree_callback_t cb, void* arg);
static int traverse_decremental_rc(base_treeNode_t* current,int* current_order,base_tree_callback_t cb, void* arg);
static void traverse_target_rc(base_treeNode_t* current, int* order, trkey_t key, base_tree_callback_t cb,void* arg);
static void print_tab(int cnt);
static void serialize(const serializable_t* self, const serializable_handler_t* serialize_handler, const serializable_callback_t* cb);


void tree_serializer_init(base_treeRoot_t* rootp, serializable_t* serializer) {
	if(serializer == NULL) {
		return;
	}
	serializer->serialize = serialize;
	serializer->target = rootp;
}

DECLARE_FOREACH_CALLBACK(serialize_for_each) {
	const struct serialize_argument* args = (const serializable_handler_t*) arg;
	const serializable_handler_t* handler = args->ser_handler;
	->on_next(handle, node, sizeof(node));
	return TRAVERSE_OK;
}


static void serialize(const serializable_t* self, const serializable_handler_t* serialize_handler, const serializable_callback_t* cb) {
	if((self == NULL) || (serialize_handler == NULL) || (cb == NULL)) {
		return;
	}
	base_treeRoot_t* rootp = (base_treeRoot_t*) self->target;
	serialize_header_t header;

	struct serialize_argument args;
	args.ser_callback = cb;
	args.ser_handler = serialize_handler;

	header.type = BASE_TREE;
	header.ver = GET_SER_VERSION();
	serialize_handler->on_head(serialize_handler, &header);
	tree_for_each(self->target, serialize_for_each, ORDER_INC, &args);
	serialize_handler->on_tail(serialize_handler);
}



void tree_for_each(base_treeRoot_t* rootp, base_tree_callback_t cb,int order, void* arg)
{
	if((cb == NULL) || (rootp == NULL) || (GET_PTR(rootp->entry) == NULL))
		return;
	int i = 0;
	if(order == ORDER_DEC)
		traverse_decremental_rc(rootp->entry,&i,cb,arg);
	else
		traverse_incremental_rc(rootp->entry,&i,cb,arg);
}

void tree_for_each_to_target(base_treeRoot_t* rootp, base_tree_callback_t cb, trkey_t key, void* arg) {
	if((cb == NULL) || (rootp == NULL) || (GET_PTR(rootp->entry) == NULL))
		return;
	int i = 0;
	traverse_target_rc(rootp->entry, &i, key, cb, arg);
}


base_treeNode_t* tree_top(base_treeRoot_t* rootp) {
	if(!rootp)
		return NULL;
	return GET_PTR(rootp->entry);
}

base_treeNode_t* tree_go_left(base_treeNode_t* cur) {
	if(!cur)
		return NULL;
	return GET_PTR(GET_PTR(cur)->left);
}

base_treeNode_t* tree_go_right(base_treeNode_t* cur) {
	if(!cur)
		return NULL;
	return GET_PTR(GET_PTR(cur)->right);
}

int tree_size(base_treeRoot_t* rootp)
{
	if(rootp == NULL)
		return 0;
	return calc_size_rc(&rootp->entry);
}

void tree_print(base_treeRoot_t* rootp,cdsl_generic_printer_t print)
{
	if(rootp == NULL)
		return;
	print_rc(rootp->entry,print,0);
}

int tree_max_depth(base_treeRoot_t* rootp)
{
	if(rootp == NULL)
		return 0;
	return calc_max_depth_rc(&rootp->entry);
}

BOOL tree_is_empty(base_treeRoot_t* rootp){
	if(!rootp)
		return TRUE;
	return (rootp->entry == NULL);
}

base_treeNode_t* tree_min(base_treeRoot_t* rootp) {
	if(!rootp)
		return NULL;
	base_treeNode_t* cur = rootp->entry;
	while(GET_PTR(cur)->left) {
		cur = GET_PTR(cur)->left;
	}
	return GET_PTR(cur);
}

base_treeNode_t* tree_max(base_treeRoot_t* rootp) {
	if(!rootp)
		return NULL;
	base_treeNode_t* cur = rootp->entry;
	while(GET_PTR(cur)->right) {
		cur = GET_PTR(cur)->right;
	}
	return GET_PTR(cur);
}

base_treeNode_t* tree_update(base_treeRoot_t* rootp, base_treeNode_t* nitem) {
	if(!rootp || !nitem)
		return NULL;
	base_treeNode_t* parent = NULL, *current = rootp->entry;
	uint8_t ctx = 0;
	while(current) {
		if(nitem->key > GET_PTR(current)->key) {
			parent = current;
			current = GET_PTR(current)->right;
			ctx = 1;
		} else if(nitem->key < GET_PTR(current)->key) {
			parent = current;
			current = GET_PTR(current)->left;
			ctx = 0;
		} else {
			nitem->left = GET_PTR(current)->left;
			nitem->right = GET_PTR(current)->right;
			if(!parent) {
				rootp->entry = nitem;
			} else {
				SET_NPTR(nitem,GET_NPTR(current));
				if(ctx == 1) {
					GET_PTR(parent)->right = nitem;
				} else {
					GET_PTR(parent)->left = nitem;
				}
			}
			return GET_PTR(current);
		}
	}
	return NULL;
}



static int calc_size_rc(base_treeNode_t** root)
{
	int cnt = 0;
	if((root == NULL) || (GET_PTR(*root) == NULL))
		return 0;
	if(GET_PTR(*root))
		cnt = 1;
	if(!GET_PTR(GET_PTR(*root)->left) && !GET_PTR(GET_PTR(*root)->right)) return cnt;
	if(GET_PTR(GET_PTR(*root)->left))
		cnt += calc_size_rc(&GET_PTR(*root)->left);
	if(GET_PTR(GET_PTR(*root)->right))
		cnt += calc_size_rc(&GET_PTR(*root)->right);
	return cnt;
}


static int calc_max_depth_rc(base_treeNode_t** root)
{
	if((root == NULL) || (GET_PTR(*root) == NULL))
		return 0;
	int max = 0;
	int temp = 0;
	if(max < (temp = calc_max_depth_rc(&GET_PTR(*root)->left)))
		max = temp;
	if(max < (temp = calc_max_depth_rc(&GET_PTR(*root)->right)))
		max = temp;
	return max + 1;
}

static void print_rc(base_treeNode_t* current,cdsl_generic_printer_t print,int depth)
{
	if(!GET_PTR(current))
		return;
	print_rc(GET_PTR(current)->right,print,depth + 1);
	print_tab(depth); if(print) print(GET_PTR(current));
	print_rc(GET_PTR(current)->left,print,depth + 1);
}

static void print_tab(int cnt)
{
	while(cnt--)
		PRINT("\t");
}

static int traverse_incremental_rc(base_treeNode_t* current, int* current_order, base_tree_callback_t cb, void* arg)
{
	if(GET_PTR(current) == NULL)
		return 0;
	switch(traverse_incremental_rc(GET_PTR(current)->left,current_order,cb,arg)) {
	case TRAVERSE_BREAK:
		return TRAVERSE_BREAK;
	}
	*current_order += 1;
	switch(cb(*current_order, GET_PTR(current),arg)) {
	case TRAVERSE_BREAK:
		return TRAVERSE_BREAK;
	}
	return traverse_incremental_rc(GET_PTR(current)->right,current_order,cb,arg);
}

static int traverse_decremental_rc(base_treeNode_t* current, int* current_order, base_tree_callback_t cb, void* arg)
{
	if(GET_PTR(current) == NULL)
		return 0;
	switch(traverse_decremental_rc(GET_PTR(current)->right,current_order,cb, arg)) {
	case TRAVERSE_BREAK:
		return TRAVERSE_BREAK;
	}
	*current_order += 1;
	switch(cb(*current_order,GET_PTR(current), arg)) {
	case TRAVERSE_BREAK:
		return TRAVERSE_BREAK;
	}
	return traverse_decremental_rc(GET_PTR(current)->left,current_order,cb, arg);
}


static void traverse_target_rc(base_treeNode_t* current, int* order, trkey_t key, base_tree_callback_t cb, void* arg) {
	if(GET_PTR(current) == NULL)
		return;
	if(GET_PTR(current)->key > key) {
		traverse_target_rc(GET_PTR(current)->left, order, key, cb,arg);
		return;
	} else if(GET_PTR(current)->key < key) {
		traverse_target_rc(GET_PTR(current)->right, order, key, cb,arg);
		return;
	}
	switch(cb((*order)++, GET_PTR(current),arg)) {
	case TRAVERSE_BREAK:
		/**
		 *  currently, this switch block has not much to do
		 */
		break;
	}
}
