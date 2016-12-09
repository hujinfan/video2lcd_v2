#ifndef _PROJECT_COMMON_H_
#define _PROJECT_COMMON_H_

#include "klist.h"
LIST_HEAD(person_list);

struct person
{
	char name[20];
	int number;
	struct list_head list;
};

#endif
