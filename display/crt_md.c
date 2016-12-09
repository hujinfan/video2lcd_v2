#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "display_ss.h"

static struct display_module bModule;

static void crt_setVar(int in)
{
	bModule.var = in;
}

static void crt_getVar(int *out)
{
	*out = bModule.var;
}

static void crt_printInfo(const char *name)
{
	printf("<%s> : %s\n", name, __FUNCTION__);
}

static void crt_initDevice(struct display_module *p)
{
	printf("%s val = %d\n", p->name, p->var);
}

static struct display_module bModule = {
	.name = "crt_name",
	.var = -1,
	.printInfo = crt_printInfo,
	.setVar = crt_setVar,
	.getVar = crt_getVar,
	.initDevice = crt_initDevice,
};

int crt_init(void)
{
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return display_register(&bModule.list);
}
