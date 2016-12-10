#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "convert_ss.h"

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(convert_list);

struct VideoConvert *convert_get_module(const char *name)
{
	struct VideoConvert *pModule;

	list_for_each_entry(pModule, &convert_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	return NULL;
}

/* 开放给底层具体模块的注册接口 */
int convert_register(struct list_head *list)
{
	list_add(list, &convert_list);
	return 0;
}

/* 开放给应用层调用 */
int VideoConvertInit(void)
{
	int iError;

	iError = Yuv2RgbInit();
	iError |= Mjpeg2RgbInit();
	iError |= Rgb2RgbInit();

	return 0;
}

void ShowVideoConvert(void)
{
	struct VideoConvert *pModule;

	list_for_each_entry(pModule, &convert_list, list)
		printf("registered convert opr %s\n", pModule->name);
}

/*
 * 根据视频数据格式来选取一个合适的转换函数
 * 调用注册到视频管理模块中的各个子模块的support函数来判断
 */
struct VideoConvert *GetVideoConvertForFormats(int iPixelFormatIn, int iPixelFormatOut)
{
	struct VideoConvert *pModule;

	list_for_each_entry(pModule, &convert_list, list)
	{
		if (pModule->isSupport(iPixelFormatIn, iPixelFormatOut))
			return pModule;
	}

	return NULL;
}

int video_convert2rgb(struct VideoConvert *pModule, struct VideoBuf *ptVideoBufIn, struct VideoBuf *ptVideoBufOut)
{
	int iError;
	iError = pModule->Convert(ptVideoBufIn, ptVideoBufOut);
	return iError;
}
