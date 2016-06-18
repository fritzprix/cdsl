/*
 * cdsl_nrbtree.c
 *
 *  Created on: Mar 25, 2016
 *      Author: innocentevil
 */


#include "cdsl_nrbtree.h"


#define GET_PTR(node)                         ((nrbtreeNode_t*)(((__cdsl_uaddr_t) node) & ~1))
#define RED                                   ((uint8_t) 1)
#define BLACK                                 ((uint8_t) 0)
#define PAINT_RED(node) do {\
	node =  (nrbtreeNode_t*) ((__cdsl_uaddr_t) node | RED);\
}while(0)

#define PAINT_BLACK(node) do {\
		node =  (nrbtreeNode_t*) ((__cdsl_uaddr_t) node & ~RED);\
}while(0)

#define PAINT_COLOR(node, color) do {\
	PAINT_BLACK(node);\
	node = (nrbtreeNode_t*) ((__cdsl_uaddr_t) node | color);\
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

const char* COLOR_STRING[] = {
		"BLACK",
		"RED"
};


static int max_depth_rc(nrbtreeNode_t* node);
static nrbtreeNode_t* insert_rc(nrbtreeNode_t* sub_root_c, nrbtreeNode_t* item, uint8_t* rc_color, uint8_t* rc_dir);
static nrbtreeNode_t* delete_rc(nrbtreeNode_t* sub_root_c, trkey_t key, nrbtreeNode_t** rm, uint8_t* ctx);
static nrbtreeNode_t* delete_lm_rc(nrbtreeNode_t* sub_root_c, nrbtreeNode_t** rm, uint8_t* ctx);
static nrbtreeNode_t* delete_rm_rc(nrbtreeNode_t* sub_root_c, nrbtreeNode_t** rm, uint8_t* ctx);
static nrbtreeNode_t* rotate_left(nrbtreeNode_t* gparent_c);
static nrbtreeNode_t* rotate_right(nrbtreeNode_t* gparent_c);
static nrbtreeNode_t* update_color(nrbtreeNode_t* node_c);
static nrbtreeNode_t* resolve_red_red(nrbtreeNode_t* gparent_c,uint8_t color_ctx, uint8_t dir_ctx);
static nrbtreeNode_t* resolve_black_black(nrbtreeNode_t* parent_c, uint8_t dir_ctx, uint8_t* ctx);

static nrbtreeNode_t* up_from_rightmost_rc(nrbtreeNode_t* node,nrbtreeNode_t** rm,uint8_t* ctx);
static nrbtreeNode_t* up_from_leftmost_rc(nrbtreeNode_t* node, nrbtreeNode_t** lm,uint8_t* ctx);


static void print_tab(int cnt);
static void node_print_rc(nrbtreeNode_t* node, int level);

static volatile __cdsl_uaddr_t stack_top;
static volatile __cdsl_uaddr_t stack_bottom;


void cdsl_nrbtreeRootInit(nrbtreeRoot_t* rootp) {
	if(rootp == NULL)
		return;
	rootp->entry = NULL;
}

void cdsl_nrbtreeNodeInit(nrbtreeNode_t* node, trkey_t key) {
	if(node == NULL)
		return;
	if((size_t) node & 1) {
		PRINT("unaligned pointer!!\n");
	}
	node->left = node->right = NULL;
	node->key = key;
}

nrbtreeNode_t* cdsl_nrbtreeInsert(nrbtreeRoot_t* rootp,nrbtreeNode_t* item) {
	if(!rootp)
		return NULL;
	if(!rootp->entry)
	{
		rootp->entry = item;
		return item;
	}
	stack_top = (__cdsl_uaddr_t) &rootp;

	uint8_t dir = 0;
	uint8_t color = 0;
	rootp->entry = insert_rc(rootp->entry, item, &color , &dir);
	__dev_log("stack usage : %lu\n",stack_top - stack_bottom);
	return item;
}

