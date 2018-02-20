/*
 * base_tree.h
 *
 *  Created on: Nov 15, 2015
 *      Author: innocentevil
 */

#ifndef BASE_TREE_H_
#define BASE_TREE_H_

#include "arch.h"
#include "cdsl_defs.h"
#include "serializer.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * \addtogroup base-tree Base Tree
 * \brief Base Tree Implementation
 * @{
 */


#define ORDER_INC			(int) 0
#define ORDER_DEC			(int) 1

#define FOREACH_CONTINUE   (int) 0
#define FOREACH_BREAK      (int) 1

#define DECLARE_FOREACH_CALLBACK(fn) int fn(int order, base_treeNode_t* node,void* arg)

/*!
 * \def ORDER_INC
 * \brief incremental traversal order
 *
 * \def ORDER_DEC
 * \brief decremental traversal order
 *
 * \def TRAVERSE_OK
 * \brief Traversal control return value which allow traversal keep continue
 *
 * \def TRAVERSE_BREAK
 * \brief Traversal control return value which force traversal operation to stop
 *
 * \def DECLARE_TRAVERSE_CALLBACK(fn)
 * \brief Declare Traversal callback
 * \sa base_tree_callback_t
 */

/*!
 * \brief key type for tree
 */
typedef __cdsl_uaddr_t  trkey_t;

typedef struct base_tree_node base_treeNode_t;  ///< Base tree node type
typedef struct base_tree_root base_treeRoot_t;  ///< Base tree root type

typedef BOOL (*condition_t)(base_treeNode_t* node, trkey_t key);

/*!
 * \brief generic callback used in tree operation (for-each)
 * \param[in] order the order of given node, for example, if this callback is used with for_each incremental order, order 0 means the given node is smallest element
 * \param[in] node the node of tree element
 * \param[in] arg argument
 */

typedef int (*base_tree_callback_t)(int order, base_treeNode_t* node ,void* arg);

/*! replacer callback function type to override default hole resolution mechanism of the given tree
 *  replacer result shall be one among the three cases below
 *  1. return false, pointer updated or not -> this will perform default hole resolve mechanism of given tree structure
 *  2. return true, pointer updated -> this will override default hole resolve mechanism, simply saying replace hole with element of updated pointer
 *  3. return true, pointer is not updated -> this will be assumed, the element not found. nothing happen
 */
typedef int (*base_tree_replacer_t)(base_treeNode_t** node, void* ctx);

/*!
 * \brief Generic tree root structure which is used widely through all the tree family implementation
 */
struct base_tree_root {
	base_treeNode_t* entry; ///< entry item (root) of the tree
};

/*!
 * \brief Generic tree node structure used widely through all the tree family implementation
 */
struct base_tree_node {
	base_treeNode_t *left,   ///< left child node
	*right;                  ///< right child node
	trkey_t          key;    ///< key value of the node
};

extern void tree_deserialize(base_treeRoot_t* rootp,
		                     const cdsl_deserializer_t* deserializer,
							 const cdsl_alloc_t* malloc);


extern void tree_serialize(const base_treeRoot_t* rootp,
						   const cdsl_serializer_t* serializer,
						   const cdsl_serializerUsrCallback_t* callback);


/*!
 * \brief traverse tree
 * \param[in] rootp pointer to root of the tree
 * \param[in] cb callback invoked for each node for the tree
 * \param[in] order traversal order, either \ref ORDER_INC or \ref ORDER_DEC or \ref ORDER_DOWN
 * \param[in] arg argument passed when callback invoked
 * \sa ORDER_INC ORDER_DEC base_tree_callback_t
 */
extern void tree_for_each(base_treeRoot_t* rootp, base_tree_callback_t cb,int order, void* arg);

/*!
 * \brief traverse to the given target
 * \param[in] rootp pointer to root of the tree
 * \param[in] cb callback invoked for each node for the tree
 * \param[in] key key value for the target
 * \param[in] arg argument passed when callback invoked
 * \sa base_tree_callback_t
 */
extern void tree_for_each_to_target(base_treeRoot_t* rootp, base_tree_callback_t cb, trkey_t key,void* arg);

/*!
 * \brief Peek left child of given tree node
 * \param[in] cur pointer to tree node
 * \return left child of the given tree node
 */
extern base_treeNode_t* tree_go_left(base_treeNode_t* cur);

/*!
 * \brief Peek right child of given tree node
 * \param[in] cur pointer to tree node
 * \return right child of the given tree node
 */
extern base_treeNode_t* tree_go_right(base_treeNode_t* cur);

/*!
 * \brief Peek top item of the tree
 * \param[in] rootp pointer to root of the tree
 * \return top node of the tree
 */
extern base_treeNode_t* tree_top(base_treeRoot_t* rootp);

/*!
 * \brief Get total item count of the tree
 * \param[in] rootp pointer to root of the tree
 * \return total item count of the tree
 */
extern int tree_size(base_treeRoot_t* rootp);

/*!
 * \brief Print whole tree nodes
 * \param[in] rootp pointer to root of the tree
 * \param[in] prt Printer function
 */
extern void tree_print(base_treeRoot_t* rootp,cdsl_generic_printer_t prt);

/*!
 * \brief Get max depth of the tree
 * \param[in] rootp pointer to root of the tree
 * \return max. depth of the tree
 */
extern int tree_max_depth(base_treeRoot_t* rootp);

/*!
 * \brief Check whether the tree is empty or not
 * \return true if tree has no item, otherwise false
 */
extern BOOL tree_is_empty(base_treeRoot_t* rootp);

/*!
 * \brief Get node with minimum key value among the all items of the tree
 * \param[in] rootp pointer to root of the tree
 * \return tree node with minimum key value
 */
extern base_treeNode_t* tree_min(base_treeRoot_t* rootp);

/*!
 * \brief Get node with maximum key value among the all items of the tree
 * \param[in] rootp pointer to root of the tree
 * \return tree node with maximum key value
 */
extern base_treeNode_t* tree_max(base_treeRoot_t* rootp);

/*!
 * \brief Replace item with new one
 * \param[in] rootp pointer to root of the tree
 * \param[in] nitem new item
 * \return the item replaced by new item
 */
extern base_treeNode_t* tree_update(base_treeRoot_t* rootp, base_treeNode_t* nitem);

/*!
 * @}
 */


#if defined(__cplusplus)
}
#endif

#endif /* BASE_TREE_H_ */
