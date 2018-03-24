/*
 * cdsl_rbtree.c
 *
 *  Created on: Mar 25, 2016
 *      Author: innocentevil
 */

#include "cdsl_rbtree.h"
#include "arch.h"

#define GET_PTR(node)                         ((rbtreeNode_t*)(((__cdsl_uaddr_t) node) & ~1))
#define RED                                   ((uint8_t) 1)
#define BLACK                                 ((uint8_t) 0)
#define PAINT_RED(node) do {\
	node =  (rbtreeNode_t*) ((__cdsl_uaddr_t) node | RED);\
}while(0)

#define PAINT_BLACK(node) do {\
		node =  (rbtreeNode_t*) ((__cdsl_uaddr_t) node & ~RED);\
}while(0)

#define PAINT_COLOR(node, color) do {\
	PAINT_BLACK(node);\
	node = (rbtreeNode_t*) ((__cdsl_uaddr_t) node | color);\
}while(0)

#define GET_COLOR(node)						  ((__cdsl_uaddr_t)(node) & 1)
#define GET_DIR(node)                         ((__cdsl_uaddr_t)(node) & 1)

#define CTX_BB                                ((uint8_t) 2)
#define CTX_RIGHT                             ((uint8_t) 1)
#define CTX_LEFT                              ((uint8_t) 0)

#define PATTERN_WINDOW                        ((uint8_t) 0x3)

#define PATTERN_RED_BLACK                     ((uint8_t) 0x2)
#define PATTERN_RED_RED                       ((uint8_t) 0x3)
#define PATTERN_BLACK_RED                     ((uint8_t) 0x1)
#define PATTERN_BLACK_BLACK                   ((uint8_t) 0x0)

#define PATTERN_RIGHT_RIGHT                   ((uint8_t) 0x3)      // right-right
#define PATTERN_RIGHT_LEFT                    ((uint8_t) 0x2)      // right-left
#define PATTERN_LEFT_RIGHT                    ((uint8_t) 0x1)      // left-right
#define PATTERN_LEFT_LEFT                     ((uint8_t) 0x0)      // left-left

#define PATTERN_REV_RIGHT_RIGHT               ((uint8_t) 0x3)
#define PATTERN_REV_RIGHT_LEFT                ((uint8_t) 0x1)
#define PATTERN_REV_LEFT_RIGHT                ((uint8_t) 0x2)
#define PATTERN_REV_LEFT_LEFT                 ((uint8_t) 0x0)

#define RCC_CTX_PATTERN                       ((uint8_t) 0x03)
#define RCC_CTX_PATTERN_REDRED                ((uint8_t) 0x03)

const char* COLOR_STRING[] = { "BLACK", "RED" };

static int max_depth_rc(rbtreeNode_t* node);
static rbtreeNode_t* insert_rc(rbtreeNode_t* sub_root_c, rbtreeNode_t* item,	uint8_t* rc_color, uint8_t* rc_dir, rbtreeNode_t** replaced);
static rbtreeNode_t* delete_rc(rbtreeNode_t* sub_root_c, trkey_t key, rbtreeNode_t** rm, uint8_t* ctx, base_tree_replacer_t replacer, void* arg);
static rbtreeNode_t* delete_lm_rc(rbtreeNode_t* sub_root_c, rbtreeNode_t** rm, uint8_t* ctx, base_tree_replacer_t replacer, void* arg);
static rbtreeNode_t* delete_rm_rc(rbtreeNode_t* sub_root_c, rbtreeNode_t** rm, uint8_t* ctx, base_tree_replacer_t replacer, void* arg);
static rbtreeNode_t* rotate_left(rbtreeNode_t* gparent_c);
static rbtreeNode_t* rotate_right(rbtreeNode_t* gparent_c);
static rbtreeNode_t* update_color(rbtreeNode_t* node_c);
static rbtreeNode_t* resolve_red_red(rbtreeNode_t* gparent_c,	uint8_t color_ctx, uint8_t dir_ctx);
static rbtreeNode_t* resolve_black_black(rbtreeNode_t* parent_c, uint8_t dir_ctx, uint8_t* ctx);

static rbtreeNode_t* up_from_rightmost_rc(rbtreeNode_t* node,	rbtreeNode_t** rm, uint8_t* ctx);
static rbtreeNode_t* up_from_leftmost_rc(rbtreeNode_t* node, rbtreeNode_t** lm, uint8_t* ctx);


