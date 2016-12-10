#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "video_ss.h"

extern struct DispOpr *display_get_module(const char *name);
extern struct VideoOpr *video_get_module(const char *name);

int main(int argc, char *argv[])
{
	int iLcdWidth;
	int iLcdHeight;
	int iLcdBpp;
	int iPixelFormatOfDisp;
	int iPixelFormatOfVideo;
	struct VideoBuf tFrameBuf;//最终刷入framebuf的数据
	struct VideoDevice tVideoDevice;

	struct DispOpr *pDispOpr;
	struct VideoOpr *pVideoOpr;

	display_init();
	display_modules_init();

	pDispOpr = display_get_module("fb");
	GetDispResolution(pDispOpr, &iLcdWidth, &iLcdHeight, &iLcdBpp);
	printf("x(%d), y(%d), bpp(%d)\n", iLcdWidth, iLcdHeight, iLcdBpp);

	/* 设置framebuffer */
	GetVideoBufForDisplay(pDispOpr, &tFrameBuf);
	iPixelFormatOfDisp = tFrameBuf.iPixelFormat;

	video_init();
	pVideoOpr = video_get_module("v4l2_name");
	video_modules_init(pVideoOpr, &tVideoDevice);
	iPixelFormatOfVideo = tVideoDevice.iPixelFormat;

	ShowVideoOpr();

	return 0;
}
