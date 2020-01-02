/*
 * baremetal_test.c
 *
 *  Created on: Sep 25, 2016
 *      Author: innocentevil
 */

#include "target.h"
#include "baremetal_test.h"
#include "assert.h"

BOOL cdsl_baremetal_dep_test(void)
{
	if (STRCMP("Hello", "Hello"))
	{
		return FALSE;
	}

	if (STRCMP("Hello", "Hello World") > 0)
	{
		return FALSE;
	}
	if (STRCMP("Hello World", "Hello") < 0)
	{
		return FALSE;
	}

	return TRUE;
}
