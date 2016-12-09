#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "video_ss.h"

int main(int argc, char *argv[])
{
	int iLcdWidth;
	int iLcdHeight;
	int iLcdBpp;
	struct VideoBuf tFrameBuf;//最终刷入framebuf的数据

	struct DispOpr *pDispOpr;

	display_init();
	display_modules_init();

	pDispOpr = display_get_module("fb");
	GetDispResolution(pDispOpr, &iLcdWidth, &iLcdHeight, &iLcdBpp);
	printf("x(%d), y(%d), bpp(%d)\n", iLcdWidth, iLcdHeight, iLcdBpp);

	/* 设置framebuffer */
	GetVideoBufForDisplay(pDispOpr, &tFrameBuf);

	video_init();
	video_modules_init();

	return 0;
}