static void _write_ser_node_header (cdsl_serializeNode_t* node_head, const void* node);
static void _write_serialize_header(cdsl_serializeHeader_t* header);
static void _build_node(const cdsl_serializeNode_t* node_header, void* node);


static void print_tab(int cnt);
static void node_print_rc(rbtreeNode_t* node, int level);

#ifdef __DBG
static int rot_count;
#endif

#ifdef __DBG
static volatile __cdsl_uaddr_t stack_top;
static volatile __cdsl_uaddr_t stack_bottom;
#endif

void cdsl_rbtreeRootInit(rbtreeRoot_t* rootp) {
	if (rootp == NULL)
		return;
	rootp->entry = NULL;
}





const cdsl_serializeExtInterface_t _cdsl_rbtree_serializer_ext = {
	.alloc_ext_node = NULL,
	.write_node_haeder = _write_ser_node_header,
	.write_serialize_header = _write_serialize_header,
	.on_node_build = _build_node
};

void cdsl_rbtreeNodeInit(rbtreeNode_t* node, trkey_t key) {
	if (node == NULL)
		return;
	if ((size_t) node & 1) {
		PRINT_ERR("unaligned pointer!!\n");
	}
	node->left = node->right = NULL;
	node->key = key;
}

rbtreeNode_t* cdsl_rbtreeInsert(rbtreeRoot_t* rootp, rbtreeNode_t* item, BOOL is_set) {
	if (!rootp)
		return NULL;
	__dev_log("INSERT : (%lu)\n", item->key);
#ifdef __DBG
	rot_count = 0;
#endif

	if (!rootp->entry) {
		rootp->entry = item;
		return NULL;
	}
#ifdef __DBG
	stack_top = (__cdsl_uaddr_t) &rootp;
#endif

	uint8_t dir = 0;
	uint8_t color = 0;
	rbtreeNode_t* replaced = NULL;
	rootp->entry = insert_rc(rootp->entry, item, &color, &dir,
			(is_set ? &replaced : NULL));
	PAINT_BLACK(rootp->entry);
	__dev_log("ROTATE_COUNT : %d\n",rot_count);
	__dev_log("stack usage : %lu\n",stack_top - stack_bottom);
	return replaced;
}

rbtreeNode_t* cdsl_rbtreeLookup(rbtreeRoot_t* rootp, trkey_t key) {
	if (!rootp) {
		return NULL;
	}
	rbtreeNode_t* cur_node = rootp->entry;	// always black so don't need to use GET_PTR() macro
	while (cur_node) {
		if (GET_PTR(cur_node)->key > key) {
			cur_node = GET_PTR(cur_node)->left;
		} else if (GET_PTR(cur_node)->key < key) {
			cur_node = GET_PTR(cur_node)->right;
		} else {
			return GET_PTR(cur_node);
		}
	}
	return NULL;
}

rbtreeNode_t* cdsl_rbtreeConditionalLookup(rbtreeRoot_t* rootp, trkey_t key, condition_t match) {
	if (!rootp) {
		return NULL;
	}
	rbtreeNode_t* cur_node = rootp->entry;	// always black so don't need to use GET_PTR() macro
	while (cur_node) {
		if(match(&cur_node->base_node, key)) {
			return cur_node;
		}
		if (GET_PTR(cur_node)->key < key) {
			cur_node = GET_PTR(cur_node)->right;
		} else {
			cur_node = GET_PTR(cur_node)->left;
		}
	}
	return NULL;
}


rbtreeNode_t* cdsl_rbtreeDeleteReplace(rbtreeRoot_t* rootp, trkey_t key,	base_tree_replacer_t replacer, void* arg) {
	if (!rootp)
		return NULL;
	__dev_log("DELETE : (%lu)\n", key);
#ifdef __DBG
	rot_count = 0;
#endif
	rbtreeNode_t* del = NULL;
	uint8_t ctx = 0;
	rootp->entry = delete_rc(rootp->entry, key, &del, &ctx, replacer, arg);
	return GET_PTR(del);
}

rbtreeNode_t* cdsl_rbtreeDeleteMinReplace(rbtreeRoot_t* rootp, base_tree_replacer_t replacer, void* arg) {
	if (!rootp)
		return NULL;
	if (!GET_PTR(rootp->entry))
		return NULL;
	__dev_log("DELETE MIN \n");
#ifdef __DBG
	rot_count = 0;
#endif
	rbtreeNode_t* del = NULL;
	uint8_t ctx = 0;
	rootp->entry = delete_lm_rc(rootp->entry, &del, &ctx, replacer, arg);
	return GET_PTR(del);
}

