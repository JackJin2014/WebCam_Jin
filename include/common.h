/**
 * @file common.h
 * @brief 常用的头文件及全局变量
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>
#include <jpeglib.h>

#define DHT_SIZE 432

#define CLEAR(x) memset(&x, 0, sizeof(x))
typedef enum {MJPEG=V4L2_PIX_FMT_MJPEG, YUYV=V4L2_PIX_FMT_YUYV} fmt_t;

//#define WIDTH 480		MJPEG可以，但是YUYV的话就重叠了
//#define HEIGHT 640

//320*240 或320*180
#define WIDTH 640	
#define HEIGHT 480

typedef struct v4l2_capability Cap;
typedef struct v4l2_input      Input;
typedef struct v4l2_fmtdesc	   Fmtd;
typedef struct v4l2_format	   Fmt;
typedef struct v4l2_streamparm Fps;
typedef struct v4l2_requestbuffers	ReqBuf;
typedef struct v4l2_buffer          Vbuf;

/**
 * @brief 摄像头配置
 */
typedef struct {
	int cam_fd;
	fmt_t fmt;
	int width;
	int height;
	int fps;
	int n_buf;
}Cam_conf;

typedef struct {
	char *start;
	int  len;
}Buf;


ReqBuf	reqbuf;		//内核空间
Vbuf	vbuf;		//队列
Buf		*bufs;		//用户映射空间
Buf     tmp_buf;	//一帧图片空间
Buf		share_buf;	//线程共享变量

Cam_conf conf;
#endif
