/*
 * cdsl_avltree.c
 *
 *  Created on: Jun 3, 2015
 *      Author: innocentevil
 */

#include "base_tree.h"
#include "cdsl_avltree.h"

#define DIR_LEFT ((uint8_t)1)
#define DIR_RIGHT ((uint8_t)2)

#define HEIGHT_COMPRESSION_RATE ((uint8_t)4)

#define PATTERN_MASK ((uint8_t)0x0F)
#define LEFTRIGTH_PATTERN ((uint8_t)(DIR_RIGHT << 2) | DIR_LEFT)
#define LEFTLEFT_PATTERN ((uint8_t)(DIR_LEFT << 2) | DIR_LEFT)
#define RIGHTLEFT_PATTERN ((uint8_t)(DIR_LEFT << 2) | DIR_RIGHT)
#define RIGHTRIGHT_PATTERN ((uint8_t)(DIR_RIGHT << 2) | DIR_RIGHT)

#define RIGHT_CHILD_OF(parent)               INTO_ABSOLUTE(parent, parent->right, avltreeNode_t*)
#define LEFT_CHILD_OF(parent)                INTO_ABSOLUTE(parent, parent->left, avltreeNode_t*)
#define ENTRY(rootp)                         INTO_ABSOLUTE(rootp, rootp->entry, avltreeNode_t*)

/**
 *  Note : function call SHOULD NOT be used as parameter
 * */
#define NULLABLE_ENTRY(rootp)                rootp->entry? ENTRY(rootp) : NULL
#define NULLABLE_RIGHT_CHILD_OF(parent)      parent->right? RIGHT_CHILD_OF(parent) : NULL
#define NULLABLE_LEFT_CHILD_OF(parent)       parent->left? LEFT_CHILD_OF(parent) : NULL


#define CLR_LEFT(parent)                     do {                \
	parent->left = NULL;                                         \
} while(0)

#define CLR_RIGHT(parent)                    do {                \
	parent->right = NULL;                                        \
} while(0)

#define CLR_ENTRY(rootp)                     do {                \
	rootp->entry = NULL;                                         \
} while(0)

#define SET_ENTRY(rootp, entry)                 set_entry((base_treeRoot_t*) rootp, (base_treeNode_t*) (entry))

#define SET_LEFT(parent, left)                  set_left((base_treeNode_t*) parent, (base_treeNode_t*) (left))

#define SET_RIGHT(parent, right)                set_right((base_treeNode_t*) parent, (base_treeNode_t*) (right))


static avltreeNode_t *insert_rc(int bal, avltreeNode_t *sub_root_c, avltreeNode_t *item, uint8_t *rc_dir, avltreeNode_t **replaced);
static avltreeNode_t *delete_rc(int bal, avltreeNode_t *sub_root_c, avltreeNode_t **deleteed, trkey_t key, base_tree_replacer_t replacer, void *args);
static avltreeNode_t *delete_lm_rc(int bal, avltreeNode_t *sub_root_c, avltreeNode_t **deleted, base_tree_replacer_t replacer, void *args);
static avltreeNode_t *delete_rm_rc(int bal, avltreeNode_t *sub_root_c, avltreeNode_t **deleted, base_tree_replacer_t replacer, void *args);
static avltreeNode_t *take_rightmost_rc(int bal, avltreeNode_t *parent, avltreeNode_t **leaf);
static avltreeNode_t *take_leftmost_rc(int bal, avltreeNode_t *parent, avltreeNode_t **leaf);

static avltreeNode_t *rotate_right(avltreeNode_t *sub_root);
static avltreeNode_t *rotate_left(avltreeNode_t *sub_root);

static int max_child_height(avltreeNode_t *node);

static void _write_ser_node_header(cdsl_serializeNode_t *node_head, const void *node);
static void _write_serialize_header(cdsl_serializeHeader_t *header);
static void _build_node(const cdsl_serializeNode_t *node_header, void *node);

const cdsl_serializeExtInterface_t _cdsl_avltree_serializer_ext = {
	.free_ext_node = NULL,
	.alloc_ext_node = NULL,
	.write_node_haeder = _write_ser_node_header,
	.write_serialize_header = _write_serialize_header,
	.on_node_build = _build_node};

