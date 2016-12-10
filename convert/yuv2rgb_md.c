#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "convert_ss.h"

/* 对输入和输出的格式判断下 */
static int isSupportYuv2Rgb(int iPixelFormatIn, int iPixelFormatOut)
{
	printf("try YUV2RGB\n");
	if (iPixelFormatIn != V4L2_PIX_FMT_YUYV)
	{
		printf("YUV is not support!!!!\n");
		return 0;
	}
	if ( (iPixelFormatOut != V4L2_PIX_FMT_RGB565) && (iPixelFormatOut != V4L2_PIX_FMT_RGB32) )
	return 0;

	printf("Yuv support\n");
	return 1;
}

static struct VideoConvert bModule = {
	.name = "yuv2rgb_name",
	.isSupport = isSupportYuv2Rgb,
};

int Yuv2RgbInit(void)
{
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return convert_register(&bModule.list);
}
