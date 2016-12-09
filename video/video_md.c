#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "video_ss.h"

static int V4l2DeviceInit(void)
{
	printf("%s, %d\n", __FUNCTION__, __LINE__);
	return 0;
}

static struct VideoOpr bModule = {
	.name = "v4l2_name",
	.DeviceInit = V4l2DeviceInit,
};

int v4l2_init(void)
{
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return video_register(&bModule.list);
}
