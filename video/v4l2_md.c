#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

#include "video_ss.h"

/* 判断是否支持制定的格式 */
static int g_aiSupportFormats[] = {V4L2_PIX_FMT_MJPEG};
static int isSupportThisFormat(int iPixelFormat)
{
	int i;

	for (i = 0; i < sizeof(g_aiSupportFormats) / sizeof(g_aiSupportFormats[0]); i++)
	{
		if (iPixelFormat == g_aiSupportFormats[i])
			return 1;
	}

	return 0;
}

static int V4l2DeviceInit(struct VideoOpr *pVideoOpr)
{
	int i;
	int iFd;
	int iError;
	struct v4l2_capability tV4l2Cap;
	struct v4l2_fmtdesc tFmtDesc;
	struct v4l2_format tV4l2Fmt;
	struct v4l2_requestbuffers tV4l2ReqBuffs;
	struct v4l2_buffer tV4l2Buf;

	/* 打开设备 */
	iFd = open(pVideoOpr->device_name, O_RDWR);
	if (iFd < 0)
	{
		printf("open %s ERROR\n", pVideoOpr->name);
		return -1;
	}
	pVideoOpr->iFd = iFd;

	/* 查询属性 */
	memset(&tV4l2Cap, 0, sizeof(struct v4l2_capability));
	iError = ioctl(iFd, VIDIOC_QUERYCAP, &tV4l2Cap);
	if (iError)
	{
		printf("querycap error\n");
		goto err_exit;
	}

	/* 根据查询到的属性判断 */
	if (!(tV4l2Cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		printf("capabilities ERROR\n");
		goto err_exit;
	}
	if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING)
	{
	}
	if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE)
	{
	}

	/* 获取设备能支持的格式 */
	memset(&tFmtDesc, 0, sizeof(struct v4l2_fmtdesc));
	tFmtDesc.index = 0; /* 记录里能够支持的格式的INDEX */
	tFmtDesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while ( (iError = ioctl(iFd, VIDIOC_ENUM_FMT, &tFmtDesc)) == 0 )
	{
		if (isSupportThisFormat(tFmtDesc.pixelformat))
		{
			pVideoOpr->iPixelFormat = tFmtDesc.pixelformat;
			break;
		}
		tFmtDesc.index++;
	}

	/* 设备的格式不在该驱动支持的范围内 */
	if (!pVideoOpr->iPixelFormat)
	{
		printf("format is not support\n");
		goto err_exit;
	}

	/* 设置格式 */
	/* 1. 构造要设置的格式结构体数据 */
	memset(&tV4l2Fmt, 0, sizeof(struct v4l2_format));
	tV4l2Fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	tV4l2Fmt.fmt.pix.pixelformat = pVideoOpr->iPixelFormat;
	tV4l2Fmt.fmt.pix.width = 320;
	tV4l2Fmt.fmt.pix.height = 240;
	tV4l2Fmt.fmt.pix.field = V4L2_FIELD_ANY;

	/* 2. 设置 */
	iError = ioctl(iFd, VIDIOC_S_FMT, &tV4l2Fmt);
	if (iError)
	{
		printf("set format error\n");
		goto err_exit;
	}
	pVideoOpr->iWidth = tV4l2Fmt.fmt.pix.width;
	pVideoOpr->iHeight = tV4l2Fmt.fmt.pix.height;

	/* 请求缓冲区 */
	/* 1. 构造请求数据结构 */
	memset(&tV4l2ReqBuffs, 0, sizeof(struct v4l2_requestbuffers));
	tV4l2ReqBuffs.count = 4;
	tV4l2ReqBuffs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	tV4l2ReqBuffs.memory = V4L2_MEMORY_MMAP;

	/* 2. 请求 Request buffer */
	iError = ioctl(iFd, VIDIOC_REQBUFS, &tV4l2ReqBuffs);
	if (iError)
	{
		printf("req bufs ERROR\n");
		goto err_exit;
	}

	pVideoOpr->iVideoBufCnt = tV4l2ReqBuffs.count;

	/* 查询请求的缓冲区是否请求成功,成功就放入队列 */
	/* 分别处理STREAMING接口和ReadWrite接口 */
	if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING)
	{
		/*
		 * Query buffer
		 * streaming接口用mmap映射
		 */
		for (i = 0; i < pVideoOpr->iVideoBufCnt; i++)
		{
			memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
			tV4l2Buf.index = i;
			tV4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			tV4l2Buf.memory = V4L2_MEMORY_MMAP;
			iError = ioctl(iFd, VIDIOC_QUERYBUF, &tV4l2Buf);
			if (iError)
			{
				goto err_exit;
			}

			/* 把底层分配的数据缓冲区映射给应用层用 */
			pVideoOpr->iVideoBufMaxLen = tV4l2Buf.length;
			pVideoOpr->pucVideoBuf[i] = mmap(0, tV4l2Buf.length, PROT_READ, MAP_SHARED, iFd, tV4l2Buf.m.offset);
			if (pVideoOpr->pucVideoBuf[i] == MAP_FAILED)
			{
				printf("Map failed\n");
				goto err_exit;
			}
		}

		/* Queue 放入队列 */
		for (i = 0; i < pVideoOpr->iVideoBufCnt; i++)
		{
			memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
			tV4l2Buf.index = i;
			tV4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			tV4l2Buf.memory = V4L2_MEMORY_MMAP;
			iError = ioctl(iFd, VIDIOC_QBUF, &tV4l2Buf);
			if (iError)
			{
				printf("qbuf error\n");
				goto err_exit;
			}
		}
	}
	else if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE)
	{
	}

	return 0;
