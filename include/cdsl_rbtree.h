/*
 * cdsl_rbtree.h
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#ifndef INCLUDE_CDSL_RBTREE_H_
#define INCLUDE_CDSL_RBTREE_H_

#include "base_tree.h"

#ifdef __cplusplus
extern "C" {
#endif


/*!
 * \addtogroup red-black-tree Red-Black Tree
 * \brief Red-black Tree (Recursive Version)
 * @{
 */

#define cdsl_rbtreeMaxDepth(root)                      tree_max_depth((base_treeRoot_t*) root)
#define cdsl_rbtreeTraverse(root, cb, order, arg)      tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, order, arg)
#define cdsl_rbtreeTraverseTarget(root, cb, key, arg)  tree_traverse_target((base_treeRoot_t*) root, (base_tree_callback_t) cb, key, arg)
#define cdsl_rbtreeSize(root)                          tree_size((base_treeRoot_t*) root)
#define cdsl_rbtreePrint(root, print)                  tree_print((base_treeRoot_t*) root, print)
#define cdsl_rbtreeIsEmpty(root)                       tree_is_empty((base_treeRoot_t*) root)
#define cdsl_rbtreeTop(root)                           (rbtreeNode_t*) tree_top((base_treeRoot_t*) root)
#define cdsl_rbtreeGoLeft(cur)                         (rbtreeNode_t*) tree_go_left((base_treeNode_t*) cur)
#define cdsl_rbtreeGoRight(cur)                        (rbtreeNode_t*) tree_go_right((base_treeNode_t*) cur)
#define cdsl_rbtreeMin(root)                           (rbtreeNode_t*) tree_min((base_treeRoot_t*) root)
#define cdsl_rbtreeMax(root)                           (rbtreeNode_t*) tree_max((base_treeRoot_t*) root)
#define cdsl_rbtreeUpdate(root,nitem)                  (rbtreeNode_t*) tree_update((base_treeRoot_t*) root, (base_treeNode_t*) nitem)
#define cdsl_rbtreeDelete(root, key)                   cdsl_rbtreeDeleteReplace(root, key, NULL, NULL)
#define cdsl_rbtreeDeleteMin(root)                     cdsl_rbtreeDeleteMinReplace(root, NULL, NULL)
#define cdsl_rbtreeDeleteMax(root)                     cdsl_rbtreeDeleteMaxReplace(root, NULL, NULL)


/*!
 * \def cdsl_rbtreePrint_dev(root)
 * \brief Print tree structure
 * \param[in] root pointer to the root of tree
 *
 * \def cdsl_rbtreeMaxDepth(root)
 * \brief Get max depth of the tree

 * \param[in] root pointer to the root of tree
 * \return max. depth of the tree

 *
 *
 * \def cdsl_rbtreeTraverse(root, cb, order, arg)
 * \brief Traverse all the items within the tree, cb is callback function

 * \param[in] root pointer to the root of tree
 * \param[in] cb callback function called for each item of the tree
 * \param[in] order incremental / decremental traversal order
 * \param[in] arg argument which passed as argument when callback is invoked
 *
 *
 * \def cdsl_rbtreeTraverseTarget(root,cb,key, arg)
 * \brief Traverse all the parent item to the target node with given key value
 * \param[in] root pointer to the root of tree
 * \param[in] cb callback function called for each item of the tree
 * \param[in] key key value for the target
 * \param[in] arg argument which passed as argument when callback is invoked

 *
 * \def cdsl_rbtreeSize(root)
 * \brief Get total item count of the tree
 * \param[in] root pointer to the root of tree
 * \return total item count of the tree
 *
 * \def cdsl_rbtreePrint(root, print)
 * \brief Print root structure of tree
 * \param[in] root pointer to the root of tree
 * \param[in] print printer callback which called with arg

 *
 * \def  cdsl_rbtreeIsEmpty(root)
 * \brief Check whether the tree is empty or not
 * \param[in] root pointer to the root of tree
 * \return return true, if there is no item on the tree, otherwise return false
 *
 * \def cdsl_rbtreeTop(root)
 * \brief Get root item of the tree (not very meaningful except for heap)
 * \param[in] root pointer to the root of tree
 * \return top (entry) item of the tree
 *
 * \def cdsl_rbtreeGoLeft(cur)
 * \brief Peek left child of current item
 * \param[in] cur pointer to tree node
 * \return left child of the given tree node
 *
 * \def cdsl_rbtreeGoRight(cur)
 * \brief Peek right child of current item
 * \param[in] cur pointer to tree node
 * \return right child of the given tree node
 *
 * \def cdsl_rbtreeMin(root)
 * \brief Peek item with minimum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with minimum key value
 *
 * \def cdsl_rbtreeMax(root)
 * \brief Peek item with maximum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with maximum key value
 *
 * \def cdsl_rbtreeUpdate(root,nitem)
 * \brief Replace item with new one with same key value
 * \param[in] root pointer to tree root
 * \param[in] nitem new item
 * \return old item replaced by nitem
 *
 * \def cdsl_rbtreeDelete(root, key)
 * \brief Delete item with the key
 * \param[in] root pointer to tree root
 * \param[in] key value for item to be deleted
 * \return deleted item
 *
 * \def cdsl_rbtreeDeleteMin(root)
 * \brief Delete item with minimum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with minimum key value
 *
 * \def cdsl_rbtreeDeleteMax(root)
 * \brief Delete item with maximum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with maximum key value

 */

