/*
 * tch_btree.c
 *
 *  Created on: 2014. 9. 12.
 *      Author: innocentevil
 */


#include "cdsl_bstree.h"
#include "cdsl.h"
#include <stdio.h>
#include <stddef.h>

static int calc_max_depth_rc(bs_treeNode_t** root);
static int calc_size_rc(bs_treeNode_t** root);
static void print_rc(bs_treeNode_t* current,cdsl_generic_printer_t prt,int depth);
static int traverse_incremental_rc(bs_treeNode_t* current,int* current_order,bs_tree_callback_t cb);
static int traverse_decremental_rc(bs_treeNode_t* current,int* current_order,bs_tree_callback_t cb);
static void print_tab(int cnt);


void bstree_root_init(bs_treeRoot_t* rootp)
{
	if(rootp == NULL)
		return;
	rootp->entry = NULL;
}


void bstree_node_init(bs_treeNode_t* node,int key)
{
	if(node == NULL)
		return;
	node->key = key;
	node->left = NULL;
	node->right = NULL;
}

static bs_treeNode_t* move_up_rightmost_rc(bs_treeNode_t* node)
{
	if(!node)	return NULL;
	if(!node->right)	return node;

	/** recursively do this (0 is null termination)
	 *       p             b
	 *      / \    ->     / \
	 *     a   b         p   0
	 *        / \       / \
	 *       c   0     a   c
	 */
	bs_treeNode_t* rightmost = move_up_rightmost_rc(node->right);
	node->right = rightmost->left;
	rightmost->left = node;
	return rightmost;
}

static bs_treeNode_t* move_up_leftmost_rc(bs_treeNode_t* node)
{
	if(!node) 	return NULL;
	if(!node->left) 	return node;

	/** recursively do this (0 is null termination)
	 *        p              a
	 *       / \            / \
	 *      a   b   ->     0   p
	 *     / \                / \
	 *    0   d              d   b
	 */
	bs_treeNode_t* leftmost = move_up_leftmost_rc(node->left);
	node->left = leftmost->right;
	leftmost->right = node;
	return leftmost;
}

bs_treeNode_t* bstree_insert(bs_treeRoot_t* rootp,bs_treeNode_t* item){
	if((rootp == NULL) || (item == NULL))
		return NULL;
	if(rootp->entry == NULL)
	{
		rootp->entry = item;
		return rootp->entry;
	}

	item->left = NULL;
	item->right = NULL;
	bs_treeNode_t* current = rootp->entry;
	bs_treeNode_t* parent = NULL;

	while(current)
	{
		if(current->key < item->key)
		{
			if(!current->right)
			{
				current->right = item;
				return item;
			}
			parent = current;
			current = current->right;
		}
		else
		{
			if(!current->left)
			{
				current->left = item;
				return item;
			}
			parent = current;
			current = current->left;
		}
	}
	return NULL;
}

bs_treeNode_t* bstree_lookup(bs_treeRoot_t* rootp,int key)
{
	if(rootp == NULL)
		return NULL;
	bs_treeNode_t* current = rootp->entry;
	while(current == NULL && (current->key != key))
	{
		if(current->key < key)
			current = current->right;
		else
			current = current->left;
	}
	return current;
}

bs_treeNode_t* bstree_delete(bs_treeRoot_t* rootp,int key)
{
	bs_treeNode_t* todelete = NULL;
	if((rootp == NULL) || (rootp->entry == NULL))
		return NULL;
	bs_treeNode_t* parent = NULL;
	bs_treeNode_t** current = &rootp->entry;
	while((*current) && ((*current)->key != key))
	{
		if((*current)->key < key)
		{
			parent = *current;
			current = &(*current)->right;
		}
		else
		{
			parent = *current;
			current = &(*current)->left;
		}
	}

	todelete = *current;
	if (todelete->left)
	{
		*current = move_up_rightmost_rc(todelete->left);
		if (*current)
		{
			(*current)->right = todelete->right;
		}
	}
	else if(todelete->right)
	{
		*current = move_up_leftmost_rc(todelete->right);
		(*current)->left = todelete->left;
	}
	else
	{
		*current = NULL;
	}
	return todelete;
}

void bstree_traverse(bs_treeRoot_t* rootp,bs_tree_callback_t cb,int order)
{
	if((cb == NULL) || (rootp == NULL) || (rootp->entry == NULL))
		return;
	int i = 0;
	if(order == ORDER_DEC)
		traverse_decremental_rc(rootp->entry,&i,cb);
	else
		traverse_incremental_rc(rootp->entry,&i,cb);
}


void bstree_print(bs_treeRoot_t* rootp,cdsl_generic_printer_t prt)
{
	if(rootp == NULL)
		return;
	print_rc(rootp->entry,prt,0);
}

int bstree_max_depth(bs_treeRoot_t* rootp)
{
	if((rootp == NULL) || (rootp->entry == NULL))
		return 0;
	return calc_max_depth_rc(&rootp->entry);
}

int bstree_size(bs_treeRoot_t* rootp)
{
	if((rootp == NULL) || (rootp->entry == NULL))
		return 0;
	return calc_size_rc(&rootp->entry);
}

static int calc_size_rc(bs_treeNode_t** root)
{
	int cnt = 0;
	if((root == NULL) || (*root == NULL))
		return 0;
	if((*root))
		cnt = 1;
	if(!(*root)->left && !(*root)->right) return cnt;
	if((*root)->left)
		cnt += calc_size_rc(&(*root)->left);
	if((*root)->right)
		cnt += calc_size_rc(&(*root)->right);
	return cnt;
}


static int calc_max_depth_rc(bs_treeNode_t** root)
{
	if((root == NULL) || (*root == NULL))
		return 0;
	int max = 0;
	int temp = 0;
	if(max < (temp = calc_max_depth_rc(&(*root)->left)))
		max = temp;
	if(max < (temp = calc_max_depth_rc(&(*root)->right)))
		max = temp;
	return max + 1;
}

static void print_rc(bs_treeNode_t* current,cdsl_generic_printer_t prt,int depth)
{
	if(!current)
		return;
	print_rc(current->right,prt,depth + 1);
	print_tab(depth); if(prt) prt(current); printf("{key : %d} @ depth %d\n",current->key,depth);
	print_rc(current->left,prt,depth + 1);
}

static void print_tab(int cnt)
{
	while(cnt--)
		printf("\t");
}

static int traverse_incremental_rc(bs_treeNode_t* current, int* current_order, bs_tree_callback_t cb)
{
	if(current == NULL)
		return 0;
	if(traverse_incremental_rc(current->left,current_order,cb) == BREAK_TRAVERS)
		return BREAK_TRAVERS;
	*current_order += 1;
	if(cb(*current_order,current) == BREAK_TRAVERS)
		return BREAK_TRAVERS;
	return traverse_incremental_rc(current->right,current_order,cb);
}

static int traverse_decremental_rc(bs_treeNode_t* current, int* current_order, bs_tree_callback_t cb)
{
	if(current == NULL)
		return 0;
	if(traverse_decremental_rc(current->right,current_order,cb) == BREAK_TRAVERS)
		return BREAK_TRAVERS;
	if(cb(*current_order,current) == BREAK_TRAVERS)
		return BREAK_TRAVERS;
	return traverse_decremental_rc(current->left,current_order,cb);
}
