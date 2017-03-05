/*
 * cdsl_nrbtree.h
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#ifndef INCLUDE_CDSL_NRBTREE_H_
#define INCLUDE_CDSL_NRBTREE_H_

#include "base_tree.h"

#ifdef __cplusplus
extern "C" {
#endif


/*!
 * \addtogroup red-black-tree Red-Black Tree
 * \brief Red-black Tree (Recursive Version)
 * @{
 */

#define cdsl_nrbtreeMaxDepth(root)                      tree_max_depth((base_treeRoot_t*) root)
#define cdsl_nrbtreeTraverse(root, cb, order, arg)      tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, order, arg)
#define cdsl_nrbtreeTraverseTarget(root,cb,key, arg)    tree_traverse((base_treeRoot_t*) root, (base_tree_callback_t) cb, key, arg)
#define cdsl_nrbtreeSize(root)                          tree_size((base_treeRoot_t*) root)
#define cdsl_nrbtreePrint(root, print)                  tree_print((base_treeRoot_t*) root, print)
#define cdsl_nrbtreeIsEmpty(root)                       tree_is_empty((base_treeRoot_t*) root)
#define cdsl_nrbtreeTop(root)                           (nrbtreeNode_t*) tree_top((base_treeRoot_t*) root)
#define cdsl_nrbtreeGoLeft(cur)                         (nrbtreeNode_t*) tree_go_left((base_treeNode_t*) cur)
#define cdsl_nrbtreeGoRight(cur)                        (nrbtreeNode_t*) tree_go_right((base_treeNode_t*) cur)
#define cdsl_nrbtreeMin(root)                           (nrbtreeNode_t*) tree_min((base_treeRoot_t*) root)
#define cdsl_nrbtreeMax(root)                           (nrbtreeNode_t*) tree_max((base_treeRoot_t*) root)
#define cdsl_nrbtreeUpdate(root,nitem)                  (nrbtreeNode_t*) tree_update((base_treeRoot_t*) root, (base_treeNode_t*) nitem)
#define cdsl_nrbtreeDelete(root, key)                   cdsl_nrbtreeDeleteReplace(root, key, NULL, NULL)
#define cdsl_nrbtreeDeleteMin(root)                     cdsl_nrbtreeDeleteMinReplace(root, NULL, NULL)
#define cdsl_nrbtreeDeleteMax(root)                     cdsl_nrbtreeDeleteMaxReplace(root, NULL, NULL)


/*!
 * \def cdsl_nrbtreePrint_dev(root)
 * \brief Print tree structure
 * \param[in] root pointer to the root of tree
 *
 * \def cdsl_nrbtreeMaxDepth(root)
 * \brief Get max depth of the tree

 * \param[in] root pointer to the root of tree
 * \return max. depth of the tree

 *
 *
 * \def cdsl_nrbtreeTraverse(root, cb, order, arg)
 * \brief Traverse all the items within the tree, cb is callback function

 * \param[in] root pointer to the root of tree
 * \param[in] cb callback function called for each item of the tree
 * \param[in] order incremental / decremental traversal order
 * \param[in] arg argument which passed as argument when callback is invoked
 *
 *
 * \def cdsl_nrbtreeTraverseTarget(root,cb,key, arg)
 * \brief Traverse all the parent item to the target node with given key value
 * \param[in] root pointer to the root of tree
 * \param[in] cb callback function called for each item of the tree
 * \param[in] key key value for the target
 * \param[in] arg argument which passed as argument when callback is invoked

 *
 * \def cdsl_nrbtreeSize(root)
 * \brief Get total item count of the tree
 * \param[in] root pointer to the root of tree
 * \return total item count of the tree
 *
 * \def cdsl_nrbtreePrint(root, print)
 * \brief Print root structure of tree
 * \param[in] root pointer to the root of tree
 * \param[in] print printer callback which called with arg

 *
 * \def  cdsl_nrbtreeIsEmpty(root)
 * \brief Check whether the tree is empty or not
 * \param[in] root pointer to the root of tree
 * \return return true, if there is no item on the tree, otherwise return false
 *
 * \def cdsl_nrbtreeTop(root)
 * \brief Get root item of the tree (not very meaningful except for heap)
 * \param[in] root pointer to the root of tree
 * \return top (entry) item of the tree
 *
 * \def cdsl_nrbtreeGoLeft(cur)
 * \brief Peek left child of current item
 * \param[in] cur pointer to tree node
 * \return left child of the given tree node
 *
 * \def cdsl_nrbtreeGoRight(cur)
 * \brief Peek right child of current item
 * \param[in] cur pointer to tree node
 * \return right child of the given tree node
 *
 * \def cdsl_nrbtreeMin(root)
 * \brief Peek item with minimum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with minimum key value
 *
 * \def cdsl_nrbtreeMax(root)
 * \brief Peek item with maximum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with maximum key value
 *
 * \def cdsl_nrbtreeUpdate(root,nitem)
 * \brief Replace item with new one with same key value
 * \param[in] root pointer to tree root
 * \param[in] nitem new item
 * \return old item replaced by nitem
 *
 * \def cdsl_nrbtreeDelete(root, key)
 * \brief Delete item with the key
 * \param[in] root pointer to tree root
 * \param[in] key value for item to be deleted
 * \return deleted item
 *
 * \def cdsl_nrbtreeDeleteMin(root)
 * \brief Delete item with minimum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with minimum key value
 *
 * \def cdsl_nrbtreeDeleteMax(root)
 * \brief Delete item with maximum key value among the items in the tree
 * \param[in] root pointer to tree root
 * \return item with maximum key value

 */

