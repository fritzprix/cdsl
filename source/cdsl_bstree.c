/*
 * tch_btree.c
 *
 *  Created on: 2014. 9. 12.
 *      Author: innocentevil
 */


#include "cdsl_bstree.h"
#include "base_tree.h"
#include "cdsl.h"
#include <stdio.h>
#include <stddef.h>

#define PRINT		printf


static bs_treeNode_t* move_up_rightmost_rc(bs_treeNode_t* node);
static bs_treeNode_t* move_up_leftmost_rc(bs_treeNode_t* node);

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

void bstree_traverse(bs_treeRoot_t* rootp,base_tree_callback_t cb,int order)
{
	tree_traverse(&rootp->root,cb,order);
}


void bstree_print(bs_treeRoot_t* rootp,cdsl_generic_printer_t print)
{
	if(rootp == NULL)
		return;
	tree_print(&rootp->root,print);
}

int bstree_max_depth(bs_treeRoot_t* rootp)
{
	if((rootp == NULL) || (rootp->entry == NULL))
		return 0;
	return tree_max_depth(&rootp->root);
}

int bstree_size(bs_treeRoot_t* rootp)
{
	if((rootp == NULL) || (rootp->entry == NULL))
		return 0;
	return tree_size(&rootp->root);
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

