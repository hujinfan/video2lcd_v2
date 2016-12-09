#ifndef _display_H_
#define _display_H_

#include "klist.h"

typedef struct DispOpr {
	char *name;
	int (*DeviceInit)(void);
	int (*CleanScreen)(void);
	int (*ShowPage)(void);

	struct list_head list;
}T_DispOpr, *PT_DispOpr;

#endif
