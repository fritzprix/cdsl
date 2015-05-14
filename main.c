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

rb_treeNode_t rbpool[500];
rb_treeNode_t* rb_root;

int main(void){
	setbuf(stdout,NULL);
	cdsl_listInit(&listEntry);
	int i;
	rb_root = NULL;
	for(i = 0;i < 10;i++){
		people[i].age = rand() % 100 + 2;
		people[i].name = name[i];
		cdsl_listEnqueuePriority(&listEntry,&people[i],person_rule);
	}

	for(i = 0;i < 10;i++){
		person_t* p = (person_t*) cdsl_listDequeue(&listEntry);
		if(p)
			printf("Person name : %s age : %d\n",p->name,p->age);
		else
			printf("No more element\n");
	}


	for(i = 1;i < 300;i++){
		cdsl_rbtreeNodeInit(&rbpool[i],i);
		cdsl_rbtreeInsert(&rb_root,&rbpool[i]);
	}
	cdsl_rbtreePrint(&rb_root);

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