nrbtreeNode_t* cdsl_nrbtreeLookup(nrbtreeRoot_t* rootp, trkey_t key) {
	if(!rootp)
		return NULL;
	nrbtreeNode_t* cur_node = rootp->entry;		// always black so don't need to use GET_PTR() macro
	while(cur_node) {
		if(GET_PTR(cur_node)->key > key) {
			cur_node = GET_PTR(cur_node)->left;
		}else if(GET_PTR(cur_node)->key < key) {
			cur_node = GET_PTR(cur_node)->right;
		}else {
			return GET_PTR(cur_node);
		}
	}
	return NULL;
}

nrbtreeNode_t* cdsl_nrbtreeDelete(nrbtreeRoot_t* rootp, trkey_t key)
{
	if(!rootp)
		return NULL;
	nrbtreeNode_t* del = NULL;
	uint8_t ctx = 0;
	rootp->entry = delete_rc(rootp->entry, key, &del, &ctx);
	return GET_PTR(del);
}


nrbtreeNode_t* cdsl_nrbtreeDeleteMin(nrbtreeRoot_t* rootp)
{
	if(!rootp)
		return NULL;
	if(!GET_PTR(rootp->entry))
		return NULL;
	nrbtreeNode_t* del = NULL;
	uint8_t ctx = 0;
	rootp->entry = delete_lm_rc(rootp->entry, &del, &ctx);
	return GET_PTR(del);
}

nrbtreeNode_t* cdsl_nrbtreeDeleteMax(nrbtreeRoot_t* rootp)
{
	if(!rootp)
		return NULL;
	if(!GET_PTR(rootp->entry))
		return NULL;
	nrbtreeNode_t* del = NULL;
	uint8_t ctx = 0;
	rootp->entry = delete_rm_rc(rootp->entry, &del, &ctx);
	return GET_PTR(del);
}





#ifdef __DBG
void cdsl_nrbtreePrint_dev (nrbtreeRoot_t* root)
{
	if(!root)
		return;
	printf("\n");
	node_print_rc(root->entry, 0);
	printf("\n");

}
#endif

static void print_tab(int cnt){
	while(cnt--)
		PRINT("\t");
}

static void node_print_rc(nrbtreeNode_t* node, int level) {
	if(!GET_PTR(node))
	{
		print_tab(level);
		PRINT("NIL(black) \n");
		return;
	}
	node_print_rc(GET_PTR(node)->right, level + 1);
	print_tab(level); PRINT("%s node : %lu / order %d \n", COLOR_STRING[GET_COLOR(node)], GET_PTR(node)->key, level);
	node_print_rc(GET_PTR(node)->left, level + 1);
}

/*
 *  *  ctx[0] = color
 *  ctx[1] = dir
 *  ctx[2] = is_bb
 *  ctx[3] = res for future
 */

