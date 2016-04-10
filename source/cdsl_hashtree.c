/*
 * cdsl_hashtree.c
 *
 *  Created on: Apr 8, 2016
 *      Author: innocentevil
 */



#include "cdsl_hash.h"
#include "cdsl_nrbtree.h"
#include "cdsl_hashtree.h"


void cdsl_hashtreeNodeInit(hashNode_t* node, const char* key) {
	node->_str_key = key;
	cdsl_nrbtreeNodeInit(&node->_base, sbdm_hash((unsigned char*) key));
}

void cdsl_hashtreeRootInit(hashRoot_t* root) {
	cdsl_nrbtreeRootInit(&root->_base);
}

hashNode_t* cdsl_hashtreeInsert(hashRoot_t* root, hashNode_t* node) {
	node = (hashNode_t* )cdsl_nrbtreeInsert(&root->_base, &node->_base);
	if(node) {
		node = container_of(node, hashNode_t, _base);
	}
	return node;
}

hashNode_t* cdsl_hashtreeLookup(hashRoot_t* root, const char* key) {
	trkey_t tkey = sbdm_hash((unsigned char*) key);
	hashNode_t* node = (hashNode_t* )cdsl_nrbtreeLookup(&root->_base,tkey);
	if(node) {
		node = container_of(node, hashNode_t, _base);
	}
	return node;
}

hashNode_t* cdsl_hashtreeRemove(hashRoot_t* root, const char* key) {
	trkey_t tkey = sbdm_hash((unsigned char*) key);
	hashNode_t* node = (hashNode_t* ) cdsl_nrbtreeDelete(&root->_base, tkey);
	if(node) {
		node = container_of(node, hashNode_t, _base);
	}
	return node;
}

hashNode_t* cdsl_hashtreeRemoveRandom(hashRoot_t* root) {
	hashNode_t* node = (hashNode_t*) cdsl_nrbtreeDeleteMin(&root->_base);
	if(node) {
		node = container_of(node, hashNode_t, _base);
	}
	return node;
}