void cdsl_avltreeRootInit(avltreeRoot_t *rootp, uint8_t bal)
{
	if (!rootp)
	{
		return;
	}
	rootp->balance_factor = bal;
	rootp->entry = NULL;
}

void cdsl_avltreeNodeInit(avltreeNode_t *node, trkey_t key)
{
	if (!node)
	{
		return;
	}
	node->height = 0;
	node->key = key;
	CLR_LEFT(node);
	CLR_RIGHT(node);
}


/*!
 * insert element to tree
 *
 */
avltreeNode_t *cdsl_avltreeInsert(avltreeRoot_t *rootp, avltreeNode_t *item, BOOL is_set)
{
	if (!rootp || !item)
	{
		return NULL;
	}
	uint8_t dir = 0;
	avltreeNode_t *replaced = NULL;
	SET_ENTRY(rootp, insert_rc(rootp->balance_factor, NULLABLE_ENTRY(rootp), item, &dir, is_set ? &replaced : NULL));
	return replaced;
}

avltreeNode_t *cdsl_avltreeLookup(avltreeRoot_t *rootp, trkey_t key)
{
	if (!rootp)
	{
		return NULL;
	}
	avltreeNode_t *cur = NULLABLE_ENTRY(rootp);
	while (cur)
	{
		if (cur->key > key)
		{
			cur = NULLABLE_LEFT_CHILD_OF(cur);
		}
		else if (cur->key < key)
		{
			cur = NULLABLE_RIGHT_CHILD_OF(cur);
		}
		else
		{
			return cur;
		}
	}
	return NULL;
}

avltreeNode_t *cdsl_avltreeConditionalLookup(avltreeRoot_t *rootp, trkey_t key, condition_t match)
{
	if (!rootp)
	{
		return NULL;
	}
	avltreeNode_t *cur = NULLABLE_ENTRY(rootp);
	while (cur)
	{
		if (match(&cur->base_node, key))
		{
			return cur;
		}
		if (cur->key < key)
		{
			cur = NULLABLE_RIGHT_CHILD_OF(cur);
		}
		else
		{
			cur = NULLABLE_LEFT_CHILD_OF(cur);
		}
	}
	return NULL;
}

avltreeNode_t *cdsl_avltreeDeleteReplace(avltreeRoot_t *rootp, trkey_t key, base_tree_replacer_t replacer, void *cb_arg)
{
	if (!rootp)
	{
		return NULL;
	}
	avltreeNode_t *deleted = NULL;
	SET_ENTRY(rootp, delete_rc(rootp->balance_factor, NULLABLE_ENTRY(rootp), &deleted, key, replacer, cb_arg));
	return deleted;
}

avltreeNode_t *cdsl_avltreeDeleteMinReplace(avltreeRoot_t *rootp, base_tree_replacer_t replacer, void *cb_arg)
{
	if (!rootp)
	{
		return NULL;
	}
	avltreeNode_t *deleted = NULL;
	SET_ENTRY(rootp, delete_lm_rc(rootp->balance_factor, NULLABLE_ENTRY(rootp), &deleted, replacer, cb_arg));
	return deleted;
}

avltreeNode_t *cdsl_avltreeDeleteMaxReplace(avltreeRoot_t *rootp, base_tree_replacer_t replacer, void *cb_arg)
{
	if (!rootp)
	{
		return NULL;
	}
	avltreeNode_t *deleted = NULL;
	SET_ENTRY(rootp, delete_rm_rc(rootp->balance_factor, NULLABLE_ENTRY(rootp), &deleted, replacer, cb_arg));
	return deleted;
}

