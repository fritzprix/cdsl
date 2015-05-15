/*
 * main.c
 *
 *  Created on: 2015. 5. 10.
 *      Author: innocentevil
 */

#include <stdlib.h>
#include <stdio.h>
#include "cdsl_list.h"
#include "cdsl_rbtree.h"
#include "cdsl_bst.h"


char* name[] = {
		"james",
		"helga",
		"david",
		"john",
		"chloe",
		"richard",
		"frank",
		"tommy",
		"jim",
		"alice",
		"dave", "Latia Coryell",
		"Vivan Rathburn",
		"Windy Elsey",
		"Cheree Cagley",
		"Maryjane Souders",
		"Cuc Gale",
		"Mabel Melcher",
		"Roseanne Petillo",
		"Estell Brissette",
		"Katherina Cothren",
		"Corrine Meiners",
		"Danuta Wolford",
		"Zula Criado",
		"Ernest Hepfer",
		"Mitsue Bahl",
		"Kourtney Cianci",
		"Federico Meader",
		"Reuben Turk",
		"Tomas Sheridan" ,
		"Krishna Kawasaki"
};

static DECLARE_COMPARE_FN(person_rule);

typedef struct person {
	cdsl_dlistNode_t  ln;
	int age;
	char* name;
} person_t;


person_t people[10];
cdsl_dlistNode_t listEntry;

uint32_t rbpool[500];
rb_treeNode_t* rb_root;
bs_treeNode_t* bst_root;

int main(void){
	setbuf(stdout,NULL);
	cdsl_listInit(&listEntry);
	int i;
	rb_root = NULL;
	bst_root = NULL;


	rb_treeNode_t* rb_arry = (rb_treeNode_t*) &rbpool;
	for(i = 100;i > 1;i--){
		cdsl_rbtreeNodeInit(&rb_arry[i],1);
		cdsl_rbtreeInsert(&rb_root,&rb_arry[i]);
	}
	cdsl_rbtreePrint(&rb_root);


	bs_treeNode_t* bst_array = (bs_treeNode_t*) &rbpool;
	for(i = 0;i < 100;i++){
		cdsl_bstreeNodeInit(&bst_array[i],(rand() % 100 + 1));
		cdsl_bstreeInsert(&bst_root,&bst_array[i]);
	}
	cdsl_bstreePrint(&bst_root,NULL);
	return 0;
}


static DECLARE_COMPARE_FN(person_rule){
	if(!a || !b)
		return NULL;
	if(!a)
		return b;
	if(!b)
		return a;
	return ((person_t*)a)->age > ((person_t*) b)->age? a : b;
}
