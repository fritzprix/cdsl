/*
 * main.c
 *
 *  Created on: 2015. 5. 10.
 *      Author: innocentevil
 */


#include <stdio.h>

const char* RESULT_STRING[] = {
		"FAIL",
		"PASS"
};

int main(void){
	setbuf(stdout,NULL);
	printf("Binary Search Tree Test Result : %s\n",RESULT_STRING[cdsl_bstreeDoTest()]);
	printf("Splay tree Test Result : %s\n",RESULT_STRING[cdsl_spltreeDoTest()]);
	printf("Red Black Tree Test Result : %s\n",RESULT_STRING[cdsl_rbtreeDoTest()]);
//	printf("Singly-Linked List Test Result : %s\n",RESULT_STRING[cdsl_slistDoTest()]);
	return 0;
}

