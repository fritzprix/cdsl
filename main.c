/*
 * main.c
 *
 *  Created on: 2015. 5. 10.
 *      Author: innocentevil
 */
#include <stdlib.h>
#include <stdio.h>

const char* RESULT_STRING[] = {
		"PASS",
		"FAIL"
};

int main(void){
	setbuf(stdout,NULL);
	if(cdsl_bstreeDoTest()){
		printf("Binary Search Tree Test Result : %s\n",RESULT_STRING[0]);
	}else{
		printf("Binary Search Tree Test Result : %s\n",RESULT_STRING[1]);
		return 1;
	}
	return 0;
}