static avltreeNode_t *delete_lm_rc(int bal, avltreeNode_t *sub_root_c, avltreeNode_t **deleted, base_tree_replacer_t replacer, void *args)
{
	if (!sub_root_c)
	{
		return NULL;
	}
	if (!sub_root_c->left)
	{
		*deleted = sub_root_c;
		avltreeNode_t *nroot = NULL;
		if (replacer && replacer((base_treeNode_t **)&nroot, args))
		{
			if (nroot == NULL)
			{
				return sub_root_c;
			}
			nroot->height = sub_root_c->height;
			SET_LEFT(nroot, NULLABLE_LEFT_CHILD_OF(sub_root_c));
			SET_RIGHT(nroot, NULLABLE_RIGHT_CHILD_OF(sub_root_c));
			CLR_LEFT(sub_root_c);
			CLR_RIGHT(sub_root_c);
			return nroot;
		}
		if (sub_root_c->right)
		{
			SET_RIGHT(sub_root_c, take_leftmost_rc(bal, NULLABLE_RIGHT_CHILD_OF(sub_root_c), &nroot));
			SET_RIGHT(nroot, NULLABLE_RIGHT_CHILD_OF(sub_root_c));
			SET_LEFT(nroot, NULLABLE_LEFT_CHILD_OF(sub_root_c));
			nroot->height = sub_root_c->height;
			CLR_LEFT(sub_root_c);
			CLR_RIGHT(sub_root_c);
			sub_root_c = nroot;
		}
		else
		{
			return NULL;
		}
	}
	SET_LEFT(sub_root_c, delete_lm_rc(bal, NULLABLE_LEFT_CHILD_OF(sub_root_c), deleted, replacer, args));
	if (sub_root_c->right && sub_root_c->left)
	{
		if (RIGHT_CHILD_OF(sub_root_c)->height - LEFT_CHILD_OF(sub_root_c)->height > bal)
		{
			if (!RIGHT_CHILD_OF(sub_root_c)->right) 
			{
				SET_RIGHT(sub_root_c, rotate_right(RIGHT_CHILD_OF(sub_root_c)));
			}
			sub_root_c = rotate_left(sub_root_c);
		}
		sub_root_c->height = max_child_height(sub_root_c) + 1;
	}
	else if (sub_root_c->right)
	{
		if (RIGHT_CHILD_OF(sub_root_c)->height > bal)
		{
			if (!RIGHT_CHILD_OF(sub_root_c)->right)
			{
				SET_RIGHT(sub_root_c, rotate_right(RIGHT_CHILD_OF(sub_root_c)));
			}
			sub_root_c = rotate_left(sub_root_c);
		}
		sub_root_c->height = RIGHT_CHILD_OF(sub_root_c)->height + 1;
	}
	else if (sub_root_c->left)
	{
		if (LEFT_CHILD_OF(sub_root_c)->height > bal)
		{
			if (!LEFT_CHILD_OF(sub_root_c)->left)
			{
				SET_LEFT(sub_root_c, rotate_left(LEFT_CHILD_OF(sub_root_c)));
			}
			sub_root_c = rotate_right(sub_root_c);
		}
		sub_root_c->height = LEFT_CHILD_OF(sub_root_c)->height + 1;
	}
	else
	{
		sub_root_c->height = 1;
	}
	return sub_root_c;
}

