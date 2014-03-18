/**
 * @file yuv2jpeg.h
 * @brief yuv格式转jpeg格式接口文件
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#ifndef __YUV2JPEG_H__
#define __YUV2JPEG_H__

#include <jpeglib.h>
#include "common.h"

//Config of the jpeg picture which
//changed from yuyv format
typedef struct{
	struct jpeg_destination_mgr jpg_p;
	JOCTET*			buffer;				// /usr/include/jmorecfg.h  unsigned char
	int				out_buf_size;
	int*				written; 
	unsigned char*	out_buf_cur;
	unsigned char*	out_buf;
}jpg_dst;

typedef jpg_dst *jpg_ptr;

/* yuyv to jpeg function */
extern void			dst_buffer(j_compress_ptr,unsigned char*,int,int*);
extern int			yuv_to_jpeg(unsigned char*, unsigned char*,int, int);

#endif
