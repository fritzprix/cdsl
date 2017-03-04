/*
 * heap.c
 *
 *  Created on: 2015. 5. 3.
 *      Author: innocentevil
 */


#include "cdsl_heap.h"
#include <stddef.h>

#define DIR_LEFT  1
#define DIR_RIGHT !DIR_LEFT


static heapNode_t* insert_from_bottom_rc(heapRoot_t* rootp, heapNode_t* current,heapNode_t* item);
static heapNode_t* move_max_tree_down_rc(heapNode_t* current);
static heapNode_t* move_min_tree_down_rc(heapNode_t* current);
static heapNode_t* get_leaf_node(heapNode_t* current);
static heapNode_t* heapify(heapNode_t* current,heapNode_t* child);

/**
 *
 */
void cdsl_heapRootInit(heapRoot_t* rootp, heapType_t type) {
	if(rootp == NULL) {
		return;
	}
	rootp->entry = NULL;
	rootp->type = type;
	rootp->dir = DIR_LEFT;
}

void cdsl_heapNodeInit(heapNode_t* node, trkey_t key) {
	if(node == NULL) {
		return;
	}
	node->left = node->right = NULL;
	node->key = key;
}

int cdsl_heapEnqueue(heapRoot_t* rootp,heapNode_t* item){
	item->left = item->right = NULL;
	if(!rootp) {
		return (1 < 0);
	}
	rootp->entry = insert_from_bottom_rc(rootp,rootp->entry,item);
	rootp->dir = !rootp->dir;
	return (1 > 0);
}

heapNode_t* cdsl_heapDeqeue(heapRoot_t* rootp){
	if(!rootp) {
		return NULL;
	}
	heapNode_t* current = rootp->entry;
	rootp->entry = get_leaf_node(rootp->entry);
	if(current == rootp->entry){
		rootp->entry = NULL;
		return current;
	}
	rootp->entry->left = current->left;
	rootp->entry->right = current->right;
	if(rootp->type == MAX_HEAP) {
		rootp->entry = move_max_tree_down_rc(rootp->entry);
	} else {
		rootp->entry = move_min_tree_down_rc(rootp->entry);
	}
	return current;
}



static heapNode_t* insert_from_bottom_rc(heapRoot_t* rootp, heapNode_t* current,heapNode_t* item)
{
	heapNode_t* child = NULL;
	if(!current) {
		return item;
	}
	if(current->right == NULL) {
		current->right = item;
		child = item;
	}
	else if(current->left == NULL) {
		current->left = item;
		child = item;
	}
	else {
		rootp->dir = !rootp->dir;
		if(rootp->dir == DIR_RIGHT) {
			child = insert_from_bottom_rc(rootp, current->right,item);
			current->right = child;
		} else {
			child = insert_from_bottom_rc(rootp, current->left,item);
			current->left = child;
		}
	}

	if(rootp->type == MAX_HEAP)	{
		if(current->key >= child->key) {
			return current;
		}
		return heapify(current,child);
	} else {
		if(current->key <= child->key) {
			return current;
		}
		return heapify(current,child);
	}
}


static heapNode_t* heapify(heapNode_t* current,heapNode_t* child) {
	heapNode_t *left, *right;
	if(child == current->right)	{
		left = child->left;
		right = child->right;
		child->left = current->left;
		child->right = current;
		current->left = left;
		current->right = right;
		child->right = current;
		return child;
	} else {
		left = child->left;
		right = child->right;
		child->left = current;
		child->right = current->right;
		current->left = left;
		current->right = right;
		child->left = current;
		return child;
	}
}

static heapNode_t* get_leaf_node(heapNode_t* current) {
	heapNode_t* leaf = NULL;
	if(!current) {
		return leaf;
	}
	if(!current->right && !current->left) {
		return current;
	}
	if (current->right)	{
		leaf = get_leaf_node(current->right);
		if (leaf) {
			if (leaf == current->right)
				current->right = NULL;
			return leaf;
		}
	}
	if (current->left) {
		leaf = get_leaf_node(current->left);
		if (leaf) {
			if (leaf == current->left)
				current->left = NULL;
			return leaf;
		}
	}
	return current;
}

static heapNode_t* move_max_tree_down_rc(heapNode_t* current)
{
	heapNode_t* largest = current,*left = NULL,*right = NULL;
	if(!largest) {
		return NULL;
	}
	if((current->right != NULL) && (largest->key < current->right->key)) {
		largest = current->right;
	}
	if((current->left != NULL) && (largest->key < current->left->key)) {
		largest = current->left;
	}

	if(largest == current->left) {
		left = largest->left;
		right = largest->right;
		largest->right = current->right;
		current->right = right;
		current->left = left;
		largest->left = move_max_tree_down_rc(current);
	} else if(largest == current->right) {
		left = largest->left;
		right = largest->right;
		largest->left =current->left;
		current->right = right;
		current->left = left;
		largest->right = move_max_tree_down_rc(current);
	}
	return largest;
}

static heapNode_t* move_min_tree_down_rc(heapNode_t* current) {
	heapNode_t* smallest = current,*left = NULL,*right = NULL;
	if(!smallest) {
		return NULL;
	}
	if((current->right != NULL) && (smallest->key > current->right->key)) {
		smallest = current->right;
	}
	if((current->left != NULL) && (smallest->key > current->left->key)) {
		smallest = current->left;
	}

	if(smallest == current->left) {
		left = smallest->left;
		right = smallest->right;
		smallest->right = current->right;
		current->right = right;
		current->left = left;
		smallest->left = move_min_tree_down_rc(current);
	} else if(smallest == current->right) {
		left = smallest->left;
		right = smallest->right;
		smallest->left =current->left;
		current->right = right;
		current->left = left;
		smallest->right = move_min_tree_down_rc(current);
	}
	return smallest;
}



