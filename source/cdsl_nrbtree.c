/*
 * cdsl_nrbtree.c
 *
 *  Created on: Mar 25, 2016
 *      Author: innocentevil
 */


#include <stdio.h>
#include <stdlib.h>
#include "cdsl_nrbtree.h"


#define GET_PTR(node)                         ((nrbtreeNode_t*)(((uint64_t) node) & ~1))
#define RED                                   ((uint8_t) 1)
#define BLACK                                 ((uint8_t) 0)
#define PAINT_RED(node) do {\
	node =  (nrbtreeNode_t*) ((uint64_t) node | RED);\
}while(0)

#define PAINT_BLACK(node) do {\
		node =  (nrbtreeNode_t*) ((uint64_t) node & ~RED);\
}while(0)

#define PAINT_COLOR(node, color) do {\
	PAINT_BLACK(node);\
	node = (nrbtreeNode_t*) ((uint64_t) node | color);\
}while(0)

#define GET_COLOR(node)						  ((uint32_t)(node) & 1)

#define CTX_RIGHT                             ((uint8_t) 1)
#define CTX_LEFT                              ((uint8_t) 0)


#define PATTERN_WINDOW                        ((uint8_t) 0x3)

#define PATTERN_RED_BLACK                     ((uint8_t) 0x2)
#define PATTERN_RED_RED                       ((uint8_t) 0x3)
#define PATTERN_BLACK_RED                     ((uint8_t) 0x1)
#define PATTERN_BLACK_BLACK                   ((uint8_t) 0x0)

#define BB_ALERT                              ((uint8_t) 0x1)

#define PATTERN_RIGHT_RIGHT                   ((uint8_t) 0x3)      // right-right
#define PATTERN_RIGHT_LEFT                    ((uint8_t) 0x2)      // right-left
#define PATTERN_LEFT_RIGHT                    ((uint8_t) 0x1)      // left-right
#define PATTERN_LEFT_LEFT                     ((uint8_t) 0x0)      // left-left

const char* COLOR_STRING[] = {
		"BLACK",
		"RED"
};


static int max_depth_rc(nrbtreeNode_t* node);
static nrbtreeNode_t* rotate_left(nrbtreeNode_t* gparent_c);
static nrbtreeNode_t* rotate_right(nrbtreeNode_t* gparent_c);
static nrbtreeNode_t* update_color(nrbtreeNode_t* node_c);
static nrbtreeNode_t* resolve_red_red(nrbtreeNode_t* gparent_c,uint8_t color_ctx, uint8_t dir_ctx);
static nrbtreeNode_t* resolve_black_black(nrbtreeNode_t* parent_c, uint8_t color_ctx, uint8_t dir_ctx, nrbtreeNode_t** ctx);

static nrbtreeNode_t* up_from_rightmost_rc(nrbtreeNode_t* node,nrbtreeNode_t** rm);
static nrbtreeNode_t* up_from_leftmost_rc(nrbtreeNode_t* node, nrbtreeNode_t** lm);

static BOOL replace_from_next(nrbtreeNode_t** node);
static BOOL replace_from_prev(nrbtreeNode_t** node);

static void print_tab(int cnt);
static void node_print_rc(nrbtreeNode_t* node,int order);


void cdsl_nrbtreeRootInit(nrbtreeRoot_t* rootp) {
	if(rootp == NULL)
		return;
	rootp->entry = NULL;
}

void cdsl_nrbtreeNodeInit(nrbtreeNode_t* node, rb_key_t key) {
	if(node == NULL)
		return;
	node->left = node->right = NULL;
	node->key = key;
}



