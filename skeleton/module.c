#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SUBSYSTEM_ss.h"

static struct SUBSYSTEM_module bModule;

static void MODULE_setVar(int in)
{
	bModule.var = in;
}

static void MODULE_getVar(int *out)
{
	*out = bModule.var;
}

static void MODULE_printInfo(const char *name)
{
	printf("<%s> : %s\n", name, __FUNCTION__);
}

static struct SUBSYSTEM_module bModule = {
	.name = "MODULE_name",
	.var = -1,
	.printInfo = MODULE_printInfo,
	.setVar = MODULE_setVar,
	.getVar = MODULE_getVar,
};

int MODULE_init(void)
{
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return SUBSYSTEM_register(&bModule.list);
}
