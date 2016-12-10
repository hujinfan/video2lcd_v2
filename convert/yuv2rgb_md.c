#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "convert_ss.h"

static struct VideoConvert bModule;

static struct VideoConvert bModule = {
	.name = "yuv2rgb_name",
};

int Yuv2RgbInit(void)
{
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return convert_register(&bModule.list);
}