/*!
 * \brief red-black node type
 */
typedef struct cdsl_nrbtree nrbtreeNode_t;
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
		nrbtreeNode_t* entry;
	};
}nrbtreeRoot_t;

/*!
 * \brief node of red-black tree
 */
struct cdsl_nrbtree {
	union {
		struct base_tree_node 	base_node;
		struct {
			nrbtreeNode_t* left;
			nrbtreeNode_t* right;
			trkey_t          key;
		};

	};
};


/*!
 * \fn void cdsl_nrbtreeRootInit(nrbtreeRoot_t* rootp)
 * \brief Initialize red-black tree root
 * \param[in] rootp pointer to uninitialized \ref nrbtreeRoot_t
 */
extern void cdsl_nrbtreeRootInit(nrbtreeRoot_t* rootp);
/*!
 * \fn void cdsl_nrbtreeNodeInit(nrbtreeNode_t* node, trkey_t key)
 * \brief Initialize red-black tree node
 * \param[in] node pointer to uninitialized \ref nrbtreeNode_t
 * \param[in] key key value for the node
 */
extern void cdsl_nrbtreeNodeInit(nrbtreeNode_t* node, trkey_t key);

/*!\fn nrbtreeNode_t* cdsl_nrbtreeInsert(nrbtreeRoot_t* rootp,nrbtreeNode_t* item, BOOL is_set)
 * \brief Insert element to tree
 * \param[in] rootp pointer of tree root
 * \param[in] item new tree node item to be inserted
 * \param[in] is_set define conflict handling mechanism, if true,
 *  	replacement performed if the element with same key value exists
 *  	within the tree, otherwise, added without replacement
*  \return if is_set is true, return replaced node otherwise, return null
 */
extern nrbtreeNode_t* cdsl_nrbtreeInsert(nrbtreeRoot_t* rootp,nrbtreeNode_t* item, BOOL is_set);

/*!
 * \brief Perform lookup operation and return node with given key, if there are multiple items with same key, the first encountered item during lookup operation will be returned
 * \param[in] rootp pointer of tree root
 * \param[in] key key value for the target node
 * \return found node (\ref nrbtreeNode_t) with given key
 */
extern nrbtreeNode_t* cdsl_nrbtreeLookup(nrbtreeRoot_t* rootp,trkey_t key);

/*!
 * \brief Delete item with or without replacement
 * \param[in] rootp pointer of tree root
 * \param[in] key key value for the target node
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 */
extern nrbtreeNode_t* cdsl_nrbtreeDeleteReplace(nrbtreeRoot_t* rootp,trkey_t key, base_tree_replacer_t replacer, void* cb_arg);

/*!
 * \brief Delete item with minimum key value within the tree
 * \param[in] rootp pointer of tree root
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 */
extern nrbtreeNode_t* cdsl_nrbtreeDeleteMinReplace(nrbtreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg);
/*!
 * \brief Delete item with maximum key value within the tree
 * \param[in] rootp pointer of tree root
 * \param[in] replacer callback function called when the item to be deleted is found
 * \param[in] cb_arg callback argument for replacer
 * \return deleted item
 */
extern nrbtreeNode_t* cdsl_nrbtreeDeleteMaxReplace(nrbtreeRoot_t* rootp, base_tree_replacer_t replacer, void* cb_arg);


#ifdef __DBG
extern void cdsl_nrbtreePrint_dev(nrbtreeRoot_t* root);
#else
#define cdsl_nrbtreePrint_dev(root)
#endif

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CDSL_NRBTREE_H_ */
