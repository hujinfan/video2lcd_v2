#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "video_manager.h"

int main(int argc, char *argv[])
{
	int iLcdWidth;
	int iLcdHeight;
	int iLcdBpp;
	struct VideoBuf tFrameBuf;//最终刷入framebuf的数据

	struct DispOpr *pDispOpr;

	display_init();
	DispOprs_init();

	pDispOpr = display_get_module("fb");
	GetDispResolution(pDispOpr, &iLcdWidth, &iLcdHeight, &iLcdBpp);
	printf("x(%d), y(%d), bpp(%d)\n", iLcdWidth, iLcdHeight, iLcdBpp);

	/* 设置framebuffer */
	GetVideoBufForDisplay(pDispOpr, &tFrameBuf);

	return 0;
}
