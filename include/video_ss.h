#ifndef _VIDEO_MANAGER_H
#define _VIDEO_MANAGER_H

#include <linux/videodev2.h>
#include "pic_operation.h"
#include "klist.h"

typedef struct VideoDevice T_VideoDevice, *PT_VideoDevice;
typedef struct VideoOpr T_VideoOpr, *PT_VideoOpr;
typedef struct VideoBuf T_VideoBuf, *PT_VideoBuf;

/*
 * 为了能提同给调用视频模块的主函数统一的接口
 * 而不必关心底层具体实现将所有操作都统一化
 * 底层具体模块各自实现相应的操作函数即可
 * 且将各个具体模块的具体实现放入一个全局链表里
 */
struct VideoOpr {
	char *name;/* 具体模块的名字 */
	int (*DeviceInit)(void);
	int (*ExitDevice)();
	int (*GetFrame)(PT_VideoDevice ptVideoDevice, PT_VideoBuf ptVideoBuf);
	int (*GetFormat)();
	int (*PutFrame)(PT_VideoDevice ptVideoDevice, PT_VideoBuf ptVideoBuf);
	int (*StartDevice)(PT_VideoDevice ptVideoDevice);
	int (*StopDevice)();
	struct list_head list;
};

struct VideoDevice {
	int iFd;
	int iPixelFormat;
	int iWidth;
	int iHeight;
	int iVideoBufCnt;
	int iVideoBufMaxLen;
	int iVideoBufCurIndex;

	unsigned char *pucVideoBuf[4];

	PT_VideoOpr	ptVideoOpr;
};

/* 存储视频数据 */
struct VideoBuf {
	T_PixelDatas tPixelDatas;
	int iPixelFormat;
};

/*
 * strDevName is the /dev/video* node
 * open the strDevName device to get what we want
 */
int VideoDeviceInit(const char *strDevName, PT_VideoDevice ptVideoDevice);

/*
 * 提供给各个具体模块注册到video manager中来
 * 将各个具体模块操作挂入链表
 */
int RegisterVideoOpr(PT_VideoOpr ptVideoOpr);
int VideoInit(void);
void ShowVideoOpr(void);
extern int V4l2Init(void);
#endif
