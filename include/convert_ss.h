#ifndef _convert_H_
#define _convert_H_

#include "klist.h"
#include <linux/videodev2.h>
#include "pic_operation.h"
#include "video_ss.h"

struct VideoConvert {
	char *name;
	int use_as_default;
	int (*isSupport)(int iPixelFormatIn, int iPixelFormatOut);
	int (*Convert)(PT_VideoBuf ptVideoBufIn, PT_VideoBuf ptVideoBufOut);
	int (*ConvertExit)(PT_VideoBuf ptVideoBufOut);

	struct list_head list;
};

int convert_register(struct list_head *list);
int Mjpeg2RgbInit(void);
int Yuv2RgbInit(void);
int Rgb2RgbInit(void);

/* call for app */
struct VideoConvert *convert_get_module(const char *name);
int VideoConvertInit(void);
void ShowVideoConvert(void);
int find_support_convert_module(int iPixelFormatIn, int iPixelFormatOut);
int video_convert2rgb(struct VideoBuf *ptVideoBufIn, struct VideoBuf *ptVideoBufOut);
void ShowVideoConvertInfo(struct VideoConvert *pModule);

#endif