rbtreeNode_t* cdsl_rbtreeDeleteMaxReplace(rbtreeRoot_t* rootp, base_tree_replacer_t replacer, void* arg) {
	if (!rootp)
		return NULL;
	if (!GET_PTR(rootp->entry))
		return NULL;
	__dev_log("DELETE MAX \n");
#ifdef __DBG
	rot_count = 0;
#endif
	rbtreeNode_t* del = NULL;
	uint8_t ctx = 0;
	rootp->entry = delete_rm_rc(rootp->entry, &del, &ctx, replacer, arg);
	return GET_PTR(del);
}

#ifdef __DBG
void cdsl_rbtreePrint_dev (rbtreeRoot_t* root)
{
	if(!root)
	return;
	PRINT("\n");
	node_print_rc(root->entry, 0);
	PRINT("\n");

}
#endif

static void print_tab(int cnt) {
	while (cnt--)
		PRINT("\t");
}

static void node_print_rc(rbtreeNode_t* node, int level) {
	if (!GET_PTR(node)) {
		print_tab(level);
		PRINT("NIL(black) \n");
		return;
	}
	node_print_rc(GET_PTR(node)->right, level + 1);
	print_tab(level);
	PRINT("%s node : %lu / order %d \n", COLOR_STRING[GET_COLOR(node)], GET_PTR(node)->key, level);
	node_print_rc(GET_PTR(node)->left, level + 1);
}

/*
 *  ctx[0] = color
 *  ctx[1] = dir
 *  ctx[2] = is_bb
 *  ctx[3] = res for future
 */

static rbtreeNode_t* delete_rc(rbtreeNode_t* sub_root_c, trkey_t key,	rbtreeNode_t** rm, uint8_t* ctx, base_tree_replacer_t replacer, void* cb_arg) {
	if (!sub_root_c) {
		*rm = NULL;
		return NULL;
	}

	if (GET_PTR(sub_root_c)->key < key) {
		GET_PTR(sub_root_c)->right = delete_rc(GET_PTR(sub_root_c)->right, key,
				rm, ctx, replacer, cb_arg);
		if (*ctx & (1 << CTX_BB)) {
			sub_root_c = resolve_black_black(sub_root_c, CTX_RIGHT, ctx);
		}
		return sub_root_c;
	} else if (GET_PTR(sub_root_c)->key > key) {
		GET_PTR(sub_root_c)->left = delete_rc(GET_PTR(sub_root_c)->left, key,
				rm, ctx, replacer, cb_arg);
		if (*ctx & (1 << CTX_BB)) {
			sub_root_c = resolve_black_black(sub_root_c, CTX_LEFT, ctx);
		}
		return sub_root_c;
	}
	*rm = sub_root_c;
	if (replacer) {
		rbtreeNode_t* replace = GET_PTR(sub_root_c);
		if (replacer((base_treeNode_t**) &replace, cb_arg)) {
			if (replace == GET_PTR(sub_root_c)) {
				*rm = NULL;  // not found case, return immediately
				return sub_root_c;
			}
			// override default hole handling behavior
			replace->left = GET_PTR(sub_root_c)->left;
			replace->right = GET_PTR(sub_root_c)->right;
			PAINT_COLOR(replace, GET_COLOR(sub_root_c));
			return replace;
		}
		// otherwise, perform default hole handling
	}
	if (GET_PTR(sub_root_c)->left) {
		GET_PTR(sub_root_c)->left = up_from_rightmost_rc(GET_PTR(sub_root_c)->left, &sub_root_c, ctx);
		GET_PTR(sub_root_c)->right = GET_PTR(*rm)->right;
		if (*ctx & (1 << CTX_BB)) {
			sub_root_c = resolve_black_black(sub_root_c, CTX_LEFT, ctx);
		}
		return sub_root_c;
	} else if (GET_PTR(sub_root_c)->right) {
		GET_PTR(sub_root_c)->right = up_from_leftmost_rc(GET_PTR(sub_root_c)->right, &sub_root_c, ctx);
		GET_PTR(sub_root_c)->left = GET_PTR(*rm)->left;
		if (*ctx & (1 << CTX_BB)) {
			sub_root_c = resolve_black_black(sub_root_c, CTX_RIGHT, ctx);
		}
		return sub_root_c;
	} else {
		*ctx |= (1 << CTX_BB);
		return NULL;
	}
}