static avltreeNode_t *delete_rm_rc(int bal, avltreeNode_t *sub_root_c, avltreeNode_t **deleted, base_tree_replacer_t replacer, void *args)
{
	if (!sub_root_c)
	{
		return NULL;
	}
	if (!sub_root_c->right)
	{
		*deleted = sub_root_c;
		avltreeNode_t *nroot = NULL;
		if (replacer && replacer((base_treeNode_t **)&nroot, args))
		{
			if (nroot == NULL)
			{
				return sub_root_c;
			}
			nroot->height = sub_root_c->height;
			SET_LEFT(nroot, NULLABLE_LEFT_CHILD_OF(sub_root_c));
			SET_RIGHT(nroot, NULLABLE_RIGHT_CHILD_OF(sub_root_c));
			CLR_RIGHT(sub_root_c);
			CLR_LEFT(sub_root_c);
			return nroot;
		}
		else if (sub_root_c->left)
		{
			SET_LEFT(sub_root_c, take_rightmost_rc(bal, NULLABLE_LEFT_CHILD_OF(sub_root_c), &nroot));
			SET_LEFT(nroot, NULLABLE_LEFT_CHILD_OF(sub_root_c));
			SET_RIGHT(nroot, NULLABLE_RIGHT_CHILD_OF(sub_root_c));
			nroot->height = sub_root_c->height;
			CLR_LEFT(sub_root_c);
			CLR_RIGHT(sub_root_c);
			sub_root_c = nroot;
		}
		else
		{
			return NULL;
		}
	}
	SET_RIGHT(sub_root_c,delete_rm_rc(bal, NULLABLE_RIGHT_CHILD_OF(sub_root_c), deleted, replacer, args));
	if (sub_root_c->right && sub_root_c->left)
	{
		if (LEFT_CHILD_OF(sub_root_c)->height - RIGHT_CHILD_OF(sub_root_c)->height > bal)
		{
			if (!LEFT_CHILD_OF(sub_root_c)->left)
			{
				SET_LEFT(sub_root_c, rotate_left(LEFT_CHILD_OF(sub_root_c)));
			}
			sub_root_c = rotate_right(sub_root_c);
		}
		sub_root_c->height = max_child_height(sub_root_c) + 1;
	}
	else if (sub_root_c->right)
	{
		if (RIGHT_CHILD_OF(sub_root_c)->height > bal)
		{
			if (!RIGHT_CHILD_OF(sub_root_c)->right)
			{
				SET_RIGHT(sub_root_c, rotate_right(RIGHT_CHILD_OF(sub_root_c)));
			}
			sub_root_c = rotate_left(sub_root_c);
		}
		sub_root_c->height = RIGHT_CHILD_OF(sub_root_c)->height + 1;
	}
	else if (sub_root_c->left)
	{
		if (LEFT_CHILD_OF(sub_root_c)->height > bal)
		{
			if (!LEFT_CHILD_OF(sub_root_c)->left)
			{
				SET_LEFT(sub_root_c, rotate_left(LEFT_CHILD_OF(sub_root_c)));
			}
			sub_root_c = rotate_right(sub_root_c);
		}
		sub_root_c->height = LEFT_CHILD_OF(sub_root_c)->height + 1;
	}
	else
	{
		sub_root_c->height = 1;
	}
	return sub_root_c;
}

