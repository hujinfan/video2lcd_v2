LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	main.c \
	display/display_ss.c \
	display/fb_md.c \
	video/video_ss.c \
	video/v4l2_md.c \
	convert/convert_ss.c \
	convert/jdatasrc-tj.c \
	convert/mjpeg2rgb_md.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/builds \
	$(LOCAL_PATH)/include \
	external/jpeg \
	external/zlib

LOCAL_CFLAGS += -W -Wall

LOCAL_SHARED_LIBRARIES += libjpeg libm

LOCAL_CFLAGS += -O2

LOCAL_MODULE:= video2lcd

include $(BUILD_EXECUTABLE)
