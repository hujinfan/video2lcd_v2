#ifndef _SUBSYSTEM_H_
#define _SUBSYSTEM_H_

#include "klist.h"

struct SUBSYSTEM_module {
	/* variables */
	char name[20];
	int var;

	/* functions */
	void (*printInfo)(const char *name);
	void (*setVar)(int in);
	void (*getVar)(int *out);

	struct list_head list;
};

#endif
