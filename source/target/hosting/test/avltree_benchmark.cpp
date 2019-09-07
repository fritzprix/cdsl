/*
 * avltree_benchmark.cpp
 *
 *  Created on: 2017. 3. 29.
 *      Author: innocentevil
 */

#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "base_tree.h"
#include "cdsl_avltree.h"
#include "avltree_benchmark.h"


#define TEST_CNT  2000000
#define TEST_LOOP 10

typedef struct {
	avltreeNode_t node;
	int val;
} data_t;

bool perform_avltree_benchmark(void) {
	double insert_avg = 0;
	double lookup_avg = 0;
	double remove_avg = 0;
	int loop = 0;
	avltreeRoot_t root;
	clock_t start, end;
	data_t* dp;
	trkey_t key = 0;
	cdsl_avltreeRootInit(&root, 1);
	for (loop = 0; loop < TEST_LOOP; loop++) {
		start = clock();
		for (key = 0; key < TEST_CNT; key++) {
			dp = new data_t();
			cdsl_avltreeNodeInit(&dp->node, key);
			cdsl_avltreeInsert(&root, &dp->node, FALSE);
		}
		end = clock();
		insert_avg += (double) ((end - start)) / CLOCKS_PER_SEC;

		start = clock();
		for (key = 0; key < TEST_CNT; key++) {
			if (((data_t*) cdsl_avltreeLookup(&root, key))->node.key != key)
				return false;
		}
		end = clock();
		lookup_avg += (double) ((end - start)) / CLOCKS_PER_SEC;

		start = clock();
		for (key = 0; key < TEST_CNT; key++) {
			dp = (data_t*) cdsl_avltreeDelete(&root, key);
			if (!dp)
				return false;
			delete dp;
		}
		end = clock();
		remove_avg += (double) ((end - start)) / CLOCKS_PER_SEC;
	}

	insert_avg /= TEST_LOOP;
	lookup_avg /= TEST_LOOP;
	remove_avg /= TEST_LOOP;

	printf("avg.insert test for avltree : %f\n",insert_avg);
	printf("avg.lookup test for avltree : %f\n",lookup_avg);
	printf("avg.delete test for avltree : %f\n",remove_avg);
	return true;
}
