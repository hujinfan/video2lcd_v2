#ifndef _convert_H_
#define _convert_H_

#include "klist.h"
#include <linux/videodev2.h>
#include "pic_operation.h"
#include "video_ss.h"

struct VideoConvert {
	char *name;
	int (*isSupport)(int iPixelFormatIn, int iPixelFormatOut);
	int (*Convert)(PT_VideoBuf ptVideoBufIn, PT_VideoBuf ptVideoBufOut);
	int (*ConvertExit)(PT_VideoBuf ptVideoBufOut);

	struct list_head list;
};

#endif
