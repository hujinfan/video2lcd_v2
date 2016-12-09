#ifndef _display_H_
#define _display_H_

#include "klist.h"

typedef struct DispOpr {
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	int iLineWidth;
	unsigned char *pucDispMem;
	int (*DeviceInit)(struct DispOpr *pDispOpr);
	int (*CleanScreen)(unsigned int dwBackColor);
	int (*ShowPage)(void);

	struct list_head list;
}T_DispOpr, *PT_DispOpr;

#endif