static rbtreeNode_t* delete_lm_rc(rbtreeNode_t* sub_root_c, rbtreeNode_t** rm, uint8_t* ctx, base_tree_replacer_t replacer, void* cb_arg) {
	if (!GET_PTR(sub_root_c)->left) {
		*rm = sub_root_c;
		if (replacer) {
			rbtreeNode_t* replace = GET_PTR(sub_root_c);
			if (replacer((base_treeNode_t**) &replace, cb_arg)) {
				if (replace == GET_PTR(sub_root_c)) {
					*rm = NULL;  // not found case, return immediately
					return sub_root_c;
				}
				// override default hole handling behavior
				replace->left = GET_PTR(sub_root_c)->left;
				replace->right = GET_PTR(sub_root_c)->right;
				PAINT_COLOR(replace, GET_COLOR(sub_root_c));
				return replace;
			}
			// otherwise, perform default hole handling
		}
		if (GET_PTR(sub_root_c)->right) {
			GET_PTR(sub_root_c)->right = up_from_leftmost_rc(GET_PTR(sub_root_c)->right, &sub_root_c, ctx);
			GET_PTR(sub_root_c)->left = GET_PTR(*rm)->left;
			if (*ctx & (1 << CTX_BB)) {
				sub_root_c = resolve_black_black(sub_root_c, CTX_RIGHT, ctx);
			}
			return sub_root_c;
		}
		return NULL;
	}
	GET_PTR(sub_root_c)->left = delete_lm_rc(GET_PTR(sub_root_c)->left, rm, ctx,
			replacer, cb_arg);
	if (*ctx & (1 << CTX_BB)) {
		sub_root_c = resolve_black_black(sub_root_c, CTX_LEFT, ctx);
	}
	return sub_root_c;
}

static rbtreeNode_t* delete_rm_rc(rbtreeNode_t* sub_root_c,
		rbtreeNode_t** rm, uint8_t* ctx, base_tree_replacer_t replacer,
		void* cb_arg) {
	if (!GET_PTR(sub_root_c)->right) {
		*rm = sub_root_c;
		if (replacer) {
			rbtreeNode_t* replace = GET_PTR(sub_root_c);
			if (replacer((base_treeNode_t**) &replace, cb_arg)) {
				if (replace == GET_PTR(sub_root_c)) {
					*rm = NULL;  // not found case, return immediately
					return sub_root_c;
				}
				// override default hole handling behavior
				replace->left = GET_PTR(sub_root_c)->left;
				replace->right = GET_PTR(sub_root_c)->right;
				PAINT_COLOR(replace, GET_COLOR(sub_root_c));
				return replace;
			}
			// otherwise, perform default hole handling
		}
		if (GET_PTR(sub_root_c)->left) {
			GET_PTR(sub_root_c)->left = up_from_rightmost_rc(
					GET_PTR(sub_root_c)->left, &sub_root_c, ctx);
			GET_PTR(sub_root_c)->right = GET_PTR(*rm)->right;
			if (*ctx & (1 << CTX_BB)) {
				sub_root_c = resolve_black_black(sub_root_c, CTX_LEFT, ctx);
			}
			return sub_root_c;
		}
		return NULL;
	}
	GET_PTR(sub_root_c)->right = delete_rm_rc(GET_PTR(sub_root_c)->right, rm,
			ctx, replacer, cb_arg);
	if (*ctx & (1 << CTX_BB)) {
		sub_root_c = resolve_black_black(sub_root_c, CTX_RIGHT, ctx);
	}
	return sub_root_c;
}


static rbtreeNode_t* rotate_left(rbtreeNode_t* gparent_c) {
	/*
	 *         n0(gparent_c)          n1
	 *        /  \          -->      / \
	 *      n2   n1                n0   n4
	 *           / \              / \    \
	 *         n3  n4            n2 n3    n1
	 */
	if (!GET_PTR(gparent_c)) {
		return NULL;
	}
	__dev_log("ROTATE_LEFT @ %lu\n",GET_PTR(gparent_c)->key);
	rbtreeNode_t* ngp = GET_PTR(gparent_c)->right;
	GET_PTR(gparent_c)->right = GET_PTR(ngp)->left;
	GET_PTR(ngp)->left = update_color(gparent_c);
#ifdef __DBG
	rot_count++;
#endif

	return update_color(ngp);
}