static avltreeNode_t *delete_rc(int bal, avltreeNode_t *sub_root_c, avltreeNode_t **deleted, trkey_t key, base_tree_replacer_t replacer, void *args)
{
	if (!sub_root_c)
	{
		return NULL;
	}
	if (sub_root_c->key > key)
	{
		SET_LEFT(sub_root_c, delete_rc(bal, NULLABLE_LEFT_CHILD_OF(sub_root_c), deleted, key, replacer, args));
		if (!sub_root_c->left)
		{
			if (sub_root_c->right)
			{
				sub_root_c->height = RIGHT_CHILD_OF(sub_root_c)->height + 1;
				if (RIGHT_CHILD_OF(sub_root_c)->height > bal)
				{
					if (!RIGHT_CHILD_OF(sub_root_c)->right)
					{
						SET_RIGHT(sub_root_c, rotate_right(RIGHT_CHILD_OF(sub_root_c)));
					}
					sub_root_c = rotate_left(sub_root_c);
				}
			}
			else
			{
				sub_root_c->height = 1;
			}
		}
		else
		{
			if (sub_root_c->right)
			{
				sub_root_c->height = RIGHT_CHILD_OF(sub_root_c)->height > LEFT_CHILD_OF(sub_root_c)->height ? RIGHT_CHILD_OF(sub_root_c)->height + 1 : LEFT_CHILD_OF(sub_root_c)->height + 1;
				if (RIGHT_CHILD_OF(sub_root_c)->height - LEFT_CHILD_OF(sub_root_c)->height > bal)
				{
					if (!RIGHT_CHILD_OF(sub_root_c)->right)
					{
						SET_RIGHT(sub_root_c, rotate_right(RIGHT_CHILD_OF(sub_root_c)));
					}
					sub_root_c = rotate_left(sub_root_c);
				}
			}
			else
			{
				sub_root_c->height = LEFT_CHILD_OF(sub_root_c)->height + 1;
			}
		}
		return sub_root_c;
	}
	else if (sub_root_c->key < key)
	{
		SET_RIGHT(sub_root_c, delete_rc(bal, NULLABLE_RIGHT_CHILD_OF(sub_root_c), deleted, key, replacer, args));
		if (!sub_root_c->right)
		{
			if (sub_root_c->left)
			{
				sub_root_c->height = LEFT_CHILD_OF(sub_root_c)->height + 1;
				if (LEFT_CHILD_OF(sub_root_c)->height > bal)
				{
					if (!LEFT_CHILD_OF(sub_root_c)->left)
					{
						SET_LEFT(sub_root_c, rotate_left(LEFT_CHILD_OF(sub_root_c)));
					}
					sub_root_c = rotate_right(sub_root_c);
				}
			}
			else
			{
				sub_root_c->height = 1;
			}
		}
		else
		{
			if (sub_root_c->left)
			{
				sub_root_c->height = RIGHT_CHILD_OF(sub_root_c)->height > LEFT_CHILD_OF(sub_root_c)->height ? RIGHT_CHILD_OF(sub_root_c)->height + 1 : LEFT_CHILD_OF(sub_root_c)->height + 1;
				if (LEFT_CHILD_OF(sub_root_c)->height - RIGHT_CHILD_OF(sub_root_c)->height > bal)
				{
					if (!LEFT_CHILD_OF(sub_root_c)->left)
					{
						SET_LEFT(sub_root_c, rotate_left(LEFT_CHILD_OF(sub_root_c)));
					}
					sub_root_c = rotate_right(sub_root_c);
				}
			}
			else
			{
				sub_root_c->height = RIGHT_CHILD_OF(sub_root_c)->height + 1;
			}
		}
		return sub_root_c;
	}
	else
	{
		*deleted = sub_root_c;
		avltreeNode_t *nroot = NULL;
		if (replacer && replacer((base_treeNode_t **)&nroot, args))
		{
			if (nroot == NULL)
			{
				return sub_root_c;
			}
			nroot->height = sub_root_c->height;
			SET_LEFT(nroot, NULLABLE_LEFT_CHILD_OF(sub_root_c));
			SET_RIGHT(nroot, NULLABLE_RIGHT_CHILD_OF(sub_root_c));
			CLR_RIGHT(sub_root_c);
			CLR_LEFT(sub_root_c);
			return nroot;
		}
		if (sub_root_c->right)
		{
			SET_RIGHT(sub_root_c, take_leftmost_rc(bal, RIGHT_CHILD_OF(sub_root_c), &nroot));
			SET_RIGHT(nroot, NULLABLE_RIGHT_CHILD_OF(sub_root_c));
			SET_LEFT(nroot, NULLABLE_LEFT_CHILD_OF(sub_root_c));
			nroot->height = sub_root_c->height;
			CLR_LEFT(sub_root_c);
			CLR_RIGHT(sub_root_c);
			sub_root_c = nroot;
		}
		else if (sub_root_c->left)
		{
			SET_LEFT(sub_root_c, take_rightmost_rc(bal, LEFT_CHILD_OF(sub_root_c), &nroot));
			SET_LEFT(nroot, NULLABLE_LEFT_CHILD_OF(sub_root_c));
			SET_RIGHT(nroot, NULLABLE_RIGHT_CHILD_OF(sub_root_c));
			nroot->height = sub_root_c->height;
			CLR_RIGHT(sub_root_c);
			CLR_LEFT(sub_root_c);
			sub_root_c = nroot;
		}
		else
		{
			return NULL;
		}
	}
	sub_root_c->height = max_child_height(sub_root_c) + 1;
	return sub_root_c;
}

