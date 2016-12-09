#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>
#include <fcntl.h>

#include "display_ss.h"

void FbDeviceInit(void)
{
	printf("%s, %d\n", __FUNCTION__, __LINE__);
}

void FbShowPage(void)
{
	printf("%s, %d\n", __FUNCTION__, __LINE__);
}

void FbCleanScreen(void)
{
	printf("%s, %d\n", __FUNCTION__, __LINE__);
}

static T_DispOpr g_tFbOpr = {
	.name = "fb",
	.DeviceInit = FbDeviceInit,
	.CleanScreen = FbCleanScreen,
	.ShowPage = FbShowPage,
};

int fb_init(void)
{
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return display_register(&g_tFbOpr.list);
}