static rbtreeNode_t* rotate_right(rbtreeNode_t* gparent_c) {
	/*
	 *         n0(gparent_c)          n2
	 *        /  \          -->      / \
	 *      n2   n1                 n3  n0
	 *     / \                         / \
	 *    n3  n4                      n4  n1
	 */
	if (!GET_PTR(gparent_c)) {
		return NULL;
	}
	__dev_log("ROTATE_RIGHT @ %lu\n",GET_PTR(gparent_c)->key);
	rbtreeNode_t* ngp = GET_PTR(gparent_c)->left;
	GET_PTR(gparent_c)->left = GET_PTR(ngp)->right;
	GET_PTR(ngp)->right = update_color(gparent_c);
#ifdef __DBG
	rot_count++;
#endif

	return update_color(ngp);
}

static rbtreeNode_t* update_color(rbtreeNode_t* node_c) {
	/*
	 *      r0
	 *     / \
	 *    b1 b2
	 */
	if (!GET_PTR(node_c)) {
		return NULL;
	}
	if ((GET_COLOR(GET_PTR(node_c)->left) == BLACK)
			&& (GET_COLOR(GET_PTR(node_c)->right) == BLACK)) {
		return (rbtreeNode_t*) ((__cdsl_uaddr_t ) node_c | RED);
	}
	if ((GET_COLOR(GET_PTR(node_c)->left) == RED)
			&& (GET_COLOR(GET_PTR(node_c)->right) == RED)) {
		return (rbtreeNode_t*) ((__cdsl_uaddr_t ) node_c & ~RED);
	}
	return node_c;
}

static int max_depth_rc(rbtreeNode_t* node) {
	if (!GET_PTR(node)) {
		return 0;
	}
	int a, b;
	a = max_depth_rc(GET_PTR(node)->left);
	b = max_depth_rc(GET_PTR(node)->right);
	return a > b ? a : b;
}

static rbtreeNode_t* resolve_red_red(rbtreeNode_t* gparent_c,	uint8_t color_ctx, uint8_t dir_ctx) {
	if(!GET_PTR(gparent_c)) {
		return gparent_c;
	}
	rbtreeNode_t* bare_ptr = GET_PTR(gparent_c);
	if((GET_COLOR(bare_ptr->left) == RED) && (GET_COLOR(bare_ptr->right) == RED)) {
		PAINT_BLACK(bare_ptr->left);
		PAINT_BLACK(bare_ptr->right);
		PAINT_RED(gparent_c);
		return gparent_c;
	}
	switch(dir_ctx & PATTERN_WINDOW) {
	case PATTERN_REV_LEFT_RIGHT:
			/*
			 * resolve left-right sequence into left-left
			 *         gp                        gp
			 *        /  \                      /
			 *      left  ..        --->      left
			 *      /  \                      /  \
			 *     ..  new                  new
			 */
			bare_ptr->left = rotate_left(bare_ptr->left);
			/*
			 *  rotate performed to make left-left sequence
			 *  so now PATTERN_LEFT_LEFT case
			 *  I think pass-through can be used here by just omit break statement
			 *  SO PLEASE DON'T PUT BREAK HERE. IT'S INTENDED
			 */
		case PATTERN_REV_LEFT_LEFT:
			return rotate_right(gparent_c);
		case PATTERN_REV_RIGHT_LEFT:
			/*
			 * resolve right-left sequence into left-left
			 *         gp                        gp
			 *        /  \                      /  \
			 *      ..  right        --->     ..   right
			 *           /  \                      /   \
			 *         new  ..                    ..   new
			 */
			bare_ptr->right = rotate_right(bare_ptr->right);
			/*
			 *  rotate performed to make right-right sequence
			 *  so now PATTERN_RIGHT_RIGHT case
			 *  I think pass-through can be used here by just omit break statement
			 *  SO PLEASE DON'T PUT BREAK HERE. IT'S INTENDED
			 */
		case PATTERN_REV_RIGHT_RIGHT:
			return rotate_left(gparent_c);
	}
	return NULL;
}



