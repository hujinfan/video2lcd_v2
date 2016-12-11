#ifndef _VIDEO_MANAGER_H
#define _VIDEO_MANAGER_H

#include <linux/videodev2.h>
#include "pic_operation.h"
#include "klist.h"

typedef struct VideoOpr T_VideoOpr, *PT_VideoOpr;
typedef struct VideoBuf T_VideoBuf, *PT_VideoBuf;

/*
 * 为了能提同给调用视频模块的主函数统一的接口
 * 而不必关心底层具体实现将所有操作都统一化
 * 底层具体模块各自实现相应的操作函数即可
 * 且将各个具体模块的具体实现放入一个全局链表里
 */
struct VideoOpr {
	/* camera device info */
	int iFd;
	int iPixelFormat;
	int iWidth;
	int iHeight;
	int iVideoBufCnt;
	int iVideoBufMaxLen;
	int iVideoBufCurIndex;

	/* video buffers */
	unsigned char *pucVideoBuf[4];

	char *name;/* 具体模块的名字 */
	char *device_name;
	int use_as_default;

	int (*DeviceInit)(struct VideoOpr *pModule);
	int (*ExitDevice)();
	int (*GetFrame)(PT_VideoOpr pModule, PT_VideoBuf ptVideoBuf);
	int (*GetFormat)();
	int (*PutFrame)(PT_VideoOpr pModule, PT_VideoBuf ptVideoBuf);
	int (*StartDevice)(PT_VideoOpr pModule);
	int (*StopDevice)();

	struct list_head list;
};

/* 存储视频数据 */
struct VideoBuf {
	T_PixelDatas tPixelDatas;
	int iPixelFormat;
};

/*
 * 提供给各个具体模块注册到video manager中来
 * 将各个具体模块操作挂入链表
 */
void ShowVideoOpr(void);
int video_register(struct list_head *list);
int v4l2_init(void);

/* call for app */
int video_init(void);
void video_modules_init(void);
void ShowVideoOpr(void);
void get_camera_format(int *Width, int *Height, int *format);
int start_camera(void);
int get_frame(PT_VideoBuf ptVideoBuf);
int put_frame(PT_VideoBuf ptVideoBuf);
void choose_default_video_module(const char *name);

#endif
