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

static struct fb_var_screeninfo g_tFbVar;
static struct fb_fix_screeninfo g_tFbFix;
static unsigned int g_dwScreenSize;//屏幕尺寸
static unsigned char *g_pucFbMem;//framebuffer

static unsigned int g_dwLineWidth;
static unsigned int g_dwPixelWidth;

static int FbDeviceInit(struct DispOpr *pDispOpr)
{
	int iError;
	int iFd;
	printf("%s, %d\n", __FUNCTION__, __LINE__);

	iFd = open("/dev/fb0", O_RDWR);
	if (iFd < 0)
	{
		printf("can't open device fb\n");
		return -1;
	}

	/* 获取LCD的可变参数和固定参数 */
	iError = ioctl(iFd, FBIOGET_VSCREENINFO, &g_tFbVar);
	if (iError < 0)
	{
		printf("can't get fb's var\n");
		return -1;
	}

	iError = ioctl(iFd, FBIOGET_FSCREENINFO, &g_tFbFix);
	if (iError < 0)
	{
		printf("can't get fb's fix\n");
		return -1;
	}

	g_dwScreenSize = g_tFbVar.xres * g_tFbVar.yres * g_tFbVar.bits_per_pixel / 8;
	g_pucFbMem = (unsigned char *)mmap(NULL, g_dwScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (g_pucFbMem < 0)
	{
		printf("can't mmap\n");
		return -1;
	}

	pDispOpr->iXres = g_tFbVar.xres;
	pDispOpr->iYres = g_tFbVar.yres;
	pDispOpr->iBpp = g_tFbVar.bits_per_pixel;
	pDispOpr->iLineWidth = g_tFbVar.xres * pDispOpr->iBpp / 8;

	/* 显存指向framebuffer */
	pDispOpr->pucDispMem = g_pucFbMem;

	g_dwLineWidth = g_tFbVar.xres * g_tFbVar.bits_per_pixel / 8;
	g_dwPixelWidth = g_tFbVar.bits_per_pixel / 8;

	return 0;
}

static int FbCleanScreen(unsigned int dwBackColor)
{
	unsigned char *pucFb;
	unsigned short *pwFb16bpp;
	unsigned int *pdwFb32bpp;
	unsigned short wColor16bpp;

	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;

	pucFb = g_pucFbMem;
	pwFb16bpp = (unsigned short *)pucFb;
	pdwFb32bpp = (unsigned int *)pucFb;

	switch (g_tFbVar.bits_per_pixel)
	{
		case 8:
			memset(g_pucFbMem, dwBackColor, g_dwScreenSize);
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

			while (i < g_dwScreenSize)
			{
				*pwFb16bpp = wColor16bpp;
				pwFb16bpp++;
				i += 2;
			}
			break;
		case 32:
			while (i < g_dwScreenSize)
			{
				*pdwFb32bpp = dwBackColor;
				pdwFb32bpp++;
				i+= 4;
			}
			break;
		default:
			printf("can't support %d bpp\n", g_tFbVar.bits_per_pixel);
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
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return display_register(&pDispOpr.list);
}