static rbtreeNode_t* insert_rc(rbtreeNode_t* sub_root_c, rbtreeNode_t* item, uint8_t* rc_color, uint8_t* rc_dir, rbtreeNode_t** replaced) {
	rbtreeNode_t* bare_ptr = GET_PTR(sub_root_c);
	if (!GET_PTR(sub_root_c)) {
		PAINT_RED(item);
#ifdef __DBG
		stack_bottom = (__cdsl_uaddr_t) &sub_root_c;
#endif
		return item;
	}
	if (bare_ptr->key < item->key) {
		bare_ptr->right = insert_rc(bare_ptr->right, item, rc_color, rc_dir, replaced);
		*rc_color <<= 1;
		*rc_dir <<= 1;
		*rc_color |= GET_COLOR(bare_ptr->right);
		*rc_dir |= CTX_RIGHT;
		if ((*rc_color & RCC_CTX_PATTERN) == RCC_CTX_PATTERN_REDRED) {
			sub_root_c = resolve_red_red(sub_root_c, *rc_color, *rc_dir);
			*rc_color = GET_COLOR(bare_ptr->right);
		}
		return sub_root_c;
	} else {
		if (replaced && (bare_ptr->key == item->key)) {
			// if the key value of new item collides to another and the insert operation is performed with (is_set == true)
			// replace old one with new item
			*replaced = bare_ptr;
			item->left = bare_ptr->left;
			item->right = bare_ptr->right;
			return (rbtreeNode_t*) ((__cdsl_uaddr_t) item | GET_COLOR(sub_root_c));
		}
		bare_ptr->left = insert_rc(bare_ptr->left, item, rc_color, rc_dir, replaced);
		*rc_color <<= 1;
		*rc_dir <<= 1;
		*rc_color |= GET_COLOR(bare_ptr->left);
		*rc_dir |= CTX_LEFT;
		if((*rc_color & RCC_CTX_PATTERN) == RCC_CTX_PATTERN_REDRED) {
			sub_root_c = resolve_red_red(sub_root_c, *rc_color, *rc_dir);
			*rc_color = GET_COLOR(bare_ptr->left);
		}
		return sub_root_c;
	}
}

/**
 *  try to resolve black-black situation
 *  in this context, parent_c is parent of double blacked node
 *  and it's actually root of sub-tree where double black is dealt with
 *  if there is any rotation, new root of sub-tree will be returned
 *  if there is no rotation, so root of sub-tree is kept same, just return original parent_c
 *  if fail to resolve double black return original parent but write PATTERN_BLACK_BLACK into ctx
 *
 *  by checking ctx, caller knows the result of resolving operation
 *
 *  ctx bit pattern should place
 *  [ ... |  2   |   1   |   0  ]
 *                parent double black
 */
