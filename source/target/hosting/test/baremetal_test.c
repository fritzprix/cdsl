/*
 * baremetal_test.c
 *
 *  Created on: Sep 25, 2016
 *      Author: innocentevil
 */

#include "baremetal.h"
#include "baremetal_test.h"
#include "assert.h"

BOOL cdsl_baremetal_dep_test(void) {
	if(baremetal_strcmp("Hello", "Hello")) {
		return FALSE;
	}

	if(baremetal_strcmp("Hello", "Hello World") > 0) {
		return FALSE;
	}
	if(baremetal_strcmp("Hello World", "Hello") < 0) {
		return FALSE;
	}

	return TRUE;
}
