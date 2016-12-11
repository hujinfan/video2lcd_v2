#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "video_ss.h"

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(video_list);

/* 开放给底层具体模块的注册接口 */
int video_register(struct list_head *list)
{
	list_add(list, &video_list);
	return 0;
}

/* 开放给应用层调用 */
int video_init(void)
{
	int iError;

	printf("Video subsystem init\n");
	/* 调用个模块初始化函数 */
	iError = v4l2_init();

	return iError;
}

/* 调用各个子模块的初始化函数 */
void video_modules_init(void)
{
	struct VideoOpr *pModule;

	list_for_each_entry(pModule, &video_list, list)
	{
		if (pModule->DeviceInit)
			pModule->DeviceInit(pModule);
	}
}

void ShowVideoOpr(void)
{
	struct VideoOpr *pModule;

	list_for_each_entry(pModule, &video_list, list)
		printf("registered video opr %s\n", pModule->name);
}

/* 获取摄像头数据格式 */
void get_camera_format(struct VideoOpr *pModule, int *Width, int *Height, int *format)
{
	*Width = pModule->iWidth;
	*Height = pModule->iHeight;
	*format = pModule->iPixelFormat;
}

/* 开启摄像头 */
int start_camera(struct VideoOpr *pModule)
{
	int iError;

	iError = pModule->StartDevice(pModule);

	return iError;
}

/* 获取一帧数据 */
int get_frame(struct VideoOpr *pModule, PT_VideoBuf ptVideoBuf)
{
	int iError;

	iError = pModule->GetFrame(pModule, ptVideoBuf);

	return iError;
}

/* 释放一帧数据 */
int put_frame(PT_VideoOpr pModule, PT_VideoBuf ptVideoBuf)
{
	int iError;

	iError = pModule->PutFrame(pModule, ptVideoBuf);

	return iError;
}

struct VideoOpr *video_get_module(const char *name)
{
	struct VideoOpr *pModule;

	list_for_each_entry(pModule, &video_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	printf("no sub module ERROR\n");
	return NULL;
}
