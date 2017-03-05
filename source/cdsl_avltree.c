/*
 * cdsl_avltree.c
 *
 *  Created on: Jun 3, 2015
 *      Author: innocentevil
 */

#include "cdsl_avltree.h"


#define DIR_LEFT          ((uint8_t) 1)
#define DIR_RIGHT         ((uint8_t) 2)

#define PATTERN_MASK      ((uint8_t) 0x3F)
#define LEFTRIGTH_PATTERN ((uint8_t) (DIR_RIGHT << 2) | DIR_LEFT)
#define LEFTLEFT_PATTERN  ((uint8_t) (DIR_LEFT << 2) | DIR_LEFT)
#define RIGHTLEFT_PATTERN ((uint8_t) (DIR_LEFT << 2) | DIR_RIGHT)
#define RIGHTRIGHT_PATTER ((uint8_t) (DIR_RIGHT << 2) | DIR_RIGHT)

static avltreeNode_t* insert_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t* item, uint8_t *rc_dir, avltreeNode_t** replaced);
static avltreeNode_t* rotate_right(avltreeNode_t* sub_root);
static avltreeNode_t* rotate_left(avltreeNode_t* sub_root);


void cdsl_avltreeRootInit(avltreeRoot_t* rootp, int bal) {
	if(!rootp) {
		return;
	}
	rootp->balance_factor = bal;
	rootp->entry = NULL;
}

void cdsl_avltreeNodeInit(avltreeNode_t* node, trkey_t key) {
	if(!node) {
		return;
	}
	node->depth = 0;
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
	return NULL;
}

avltreeNode_t* cdsl_avltreeDeleteReplace(avltreeRoot_t* rootp,trkey_t key, base_tree_replacer_t replacer, void* cb_arg) {
	return NULL;
}

avltreeNode_t* cdsl_avltreeDeleteMinReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg) {
	return NULL;
}

avltreeNode_t* cdsl_avltreeDeleteMaxReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg) {
	return NULL;
}


static avltreeNode_t* insert_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t* item, uint8_t *rc_dir,  avltreeNode_t** replaced) {
	if(!sub_root_c) {
		item->depth = 1;
		return item;
	}
	if(sub_root_c->key > item->key) {
		sub_root_c->left = insert_rc(bal, sub_root_c->left, item, rc_dir, replaced);
		sub_root_c->depth = sub_root_c->left->depth + 1;
		*rc_dir |= DIR_LEFT;
	} else if(sub_root_c->key < item->key) {
		sub_root_c->right = insert_rc(bal, sub_root_c->right, item, rc_dir, replaced);
		sub_root_c->depth = sub_root_c->right->depth + 1;
		*rc_dir |= DIR_RIGHT;
	} else {
		if(*replaced) {
			*replaced = sub_root_c;
			item->left = sub_root_c->left;
			item->right = sub_root_c->right;
			item->depth = sub_root_c->depth;
			return item;
		} else {
			sub_root_c->left = insert_rc(bal, sub_root_c->left, item, rc_dir, replaced);
			sub_root_c->depth = sub_root_c->left->depth + 1;
			*rc_dir |= DIR_LEFT;
		}
	}
	switch((*rc_dir) & PATTERN_MASK) {
	case RIGHTLEFT_PATTERN:
		/*
		 *
		 */
		if(!sub_root_c->left) {
			if(sub_root_c->right->depth > bal) {
				sub_root_c->right->left = rotate_right(sub_root_c->right->left);
				sub_root_c->right = rotate_left(sub_root_c->right);
			}
		} else {
			if(sub_root_c->right->depth - sub_root_c->left->depth > bal) {
			}
		}
		break;
	case RIGHTRIGHT_PATTER:
		break;
	case LEFTLEFT_PATTERN:
		break;
	case LEFTRIGTH_PATTERN:
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
	return nroot;
}

static avltreeNode_t* rotate_left(avltreeNode_t* sub_root) {
	if(!sub_root) {
		return sub_root;
	}
	avltreeNode_t* nroot = sub_root->left;
	sub_root->right = nroot->left;
	nroot->left = sub_root;
	return nroot;
}

