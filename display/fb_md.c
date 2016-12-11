#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#include "display_ss.h"

static int FbDeviceInit(struct DispOpr *pDispOpr)
{
	int iError;
	int iFd;

	iFd = open("/dev/fb0", O_RDWR);
	if (iFd < 0)
	{
		printf("can't open device fb\n");
		return -1;
	}

	/* 获取LCD的可变参数和固定参数 */
	iError = ioctl(iFd, FBIOGET_VSCREENINFO, &pDispOpr->fb_var);
	if (iError < 0)
	{
		printf("can't get fb's var\n");
		return -1;
	}

	iError = ioctl(iFd, FBIOGET_FSCREENINFO, &pDispOpr->fb_fix);
	if (iError < 0)
	{
		printf("can't get fb's fix\n");
		return -1;
	}

	pDispOpr->dwScreenSize = pDispOpr->fb_var.xres * pDispOpr->fb_var.yres * pDispOpr->fb_var.bits_per_pixel / 8;
	pDispOpr->pucFbMem = (unsigned char *)mmap(NULL, pDispOpr->dwScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (pDispOpr->pucFbMem < 0)
	{
		printf("can't mmap\n");
		return -1;
	}

	pDispOpr->iXres = pDispOpr->fb_var.xres;
	pDispOpr->iYres = pDispOpr->fb_var.yres;
	pDispOpr->iBpp = pDispOpr->fb_var.bits_per_pixel;
	pDispOpr->iLineWidth = pDispOpr->fb_var.xres * pDispOpr->iBpp / 8;

	return 0;
}

static int FbCleanScreen(struct DispOpr *pDispOpr, unsigned int dwBackColor)
{
	unsigned char *pucFb;
	unsigned short *pwFb16bpp;
	unsigned int *pdwFb32bpp;
	unsigned short wColor16bpp;

	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;

	pucFb = pDispOpr->pucFbMem;
	pwFb16bpp = (unsigned short *)pucFb;
	pdwFb32bpp = (unsigned int *)pucFb;

	switch (pDispOpr->fb_var.bits_per_pixel)
	{
		case 8:
			memset(pDispOpr->pucFbMem, dwBackColor, pDispOpr->dwScreenSize);
			break;
		case 16:
			/*
			 * 从dwBackColor中取出红绿蓝三原色
			 * 构造为16bpp的颜色
			 */
			iRed = (dwBackColor >> (16 + 3)) & 0x1f;
			iGreen = (dwBackColor >> (8 + 2)) & 0x3f;
			iBlue = (dwBackColor >> 3) & 0x1f;

			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;

			while (i < pDispOpr->dwScreenSize)
			{
				*pwFb16bpp = wColor16bpp;
				pwFb16bpp++;
				i += 2;
			}
			break;
		case 32:
			while (i < pDispOpr->dwScreenSize)
			{
				*pdwFb32bpp = dwBackColor;
				pdwFb32bpp++;
				i+= 4;
			}
			break;
		default:
			printf("can't support %d bpp\n", pDispOpr->fb_var.bits_per_pixel);
			return -1;
	}

	return 0;
}

static T_DispOpr pDispOpr = {
	.name = "fb",
	.DeviceInit = FbDeviceInit,
	.CleanScreen = FbCleanScreen,
};

int fb_init(void)
{
	printf("Display module fb init\n");
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return display_register(&pDispOpr.list);
}
