/*
 * cdsl_avltree.c
 *
 *  Created on: Jun 3, 2015
 *      Author: innocentevil
 */

#include "cdsl_avltree.h"


#define DIR_LEFT          ((uint8_t) 1)
#define DIR_RIGHT         ((uint8_t) 2)

#define PATTERN_MASK      ((uint8_t) 0x0F)
#define LEFTRIGTH_PATTERN ((uint8_t) (DIR_RIGHT << 2) | DIR_LEFT)
#define LEFTLEFT_PATTERN  ((uint8_t) (DIR_LEFT << 2) | DIR_LEFT)
#define RIGHTLEFT_PATTERN ((uint8_t) (DIR_LEFT << 2) | DIR_RIGHT)
#define RIGHTRIGHT_PATTER ((uint8_t) (DIR_RIGHT << 2) | DIR_RIGHT)

static avltreeNode_t* insert_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t* item, uint8_t *rc_dir, avltreeNode_t** replaced);
static avltreeNode_t* delete_rc(int bal, avltreeNode_t* sub_root_c, trkey_t key, uint8_t *rc_dir);

static avltreeNode_t* rotate_right(avltreeNode_t* sub_root);
static avltreeNode_t* rotate_left(avltreeNode_t* sub_root);

static int max_child_height(avltreeNode_t* node);




void cdsl_avltreeRootInit(avltreeRoot_t* rootp, int bal) {
	if(!rootp) {
		return;
	}
	PRINT("LEFT-LEFT : %d\n", LEFTLEFT_PATTERN);
	PRINT("LEFT_RIGHT : %d\n", LEFTRIGTH_PATTERN);
	PRINT("RIGHT-RIGHT : %d\n", RIGHTRIGHT_PATTER);
	PRINT("RIGHT-LEFT : %d\n", RIGHTLEFT_PATTERN);
	rootp->balance_factor = bal;
	rootp->entry = NULL;
}

void cdsl_avltreeNodeInit(avltreeNode_t* node, trkey_t key) {
	if(!node) {
		return;
	}
	node->height = 0;
	node->key = key;
	node->left = node->right = NULL;
}

/*!
 * insert element to tree
 *
 */
avltreeNode_t* cdsl_avltreeInsert(avltreeRoot_t* rootp,avltreeNode_t* item, BOOL is_set) {
	if(!rootp || !item) {
		return NULL;
	}
	uint8_t dir = 0;
	avltreeNode_t* replaced = NULL;
	rootp->entry = insert_rc(rootp->balance_factor, rootp->entry, item, &dir, is_set? &replaced : NULL);
	return replaced;
}

avltreeNode_t* cdsl_avltreeLookup(avltreeRoot_t* rootp,trkey_t key) {
	if(!rootp) {
		return NULL;
	}
	avltreeNode_t* cur = rootp->entry;
	while(cur) {
		if(cur->key > key) {
			cur = cur->left;
		} else if(cur->key < key) {
			cur = cur->right;
		} else {
			return cur;
		}
	}
	return NULL;
}

avltreeNode_t* cdsl_avltreeDeleteReplace(avltreeRoot_t* rootp,trkey_t key, base_tree_replacer_t replacer, void* cb_arg) {
	if(!rootp) {
		return NULL;
	}
	return NULL;
}

avltreeNode_t* cdsl_avltreeDeleteMinReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg) {
	if(!rootp) {
		return NULL;
	}
	return NULL;
}

avltreeNode_t* cdsl_avltreeDeleteMaxReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg) {
	if(!rootp) {
		return NULL;
	}
	return NULL;
}

static avltreeNode_t* delete_rc(int bal, avltreeNode_t* sub_root_c, trkey_t key, uint8_t *rc_dir) {
	if(!sub_root_c) {
		return NULL;
	}
	if(sub_root_c->key > key) {
		sub_root_c->left = delete_rc(bal, sub_root_c->left, key, rc_dir);
		*rc_dir |= DIR_LEFT;
	} else if(sub_root_c->key < key) {
		sub_root_c->right = delete_rc(bal, sub_root_c->right, key, rc_dir);
		*rc_dir |= DIR_RIGHT;
	}
	return NULL;
}


