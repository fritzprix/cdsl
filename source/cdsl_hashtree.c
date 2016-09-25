/*
 * cdsl_hashtree.c
 *
 *  Created on: Apr 8, 2016
 *      Author: innocentevil
 */



#include "cdsl_hash.h"
#include "cdsl_nrbtree.h"
#include "cdsl_hashtree.h"
#include "cdsl_slist.h"


static int cdsl_hashtreeReplacer(hashNode_t** node);


void cdsl_hashtreeNodeInit(hashNode_t* node, const char* key) {
	node->_str_key = key;
	cdsl_nrbtreeNodeInit(&node->_base, sbdm_hash((unsigned char*) key));
	cdsl_slistEntryInit(&node->col_lentry);
}

void cdsl_hashtreeRootInit(hashRoot_t* root) {
	cdsl_nrbtreeRootInit(&root->_base);
}


hashNode_t* cdsl_hashtreeInsert(hashRoot_t* root, hashNode_t* node) {
	// if collision happens, new item replace old one,
	// and old one returned as collision here
	hashNode_t* collision  = (hashNode_t* )cdsl_nrbtreeInsert(&root->_base, &node->_base, TRUE);
	if(collision) {
		collision = container_of(collision, hashNode_t, _base);
		if(STRCMP(collision->_str_key, node->_str_key)) {
			// if key string of new item is different from collision, otherwise, discard collision
			cdsl_slistPutHead(&node->col_lentry, (slistNode_t*) &collision->col_lentry);   // put collision element in the collision list of new item node
			/*
			 *  |`````````````|       |``````````````````````|
			 *  |new item node|       |collision item node #1|
			 *  |`````````````|       |``````````````````````|
			 *  | right       |       | right                |
			 *  | left        |       | left                 |
			 *  | key         |       | key                  |
			 *  | str_key     |       | str_key              |
			 *  | col_lentry  |  ---> | col_lentry           |  ---> ......
			 *  ``````````````        ```````````````````````
			 */
		}

	}
	return node;
}

hashNode_t* cdsl_hashtreeLookup(hashRoot_t* root, const char* key) {
	trkey_t tkey = sbdm_hash((unsigned char*) key);
	hashNode_t* node = (hashNode_t* )cdsl_nrbtreeLookup(&root->_base,tkey);
	if(node) {
		node = container_of(node, hashNode_t, _base);
		if(!cdsl_slistIsEmpty(&node->col_lentry)) {
			listIter_t iter;
			cdsl_slistIterInit(&node->col_lentry, &iter);
			while(cdsl_slistIterHasNext(&iter)) {
				hashNode_t* collision = (hashNode_t*) cdsl_slistIterNext(&iter);
				collision = container_of(collision, hashNode_t, col_lentry);
				if(collision->_str_key == key) {
					return collision;
				}
			}
		}
		return node;
	}
	return NULL;
}

hashNode_t* cdsl_hashtreeRemove(hashRoot_t* root, const char* key) {
	trkey_t tkey = sbdm_hash((unsigned char*) key);
	hashNode_t* node = (hashNode_t* ) cdsl_nrbtreeDelete(&root->_base, tkey, (base_tree_replacer_t) cdsl_hashtreeReplacer);
	if(node) {
		node = container_of(node, hashNode_t, _base);
		return node;
	}
	return NULL;
}

hashNode_t* cdsl_hashtreeRemoveRandom(hashRoot_t* root) {
	hashNode_t* node = (hashNode_t*) cdsl_nrbtreeDeleteMin(&root->_base, (base_tree_replacer_t) cdsl_hashtreeReplacer);
	if(node) {
		node = container_of(node, hashNode_t, _base);
		return node;
	}
	return NULL;
}

static int cdsl_hashtreeReplacer(hashNode_t** node) {
	if(!*node)
		return FALSE;
	if((*node)->col_lentry.head) {
		*node = container_of((*node)->col_lentry.head, hashNode_t, col_lentry);
		return TRUE;
	}
	return FALSE;
}


