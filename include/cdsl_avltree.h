/*
 * cdsl_avltree.h
 *
 *  Created on: Jun 3, 2015
 *      Author: innocentevil
 */

#ifndef CDSL_AVLTREE_H_
#define CDSL_AVLTREE_H_

#include "base_tree.h"

#if defined(__cplusplus)
extern "C" {
#endif



/*!
 * \addtogroup avl-tree AVL Tree
 * \brief AVL Tree (Recursive Version)
 * @{
 */


#define cdsl_avltreeMaxDepth(root)                       tree_max_depth((base_treeRoot_t*) root)
#define cdsl_avltreeForEach(root, cb, order, arg)        tree_for_each((base_treeRoot_t*) root, (base_tree_callback_t) cb, order, arg)
#define cdsl_avltreeForEachToTarget(root,cb,key, arg)    tree_for_each_to_target((base_treeRoot_t*) root, (base_tree_callback_t) cb, key, arg)
#define cdsl_avltreeSize(root)                           tree_size((base_treeRoot_t*) root)
#define cdsl_avltreePrint(root, print)                   tree_print((base_treeRoot_t*) root, print)
#define cdsl_avltreeIsEmpty(root)                        tree_is_empty((base_treeRoot_t*) root)
#define cdsl_avltreeTop(root)                            (avltreeNode_t*) tree_top((base_treeRoot_t*) root)
#define cdsl_avltreeGoLeft(cur)                          (avltreeNode_t*) tree_go_left((base_treeNode_t*) cur)
#define cdsl_avltreeGoRight(cur)                         (avltreeNode_t*) tree_go_right((base_treeNode_t*) cur)
#define cdsl_avltreeMin(root)                            (avltreeNode_t*) tree_min((base_treeRoot_t*) root)
#define cdsl_avltreeMax(root)                            (avltreeNode_t*) tree_max((base_treeRoot_t*) root)
#define cdsl_avltreeUpdate(root,nitem)                   (avltreeNode_t*) tree_update((base_treeRoot_t*) root, (base_treeNode_t*) nitem)
#define cdsl_avltreeDelete(root, key)                    cdsl_avltreeDeleteReplace(root, key, NULL, NULL)
#define cdsl_avltreeDeleteMin(root)                      cdsl_avltreeDeleteMinReplace(root, NULL, NULL)
#define cdsl_avltreeDeleteMax(root)                      cdsl_avltreeDeleteMaxReplace(root, NULL, NULL)




/*!
 *
 * \def cdsl_avltreeMaxDepth(root)
 * \brief Get max depth of the tree
 * \param[in] root pointer to the root of tree
 * \return max. depth of the tree
 *
 *
 * \def cdsl_avltreeTraverse(root, cb, order, arg)
 * \brief Traverse all the items within the tree, cb is callback function
 * \param[in] root pointer to the root of tree
 * \param[in] cb callback function called for each item of the tree
 * \param[in] order incremental / decremental traversal order
 * \param[in] arg argument which passed as argument when callback is invoked
 *
 *
 * \def cdsl_avltreeTraverseTarget(root,cb,key, arg)
 * \brief Traverse all the parent item to the target node with given key value
 * \param[in] root pointer to the root of tree
 * \param[in] cb callback function called for each item of the tree
 * \param[in] key key value for the target
 * \param[in] arg argument which passed as argument when callback is invoked
 *
 * \def cdsl_avltreeSize(root)
 * \brief Get total item count of the tree
 * \param[in] root pointer to the root of tree
 * \return total item count of the tree
 *
 * \def cdsl_avltreePrint(root, print)
 * \brief Print root structure of tree
 * \param[in] root pointer to the root of tree
 * \param[in] print printer callback which called with arg
 *
 * \def  cdsl_avltreeIsEmpty(root)
 * \brief Check whether the tree is empty or not
 * \param[in] root pointer to the root of tree
 * \return return true, if there is no item on the tree, otherwise return false
 *
 * \def cdsl_avltreeTop(root)
 * \brief Get root item of the tree (not very meaningful except for heap)
 * \param[in] root pointer to the root of tree
 * \return top (entry) item of the tree
 *
 * \def cdsl_avltreeGoLeft(cur)
 * \brief Peek left child of current item
 * \param[in] cur pointer to tree node
 * \return left child of the given tree node
 *
 * \def cdsl_avltreeGoRight(cur)
 * \brief Peek right child of current item
 * \param[in] cur pointer to tree node
 * \return right child of the given tree node
 *
 * \def cdsl_avltreeMin(root)
 * \brief Peek item with minimum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with minimum key value
 *
 * \def cdsl_avltreeMax(root)
 * \brief Peek item with maximum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with maximum key value
 *
 * \def cdsl_avltreeUpdate(root,nitem)
 * \brief Replace item with new one with same key value
 * \param[in] root pointer to tree root
 * \param[in] nitem new item
 * \return old item replaced by nitem
 *
 * \def cdsl_avltreeDelete(root, key)
 * \brief Delete item with the key
 * \param[in] root pointer to tree root
 * \param[in] key value for item to be deleted
 * \return deleted item
 *
 * \def cdsl_avltreeDeleteMin(root)
 * \brief Delete item with minimum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with minimum key value
 *
 * \def cdsl_avltreeDeleteMax(root)
 * \brief Delete item with maximum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with maximum key value
 */

/*!
 * \brief tree node type definition
 */
typedef struct cdsl_avlnode avltreeNode_t;

/*!
 * \brief tree root for AVL tree
 */
typedef struct cdsl_avlroot {
	union {
		base_treeRoot_t  base_root;
		avltreeNode_t *entry;
	};
	int balance_factor;   ///< balance factor for AVL tree
} avltreeRoot_t;

/*!
 * \brief tree node for AVL tree
 */
struct cdsl_avlnode {
	union {
		struct base_tree_node base_node;
		struct {
			avltreeNode_t *left;
			avltreeNode_t *right;
			trkey_t key;
		};
	};
	int height;  ///< node depth within the tree
};

/*!
 * \brief Initialize tree root with balance factor
 * \param[in] rootp pointer to root of the tree
 * \param[in] bal balance factor for the AVL tree management
 */
extern void cdsl_avltreeRootInit(avltreeRoot_t* rootp, int bal);

/*!
 * \brief Initialize tree node with key value
 * \param[in] node uninitialized tree node
 * \param[in] key key value
 */
extern void cdsl_avltreeNodeInit(avltreeNode_t* node, trkey_t key);

/*!
 * \brief Insert element to tree
 * \param[in] rootp pointer of tree root
 * \param[in] item new tree node item to be inserted
 * \param[in] is_set define conflict handling mechanism, if true,
 *  	replacement performed if the element with same key value exists
 *  	within the tree, otherwise, added without replacement
*  \return if is_set is true, return replaced node otherwise, return null
 */
extern avltreeNode_t* cdsl_avltreeInsert(avltreeRoot_t* rootp, avltreeNode_t* item, BOOL is_set);

/*!
 * \brief Perform lookup operation and return node with given key, if there are multiple items with same key, the first encountered item during lookup operation will be returned
 * \param[in] rootp pointer of tree root
 * \param[in] key key value for the target node
 * \return found node (\ref avltreeNode_t) with given key
 */
extern avltreeNode_t* cdsl_avltreeLookup(avltreeRoot_t* rootp,trkey_t key);


/*!
 * \brief Try lookup node with given key & additional condition. if both the key and the condition match to node, the node will be return, otherwise null.
 *        this API is useful if there are nodes with same key value, then additional condition can be used to pick exact node.
 *        or traverse down the tree with given key and try to pick node with additional condition
 * \param[in] rootp pointer of tree root
 * \param[in] key key value for the target node
 * \param[in] match condition callback used to pick node
 * \return found node (\ref rbtreeNode_t) with given key
 */
extern avltreeNode_t* cdsl_avltreeConditionalLookup(avltreeRoot_t* rootp, trkey_t key, condition_t match);

/*!
 * \brief Delete item with or without replacement
 * \param[in] rootp pointer of tree root
 * \param[in] key key value for the target node
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 * \sa base_tree_replacer_t
 */
extern avltreeNode_t* cdsl_avltreeDeleteReplace(avltreeRoot_t* rootp,trkey_t key, base_tree_replacer_t replacer, void* cb_arg);

/*!
 * \brief Delete item with minimum key value within the tree
 * \param[in] rootp pointer of tree root
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 * \sa base_tree_replacer_t
 */
extern avltreeNode_t* cdsl_avltreeDeleteMinReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg);

/*!
 * \brief Delete item with maximum key value within the tree
 * \param[in] rootp pointer of tree root
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 * \sa base_tree_replacer_t
 */
extern avltreeNode_t* cdsl_avltreeDeleteMaxReplace(avltreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg);

/*!
 * @}
 */


#if defined(__cplusplus)
}
#endif

#endif /* CDSL_AVLTREE_H_ */
