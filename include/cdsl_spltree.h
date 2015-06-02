/*
 * cdsl_spltree.h
 *
 *  Created on: 2015. 5. 14.
 *      Author: innocentevil
 */

#ifndef CDSL_SPLTREE_H_
#define CDSL_SPLTREE_H_

#if defined(__cplusplus)
extern "C" {
#endif


typedef struct spltree_node spltreeNode_t;
struct spltree_node {
	spltreeNode_t* left,*right;
	int key;
};

extern void cdsl_spltreeInitNode(spltreeNode_t* node,int key);
extern void cdsl_spltreeInsert(spltreeNode_t** root,spltreeNode_t* item);
extern spltreeNode_t* cdsl_spltreeLookup(spltreeNode_t** root,int key,BOOL splay);
extern spltreeNode_t* cdsl_spltreeLookupLargest(spltreeNode_t** root,BOOL splay);
extern spltreeNode_t* cdsl_spltreeLookupSmallest(spltreeNode_t** root,BOOL splay);
extern spltreeNode_t* cdsl_spltreeDelete(spltreeNode_t** root,int key);
extern void cdsl_spltreePrint(spltreeNode_t** root);



#if defined(__cplusplus)
}
#endif


#endif /* CDSL_SPLTREE_H_ */