static rbtreeNode_t* resolve_black_black(rbtreeNode_t* parent_c, uint8_t dir_ctx, uint8_t* ctx) {
	if (!parent_c) {
		return NULL;
	}
	rbtreeNode_t** sibling = NULL;

	if (!GET_PTR(parent_c)->left || !GET_PTR(parent_c)->right) {
		/*
		 * if this node is leaf it's one of below
		 *        p                        p
		 *      /   \          or        /   \          or
		 *   nl(bb)  nl                 nl   nl(bb)
		 *
		 *
		 *        p                        p
		 *      /   \          or        /   \
		 *   s0(bb)  nl                 nl   s0(bb)
		 */
		if (GET_COLOR(parent_c) == RED) {
			/*
			 * just repaint p into black, if p is red luckily.
			 * otherwise, just return and caller might be deal with double black
			 */
			PAINT_BLACK(parent_c);
			*ctx = (1 << CTX_BB);
			return parent_c;
		}
		*ctx &= ~(1 << CTX_BB);
		return parent_c;
	}

	if ((dir_ctx & 1) == CTX_LEFT) {
		/*
		 *  check right sibling
		 */
		sibling = &GET_PTR(parent_c)->right;
		if ((GET_COLOR(GET_PTR(*sibling)->left) == BLACK)
				&& (GET_COLOR(GET_PTR(*sibling)->right) == BLACK)) {
			if (GET_COLOR(*sibling) == BLACK) {
				/*
				 * black sibling with black children
				 * depend on parent color there are two option
				 */
				if (GET_COLOR(parent_c) == BLACK) {
					/*
					 * parent is black
					 * then all the nodes in sub-tree are black
					 * in this case, can't resolved within this sub-tree
					 */
					*ctx |= (1 << CTX_BB);
					return parent_c;
				} else {
					/*
					 * parent is red
					 * then just repaint the sibling into red and black-black resolved
					 */

					PAINT_BLACK(parent_c);
					PAINT_COLOR(*sibling, RED);
					*ctx &= ~(1 << CTX_BB);
					return parent_c;
				}
			} else {
				/*
				 * red sibling with black children
				 *
				 * paint sibling black
				 */
				PAINT_BLACK(*sibling);
				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			}
		} else {
			/*
			 * one or more child of sibling(right) is red
			 * obviously, the sibling is black because successive red
			 * is not allowed in red black tree
			 * so we assume sibling is black here
			 *
			 */

			if (GET_COLOR(GET_PTR(*sibling)->right) == RED) {
				/* resolve double black like below
				 *       p                    s1(r)
				 *      /  \                 /   \
				 *  s0(bb) s1(b)       -->  p(b)  c(b)
				 *           \             /
				 *           c(r)         s0(b)
				 */
				parent_c = rotate_left(parent_c);
				PAINT_RED(parent_c);
				PAINT_BLACK(GET_PTR(parent_c)->left);
				PAINT_BLACK(GET_PTR(parent_c)->right);

				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			} else {
				/* resolve double black like below
				 *       p                  p                  c(r)
				 *      /  \              /   \               /    \
				 *  s0(bb) s1(b)    --> s0(bb) c(r)    -->  p(b)   s1(b)
				 *         /                     \          /
				 *       c(r)                    s1(b)    s0(b)
				 */
				*sibling = rotate_right(*sibling);
				parent_c = rotate_left(parent_c);
				PAINT_RED(parent_c);
				PAINT_BLACK(GET_PTR(parent_c)->left);
				PAINT_BLACK(GET_PTR(parent_c)->right);
				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			}
		}
	} else {
		/*
		 *  check right sibling
		 */
		sibling = &GET_PTR(parent_c)->left;
		if ((GET_COLOR(GET_PTR(*sibling)->left) == BLACK)
				&& (GET_COLOR(GET_PTR(*sibling)->right) == BLACK)) {
			if (GET_COLOR(*sibling) == BLACK) {
				if (GET_COLOR(parent_c) == BLACK) {
					PAINT_COLOR(*sibling, RED);
					return parent_c;
				} else {
					PAINT_BLACK(parent_c);
					PAINT_COLOR(*sibling, RED);
					*ctx &= ~(1 << CTX_BB);
					return parent_c;
				}
			} else {
				PAINT_BLACK(*sibling);
				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			}
		} else {
			if (GET_COLOR(GET_PTR(*sibling)->left) == RED) {
				parent_c = rotate_right(parent_c);
				PAINT_RED(parent_c);
				PAINT_BLACK(GET_PTR(parent_c)->left);
				PAINT_BLACK(GET_PTR(parent_c)->right);
				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			} else {
				*sibling = rotate_left(*sibling);
				parent_c = rotate_right(parent_c);
				PAINT_RED(parent_c);
				PAINT_BLACK(GET_PTR(parent_c)->left);
				PAINT_BLACK(GET_PTR(parent_c)->right);
				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			}
		}
	}
	return NULL;
}

static rbtreeNode_t* up_from_rightmost_rc(rbtreeNode_t* node, rbtreeNode_t** rm, uint8_t* ctx) {
	/*
	 *  1. if rightmost(RM) node has left child, replace RM node with left child
	 *  and paint it black (no black black)
	 *  1 > no double blakc
	 *      p             p
	 *       \             \
	 *       rm(b)    ->   c(b)
	 *       /
	 *     c(r)
	 *
	 *  2 > no double black
	 *      p             p
	 *       \             \
	 *       rm(r)    ->   c(b)
	 *       /
	 *    c(b)
	 *
	 *  2. if RM node has no child(leaf node), then replace it with null node.
	 *   1 > double black
	 *        p                p
	 *         \                \
	 *         rm(b)      ->   nl(bb)
	 *         / \
	 *      nl(b) nl(b)
	 *  2 >  no double black
	 *       p                 p
	 *        \                 \
	 *        rm(r)       ->    nl(b)
	 *        / \
	 *     nl(b) nl(b)
	 *
	 *
	 */

	if (!GET_PTR(GET_PTR(node)->right) && !GET_PTR(GET_PTR(node)->left)) {
		/*
		 *  current rightmost is leaf node
		 */
		if (GET_COLOR(node) == RED) {
			/*
			 * fortunately current is red
			 * so just replace it with null causing no double black
			 */
			*ctx &= ~(1 << CTX_BB);	// clear BB
			*rm = node;
		} else {
			*ctx = (1 << CTX_BB); // set BB
			*rm = node;
		}
		return NULL;
	} else {
		if (!GET_PTR(GET_PTR(node)->right)) {
			/*
			 * RM node with left child case
			 */
			*ctx &= ~(1 << CTX_BB);
			*rm = node;
			PAINT_BLACK(GET_PTR(node)->left);
			return GET_PTR(node)->left;
		}

		/*
		 * keep going right
		 */
		GET_PTR(node)->right = up_from_rightmost_rc(GET_PTR(node)->right, rm,ctx);
		if (*ctx & (1 << CTX_BB)) {
			node = resolve_black_black(node, CTX_RIGHT, ctx);
		}
		return node;
	}

	if (!GET_PTR(node)->right) {

		if (GET_COLOR(node) != RED) {
			/*
			 *  black black case,
			 *  set BB_ALERT in pointer and return
			 */
			if (GET_PTR(GET_PTR(node)->left)) {
				/*
				 * if there is left child of rightmost
				 * and if it's not black
				 */
				if (GET_COLOR(GET_PTR(node)->left) == RED) {
					*ctx &= ~(1 << CTX_BB);
					*rm = node;
					return GET_PTR(GET_PTR(node)->left);
				}
			}
			*ctx = (1 << CTX_BB);
			*rm = node;
		} else {
			*ctx &= ~(1 << CTX_BB);
			*rm = node;
		}
		return NULL;
	}

	GET_PTR(node)->right = up_from_rightmost_rc(GET_PTR(node)->right, rm, ctx);
	if (*ctx & (1 << CTX_BB)) {
		node = resolve_black_black(node, CTX_RIGHT, ctx);
	}
	return node;
}

