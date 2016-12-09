#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SUBSYSTEM_ss.h"

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(SUBSYSTEM_list);

struct SUBSYSTEM_module *SUBSYSTEM_get_module(const char *name)
{
	struct SUBSYSTEM_module *pModule;

	list_for_each_entry(pModule, &SUBSYSTEM_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	return NULL;
}

/* 开放给底层具体模块的注册接口 */
int SUBSYSTEM_register(struct list_head *list)
{
	list_add(list, &SUBSYSTEM_list);
	return 0;
}

/* 开放给应用层调用 */
int SUBSYSTEM_init(void)
{
	/* 调用个模块初始化函数 */
	MODULE_init();
}

void SUBSYSTEM_InfoShow(struct SUBSYSTEM_module *pModule)
{
	pModule->printInfo(pModule->name);
}

void SUBSYSTEM_setVar(struct SUBSYSTEM_module *pModule, int in)
{
	pModule->setVar(in);
}

void SUBSYSTEM_getVar(struct SUBSYSTEM_module *pModule, int *out)
{
	pModule->getVar(out);
}
