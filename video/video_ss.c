#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "video_ss.h"

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(video_list);

struct video_module *video_get_module(const char *name)
{
	struct VideoOpr *pModule;

	list_for_each_entry(pModule, &video_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	return NULL;
}

/* 开放给底层具体模块的注册接口 */
int video_register(struct list_head *list)
{
	list_add(list, &video_list);
	return 0;
}

/* 开放给应用层调用 */
int video_init(void)
{
	/* 调用个模块初始化函数 */
	v4l2_init();
}

/* 调用各个子模块的初始化函数 */
void video_modules_init(void)
{
	struct VideoOpr *pModule;

	list_for_each_entry(pModule, &video_list, list)
	{
		if (pModule->DeviceInit)
			pModule->DeviceInit();
	}
}
