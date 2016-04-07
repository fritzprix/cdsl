/*
 * nrbtree_benchmark.c
 *
 *  Created on: Apr 7, 2016
 *      Author: innocentevil
 */

#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <ctime>

#include "base_tree.h"
#include "cdsl_nrbtree.h"
#include "nrbtree_benchmark.h"


#define TEST_CNT  2000000
#define TEST_LOOP 10

typedef struct {
	nrbtreeNode_t node;
	int val;
} data_t;



bool perform_nrbtree_benchmark(void){
	std::map<trkey_t,data_t*> rbtree;
	trkey_t key = 0;
	clock_t start,end;
	data_t *d;
	nrbtreeRoot_t root;
	int loop = 0;
	cdsl_nrbtreeRootInit(&root);
	double insert_avg = 0;
	double lookup_avg = 0;
	double remove_avg = 0;

	for(loop = 0;loop < TEST_LOOP;loop++)
	{
		start = clock();
		for(key = 0;key < TEST_CNT;key++) {
			d = new data_t();
			d->node.key = key;
			rbtree.insert(std::pair<trkey_t,data_t*>(key,d));
		}
		end = clock();
		insert_avg += (double)((end - start)) / CLOCKS_PER_SEC;


		start = clock();
		for(key = 0;key < TEST_CNT;key++) {
			if(rbtree.find(key)->second->node.key != key)
				return false;
		}
		end = clock();
		lookup_avg += (double)((end - start)) / CLOCKS_PER_SEC;

		start = clock();
		for(key = 0;key < TEST_CNT;key++) {
			if(rbtree.erase(key) == 0)
				return false;
		}
		end = clock();
		remove_avg += (double)((end - start)) / CLOCKS_PER_SEC;

	}
	insert_avg /= TEST_LOOP;
	lookup_avg /= TEST_LOOP;
	remove_avg /= TEST_LOOP;

	printf("avg.insert test for map : %f\n",insert_avg);
	printf("avg.lookup test for map : %f\n",lookup_avg);
	printf("avg.delete test for map : %f\n",remove_avg);

	insert_avg = 0;
	lookup_avg = 0;
	remove_avg = 0;


	for(loop = 0;loop < TEST_LOOP;loop++) {
		start = clock();
		for(key = 0;key < TEST_CNT;key++) {
			d = new data_t();
			cdsl_nrbtreeNodeInit(&d->node, key);
			cdsl_nrbtreeInsert(&root, &d->node);
		}
		end = clock();
		insert_avg += (double)((end - start)) / CLOCKS_PER_SEC;

		start = clock();
		for(key = 0;key < TEST_CNT;key++) {
			if(((data_t*) cdsl_nrbtreeLookup(&root,key))->node.key != key)
				return false;
		}
		end = clock();
		lookup_avg += (double)((end - start)) / CLOCKS_PER_SEC;

		start = clock();
		for(key = 0;key < TEST_CNT;key++) {
			d = (data_t*) cdsl_nrbtreeDelete(&root,key);
			if(!d)
				return false;
			delete d;
		}
		end = clock();
		remove_avg += (double)((end - start)) / CLOCKS_PER_SEC;
	}

	insert_avg /= TEST_LOOP;
	lookup_avg /= TEST_LOOP;
	remove_avg /= TEST_LOOP;

	printf("avg.insert test for rbtree : %f\n",insert_avg);
	printf("avg.lookup test for rbtree : %f\n",lookup_avg);
	printf("avg.delete test for rbtree : %f\n",remove_avg);


	return true;
}
