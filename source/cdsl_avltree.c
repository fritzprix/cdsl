/*
 * cdsl_avltree.c
 *
 *  Created on: Jun 3, 2015
 *      Author: innocentevil
 */

#include "cdsl_avltree.h"


#define DIR_LEFT                   ((uint8_t) 1)
#define DIR_RIGHT                  ((uint8_t) 2)

#define HEIGHT_COMPRESSION_RATE    ((uint8_t) 4)

#define PATTERN_MASK               ((uint8_t) 0x0F)
#define LEFTRIGTH_PATTERN          ((uint8_t) (DIR_RIGHT << 2) | DIR_LEFT)
#define LEFTLEFT_PATTERN           ((uint8_t) (DIR_LEFT << 2) | DIR_LEFT)
#define RIGHTLEFT_PATTERN          ((uint8_t) (DIR_LEFT << 2) | DIR_RIGHT)
#define RIGHTRIGHT_PATTERN         ((uint8_t) (DIR_RIGHT << 2) | DIR_RIGHT)

static avltreeNode_t* insert_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t* item, uint8_t *rc_dir, avltreeNode_t** replaced);
static avltreeNode_t* delete_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t** deleteed,trkey_t key, base_tree_replacer_t replacer, void* args);
static avltreeNode_t* delete_lm_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t** deleted, base_tree_replacer_t replacer, void* args);
static avltreeNode_t* delete_rm_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t** deleted, base_tree_replacer_t replacer, void* args);
static avltreeNode_t* take_rightmost_rc(int bal, avltreeNode_t* parent, avltreeNode_t** leaf);
static avltreeNode_t* take_leftmost_rc(int bal, avltreeNode_t* parent, avltreeNode_t** leaf);

static avltreeNode_t* rotate_right(avltreeNode_t* sub_root);
static avltreeNode_t* rotate_left(avltreeNode_t* sub_root);

static int max_child_height(avltreeNode_t* node);


static void _write_ser_node_header (cdsl_serializeNode_t* node_head, const void* node);
static void _write_serialize_header(cdsl_serializeHeader_t* header);
static void _build_node(const cdsl_serializeNode_t* node_header, void* node);


const cdsl_serializeExtInterface_t _cdsl_avltree_serializer_ext = {
		.free_ext_node = NULL,
		.alloc_ext_node = NULL,
		.write_node_haeder = _write_ser_node_header,
		.write_serialize_header = _write_serialize_header,
		.on_node_build = _build_node
};

void cdsl_avltreeRootInit(avltreeRoot_t* rootp, uint8_t bal) {
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

avltreeNode_t* cdsl_avltreeConditionalLookup(avltreeRoot_t* rootp, trkey_t key, condition_t match) {
	if(!rootp) {
		return NULL;
	}
	avltreeNode_t* cur = rootp->entry;
	while(cur) {
		if(match(&cur->base_node, key)) {
			return cur;
		}
		if(cur->key < key) {
			cur = cur->right;
		} else {
			cur = cur->left;
		}
	}
	return NULL;
}


avltreeNode_t* cdsl_avltreeDeleteReplace(avltreeRoot_t* rootp,trkey_t key, base_tree_replacer_t replacer, void* cb_arg) {
	if(!rootp) {
		return NULL;
	}
	avltreeNode_t* deleted = NULL;
	rootp->entry = delete_rc(rootp->balance_factor, rootp->entry, &deleted, key, replacer, cb_arg);
	return deleted;
}

avltreeNode_t* cdsl_avltreeDeleteMinReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg) {
	if(!rootp) {
		return NULL;
	}
	avltreeNode_t* deleted = NULL;
	rootp->entry = delete_lm_rc(rootp->balance_factor, rootp->entry, &deleted, replacer, cb_arg);
	return deleted;
}

