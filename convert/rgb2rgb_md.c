#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "convert_ss.h"

static int isSupportRgb2Rgb(int iPixelFormatIn, int iPixelFormatOut)
{
    if (iPixelFormatIn != V4L2_PIX_FMT_RGB565)
        return 0;
    if ((iPixelFormatOut != V4L2_PIX_FMT_RGB565) && (iPixelFormatOut != V4L2_PIX_FMT_RGB32))
    {
        return 0;
    }
    return 1;
}

static struct VideoConvert bModule = {
	.name = "rgb2rgb_name",
    .isSupport   = isSupportRgb2Rgb,
};

int Rgb2RgbInit(void)
{
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return convert_register(&bModule.list);
}