err_exit:
	close(iFd);

	return -1;
}

static int V4l2StartDevice(PT_VideoOpr pVideoOpr)
{
	int iType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int iError;

	iError = ioctl(pVideoOpr->iFd, VIDIOC_STREAMON, &iType);
	if (iError)
	{
		printf("ERROR, %s, %d\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/* 获取一帧数据 */
static int V4l2GetFrameForStreaming(PT_VideoOpr pVideoOpr, PT_VideoBuf ptVideoBuf)
{
	struct pollfd tFds[1];
	int iRet;
	struct v4l2_buffer tV4l2Buf;

	/* 等待数据 */
	tFds[0].fd = pVideoOpr->iFd;
	tFds[0].events = POLLIN;

	iRet = poll(tFds, 1, -1);
	if (iRet <= 0)
	{
		return -1;
	}

	/* 把视频缓冲区放入队列 */
	memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
	tV4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	tV4l2Buf.memory = V4L2_MEMORY_MMAP;

	iRet = ioctl(pVideoOpr->iFd, VIDIOC_DQBUF, &tV4l2Buf);
	if (iRet < 0)
	{
		return -1;
	}

	/* 标识当前哪个缓冲区有视频数据 */
	pVideoOpr->iVideoBufCurIndex = tV4l2Buf.index;

	/*
	 * 下面这里写的感觉有点怪异
	 * 没有用tV4l2Buf,用的是pVideoOpr
	 */
	ptVideoBuf->iPixelFormat = pVideoOpr->iPixelFormat;
	ptVideoBuf->tPixelDatas.iWidth = pVideoOpr->iWidth;
	ptVideoBuf->tPixelDatas.iHeight = pVideoOpr->iHeight;
	ptVideoBuf->tPixelDatas.iBpp = (pVideoOpr->iPixelFormat == V4L2_PIX_FMT_YUYV) ? 16 :\
	(pVideoOpr->iPixelFormat == V4L2_PIX_FMT_MJPEG) ? 0 :\
	(pVideoOpr->iPixelFormat == V4L2_PIX_FMT_RGB565) ? 16 : 0;

	ptVideoBuf->tPixelDatas.iLineBytes = pVideoOpr->iWidth * ptVideoBuf->tPixelDatas.iBpp / 8;
	ptVideoBuf->tPixelDatas.iTotalBytes = tV4l2Buf.bytesused;
	ptVideoBuf->tPixelDatas.aucPixelDatas = pVideoOpr->pucVideoBuf[tV4l2Buf.index];

	return 0;
}

static int V4l2PutFrameForStreaming(PT_VideoOpr pVideoOpr, PT_VideoBuf ptVideoBuf)
{
	/* VIDIOC_QBUF */
	struct v4l2_buffer tV4l2Buf;
	int iError;

	memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
	tV4l2Buf.index = pVideoOpr->iVideoBufCurIndex;
	tV4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	tV4l2Buf.memory = V4L2_MEMORY_MMAP;

	iError = ioctl(pVideoOpr->iFd, VIDIOC_QBUF, &tV4l2Buf);
	if (iError)
	{
		return 0;
	}
	return 0;
}
static struct VideoOpr bModule = {
	.name = "v4l2",
	.device_name = "/dev/video0",
	.use_as_default = 0, /* 初始不使能 */
	.DeviceInit = V4l2DeviceInit,
	.StartDevice = V4l2StartDevice,
	.GetFrame = V4l2GetFrameForStreaming,
	.PutFrame = V4l2PutFrameForStreaming,
};

int v4l2_init(void)
{
	printf("Video module v4l2 init\n");
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return video_register(&bModule.list);
}
