#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "video_ss.h"

extern struct DispOpr *display_get_module(const char *name);
extern struct VideoOpr *video_get_module(const char *name);
extern struct VideoConvert *GetVideoConvertForFormats(int iPixelFormatIn, int iPixelFormatOut);
extern int video_convert2rgb(struct VideoConvert *pModule, struct VideoBuf *ptVideoBufIn, struct VideoBuf *ptVideoBufOut);

int main(int argc, char *argv[])
{
	int i, j;

	/*
	 * 分配两块内存区域用于临时存放视频数据
	 * 因为一个像素点用16bpp表示
	 * 所以数据类型用short
	 * 摄像头采集到的数据是320x240的,放入mem320x240
	 * LCD显示器的尺寸是240x320
	 * 把mem320x240里的数据放入mem240x320
	 * 最后把mem240x320放入到framebuffer
	 */

	/*
	 *
	 * mem320x240---->	-----------320------------>x
	 * 					|         |
	 * 					|         |
	 * 					240       |
	 * 					|---------p(x, y)
	 * 					|
	 * 					V
	 * 					y
	 */

	/*
	 *
	 * mem240x320---->	------240----->x
	 * 					|    |
	 * 					|    |
	 * 					|----p(y, 320 - x)
	 * 					|
	 * 					320
	 * 					|
	 * 					|
	 * 					|
	 * 					|
	 * 					V
	 * 					y
	 */
	unsigned short mem320x240[320][240];
	unsigned short mem240x320[240][320];

	/* 用于操作每一个像素点 */
	unsigned short *s = NULL;
	unsigned short *d = NULL;;

	int iError;
	int iLcdWidth;
	int iLcdHeight;
	int iLcdBpp;
	int iPixelFormatOfDisp;
	int iPixelFormatOfVideo;
	struct VideoDevice tVideoDevice;
	struct VideoConvert *ptVideoConvert;

	struct DispOpr *pDispOpr;
	struct VideoOpr *pVideoOpr;

	struct VideoBuf	*ptVideoBufCur;
	struct VideoBuf tVideoBuf;//摄像头采集到的数据
	struct VideoBuf tConvertBuf;//转换后的数据
	struct VideoBuf tZoomBuf;//缩放后的数据
	struct VideoBuf tFrameBuf;//最终刷入framebuf的数据

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


	/* 初始化视频管理模块 */
	VideoConvertInit();
	ShowVideoConvert();

	/* 根据采集到的视频数据格式选取一个合适的转换函数 */
	ptVideoConvert = GetVideoConvertForFormats(tVideoDevice.iPixelFormat, iPixelFormatOfDisp);//这里暂时写24因为LCD的代码还没写
	if (NULL == ptVideoConvert)
	{
		printf("can not support this format convert\n");
		return -1;
	}

	/* 启动摄像头 */
	iError = tVideoDevice.ptVideoOpr->StartDevice(&tVideoDevice);
	if (iError)
	{
		printf("StartDevice %s error!!\n", argv[1]);
		return -1;
	}

	/* 分配视频数据缓冲区 */
	/* 1. 给直接通过摄像头采集到的数据使用 */
	memset(&tVideoBuf, 0, sizeof(T_VideoBuf));

	/* 2. 转化后的数据区 */
	memset(&tConvertBuf, 0, sizeof(T_VideoBuf));
	tConvertBuf.iPixelFormat = iPixelFormatOfDisp;
	tConvertBuf.tPixelDatas.iBpp = iLcdBpp;

	/* 3. 缩放后的数据区 */
	memset(&tZoomBuf, 0, sizeof(T_VideoBuf));

	/* 从摄像头读出数据后处理 */
	while (1)
	{
		/* 1. 读摄像头数据 */
		iError = tVideoDevice.ptVideoOpr->GetFrame(&tVideoDevice, &tVideoBuf);
		if (iError)
		{
			printf("####get frame ERROR####\n");
			return -1;
		}

		/* 保存当前视频数据的地址 */
		ptVideoBufCur = &tVideoBuf;

		/* 2. 判断是否需要转换为RGB */
		if (iPixelFormatOfVideo != iPixelFormatOfDisp)
		{
//			iError = ptVideoConvert->Convert(&tVideoBuf, &tConvertBuf);
			iError = video_convert2rgb(ptVideoConvert, &tVideoBuf, &tConvertBuf);
			if (iError)
			{
				printf("Convert error\n");
				return -1;
			}

			/* 设置当前数据指针 */
			ptVideoBufCur = &tConvertBuf;
		}

		/* 操作源数据 */
		s = (unsigned short *)ptVideoBufCur->tPixelDatas.aucPixelDatas;

		/* 操作framebuffer */
		d = (unsigned short *)tFrameBuf.tPixelDatas.aucPixelDatas;

		/* 把摄像头采集的数据放入mem320x240*/
		for (i = 0; i < 240; i++)
			for (j = 0; j < 320; j++)
				mem320x240[j][i] = *s++;

		/* 把mem320x240里的数据转存到mem240x320 */
		for (i = 0; i < 240; i++)
			for (j = 0; j < 320; j++)
				mem240x320[i][320 - j] = mem320x240[j][i];

		for (i = 0; i < 320; i++)
			for (j = 0; j < 240; j++)
				*d++ = mem240x320[j][i];

		/* 释放该帧数据,重新放入采集视频的队列 */
		iError = tVideoDevice.ptVideoOpr->PutFrame(&tVideoDevice, &tVideoBuf);
		if (iError)
		{
			printf("Put frame error\n");
			return -1;
		}
	}
	return 0;
}
