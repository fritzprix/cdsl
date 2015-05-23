/*
 * cdsl_rbtree.c
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */


#include <stddef.h>
#include <stdio.h>
#include "cdsl_rbtree.h"


#define BLACK					((unsigned) (1 > 0))
#define RED						((unsigned) (1 < 0))


#define CLEAN					((uint8_t) 0)
#define COLLISION				((uint8_t) 1)
#define DIR_RIGHT				((uint8_t) 2)
#define DIR_LEFT				((uint8_t) 3)


#define RB_NIL 					((rb_treeNode_t*) &NIL_NODE)
#define IS_LEAF_NODE(node) 		((node->right == RB_NIL) || (node->left == RB_NIL))


const static rb_treeNode_t NIL_NODE = {
		.left = NULL,
		.right = NULL,
		.key = 0,
		.color = BLACK
};



static rb_treeNode_t* rotateLeft(rb_treeNode_t* rot_pivot,BOOL paint);
static rb_treeNode_t* rotateRight(rb_treeNode_t* rot_pivot,BOOL paint);
static rb_treeNode_t* insert_r(rb_treeNode_t* parent,rb_treeNode_t* item,uint8_t* context);
static rb_treeNode_t* delete_r(rb_treeNode_t* cur,int key,rb_treeNode_t** del,uint8_t* context,int k);
#if 0
static rb_treeNode_t* delete_r(rb_treeNode_t* parent,rb_treeNode_t* delete_node,uint8_t* context);
#endif
static rb_treeNode_t* deleteLeft_r(rb_treeNode_t* root,rb_treeNode_t** l_most,uint8_t* context);
static rb_treeNode_t* deleteRight_r(rb_treeNode_t* root,rb_treeNode_t** r_most,uint8_t* context);
static rb_treeNode_t* handleDoubleBlack(rb_treeNode_t* parent,uint8_t* context);
static rb_treeNode_t* deleteLeftMost(rb_treeNode_t* root,rb_treeNode_t** leftmost);
static rb_treeNode_t* deleteRightMost(rb_treeNode_t* root,rb_treeNode_t** rightmost);
static void print_r(rb_treeNode_t* current,int depth);
static void print_tab(int cnt);

void cdsl_rbtreeNodeInit(rb_treeNode_t* item,int key){
	item->key = key;
	item->left = item->right = RB_NIL;
	item->color = RED;
}


BOOL cdsl_rbtreeInsert(rb_treeNode_t** root,rb_treeNode_t* item){
	if(!root)
		return FALSE;
	if(!*root){
		*root = item;
		item->color = BLACK;
		return TRUE;
	}
	uint8_t info = 0;

	*root = insert_r(*root,item,&info);
	(*root)->color = BLACK;
	return TRUE;
}

#ifdef BEFORE
rb_treeNode_t* cdsl_rbtreeDelete(rb_treeNode_t** root,int key){
	if(!root || !(*root))
		return NULL;
	uint8_t context = 0;
	rb_treeNode_t** parent_node = NULL;
	rb_treeNode_t** current_node = root;
	while((*current_node)->key != key){
		if((*current_node) == RB_NIL)
			return NULL;
		parent_node = current_node;
		if((*current_node)->key < key){
			context = DIR_RIGHT;
			current_node = &(*current_node)->right;
		}else{
			context = DIR_LEFT;
			current_node = &(*current_node)->left;
		}
	}

	*parent_node = delete_r(*parent_node,*current_node,&context);
	return (*current_node);
}
#else
rb_treeNode_t* cdsl_rbtreeDelete(rb_treeNode_t** root,int key){
	if(!root || !(*root))
		return NULL;		// red black tree is empty or invalid arg
	uint8_t context = 0;
	rb_treeNode_t* del_node = NULL;
	*root = delete_r(*root,key,&del_node,&context,0);
	return del_node;
}
#endif