static nrbtreeNode_t* delete_rc(nrbtreeNode_t* sub_root_c, trkey_t key, nrbtreeNode_t** rm, uint8_t* ctx)
{
	if(!sub_root_c)
	{
		if(rm)
			*rm = NULL;
		return NULL;
	}

	if(GET_PTR(sub_root_c)->key < key)
	{
		GET_PTR(sub_root_c)->right = delete_rc(GET_PTR(sub_root_c)->right,key, rm, ctx);
		if(*ctx & (1 << CTX_BB))
		{
			sub_root_c = resolve_black_black(sub_root_c, CTX_RIGHT, ctx);
		}
		return sub_root_c;
	}
	else if(GET_PTR(sub_root_c)->key > key)
	{
		GET_PTR(sub_root_c)->left = delete_rc(GET_PTR(sub_root_c)->left,key, rm, ctx);
		if(*ctx & (1 << CTX_BB))
		{
			sub_root_c = resolve_black_black(sub_root_c, CTX_LEFT, ctx);
		}
		return sub_root_c;
	}
	if(rm)
		*rm = sub_root_c;
	if (GET_PTR(sub_root_c)->left) {
		GET_PTR(sub_root_c)->left = up_from_rightmost_rc(GET_PTR(sub_root_c)->left, &sub_root_c,ctx);
		GET_PTR(sub_root_c)->right = GET_PTR(*rm)->right;
		if (*ctx & (1 << CTX_BB)) {
			sub_root_c = resolve_black_black(sub_root_c, CTX_LEFT, ctx);
		}
		return sub_root_c;
	} else if (GET_PTR(sub_root_c)->right) {
		GET_PTR(sub_root_c)->right = up_from_leftmost_rc(GET_PTR(sub_root_c)->right, &sub_root_c,ctx);
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



static nrbtreeNode_t* delete_lm_rc(nrbtreeNode_t* sub_root_c, nrbtreeNode_t** rm, uint8_t* ctx)
{
	if(!GET_PTR(sub_root_c)->left)
	{
		*rm = sub_root_c;
		if(GET_PTR(sub_root_c)->right) {
			GET_PTR(sub_root_c)->right = up_from_leftmost_rc(GET_PTR(sub_root_c)->right, &sub_root_c, ctx);
			GET_PTR(sub_root_c)->left = GET_PTR(*rm)->left;
			if(*ctx & (1 << CTX_BB)) {
				sub_root_c = resolve_black_black(sub_root_c, CTX_RIGHT, ctx);
			}
			return sub_root_c;
		}
		return NULL;
	}
	GET_PTR(sub_root_c)->left = delete_lm_rc(GET_PTR(sub_root_c)->left, rm, ctx);
	if(*ctx & (1 << CTX_BB))
	{
		sub_root_c = resolve_black_black(sub_root_c, CTX_LEFT, ctx);
	}
	return sub_root_c;
}

static nrbtreeNode_t* delete_rm_rc(nrbtreeNode_t* sub_root_c, nrbtreeNode_t** rm, uint8_t* ctx)
{
	if(!GET_PTR(sub_root_c)->right)
	{
		*rm = sub_root_c;
		if(GET_PTR(sub_root_c)->left) {
			GET_PTR(sub_root_c)->left = up_from_rightmost_rc(GET_PTR(sub_root_c)->left, &sub_root_c, ctx);
			GET_PTR(sub_root_c)->right = GET_PTR(*rm)->right;
			if(*ctx & (1 << CTX_BB)) {
				sub_root_c = resolve_black_black(sub_root_c, CTX_LEFT, ctx);
			}
			return sub_root_c;
		}
		return NULL;
	}
	GET_PTR(sub_root_c)->right = delete_rm_rc(GET_PTR(sub_root_c)->right, rm, ctx);
	if(*ctx & (1 << CTX_BB))
	{
		sub_root_c = resolve_black_black(sub_root_c, CTX_RIGHT, ctx);
	}
	return sub_root_c;
}

static nrbtreeNode_t* rotate_left(nrbtreeNode_t* gparent_c)
{
	/*
	 *         n0(gparent_c)          n1
	 *        /  \          -->      / \
	 *      n2   n1                n0   n4
	 *           / \              / \    \
	 *         n3  n4            n2 n3    n1
	 */
	if(!GET_PTR(gparent_c))
		return NULL;
	nrbtreeNode_t* ngp = GET_PTR(gparent_c)->right;
	GET_PTR(gparent_c)->right = GET_PTR(ngp)->left;
	GET_PTR(ngp)->left = update_color(gparent_c);

	return update_color(ngp);
}

static nrbtreeNode_t* rotate_right(nrbtreeNode_t* gparent_c)
{
	/*
	 *         n0(gparent_c)          n2
	 *        /  \          -->      / \
	 *      n2   n1                 n3  n0
	 *     / \                         / \
	 *    n3  n4                      n4  n1
	 */
	if(!GET_PTR(gparent_c))
		return NULL;
	nrbtreeNode_t* ngp = GET_PTR(gparent_c)->left;
	GET_PTR(gparent_c)->left = GET_PTR(ngp)->right;
	GET_PTR(ngp)->right = update_color(gparent_c);

	return update_color(ngp);
}

static nrbtreeNode_t* update_color(nrbtreeNode_t* node_c)
{
	/*
	 *      r0         b0
	 *     / \        / \
	 *    b1 b2      r1 r2
	 */
	if(!GET_PTR(node_c))
		return NULL;

	if((GET_COLOR(GET_PTR(node_c)->left) == BLACK) && (GET_COLOR(GET_PTR(node_c)->right) == BLACK))
	{
		return (nrbtreeNode_t*) ((__cdsl_uaddr_t)node_c | RED);
	}
	else
	{
		return (nrbtreeNode_t*) ((__cdsl_uaddr_t)node_c & ~RED);
	}
	return node_c;
}

static int max_depth_rc(nrbtreeNode_t* node)
{
	if(!GET_PTR(node))
		return 0;
	int a,b;
	a = max_depth_rc(GET_PTR(node)->left);
	b = max_depth_rc(GET_PTR(node)->right);
	return a > b? a : b;
}


static nrbtreeNode_t* resolve_red_red(nrbtreeNode_t* gparent_c,uint8_t color_ctx, uint8_t dir_ctx)
{
	/*                     [  ... |  3 bit |  2 bit |  1 bit |  0 bit ]
	 *  bit pattern of ctx [  ... |  ggp   |  gp    |  p     |  new   ]
	 */
	if(!GET_PTR(gparent_c))
		return gparent_c;
	if((GET_COLOR(GET_PTR(gparent_c)->left) == RED) && (GET_COLOR(GET_PTR(gparent_c)->right) == RED))
	{
		/*
		 *    r0 or b0                                          r0
		 *      / \                                            /  \
		 *     r1 r2       (repaint r1,r2 -> b1,b2)--->      b1    b2
		 *    /                                              /
		 *   r3(new node)                                  r3(new node)
		 */
		PAINT_BLACK(GET_PTR(gparent_c)->left);
		PAINT_BLACK(GET_PTR(gparent_c)->right);
		PAINT_RED(gparent_c);
		return gparent_c;
	}
	switch(dir_ctx & PATTERN_WINDOW)
	{
	case PATTERN_LEFT_RIGHT:
		/*
		 * resolve left-right sequence into left-left
		 *         gp                        gp
		 *        /  \                      /
		 *      left  ..        --->      left
		 *      /  \                      /  \
		 *     ..  new                  new
		 */
		GET_PTR(gparent_c)->left = rotate_left(GET_PTR(gparent_c)->left);
		/*
		 *  rotate performed to make left-left sequence
		 *  so now PATTERN_LEFT_LEFT case
		 *  I think pass-through can be used here by just omit break statement
		 *  SO PLEASE DON'T PUT BREAK HERE. IT'S INTENDED
		 */
	case PATTERN_LEFT_LEFT:
		return rotate_right(gparent_c);
	case PATTERN_RIGHT_LEFT:
		/*
		 * resolve right-left sequence into left-left
		 *         gp                        gp
		 *        /  \                      /  \
		 *      ..  right        --->     ..   right
		 *           /  \                      /   \
		 *         new  ..                    ..   new
		 */
		GET_PTR(gparent_c)->right = rotate_right(GET_PTR(gparent_c)->right);
		/*
		 *  rotate performed to make right-right sequence
		 *  so now PATTERN_RIGHT_RIGHT case
		 *  I think pass-through can be used here by just omit break statement
		 *  SO PLEASE DON'T PUT BREAK HERE. IT'S INTENDED
		 */
	case PATTERN_RIGHT_RIGHT:
		return rotate_left(gparent_c);
	}
	return NULL;
}

#define RC_CTX_PATTERN         ((uint8_t) 0xC0)
#define RC_CTX_PATTERN_REDRED  ((uint8_t) 0xC0)

static nrbtreeNode_t* insert_rc(nrbtreeNode_t* sub_root_c, nrbtreeNode_t* item,uint8_t* rc_color, uint8_t* rc_dir)
{
	if(!GET_PTR(sub_root_c))
	{
		PAINT_RED(item);
		stack_bottom = (__cdsl_uaddr_t) &sub_root_c;
		return item;
	}
	else
	{
		if(GET_PTR(sub_root_c)->key < GET_PTR(item)->key)
		{
			GET_PTR(sub_root_c)->right = insert_rc(GET_PTR(sub_root_c)->right, item, rc_color, rc_dir);

			*rc_color >>= 1;
			*rc_dir >>= 1;
			*rc_color |= (GET_COLOR(GET_PTR(sub_root_c)->right) << 7);
			*rc_dir |= (CTX_RIGHT << 7);


			if((*rc_color & RC_CTX_PATTERN) == RC_CTX_PATTERN_REDRED)
			{
				sub_root_c = resolve_red_red(sub_root_c, (*rc_color) >> 6 , (*rc_dir) >> 6);
				*rc_color = 0;
			}
			return sub_root_c;
		}
		else
		{
			GET_PTR(sub_root_c)->left = insert_rc(GET_PTR(sub_root_c)->left, item , rc_color, rc_dir);

			*rc_color >>= 1;
			*rc_dir >>= 1;
			*rc_color |= (GET_COLOR(GET_PTR(sub_root_c)->left) << 7);
			*rc_dir |= (CTX_LEFT << 7);



			if((*rc_color & RC_CTX_PATTERN) == RC_CTX_PATTERN_REDRED)
			{
				sub_root_c = resolve_red_red(sub_root_c, (*rc_color) >> 6 , (*rc_dir) >> 6);
				*rc_color = 0;
			}
			return sub_root_c;
		}
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
static nrbtreeNode_t* resolve_black_black(nrbtreeNode_t* parent_c, uint8_t dir_ctx, uint8_t* ctx)
{
	if(!parent_c)
		return NULL;
	nrbtreeNode_t** sibling = NULL;

	if(!GET_PTR(parent_c)->left || !GET_PTR(parent_c)->right)
	{
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
		if(GET_COLOR(parent_c) == RED)
		{
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

	if((dir_ctx & 1) == CTX_LEFT)
	{
		/*
		 *  check right sibling
		 */
		sibling = &GET_PTR(parent_c)->right;
		if((GET_COLOR(GET_PTR(*sibling)->left) == BLACK) \
				&& (GET_COLOR(GET_PTR(*sibling)->right) == BLACK))
		{
			if(GET_COLOR(*sibling) == BLACK)
			{
				/*
				 * black sibling with black children
				 * depend on parent color there are two option
				 */
				if(GET_COLOR(parent_c) == BLACK)
				{
					/*
					 * parent is black
					 * then all the nodes in sub-tree are black
					 * in this case, can't resolved within this sub-tree
					 */
					*ctx |= (1 << CTX_BB);
					return parent_c;
				}
				else
				{
					/*
					 * parent is red
					 * then just repaint the sibling into red and black-black resolved
					 */

					PAINT_BLACK(parent_c);
					PAINT_COLOR(*sibling, RED);
					*ctx &= ~(1 << CTX_BB);
					return parent_c;
				}
			}
			else
			{
				/*
				 * red sibling with black children
				 *
				 * paint sibling black
				 */
				PAINT_BLACK(*sibling);
				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			}
		}
		else
		{
			/*
			 * one or more child of sibling(right) is red
			 * obviously, the sibling is black because successive red
			 * is not allowed in red black tree
			 * so we assume sibling is black here
			 *
			 */

			if(GET_COLOR(GET_PTR(*sibling)->right) == RED)
			{
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
			}
			else
			{
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
	}
	else
	{
		/*
		 *  check right sibling
		 */
		sibling = &GET_PTR(parent_c)->left;
		if((GET_COLOR(GET_PTR(*sibling)->left) == BLACK) \
				&& (GET_COLOR(GET_PTR(*sibling)->right) == BLACK))
		{
			if(GET_COLOR(*sibling) == BLACK)
			{
				if(GET_COLOR(parent_c) == BLACK)
				{
					PAINT_COLOR(*sibling, RED);
					return parent_c;
				}
				else
				{
					PAINT_BLACK(parent_c);
					PAINT_COLOR(*sibling, RED);
					*ctx &= ~(1 << CTX_BB);
					return parent_c;
				}
			}
			else
			{
				PAINT_BLACK(*sibling);
				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			}
		}
		else
		{
			if(GET_COLOR(GET_PTR(*sibling)->left) == RED)
			{
				parent_c = rotate_right(parent_c);
				PAINT_RED(parent_c);
				PAINT_BLACK(GET_PTR(parent_c)->left);
				PAINT_BLACK(GET_PTR(parent_c)->right);
				*ctx &= ~(1 << CTX_BB);
				return parent_c;
			}
			else
			{
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

static nrbtreeNode_t* up_from_rightmost_rc(nrbtreeNode_t* node,nrbtreeNode_t** rm,uint8_t* ctx)
{
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

	if(!GET_PTR(GET_PTR(node)->right) && !GET_PTR(GET_PTR(node)->left))
	{
		/*
		 *  current rightmost is leaf node
		 */
		if(GET_COLOR(node) == RED)
		{
			/*
			 * fortunately current is red
			 * so just replace it with null causing no double black
			 */
			*ctx &= ~(1 << CTX_BB);	// clear BB
			*rm = node;
		}
		else
		{
			*ctx = (1 << CTX_BB); // set BB
			*rm = node;
		}
		return NULL;
	}
	else
	{
		if(!GET_PTR(GET_PTR(node)->right))
		{
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
		GET_PTR(node)->right = up_from_rightmost_rc(GET_PTR(node)->right, rm, ctx);
		if(*ctx & (1 << CTX_BB))
		{
			node = resolve_black_black(node, CTX_RIGHT, ctx);
		}
		return node;
	}

	if(!GET_PTR(node)->right)
	{

		if(GET_COLOR(node) != RED)
		{
			/*
			 *  black black case,
			 *  set BB_ALERT in pointer and return
			 */
			if(GET_PTR(GET_PTR(node)->left))
			{
				/*
				 * if there is left child of rightmost
				 * and if it's not black
				 */
				if(GET_COLOR(GET_PTR(node)->left) == RED)
				{
					*ctx &= ~(1 << CTX_BB);
					*rm = node;
					return GET_PTR(GET_PTR(node)->left);
				}
			}
			*ctx = (1 << CTX_BB);
			*rm = node;
		}
		else
		{
			*ctx &= ~(1 << CTX_BB);
			*rm = node;
		}
		return NULL;
	}

	GET_PTR(node)->right = up_from_rightmost_rc(GET_PTR(node)->right, rm, ctx);
	if(*ctx & (1 << CTX_BB))
	{
		node = resolve_black_black(node,CTX_RIGHT, ctx);
	}
	return node;
}

static nrbtreeNode_t* up_from_leftmost_rc(nrbtreeNode_t* node, nrbtreeNode_t** lm,uint8_t* ctx)
{
	if(!GET_PTR(GET_PTR(node)->right) && !GET_PTR(GET_PTR(node)->left))
	{
		/*
		 *  current rightmost is leaf node
		 */
		if(GET_COLOR(node) == RED)
		{
			/*
			 * fortunately current is red
			 * so just replace it with null causing no double black
			 */
			*ctx &= ~(1 << CTX_BB);	// clear BB
			*lm = node;
		}
		else
		{
			*ctx = (1 << CTX_BB); // set BB
			*lm = node;
		}
		return NULL;
	}
	else
	{
		if(!GET_PTR(GET_PTR(node)->left))
		{
			*ctx &= ~(1 << CTX_BB);
			*lm = node;
			PAINT_BLACK(GET_PTR(node)->right);
			return GET_PTR(node)->right;
		}

		/*
		 * keep going right
		 */
		GET_PTR(node)->left = up_from_leftmost_rc(GET_PTR(node)->left, lm, ctx);
		if(*ctx & (1 << CTX_BB))
		{
			node = resolve_black_black(node, CTX_LEFT,  ctx);
		}
		return node;
	}

	if(!GET_PTR(node)->left)
	{

		if(GET_COLOR(node) != RED)
		{
			/*
			 *  black black case,
			 *  set BB_ALERT in pointer and return
			 */
			if(GET_PTR(GET_PTR(node)->right))
			{
				/*
				 * if there is left child of rightmost
				 * and if it's not black
				 */
				if(GET_COLOR(GET_PTR(node)->right) == RED)
				{
					*ctx &= ~(1 << CTX_BB);
					*lm = node;
					return GET_PTR(GET_PTR(node)->right);
				}
			}
			*ctx = (1 << CTX_BB);
			*lm = node;
		}
		else
		{
			*ctx &= ~(1 << CTX_BB);
			*lm = node;
		}
		return NULL;
	}

	GET_PTR(node)->left = up_from_leftmost_rc(GET_PTR(node)->left, lm, ctx);
	if(*ctx & (1 << CTX_BB))
	{
		node = resolve_black_black(node,CTX_LEFT, ctx);
	}
	return node;
}

