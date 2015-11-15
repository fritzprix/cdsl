/*
 * heap.c
 *
 *  Created on: 2015. 5. 3.
 *      Author: innocentevil
 */


#include "cdsl_heap.h"
#include <stddef.h>


/**
 * recursive interanl function
 */
static heapNode_t* insert_from_bottom(heapNode_t* current,heapNode_t* item,heapEvaluate eval);
static heapNode_t* move_tree_down(heapNode_t* current,heapEvaluate eval);
static heapNode_t* get_leaf_node(heapNode_t* current);

int heap_enqueue(heapNode_t** heap,heapNode_t* item,heapEvaluate eval){
	item->left = item->right = NULL;
	if(!heap)
		return (1 < 0);
	*heap = insert_from_bottom(*heap,item,eval);
	return (1 > 0);
}

heapNode_t* heap_deqeue(heapNode_t** heap,heapEvaluate eval){
	if(!heap)
		return NULL;
	heapNode_t* current = *heap;
	*heap = get_leaf_node(*heap);
	if(current == *heap){
		*heap = NULL;
		return current;
	}
	(*heap)->left = current->left;
	(*heap)->right = current->right;

	*heap = move_tree_down(*heap,eval);
	return current;
}


void heap_print(heapNode_t** root,heapPrint prt){
	if(!root || !*root)
		return;
	prt(*root);
	heap_print(&(*root)->right,prt);
	heap_print(&(*root)->left,prt);
}


static heapNode_t* insert_from_bottom(heapNode_t* current,heapNode_t* item,heapEvaluate eval)
{
	heapNode_t* child,*left,*right = NULL;
	if(!current)
	{
		return item;
	}
	current->flipper = !current->flipper;
	if(current->flipper)
	{
		child = insert_from_bottom(current->right,item,eval);
		if(current->right != child)
		{
			if(current != eval(current,child))
			{
				left = child->left;
				right = child->right;
				child->left = current->left;
				child->right = current;
				current->left = left;
				current->right = right;
				child->right = current;
				return child;
			}
			else
			{
				current->right = child;
				return current;
			}
		}
		return current;
	}
	else
	{
		child = insert_from_bottom(current->left,item,eval);
		if(current->left != child)
		{
			if(current != eval(current,child))
			{
				left = child->left;
				right = child->right;
				child->left = current;
				child->right = current->right;
				current->left = left;
				current->right = right;
				child->left = current;
				return child;
			}
			else
			{
				current->left = child;
				return current;
			}
		}
		return current;
	}
}



static heapNode_t* get_leaf_node(heapNode_t* current){
	heapNode_t* leaf = NULL;
	if(!current) return leaf;
	if(!current->right && !current->left)
	{
		return current;
	}
	if (current->right)
	{
		leaf = get_leaf_node(current->right);
		if (leaf)
		{
			if (leaf == current->right)
				current->right = NULL;
			return leaf;
		}
	}
	if (current->left)
	{
		leaf = get_leaf_node(current->left);
		if (leaf)
		{
			if (leaf == current->left)
				current->left = NULL;
			return leaf;
		}
	}
	return current;
}

static heapNode_t* move_tree_down(heapNode_t* current,heapEvaluate eval)
{
	heapNode_t* largest = current,*left = NULL,*right = NULL;
	if(!largest)
		return NULL;
	largest = eval(largest,current->right);
	largest = eval(largest,current->left);

	if(largest == current->left)
	{
		left = largest->left;
		right = largest->right;
		largest->right = current->right;
		current->right = right;
		current->left = left;
		largest->left = move_tree_down(current,eval);
	}
	else if(largest == current->right)
	{
		left = largest->left;
		right = largest->right;
		largest->left =current->left;
		current->right = right;
		current->left = left;
		largest->right = move_tree_down(current,eval);
	}
	return largest;
}