static rb_treeNode_t* deleteLeft_r(rb_treeNode_t* root,rb_treeNode_t** l_most,uint8_t* context){
	if(!root)
		return NULL;
	if(root == RB_NIL)
		return RB_NIL;
	if(root->left == RB_NIL){
		*l_most = root;
		*context = CLEAN;
		if((root->color == BLACK) && (root->right->color == BLACK))
			*context = COLLISION;
		if(root->right != RB_NIL)
			root->right->color = (*l_most)->color;
		return root->right;
	}
	root->left = deleteLeft_r(root->left,l_most,context);
	if((*context) == COLLISION){
		(*context) = DIR_LEFT;
		return handleDoubleBlack(root,context);
	}
	return root;
}

static rb_treeNode_t* deleteRight_r(rb_treeNode_t* root,rb_treeNode_t** r_most,uint8_t* context){
	if(!root)
		return NULL;
	if(root == RB_NIL)
		return RB_NIL;
	if(root->right == RB_NIL){
		*r_most = root;
		*context = CLEAN;
		if((root->color == BLACK) && (root->left->color == BLACK))
			*context = COLLISION;
		if(root->left != RB_NIL)
			root->left->color = (*r_most)->color;
		return root->left;
	}
	root->right = deleteRight_r(root->right,r_most,context);
	if((*context) == COLLISION){
		*context = DIR_RIGHT;
		return handleDoubleBlack(root,context);
	}
	return root;
}

static rb_treeNode_t* delete_r(rb_treeNode_t* cur,int key,rb_treeNode_t** del,uint8_t* context,int k){
	if(!cur) return NULL;
	if(cur == RB_NIL){
		del = NULL;
		context = CLEAN;
		return cur;
	}
	uint8_t direction = *context;
	if(cur->key < key){
		*context = DIR_RIGHT;
		cur->right = delete_r(cur->right,key,del,context,k + 1);
		if((*context) == COLLISION){
			//handle subtree double black condition
			*context = DIR_RIGHT;
	/*		if(!k)
				return cur;*/
			return handleDoubleBlack(cur,context);
		}
		return cur;
	}
	if(cur->key > key){
		*context = DIR_LEFT;
		cur->left = delete_r(cur->left,key,del,context,k + 1);
		if((*context) == COLLISION){
			//handle subtree double black condition
			*context = DIR_LEFT;
	/*		if(!k)
				return cur;*/
			return handleDoubleBlack(cur,context);
		}
		return cur;
	}
	*del = cur;
	if(cur->left != RB_NIL){
		*context = DIR_LEFT;
		(*del)->left = deleteRight_r(cur->left,&cur,context);
		cur->color = (*del)->color;
		cur->right = (*del)->right;
		cur->left = (*del)->left;
		if((*context) == COLLISION){
			*context = DIR_LEFT;
	/*		if(!k)
				return cur;*/
			return handleDoubleBlack(cur,context);
		}
		return cur;
	}
	if(cur->right != RB_NIL){
		*context = DIR_RIGHT;
		(*del)->right = deleteLeft_r(cur->right,&cur,context);
		cur->color = (*del)->color;
		cur->left = (*del)->left;
		cur->right = (*del)->right;
		if((*context) == COLLISION){
			*context = DIR_RIGHT;
/*			if(!k)
				return cur;*/
			return handleDoubleBlack(cur,context);
		}
		return cur;
	}
	if(cur->color != RED)
		*context = COLLISION;
	else
		*context = CLEAN;
	return RB_NIL;

}