avltreeNode_t* cdsl_avltreeDeleteMaxReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg) {
	if(!rootp) {
		return NULL;
	}
	avltreeNode_t* deleted = NULL;
	rootp->entry = delete_rm_rc(rootp->balance_factor, rootp->entry, &deleted, replacer, cb_arg);
	return deleted;
}

static avltreeNode_t* delete_lm_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t** deleted, base_tree_replacer_t replacer, void* args) {
	if(!sub_root_c) {
		return NULL;
	}
	if(!sub_root_c->left) {
		*deleted = sub_root_c;
		avltreeNode_t* nroot = NULL;
		if(replacer  && replacer((base_treeNode_t**) &nroot, args)) {
			if (nroot == NULL) {
				return sub_root_c;
			}
			nroot->height = sub_root_c->height;
			nroot->left = sub_root_c->left;
			nroot->right = sub_root_c->right;
			sub_root_c->left = NULL;
			sub_root_c->right = NULL;
			return nroot;
		}
		if (sub_root_c->right) {
			sub_root_c->right = take_leftmost_rc(bal, sub_root_c->right, &nroot);
			nroot->right = sub_root_c->right;
			nroot->left = sub_root_c->left;
			nroot->height = sub_root_c->height;
			sub_root_c->left = sub_root_c->right = NULL;
			sub_root_c = nroot;
		} else {
			return NULL;
		}
	}
	sub_root_c->left = delete_lm_rc(bal, sub_root_c->left, deleted, replacer, args);
	if(sub_root_c->right && sub_root_c->left) {
		if(sub_root_c->right->height - sub_root_c->left->height > bal) {
			if(!sub_root_c->right->right) {
				sub_root_c->right = rotate_right(sub_root_c->right);
			}
			sub_root_c = rotate_left(sub_root_c);
		}
		sub_root_c->height = max_child_height(sub_root_c) + 1;
	} else if(sub_root_c->right) {
		if(sub_root_c->right->height > bal) {
			if(!sub_root_c->right->right) {
				sub_root_c->right = rotate_right(sub_root_c->right);
			}
			sub_root_c = rotate_left(sub_root_c);
		}
		sub_root_c->height = sub_root_c->right->height + 1;
	} else if(sub_root_c->left){
		if(sub_root_c->left->height > bal) {
			if(!sub_root_c->left->left) {
				sub_root_c->left = rotate_left(sub_root_c->left);
			}
			sub_root_c = rotate_right(sub_root_c);
		}
		sub_root_c->height = sub_root_c->left->height + 1;
	} else {
		sub_root_c->height = 1;
	}
	return sub_root_c;
}

static avltreeNode_t* delete_rm_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t** deleted, base_tree_replacer_t replacer, void* args) {
	if(!sub_root_c) {
		return NULL;
	}
	if(!sub_root_c->right) {
		*deleted = sub_root_c;
		avltreeNode_t* nroot = NULL;
		if (replacer && replacer((base_treeNode_t**) &nroot, args)) {
			if (nroot == NULL) {
				return sub_root_c;
			}
			nroot->height = sub_root_c->height;
			nroot->left = sub_root_c->left;
			nroot->right = sub_root_c->right;
			sub_root_c->left = NULL;
			sub_root_c->right = NULL;
			return nroot;
		}
		else if (sub_root_c->left) {
			sub_root_c->left = take_rightmost_rc(bal, sub_root_c->left, &nroot);
			nroot->left = sub_root_c->left;
			nroot->right = sub_root_c->right;
			nroot->height = sub_root_c->height;
			sub_root_c->left = sub_root_c->right = NULL;
			sub_root_c = nroot;
		} else {
			return NULL;
		}
	}
	sub_root_c->right = delete_rm_rc(bal, sub_root_c->right, deleted, replacer, args);
	if (sub_root_c->right && sub_root_c->left) {
		if (sub_root_c->left->height - sub_root_c->right->height > bal) {
			if (!sub_root_c->left->left) {
				sub_root_c->left = rotate_left(sub_root_c->left);
			}
			sub_root_c = rotate_right(sub_root_c);
		}
		sub_root_c->height = max_child_height(sub_root_c) + 1;
	} else if (sub_root_c->right) {
		if (sub_root_c->right->height > bal) {
			if (!sub_root_c->right->right) {
				sub_root_c->right = rotate_right(sub_root_c->right);
			}
			sub_root_c = rotate_left(sub_root_c);
		}
		sub_root_c->height = sub_root_c->right->height + 1;
	} else if (sub_root_c->left) {
		if (sub_root_c->left->height > bal) {
			if (!sub_root_c->left->left) {
				sub_root_c->left = rotate_left(sub_root_c->left);
			}
			sub_root_c = rotate_right(sub_root_c);
		}
		sub_root_c->height = sub_root_c->left->height + 1;
	} else {
		sub_root_c->height = 1;
	}
	return sub_root_c;
}

