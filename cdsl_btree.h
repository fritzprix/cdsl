/*
 * tch_btree.h
 *
 *  Created on: 2014. 9. 12.
 *      Author: innocentevil
 */

#ifndef TCH_BTREE_H_
#define TCH_BTREE_H_

#if defined(__cplusplus)
extern "C"{
#endif

#define INIT_BTREE(key)                      {NULL,NULL,key}

typedef struct cdsl_btree_node cdsl_bstNode_t;

struct cdsl_btree_node {
	cdsl_bstNode_t* right;
	cdsl_bstNode_t* left;
	int key;
};



extern void cdsl_btreeInit(cdsl_bstNode_t* node,int key);
extern cdsl_bstNode_t* cdsl_btree_insert(cdsl_bstNode_t** root,cdsl_bstNode_t* item);
extern cdsl_bstNode_t* cdsl_btree_lookup(cdsl_bstNode_t* root,int key);
extern cdsl_bstNode_t* cdsl_btree_delete(cdsl_bstNode_t** root,int key);
extern int cdsl_btree_size(cdsl_bstNode_t* root);


#if defined(__cplusplus)
}
#endif


#endif /* TCH_BTREE_H_ */