/*!
 * \brief red-black node type
 */
typedef struct cdsl_rbtree rbtreeNode_t;
/*!
 * \brief root of red-black tree
 *
 */
typedef struct cdsl_nrbroot {
	/*
	 * \brief internal structure inherited from base_tree_root
	 */
	union {
		struct base_tree_root 	base_root;
		rbtreeNode_t* entry;
	};
}rbtreeRoot_t;

/*!
 * \brief node of red-black tree
 */
struct cdsl_rbtree {
	union {
		struct base_tree_node 	base_node;
		struct {
			rbtreeNode_t* left;
			rbtreeNode_t* right;
			trkey_t          key;
		};

	};
};

typedef BOOL (*condition_t)(rbtreeNode_t* node, trkey_t key);


/*!
 * \fn void cdsl_rbtreeRootInit(rbtreeRoot_t* rootp)
 * \brief Initialize red-black tree root
 * \param[in] rootp pointer to uninitialized \ref rbtreeRoot_t
 */
extern void cdsl_rbtreeRootInit(rbtreeRoot_t* rootp);
/*!
 * \fn void cdsl_rbtreeNodeInit(rbtreeNode_t* node, trkey_t key)
 * \brief Initialize red-black tree node
 * \param[in] node pointer to uninitialized \ref rbtreeNode_t
 * \param[in] key key value for the node
 */
extern void cdsl_rbtreeNodeInit(rbtreeNode_t* node, trkey_t key);

/*!\fn rbtreeNode_t* cdsl_rbtreeInsert(rbtreeRoot_t* rootp,rbtreeNode_t* item, BOOL is_set)
 * \brief Insert element to tree
 * \param[in] rootp pointer of tree root
 * \param[in] item new tree node item to be inserted
 * \param[in] is_set define conflict handling mechanism, if true,
 *  	replacement performed if the element with same key value exists
 *  	within the tree, otherwise, added without replacement
*  \return if is_set is true, return replaced node otherwise, return null
 */
extern rbtreeNode_t* cdsl_rbtreeInsert(rbtreeRoot_t* rootp,rbtreeNode_t* item, BOOL is_set);

/*!
 * \brief Perform lookup operation and return node with given key, if there are multiple items with same key, the first encountered item during lookup operation will be returned
 * \param[in] rootp pointer of tree root
 * \param[in] key key value for the target node
 * \return found node (\ref rbtreeNode_t) with given key
 */
extern rbtreeNode_t* cdsl_rbtreeLookup(rbtreeRoot_t* rootp,trkey_t key);

/*!
 * \brief Try lookup node with given key & additional condition. if both the key and the condition match to node, the node will be return, otherwise null.
 *        this API is useful if there are nodes with same key value, then additional condition can be used to pick exact node.
 *        or traverse down the tree with given key and try to pick node with additional condition
 * \param[in] rootp pointer of tree root
 * \param[in] key key value for the target node
 * \param[in] match condition callback used to pick node
 * \return found node (\ref rbtreeNode_t) with given key
 */
extern rbtreeNode_t* cdsl_rbtreeConditionalLookup(rbtreeRoot_t* rootp, trkey_t key, condition_t match);

/*!
 * \brief Delete item with or without replacement
 * \param[in] rootp pointer of tree root
 * \param[in] key key value for the target node
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 */
extern rbtreeNode_t* cdsl_rbtreeDeleteReplace(rbtreeRoot_t* rootp,trkey_t key, base_tree_replacer_t replacer, void* cb_arg);

/*!
 * \brief Delete item with minimum key value within the tree
 * \param[in] rootp pointer of tree root
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 */
extern rbtreeNode_t* cdsl_rbtreeDeleteMinReplace(rbtreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg);
/*!
 * \brief Delete item with maximum key value within the tree
 * \param[in] rootp pointer of tree root
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 */
extern rbtreeNode_t* cdsl_rbtreeDeleteMaxReplace(rbtreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg);


#ifdef __DBG
extern void cdsl_rbtreePrint_dev(rbtreeRoot_t* root);
#else
#define cdsl_rbtreePrint_dev(root)
#endif

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CDSL_RBTREE_H_ */