static avltreeNode_t* delete_rc(int bal, avltreeNode_t* sub_root_c, avltreeNode_t** deleted,trkey_t key, base_tree_replacer_t replacer, void* args) {
	if(!sub_root_c) {
		return NULL;
	}
	if(sub_root_c->key > key) {
		sub_root_c->left = delete_rc(bal, sub_root_c->left, deleted, key, replacer, args);
		if(!sub_root_c->left) {
			if(sub_root_c->right) {
				sub_root_c->height = sub_root_c->right->height + 1;
				if(sub_root_c->right->height > bal) {
					if(!sub_root_c->right->right) {
						sub_root_c->right = rotate_right(sub_root_c->right);
					}
					sub_root_c = rotate_left(sub_root_c);
				}
			} else {
				sub_root_c->height = 1;
			}
		} else {
			if(sub_root_c->right) {
				sub_root_c->height = sub_root_c->right->height > sub_root_c->left->height? sub_root_c->right->height + 1 : sub_root_c->left->height + 1;
				if(sub_root_c->right->height - sub_root_c->left->height > bal) {
					if(!sub_root_c->right->right) {
						sub_root_c->right = rotate_right(sub_root_c->right);
					}
					sub_root_c = rotate_left(sub_root_c);
				}
			} else {
				sub_root_c->height = sub_root_c->left->height + 1;
			}
		}
		return sub_root_c;
	} else if(sub_root_c->key < key) {
		sub_root_c->right = delete_rc(bal, sub_root_c->right, deleted, key,  replacer, args);
		if(!sub_root_c->right) {
			if(sub_root_c->left) {
				sub_root_c->height = sub_root_c->left->height + 1;
				if(sub_root_c->left->height > bal) {
					if (!sub_root_c->left->left) {
						sub_root_c->left = rotate_left(sub_root_c->left);
					}
					sub_root_c = rotate_right(sub_root_c);
				}
			} else {
				sub_root_c->height = 1;
			}
		} else {
			if(sub_root_c->left) {
				sub_root_c->height = sub_root_c->right->height > sub_root_c->left->height? sub_root_c->right->height + 1 : sub_root_c->left->height + 1;
				if(sub_root_c->left->height - sub_root_c->right->height > bal) {
					if(!sub_root_c->left->left) {
						sub_root_c->left = rotate_left(sub_root_c->left);
					}
					sub_root_c = rotate_right(sub_root_c);
				}
			} else {
				sub_root_c->height = sub_root_c->right->height + 1;
			}
		}
		return sub_root_c;
	} else {
		*deleted = sub_root_c;
		avltreeNode_t* nroot = NULL;
		if(replacer && replacer((base_treeNode_t**) &nroot, args)) {
			if(nroot == NULL) {
				return sub_root_c;
			}
			nroot->height = sub_root_c->height;
			nroot->left = sub_root_c->left;
			nroot->right = sub_root_c->right;
			sub_root_c->left = NULL;
			sub_root_c->right = NULL;
			return nroot;
		}
		if (sub_root_c->right) {
			sub_root_c->right = take_leftmost_rc(bal, sub_root_c->right, &nroot);
			nroot->right = sub_root_c->right;
			nroot->left = sub_root_c->left;
			nroot->height = sub_root_c->height;
			sub_root_c->left = sub_root_c->right = NULL;
			sub_root_c = nroot;
		} else if (sub_root_c->left) {
			sub_root_c->left = take_rightmost_rc(bal, sub_root_c->left, &nroot);
			nroot->left = sub_root_c->left;
			nroot->right = sub_root_c->right;
			nroot->height = sub_root_c->height;
			sub_root_c->left = sub_root_c->right = NULL;
			sub_root_c = nroot;
		} else {
			return NULL;
		}
	}
	sub_root_c->height = max_child_height(sub_root_c) + 1;
	return sub_root_c;
}