nrbtreeNode_t* cdsl_nrbtreeInsert(nrbtreeRoot_t* rootp,nrbtreeNode_t* item) {
	if(!rootp || !item)
		return NULL;
	nrbtreeNode_t** cur_node = &rootp->entry;
	nrbtreeNode_t** parent = NULL;
	nrbtreeNode_t** gparent = NULL;
	nrbtreeNode_t** ggp = NULL;
	uint8_t dir_ctx = 0;
	uint8_t color_ctx = 0;
	if(!GET_PTR(*cur_node)) {
		rootp->entry = item;
		return item;
	}

	while(*cur_node)
		/*
		 * leaf node is always null
		 * so you don't need to mask out LSB of pointer (meaning GET_PTR() macro)
		 */
	{
		if(GET_PTR(*cur_node)->key > item->key)
		{
			if(!GET_PTR(*cur_node)->left)
			{
				// add item here
				GET_PTR(*cur_node)->left = item;
				PAINT_RED(GET_PTR(*cur_node)->left);
				// update context info
				color_ctx |= RED;
				dir_ctx |= CTX_LEFT;

				if((color_ctx & PATTERN_WINDOW) == PATTERN_RED_RED)
				{
					goto RESOLVE_REDRED;
				}
				return item;
			}
			// go to left
			ggp = gparent;
			gparent = parent;
			parent = cur_node;
			cur_node = &(GET_PTR(*cur_node)->left);
			color_ctx |= GET_COLOR(*cur_node);
			dir_ctx |= CTX_LEFT;
		}
		else
		{
			if(!GET_PTR(*cur_node)->right)
			{
				GET_PTR(*cur_node)->right = item;
				PAINT_RED(GET_PTR(*cur_node)->right);

				color_ctx |= RED;
				dir_ctx |= CTX_RIGHT;

				if((color_ctx & PATTERN_WINDOW) == PATTERN_RED_RED)
				{
					goto RESOLVE_REDRED;
				}
				return item;
			}
			// go to right
			ggp = gparent;
			gparent = parent;
			parent = cur_node;
			cur_node = &(GET_PTR(*cur_node)->right);
			color_ctx |= GET_COLOR(*cur_node);
			dir_ctx |= CTX_RIGHT;
		}
		// shift ctx left
		dir_ctx <<= 1;
		color_ctx <<= 1;
	}

RESOLVE_REDRED:

    /*
     *  hierarchical order is like below
     *  so grand parent of new item is 'parent'
     *  gparent - parent - cur_node - item
     */

	*parent = resolve_red_red(*parent, color_ctx,dir_ctx);
	color_ctx >>= 3;
	color_ctx <<= 1;
	color_ctx |= GET_COLOR(*parent);
	dir_ctx >>= 2;

	if((color_ctx & PATTERN_WINDOW) == PATTERN_RED_RED)
	{
		*ggp = resolve_red_red(*ggp, color_ctx,dir_ctx);
	}
	PAINT_BLACK(rootp->entry);
	return item;
}

