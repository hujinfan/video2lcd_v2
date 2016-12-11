#ifndef _display_H_
#define _display_H_

#include "klist.h"
#include <linux/fb.h>
#include "video_ss.h"

typedef struct DispOpr {
	/* variables */
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	int iLineWidth;

	/* 显示屏大小*/
	unsigned int dwScreenSize;

	/* 显存地址 */
	unsigned char *pucFbMem;

	/* functions */
	int (*DeviceInit)(struct DispOpr *pDispOpr);
	int (*CleanScreen)(struct DispOpr *pDispOpr, unsigned int dwBackColor);
	int (*ShowPage)(void);

	struct fb_var_screeninfo fb_var;
	struct fb_fix_screeninfo fb_fix;

	struct list_head list;
}T_DispOpr, *PT_DispOpr;

int display_register(struct list_head *list);
int fb_init(void);


/* call for app */
int display_init(void);
int GetDispResolution(struct DispOpr *pDispOpr, int *piXres, int *piYres, int *piBpp);
int GetVideoBufForDisplay(struct DispOpr *pDispOpr, struct VideoBuf *ptFrameBuf);
struct DispOpr *display_get_module(const char *name);
void display_modules_init(void);
#endif
