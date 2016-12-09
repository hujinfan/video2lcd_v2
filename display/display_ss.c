#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "display_ss.h"

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(display_list);

struct DispOpr *display_get_module(const char *name)
{
	struct DispOpr *pModule;

	list_for_each_entry(pModule, &display_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	return NULL;
}

/* 调用各个子模块的初始化函数 */
void DispOprs_init(void)
{
	struct DispOpr *pModule;

	list_for_each_entry(pModule, &display_list, list)
	{
		if (pModule->DeviceInit)
			pModule->DeviceInit();

		if (pModule->CleanScreen)
			pModule->CleanScreen();
	}
}

/* 开放给底层具体模块的注册接口 */
int display_register(struct list_head *list)
{
	list_add(list, &display_list);
	return 0;
}

/* 开放给应用层调用 */
int display_init(void)
{
	/* 调用个模块初始化函数 */
	fb_init();
}
