/*
 * cdsl_hashtree.c
 *
 *  Created on: Apr 8, 2016
 *      Author: innocentevil
 */



#include "cdsl_hash.h"
#include "cdsl_hashtree.h"

#include "cdsl_rbtree.h"
#include "cdsl_slist.h"

#define REPLACER_ARG_INIT      {0,}

struct replacer_arg {
	const char* key_str;
	hashNode_t* target;
};


static int cdsl_hashtreeReplacer(hashNode_t** node, struct replacer_arg* rep_arg);


void cdsl_hashtreeNodeInit(hashNode_t* node, const char* key) {
	node->_str_key = key;
	cdsl_rbtreeNodeInit(&node->_base, sbdm_hash((unsigned char*) key));
	cdsl_slistEntryInit(&node->col_lentry);
}

void cdsl_hashtreeRootInit(hashRoot_t* root) {
	cdsl_rbtreeRootInit(&root->_base);
}


hashNode_t* cdsl_hashtreeInsert(hashRoot_t* root, hashNode_t* node) {
	// if collision happens, new item replace old one,
	// and old one returned as collision here
	hashNode_t* collision  = (hashNode_t* )cdsl_rbtreeInsert(&root->_base, &node->_base, TRUE);
	if(collision) {
		PRINT("collision\n");
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
	hashNode_t* node = (hashNode_t* )cdsl_rbtreeLookup(&root->_base,tkey);
	if(node) {
		node = container_of(node, hashNode_t, _base);
		if(!cdsl_slistIsEmpty(&node->col_lentry)) {
			listIter_t iter;
			cdsl_slistIterInit(&node->col_lentry, &iter);
			while(cdsl_slistIterHasNext(&iter)) {
				hashNode_t* collision = (hashNode_t*) cdsl_slistIterNext(&iter);
				collision = container_of(collision, hashNode_t, col_lentry);
				if(!STRCMP(collision->_str_key, key)) {
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
	struct replacer_arg rep_arg = REPLACER_ARG_INIT;
	rep_arg.key_str = key;
	hashNode_t* node = (hashNode_t* ) cdsl_rbtreeDeleteReplace(&root->_base, tkey, (base_tree_replacer_t) cdsl_hashtreeReplacer, &rep_arg);
	if(rep_arg.target) {
		return rep_arg.target;
	} else if (node) {
		node = container_of(node, hashNode_t, _base);
		return node;
	}
	return NULL;
}

static int cdsl_hashtreeReplacer(hashNode_t** node, struct replacer_arg* rep_arg) {
	if(!*node)
		return FALSE;
	listIter_t iter;
	hashNode_t* replace;
	if(!STRCMP((*node)->_str_key, rep_arg->key_str)) {
		// if entry node matches with key
		*node = (hashNode_t*) cdsl_slistDequeue(&(*node)->col_lentry);
		if(*node) {
			*node = container_of((*node), hashNode_t, col_lentry);
			return TRUE;     // replacement is not null, return true
		}
		return FALSE;    // otherwise, return false

	}

	// if list entry has not match to the key string,
	// start iterate list element to check key string


	cdsl_slistIterInit(&(*node)->col_lentry, &iter);   // initialize iterator
	while(cdsl_iterHasNext(&iter)) {
		replace = (hashNode_t*) cdsl_iterNext(&iter);
		replace = container_of(replace, hashNode_t, col_lentry);
		if(!STRCMP(replace->_str_key, rep_arg->key_str)) {
			cdsl_iterRemove(&iter);
			rep_arg->target = replace;
			return TRUE;
		}
	}
	// should be unreachable here
	PRINT_ERR("Key String not matched to any element!!");
	EXIT(-1);
	return TRUE;
}