static avltreeNode_t* insert_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t* item, uint8_t *rc_dir,  avltreeNode_t** replaced) {
	if(!sub_root_c) {
		item->height = 1;
		return item;
	}
	if(sub_root_c->key > item->key) {
		sub_root_c->left = insert_rc(bal, sub_root_c->left, item, rc_dir, replaced);
		sub_root_c->height = sub_root_c->left->height + 1;
		*rc_dir |= DIR_LEFT;
	} else if(sub_root_c->key < item->key) {
		sub_root_c->right = insert_rc(bal, sub_root_c->right, item, rc_dir, replaced);
		sub_root_c->height = sub_root_c->right->height + 1;
		*rc_dir |= DIR_RIGHT;
	} else {
		if(*replaced) {
			*replaced = sub_root_c;
			item->left = sub_root_c->left;
			item->right = sub_root_c->right;
			item->height = sub_root_c->height;
			return item;
		} else {
			sub_root_c->left = insert_rc(bal, sub_root_c->left, item, rc_dir, replaced);
			sub_root_c->height = sub_root_c->left->height + 1;
			*rc_dir |= DIR_LEFT;
		}
	}
	PRINT("DIR : %d\n",(*rc_dir & PATTERN_MASK));
	switch((*rc_dir) & PATTERN_MASK) {
	case RIGHTLEFT_PATTERN:
		/*
		 *
		 */
		if(!sub_root_c->left) {

			if(!(sub_root_c->height > bal)) {
				break;
			}
		} else {
			if(!(sub_root_c->right->height - sub_root_c->left->height > bal)) {

				break;
			}
		}
		sub_root_c->right = rotate_right(sub_root_c->right);
		sub_root_c = rotate_left(sub_root_c);
		break;
	case RIGHTRIGHT_PATTER:
		if(!sub_root_c->left) {

			if(!(sub_root_c->height > bal)) {
				break;
			}
		} else {
			if(!(sub_root_c->right->height - sub_root_c->left->height > bal)) {

				break;
			}
		}
		sub_root_c = rotate_left(sub_root_c);
		break;
	case LEFTLEFT_PATTERN:
		if(!sub_root_c->right) {

			if(!(sub_root_c->left->height > bal)) {
				break;
			}
		} else {
			if(!(sub_root_c->left->height - sub_root_c->right->height > bal)) {

				break;
			}
		}
		sub_root_c = rotate_right(sub_root_c);
		break;
	case LEFTRIGTH_PATTERN:
		if(!sub_root_c->right) {

			if(!(sub_root_c->left->height > bal)) {
				break;
			}
		} else {
			if(!(sub_root_c->left->height - sub_root_c->right->height > bal)) {

				break;
			}
		}
		sub_root_c->left = rotate_left(sub_root_c->left);
		sub_root_c = rotate_right(sub_root_c);
		break;



	}
	*rc_dir <<= 2;
	return sub_root_c;
}

static avltreeNode_t* rotate_right(avltreeNode_t* sub_root) {
	if(!sub_root) {
		return sub_root;
	}
	avltreeNode_t* nroot = sub_root->left;
	sub_root->left = nroot->right;
	nroot->right = sub_root;

	nroot->height = nroot->left->height + 1;
	nroot->right->height = max_child_height(nroot->right) + 1;

	return nroot;
}

static avltreeNode_t* rotate_left(avltreeNode_t* sub_root) {
	if(!sub_root) {
		return sub_root;
	}
	avltreeNode_t* nroot = sub_root->right;
	sub_root->right = nroot->left;
	nroot->left = sub_root;

	nroot->height = nroot->right->height + 1;
	nroot->left->height = max_child_height(nroot->left) + 1;
	return nroot;
}

static int max_child_height(avltreeNode_t* node) {
	if(node->right && node->left) {
		return node->right->height > node->left->height? node->right->height : node->left->height;
	} else if(node->left) {
		return node->left->height;
	} else if(node->right){
		return node->right->height;

	}
	return 0;
}