static rb_treeNode_t* handleDoubleBlack(rb_treeNode_t* parent,uint8_t* context){
	if(!parent)
		return parent;
	if((*context) == DIR_RIGHT){
		if(parent->left->color == BLACK){ // if sibling is black
			if(parent->left->left->color == RED){
				parent = rotateRight(parent,FALSE);
				parent->left->color = BLACK;
				*context = CLEAN;
				return parent;
			}
			if(parent->left->right->color == RED){
				parent->left = rotateLeft(parent->left,TRUE);
				parent = rotateRight(parent,FALSE);
				parent->left->color = BLACK;
				*context = CLEAN;
				return parent;
			}
			// both childs of sibling is black
			parent->left->color = RED;
			*context = COLLISION;
			return parent;
		} else{
			parent = rotateRight(parent,TRUE);
			*context = DIR_RIGHT;
			parent->right = handleDoubleBlack(parent->right,context);
			if((*context) == COLLISION){
				*context = DIR_RIGHT;
				return handleDoubleBlack(parent,context);
			}
			return parent;
		}
	}else {
		if(parent->right->color == BLACK){ // if sibling is black
			if(parent->right->right->color == RED){
				parent = rotateLeft(parent,FALSE);
				parent->right->color = BLACK;
				*context = CLEAN;
				return parent;
			}
			if(parent->right->left->color == RED){
				parent->right = rotateRight(parent->right,TRUE);
				parent = rotateLeft(parent,FALSE);
				parent->right->color = BLACK;
				*context = CLEAN;
				return parent;
			}
			// both childs of sibling is black
			parent->right->color = RED;
			*context = COLLISION;
		} else{
			parent = rotateLeft(parent,TRUE);
			*context = DIR_LEFT;
			parent->left = handleDoubleBlack(parent->left,context);
			if((*context) == COLLISION){
				*context = DIR_LEFT;
				return handleDoubleBlack(parent,context);
			}
			return parent;
		}
	}
	return parent;
}



void cdsl_rbtreePrint(rb_treeNode_t** root){
	if(!root)
		return;
	printf("\n");
	print_r(*root,0);
	printf("\n");
}

static rb_treeNode_t* insert_r(rb_treeNode_t* parent,rb_treeNode_t* item,uint8_t* context){
	if(!item || !parent)
		return RB_NIL;
	if(parent == RB_NIL){
		*context = CLEAN;
		return item;
	}
	uint8_t direction = *context;
	if(parent->key <= item->key){
		*context = DIR_RIGHT;
		parent->right = insert_r(parent->right,item,context);
		if((*context) == COLLISION)		// if collision
		{
			if(parent->left->color == parent->right->color){
				parent->right->color = !parent->left->color;
				parent->left->color =!parent->left->color;
				if((parent->left->color == BLACK) && (parent->right->color == BLACK))
					parent->color = RED;
				*context = CLEAN;
				return parent;
			}
			parent = rotateLeft(parent,TRUE);
			*context = CLEAN;
			return parent;
		}

		if((parent->color == RED) && (parent->right->color == RED)){
			*context = COLLISION;
			if(direction == DIR_LEFT){
				parent = rotateLeft(parent,FALSE);
			}
		}
	}else{
		*context = DIR_LEFT;
		parent->left = insert_r(parent->left,item,context);
		if((*context) == COLLISION)
		{
			if(parent->left->color == parent->right->color){
				parent->right->color = !parent->left->color;
				parent->left->color =!parent->left->color;
				if((parent->left->color == BLACK) && (parent->right->color == BLACK))
					parent->color = RED;
				*context = CLEAN;
				return parent;
			}
			parent = rotateRight(parent,TRUE);
			*context = CLEAN;
			return parent;
		}

		if((parent->color == RED) && (parent->left->color == RED)){
			*context = COLLISION;
			if(direction == DIR_RIGHT){
				parent = rotateRight(parent,FALSE);
			}
		}
	}
	return parent;
}

static rb_treeNode_t* rotateLeft(rb_treeNode_t* rot_pivot,BOOL paint){
	rb_treeNode_t* nparent = rot_pivot->right;
	rot_pivot->right = nparent->left;
	nparent->left = rot_pivot;
	if(paint){
		nparent->color = nparent->left->color;
		nparent->left->color = !nparent->left->color;
	}
	return nparent;
}