static rbtreeNode_t* up_from_leftmost_rc(rbtreeNode_t* node, rbtreeNode_t** lm, uint8_t* ctx) {
	if (!GET_PTR(GET_PTR(node)->right) && !GET_PTR(GET_PTR(node)->left)) {
		/*
		 *  current rightmost is leaf node
		 */
		if (GET_COLOR(node) == RED) {
			/*
			 * fortunately current is red
			 * so just replace it with null causing no double black
			 */
			*ctx &= ~(1 << CTX_BB);	// clear BB
			*lm = node;
		} else {
			*ctx = (1 << CTX_BB); // set BB
			*lm = node;
		}
		return NULL;
	} else {
		if (!GET_PTR(GET_PTR(node)->left)) {
			*ctx &= ~(1 << CTX_BB);
			*lm = node;
			PAINT_BLACK(GET_PTR(node)->right);
			return GET_PTR(node)->right;
		}

		/*
		 * keep going right
		 */
		GET_PTR(node)->left = up_from_leftmost_rc(GET_PTR(node)->left, lm, ctx);
		if (*ctx & (1 << CTX_BB)) {
			node = resolve_black_black(node, CTX_LEFT, ctx);
		}
		return node;
	}

	if (!GET_PTR(node)->left) {

		if (GET_COLOR(node) != RED) {
			/*
			 *  black black case,
			 *  set BB_ALERT in pointer and return
			 */
			if (GET_PTR(GET_PTR(node)->right)) {
				/*
				 * if there is left child of rightmost
				 * and if it's not black
				 */
				if (GET_COLOR(GET_PTR(node)->right) == RED) {
					*ctx &= ~(1 << CTX_BB);
					*lm = node;
					return GET_PTR(GET_PTR(node)->right);
				}
			}
			*ctx = (1 << CTX_BB);
			*lm = node;
		} else {
			*ctx &= ~(1 << CTX_BB);
			*lm = node;
		}
		return NULL;
	}

	GET_PTR(node)->left = up_from_leftmost_rc(GET_PTR(node)->left, lm, ctx);
	if (*ctx & (1 << CTX_BB)) {
		node = resolve_black_black(node, CTX_LEFT, ctx);
	}
	return node;
}


static void _write_ser_node_header (cdsl_serializeNode_t* node_head, const void* node) {
	if(node_head == NULL) return;
	rbtreeNode_t* rbnode = (rbtreeNode_t*) node;
	SET_SPEC(node_head,(GET_COLOR(rbnode->left) << 1 | GET_COLOR(rbnode->right)));
}

static void _write_serialize_header(cdsl_serializeHeader_t* header) {
	header->type |= SUB_TYPE_REDBLACK;
}

static void _build_node(const cdsl_serializeNode_t* node_header, void* node) {
	uint8_t spec = GET_SPEC(node_header);
	rbtreeNode_t* rb_node = (rbtreeNode_t*) node;
	PAINT_COLOR(rb_node->left, spec >> 1);
	PAINT_COLOR(rb_node->right, (spec & 1));
}