static avltreeNode_t *take_rightmost_rc(int bal, avltreeNode_t *parent, avltreeNode_t **leaf)
{
	if (!parent)
	{
		return NULL;
	}
	if (!parent->right)
	{
		*leaf = parent;
		return NULLABLE_LEFT_CHILD_OF(parent);
	}
	SET_RIGHT(parent, take_rightmost_rc(bal, NULLABLE_RIGHT_CHILD_OF(parent), leaf));
	if (!parent->right)
	{
		if (!parent->left)
		{
			parent->height = 1;
			return parent;
		}

		if (LEFT_CHILD_OF(parent)->height > bal)
		{
			return rotate_right(parent);
		}
	}
	else
	{
		if (!parent->left)
		{
			parent->height = RIGHT_CHILD_OF(parent)->height + 1;
			return parent;
		}

		if (LEFT_CHILD_OF(parent)->height - RIGHT_CHILD_OF(parent)->height > bal)
		{
			return rotate_right(parent);
		}
	}
	return parent;
}

static avltreeNode_t *take_leftmost_rc(int bal, avltreeNode_t *parent, avltreeNode_t **leaf)
{
	if (!parent)
	{
		return NULL;
	}
	if (!parent->left)
	{
		*leaf = parent;
		return NULLABLE_RIGHT_CHILD_OF(parent);
	}
	SET_LEFT(parent, take_leftmost_rc(bal, NULLABLE_LEFT_CHILD_OF(parent), leaf));
	if (!parent->left)
	{
		if (!parent->right)
		{
			parent->height = 1;
			return parent;
		}

		if (parent->right->height > bal)
		{
			return rotate_left(parent);
		}
	}
	else
	{
		if (!parent->right)
		{
			parent->height = LEFT_CHILD_OF(parent)->height + 1;
			return parent;
		}

		if (RIGHT_CHILD_OF(parent)->height - LEFT_CHILD_OF(parent)->height > bal)
		{
			return rotate_left(parent);
		}
	}
	return parent;
}

static avltreeNode_t *insert_rc(int bal, avltreeNode_t *sub_root_c, avltreeNode_t *item, uint8_t *rc_dir, avltreeNode_t **replaced)
{
	if (!sub_root_c)
	{
		item->height = 1;
		return item;
	}
	if (sub_root_c->key > item->key)
	{
		SET_LEFT(sub_root_c, insert_rc(bal, NULLABLE_LEFT_CHILD_OF(sub_root_c), item, rc_dir, replaced));
		sub_root_c->height = LEFT_CHILD_OF(sub_root_c)->height + 1;
		*rc_dir |= DIR_LEFT;
	}
	else if (sub_root_c->key < item->key)
	{
		SET_RIGHT(sub_root_c, insert_rc(bal, NULLABLE_RIGHT_CHILD_OF(sub_root_c), item, rc_dir, replaced));
		sub_root_c->height = RIGHT_CHILD_OF(sub_root_c)->height + 1;
		*rc_dir |= DIR_RIGHT;
	}
	else
	{
		if (replaced)
		{
			*replaced = sub_root_c;
			SET_LEFT(item, NULLABLE_LEFT_CHILD_OF(sub_root_c));
			SET_RIGHT(item, NULLABLE_RIGHT_CHILD_OF(sub_root_c));
			item->height = sub_root_c->height;
			return item;
		}
		else
		{
			SET_LEFT(sub_root_c, insert_rc(bal, NULLABLE_LEFT_CHILD_OF(sub_root_c), item, rc_dir, replaced));
			sub_root_c->height = LEFT_CHILD_OF(sub_root_c)->height + 1;
			*rc_dir |= DIR_LEFT;
		}
	}
	switch ((*rc_dir) & PATTERN_MASK)
	{
	case RIGHTLEFT_PATTERN:
		/*
		 *
		 */
		if (!sub_root_c->left)
		{

			if (!(sub_root_c->height > bal))
			{
				break;
			}
		}
		else
		{
			if (!(RIGHT_CHILD_OF(sub_root_c)->height - LEFT_CHILD_OF(sub_root_c)->height > bal))
			{

				break;
			}
		}
		SET_RIGHT(sub_root_c, rotate_right(NULLABLE_RIGHT_CHILD_OF(sub_root_c)));
		sub_root_c = rotate_left(sub_root_c);
		break;
	case RIGHTRIGHT_PATTERN:
		if (!sub_root_c->left)
		{

			if (!(sub_root_c->height > bal))
			{
				break;
			}
		}
		else
		{
			if (!(RIGHT_CHILD_OF(sub_root_c)->height - LEFT_CHILD_OF(sub_root_c)->height > bal))
			{

				break;
			}
		}
		sub_root_c = rotate_left(sub_root_c);
		break;
	case LEFTLEFT_PATTERN:
		if (!sub_root_c->right)
		{

			if (!(LEFT_CHILD_OF(sub_root_c)->height > bal))
			{
				break;
			}
		}
		else
		{
			if (!(LEFT_CHILD_OF(sub_root_c)->height - RIGHT_CHILD_OF(sub_root_c)->height > bal))
			{

				break;
			}
		}
		sub_root_c = rotate_right(sub_root_c);
		break;
	case LEFTRIGTH_PATTERN:
		if (!sub_root_c->right)
		{

			if (!(LEFT_CHILD_OF(sub_root_c)->height > bal))
			{
				break;
			}
		}
		else
		{
			if (!(LEFT_CHILD_OF(sub_root_c)->height - RIGHT_CHILD_OF(sub_root_c)->height > bal))
			{

				break;
			}
		}
		SET_LEFT(sub_root_c, rotate_left(NULLABLE_LEFT_CHILD_OF(sub_root_c)));
		sub_root_c = rotate_right(sub_root_c);
		break;
	}
	*rc_dir <<= 2;
	return sub_root_c;
}

