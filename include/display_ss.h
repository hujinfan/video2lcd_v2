#ifndef _display_H_
#define _display_H_

#include "klist.h"
#include <linux/fb.h>

typedef struct DispOpr {
	/* variables */
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	int iLineWidth;
	unsigned int dwScreenSize;
	unsigned char *pucFbMem;

	/* functions */
	int (*DeviceInit)(struct DispOpr *pDispOpr);
	int (*CleanScreen)(struct DispOpr *pDispOpr, unsigned int dwBackColor);
	int (*ShowPage)(void);

	struct fb_var_screeninfo fb_var;
	struct fb_fix_screeninfo fb_fix;

	struct list_head list;
}T_DispOpr, *PT_DispOpr;

#endif