static rb_treeNode_t* rotateRight(rb_treeNode_t* rot_pivot,BOOL paint){
	rb_treeNode_t* nparent = rot_pivot->left;
	rot_pivot->left = nparent->right;
	nparent->right = rot_pivot;
	if(paint){
		nparent->color = nparent->right->color;
		nparent->right->color = !nparent->right->color;
	}
	return nparent;
}

static void print_tab(int cnt){
	while(cnt--)
		printf("\t");
}


static void print_r(rb_treeNode_t* current,int depth){

	if(current == RB_NIL){
		print_tab(depth); printf("NIL {Color : BLACK, Key: %d} @Depth %d \n",current->key,depth);
		return;
	}
	printf("\n");
	print_r(current->right,depth + 1);
	print_tab(depth); printf("{Color : %s, Key: %d} @Depth %d \n",current->color == BLACK? "BLACK" : "RED",current->key,depth);
	print_r(current->left,depth + 1);
	printf("\n");
}



#if 0
/**
 *  \param[in] parent parent node of the node to be deleted
 *  \param[in] delete_node	node to be deleted
 *  \param[in] context  contains relational information between parent and delete node
 */
static rb_treeNode_t* delete_r(rb_treeNode_t* parent,rb_treeNode_t* delete_node,uint8_t* context){
	if(!parent || !delete_node || !context)		// unexpected parameter error
		return RB_NIL;
	uint8_t direction = *context;
	if(IS_LEAF_NODE(delete_node)){
		// if leaf node (meaning at least one of two childs is nil)
		if((delete_node->left == RB_NIL) && (delete_node->right == RB_NIL)){
			if(direction == DIR_RIGHT)
				parent->right = RB_NIL;
			else
				parent->left = RB_NIL;
			return parent;
		}

		if(direction == DIR_RIGHT){
			if(delete_node->right != RB_NIL){
				if((delete_node->color == RED) || (delete_node->right->color == RED)){
					parent->right = delete_node->right;		// establish link between right child of delete node an parent as right
					parent->right->color = BLACK;			// change color
					return parent;
				}
			} else {
				if ((delete_node->color == RED) || (delete_node->left->color == RED)) {
					parent->right = delete_node->left;
					parent->left->color = BLACK;
					return parent;
				}
			}

			// both delete node and its child are black
			if(parent->left->color == BLACK) {	// color of sibling is also black
				if(parent->left->left->color == RED){
					parent->right = delete_node->right;
					parent = rotateRight(parent,FALSE);
					return parent;
				}
				if(parent->left->right->color == RED){
					parent->left = rotateLeft(parent->left,TRUE);
					parent = rotateRight(parent,FALSE);
					return parent;
				}
				// both child of sibling are black
			}

		}else{
			if(delete_node->left != RB_NIL){
				if((delete_node->color == RED) || (delete_node->left->color == RED)){
					parent->left = delete_node->left;
					parent->left->color = BLACK;
					return parent;
				}
			}else{
				if ((delete_node->color == RED) || (delete_node->right->color == RED)) {
					parent->right = delete_node->right;
					parent->left->color = BLACK;
					return parent;
				}
			}

			// both delete node and its child are black
		}
		return parent;
	}

	rb_treeNode_t* replace = NULL;
	if(delete_node->right != RB_NIL){
		delete_node->right = deleteLeftMost(delete_node->right,&replace);
		if(delete_node->right == replace)
			delete_node->right = RB_NIL;
	}

	if(delete_node->left != RB_NIL){
		delete_node->left = deleteRightMost(delete_node->left,&replace);
		if(delete_node->left == replace)
			delete_node->left = RB_NIL;
	}
	replace->color = delete_node->color;
	replace->left = delete_node->left;
	replace->right = delete_node->right;
	if(direction == DIR_RIGHT)
		parent->right = replace;
	else
		parent->left = replace;
	return parent;
}

#endif

static rb_treeNode_t* deleteLeftMost(rb_treeNode_t* root,rb_treeNode_t** leftmost){

}

static rb_treeNode_t* deleteRightMost(rb_treeNode_t* root,rb_treeNode_t** rightmost){

}