static avltreeNode_t* take_rightmost_rc(int bal, avltreeNode_t* parent, avltreeNode_t** leaf) {
	if(!parent) {
		return NULL;
	}
	if(!parent->right) {
		*leaf = parent;
		return parent->left;
	}
	parent->right = take_rightmost_rc(bal, parent->right, leaf);
	if(!parent->right) {
		if(!parent->left) {
			parent->height = 1;
			return parent;
		}

		if(parent->left->height > bal) {
			return rotate_right(parent);
		}
	} else {
		if(!parent->left) {
			parent->height = parent->right->height + 1;
			return parent;
		}

		if(parent->left->height - parent->right->height > bal) {
			return rotate_right(parent);
		}
	}
	return parent;
}

static avltreeNode_t* take_leftmost_rc(int bal, avltreeNode_t* parent, avltreeNode_t** leaf) {
	if(!parent) {
		return NULL;
	}
	if(!parent->left) {
		*leaf = parent;
		return parent->right;
	}
	parent->left = take_leftmost_rc(bal, parent->left, leaf);
	if(!parent->left) {
		if(!parent->right) {
			parent->height = 1;
			return parent;
		}

		if(parent->right->height > bal) {
			return rotate_left(parent);
		}
	} else {
		if(!parent->right) {
			parent->height = parent->left->height + 1;
			return parent;
		}

		if(parent->right->height - parent->left->height > bal) {
			return rotate_left(parent);
		}
	}
	return parent;
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
		if(replaced) {
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
	case RIGHTRIGHT_PATTERN:
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
	if(!sub_root->left) {
		return sub_root;
	}
	avltreeNode_t* nroot = sub_root->left;
	sub_root->left = nroot->right;
	nroot->right = sub_root;
	if(nroot->left) {
		nroot->height = nroot->left->height + 1;
	} else {
		nroot->height = 1;
	}
	nroot->right->height = max_child_height(nroot->right) + 1;

	return nroot;
}

static avltreeNode_t* rotate_left(avltreeNode_t* sub_root) {
	if(!sub_root) {
		return sub_root;
	}
	if(!sub_root->right) {
		return sub_root;
	}
	avltreeNode_t* nroot = sub_root->right;
	sub_root->right = nroot->left;
	nroot->left = sub_root;

	if(nroot->right) {
		nroot->height = nroot->right->height + 1;
	} else {
		nroot->height = 1;
	}
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

static void _write_ser_node_header (cdsl_serializeNode_t* node_head, const void* node) {
	if(node_head == NULL) return;
	avltreeNode_t* avlnode = (avltreeNode_t*) node;
	SET_SPEC(node_head,avlnode->height);
}

static void _write_serialize_header(cdsl_serializeHeader_t* header) {
	header->type |= SUB_TYPE_AVLNODE;
}

static void _build_node(const cdsl_serializeNode_t* node_header, void* node) {
	uint8_t spec = GET_SPEC(node_header);
	avltreeNode_t* avlnode = (avltreeNode_t*) node;
	avlnode->height = spec;
}


