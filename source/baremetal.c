
#include "baremetal.h"


int baremetal_strcmp(const char* s1, const char* s2) {
	const char* c1 = s1,*c2 = s2;
	while((*c1 != '\0') && (*c2 != '\0')) {
		if(*(c1++) != *(c2++)) {
			return *c1 - *c2;
		}
	}
	return *c1 -*c2;
}