nrbtreeNode_t* cdsl_nrbtreeLookup(nrbtreeRoot_t* rootp, rb_key_t key) {
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

/*
 *  1> rmv node is leaf node
 *    gp
 *     \
 *      p
 *     / \
 *    s  rmv
 *   / \ / \
 *
 *
 */

nrbtreeNode_t* cdsl_nrbtreeDelete(nrbtreeRoot_t* rootp, rb_key_t key)
{
	if(!rootp)
		return NULL;
	if(!GET_PTR(rootp->entry))
		return NULL;
	uint8_t color_ctx = 0;
	uint8_t dir_ctx = 0;
	uint32_t res = 0;
	nrbtreeNode_t** cur_node = &rootp->entry;
	nrbtreeNode_t** parent = NULL;
	nrbtreeNode_t** gparent = NULL;

	nrbtreeNode_t* rmv = NULL;

	while(*cur_node)
	{
		if(GET_PTR(*cur_node)->key > key)
		{
			dir_ctx |= CTX_LEFT;
			color_ctx |= GET_COLOR(*cur_node);
			gparent = parent;
			parent = cur_node;
			cur_node = &GET_PTR(*cur_node)->left;
		}
		else if(GET_PTR(*cur_node)->key < key)
		{
			dir_ctx |= CTX_RIGHT;
			color_ctx |= GET_COLOR(*cur_node);
			gparent = parent;
			parent = cur_node;
			cur_node = &GET_PTR(*cur_node)->right;
		}
		else
		{
			/*
			 *  matched node found
			 */
			if(!GET_PTR(GET_PTR(*cur_node)->left) && !GET_PTR(GET_PTR(*cur_node)->right))
			{

				rmv = *cur_node;
				if(GET_COLOR(rmv) == RED)
				{
					*cur_node = NULL;
					return GET_PTR(rmv);
				}
				else
				{
					/*
					 *  try to resolve black black with sub-tree whose root is parent of black black
					 *
					 *           parent
					 *           /    \
					 *         cur    cur
					 *
					 *  relative direction of cur from parent(right / left) is checked in resolve_black_black routine
	 				 */

					res = PATTERN_BLACK_BLACK;
					*cur_node = resolve_black_black(*parent, color_ctx, dir_ctx, &rmv);
					if(!((uint64_t) *parent & BB_ALERT))
					{
						*parent = *cur_node;
						*cur_node = NULL;
						return GET_PTR(rmv);
					}

					/*
					 *  if fail to resolve with sub-tree whose root is parent of black black
					 *  retry with sub-tree whose root is grand parent of black-black's origin.
					 *
					 *           grand parent
					 *           /          \
					 *         parent     parent
					 *
					 */
					*cur_node = resolve_black_black(*gparent, color_ctx >> 1, dir_ctx >> 1, &rmv);
					if(!((uint64_t) rmv & BB_ALERT))
					{
						*gparent = *cur_node;
						*cur_node = NULL;
						return GET_PTR(rmv);
					}
					fprintf(stderr, "two step is maximum \n");
					exit(1);
				}

			}
			else if(GET_PTR(GET_PTR(*cur_node)->left))
			{
				/*
				 * non-leaf node with left child
				 * rightmost-end leaf is chosen as replacement for removed
				 */
				rmv = *cur_node;
				GET_PTR(*cur_node)->left = up_from_rightmost_rc(rmv->left, cur_node);
				GET_PTR(*cur_node)->right = GET_PTR(rmv)->right;
				PAINT_COLOR(*cur_node,GET_COLOR(rmv));
				return GET_PTR(rmv);
			}
			else
			{
				/*
				 * non-leaf node with right child
				 * leftmost-end is chosen as replacement for removed
				 */
				rmv = *cur_node;
				GET_PTR(*cur_node)->right = up_from_leftmost_rc(rmv->right, cur_node);
				GET_PTR(*cur_node)->left = GET_PTR(rmv)->left;
				PAINT_COLOR(*cur_node,GET_COLOR(rmv));
				return GET_PTR(rmv);
			}
		}
	}
	return NULL;
}


#ifdef __DBG
void cdsl_nrbtreePrint_dev (nrbtreeRoot_t* root)
{
	if(!root)
		return;
	node_print_rc(root->entry, 0);

}
#endif

static BOOL replace_from_next(nrbtreeNode_t** node)
{
	if(!node)
		return FALSE;
	if(!GET_PTR(*node))
		return FALSE;
	if(!GET_PTR(GET_PTR(*node)->right))
		return FALSE;

	nrbtreeNode_t** cur_node = &(GET_PTR(*node)->right);
	nrbtreeNode_t** parent = node;
	nrbtreeNode_t** gparent = NULL;

	uint8_t color_ctx = GET_COLOR(*cur_node);
	uint8_t dir_ctx = CTX_RIGHT;


	while(GET_PTR(*cur_node))
	{
		color_ctx <<= 1;
		dir_ctx <<= 1;

		color_ctx |= GET_COLOR(*cur_node);
		dir_ctx |= CTX_LEFT;

		gparent = parent;
		parent = cur_node;
		cur_node = &GET_PTR(*cur_node)->left;
	}
	/*
	 *  parent is actually the replacement itself
	 *  cur_node is pointing null node
	 */
	if(parent == node) {
		parent = (nrbtreeNode_t**) *node;
		*node = NULL;
		/*
		 * don't need to update color
		 * return false
		 */
		return FALSE;
	}

	switch(color_ctx & PATTERN_WINDOW)
	{
	case PATTERN_RED_RED:
	case PATTERN_BLACK_RED:
		PAINT_COLOR(*parent, GET_COLOR(*node));
		GET_PTR(*parent)->left = GET_PTR(*node)->left;
		GET_PTR(*parent)->right = GET_PTR(*node)->right;
		*node = *parent;
		return TRUE;
	case PATTERN_BLACK_BLACK:
		break;
	case PATTERN_RED_BLACK:
		break;
	}
}

static BOOL replace_from_prev(nrbtreeNode_t** node)
{

}

static void print_tab(int cnt){
	while(cnt--)
		printf("\t");
}

static void node_print_rc(nrbtreeNode_t* node,int order) {
	if(!GET_PTR(node))
	{
		print_tab(order);
		printf("NIL(black) \n");
		return;
	}
	node_print_rc(GET_PTR(node)->right, order + 1);
	print_tab(order); printf("%s node : %d / order %d \n", COLOR_STRING[GET_COLOR(node)], GET_PTR(node)->key, order);
	node_print_rc(GET_PTR(node)->left, order + 1);
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
		return (nrbtreeNode_t*) ((uint64_t)node_c | RED);
	}
	else
	{
		return (nrbtreeNode_t*) ((uint64_t)node_c & ~RED);
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
 */
static nrbtreeNode_t* resolve_black_black(nrbtreeNode_t* parent_c, uint8_t color_ctx, uint8_t dir_ctx, nrbtreeNode_t** ctx)
{
	if(!parent_c)
		return NULL;
	nrbtreeNode_t** sibling = NULL;
	uint8_t sibling_dir_ctx = 0;

	if(dir_ctx >> 1 == CTX_LEFT)
	{
		/*
		 *  check right sibling
		 */
		sibling_dir_ctx |= CTX_RIGHT;
		sibling_dir_ctx <<= 1;
		sibling = &GET_PTR(parent_c)->right;
		if((GET_COLOR(GET_PTR(*sibling)->left) == BLACK) \
				&& (GET_COLOR(GET_PTR(*sibling)->right) == BLACK))
		{
			if(GET_COLOR(*sibling) == BLACK)
			{
				/*
				 * black sibling with black children
				 *
				 * depend on parent color there are two option
				 */
				if(GET_COLOR(parent_c) == BLACK)
				{
					/*
					 * parent is black
					 * then all the nodes in sub-tree are black
					 * in this case, can't resolved within this sub-tree
					 */
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
					PAINT_COLOR(*ctx, !BB_ALERT);
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
				PAINT_COLOR(*ctx, !BB_ALERT);
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
			 */

			if(GET_COLOR(GET_PTR(*sibling)->right) == RED)
			{
				parent_c = rotate_left(parent_c);
				PAINT_BLACK(GET_PTR(parent_c)->right);
				PAINT_COLOR(*ctx, !BB_ALERT);
				return parent_c;
			}
			else
			{
				*sibling = rotate_right(*sibling);
				parent_c = rotate_left(parent_c);
				//TODO: check whether parent don't be required to be repaint into black
				PAINT_COLOR(*ctx, !BB_ALERT);
				return parent_c;
			}
		}
	}
	else
	{
		/*
		 *  check right sibling
		 */
		sibling_dir_ctx |= CTX_LEFT;
		sibling_dir_ctx <<= 1;
		sibling = &GET_PTR(parent_c)->left;
		if((GET_COLOR(GET_PTR(*sibling)->left) == BLACK) \
				&& (GET_COLOR(GET_PTR(*sibling)->right) == BLACK))
		{
			if(GET_COLOR(*sibling) == BLACK)
			{
				/*
				 * black sibling with black children
				 *
				 * depend on parent color there are two option
				 */
				if(GET_COLOR(parent_c) == BLACK)
				{
					/*
					 * parent is black
					 * then all the nodes in sub-tree are black
					 * in this case, can't resolved within this sub-tree
					 */
					PAINT_COLOR(*sibling, RED);
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
					PAINT_COLOR(*ctx, !BB_ALERT);
					return parent_c;
				}
			}
			else
			{
				/*
				 * red sibling with black children
				 *
				 * paint sibling blakc
				 */
				PAINT_BLACK(*sibling);
				PAINT_COLOR(*ctx, !BB_ALERT);
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
			 */

			if(GET_COLOR(GET_PTR(*sibling)->left) == RED)
			{
				parent_c = rotate_right(parent_c);
				PAINT_BLACK(GET_PTR(parent_c)->left);
				PAINT_COLOR(*ctx, !BB_ALERT);
				return parent_c;
			}
			else
			{
				*sibling = rotate_left(*sibling);
				parent_c = rotate_right(parent_c);
				//TODO: check whether parent don't be required to be repaint into black
				PAINT_COLOR(*ctx, !BB_ALERT);
				return parent_c;
			}
		}
	}
	return NULL;
}

static nrbtreeNode_t* up_from_rightmost_rc(nrbtreeNode_t* node,nrbtreeNode_t** rm)
{
	if(!GET_PTR(node)->right)
	{
		if(GET_COLOR(node) != RED)
		{
			/*
			 *  black black case,
			 *  set BB_ALERT in pointer and return
			 */
			*rm = (nrbtreeNode_t*) ((uint64_t)node | BB_ALERT);
		}
		else
		{
			*rm = (nrbtreeNode_t*) ((uint64_t)node & ~BB_ALERT);
		}
		return NULL;
	}

	GET_PTR(node)->right = up_from_rightmost_rc(GET_PTR(node)->right, rm);
	if((uint64_t) *rm & BB_ALERT)
	{
		node = resolve_black_black(node, (GET_COLOR(node) << 1),((CTX_RIGHT << 1) | CTX_RIGHT),rm);
	}
	return node;
}

static nrbtreeNode_t* up_from_leftmost_rc(nrbtreeNode_t* node, nrbtreeNode_t** lm)
{
	if(!GET_PTR(node)->left)
	{
		if(GET_COLOR(node) != RED)
		{
			/*
			 *  black black case
			 */
			*lm = (nrbtreeNode_t*) ((uint64_t) node | BB_ALERT);
		}
		else
		{
			*lm = (nrbtreeNode_t*) ((uint64_t) node & ~BB_ALERT);
		}
		return NULL;
	}

	GET_PTR(node)->left = up_from_leftmost_rc(GET_PTR(node)->left, lm);
	if((uint64_t) *lm & BB_ALERT)
	{
		node = resolve_black_black(node, (GET_COLOR(node) << 1),((CTX_LEFT << 1) | CTX_LEFT),lm);
	}
	return node;
}