static avltreeNode_t *rotate_right(avltreeNode_t *sub_root)
{
	if (!sub_root)
	{
		return sub_root;
	}
	if (!sub_root->left)
	{
		return sub_root;
	}
	avltreeNode_t *nroot = LEFT_CHILD_OF(sub_root);
	SET_LEFT(sub_root, NULLABLE_RIGHT_CHILD_OF(nroot));
	SET_RIGHT(nroot, sub_root);
	if (nroot->left)
	{
		nroot->height = LEFT_CHILD_OF(nroot)->height + 1;
	}
	else
	{
		nroot->height = 1;
	}
	RIGHT_CHILD_OF(nroot)->height = max_child_height(NULLABLE_RIGHT_CHILD_OF(nroot)) + 1;
	return nroot;
}

static avltreeNode_t *rotate_left(avltreeNode_t *sub_root)
{
	if (!sub_root)
	{
		return sub_root;
	}
	if (!sub_root->right)
	{
		return sub_root;
	}
	avltreeNode_t *nroot = RIGHT_CHILD_OF(sub_root);
	SET_RIGHT(sub_root, NULLABLE_LEFT_CHILD_OF(nroot));
	SET_LEFT(nroot, sub_root);

	if (nroot->right)
	{
		nroot->height = RIGHT_CHILD_OF(nroot)->height + 1;
	}
	else
	{
		nroot->height = 1;
	}
	LEFT_CHILD_OF(nroot)->height = max_child_height(NULLABLE_LEFT_CHILD_OF(nroot)) + 1;
	return nroot;
}

static int max_child_height(avltreeNode_t *node)
{
	if (node->right && node->left)
	{
		return RIGHT_CHILD_OF(node)->height > LEFT_CHILD_OF(node)->height ? RIGHT_CHILD_OF(node)->height : LEFT_CHILD_OF(node)->height;
	}
	else if (node->left)
	{
		return LEFT_CHILD_OF(node)->height;
	}
	else if (node->right)
	{
		return RIGHT_CHILD_OF(node)->height;
	}
	return 0;
}

static void _write_ser_node_header(cdsl_serializeNode_t *node_head, const void *node)
{
	if (node_head == NULL)
		return;
	avltreeNode_t *avlnode = (avltreeNode_t *)node;
	SET_SPEC(node_head, avlnode->height);
}

static void _write_serialize_header(cdsl_serializeHeader_t *header)
{
	header->type |= SUB_TYPE_AVLNODE;
}

static void _build_node(const cdsl_serializeNode_t *node_header, void *node)
{
	uint8_t spec = GET_SPEC(node_header);
	avltreeNode_t *avlnode = (avltreeNode_t *)node;
	avlnode->height = spec;
}
