/**
 * @file yuv2jpeg.c
 * @brief yuv格式转jpeg格式接口文件
 * This file has implements changing YUYV format to JPEG format
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */
#include "webcam.h"
#include "yuv2jpeg.h"
#include <jpeglib.h>

#define OUTPUT_BUF_SIZE  4096
METHODDEF(void) 	init_dst(j_compress_ptr);
METHODDEF(boolean) 	empty_out_buf(j_compress_ptr);
METHODDEF(void) 	term_dst(j_compress_ptr);

METHODDEF(void) init_dst(j_compress_ptr cinfo){
  jpg_ptr dest = (jpg_ptr)cinfo->dest;
  dest->buffer = (JOCTET *)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_IMAGE, OUTPUT_BUF_SIZE * sizeof(JOCTET));
  *(dest->written) = 0;
  dest->jpg_p.next_output_byte = dest->buffer;
  dest->jpg_p.free_in_buffer = OUTPUT_BUF_SIZE;
}

METHODDEF(boolean) empty_out_buf(j_compress_ptr cinfo){
  jpg_ptr dest = (jpg_ptr)cinfo->dest;
  memcpy(dest->out_buf_cur, dest->buffer, OUTPUT_BUF_SIZE);
  dest->out_buf_cur += OUTPUT_BUF_SIZE;
  *(dest->written) += OUTPUT_BUF_SIZE;
  dest->jpg_p.next_output_byte = dest->buffer;
  dest->jpg_p.free_in_buffer = OUTPUT_BUF_SIZE;

  return TRUE;
}

METHODDEF(void) term_dst(j_compress_ptr cinfo){
  jpg_ptr dest = (jpg_ptr)cinfo->dest;
  size_t datacount = OUTPUT_BUF_SIZE - dest->jpg_p.free_in_buffer;

  /* Write any data remaining in the buffer */
  memcpy(dest->out_buf_cur, dest->buffer, datacount);
  dest->out_buf_cur += datacount;
  *(dest->written) += datacount;
}

void dst_buffer(j_compress_ptr cinfo, unsigned char *buffer, int size, int *written){
  jpg_ptr dest;
  if (cinfo->dest == NULL) {
    cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small)((j_common_ptr) cinfo, \
			JPOOL_PERMANENT, sizeof(jpg_dst));
  }

  dest = (jpg_ptr)cinfo->dest;
  dest->jpg_p.init_destination = init_dst;
  dest->jpg_p.empty_output_buffer = empty_out_buf;
  dest->jpg_p.term_destination = term_dst;
  dest->out_buf = buffer;
  dest->out_buf_size = size;
  dest->out_buf_cur = buffer;
  dest->written = written;

}

//摄像头采集帧图像的YUYV格式转换为JPEG格式
int yuv_to_jpeg(unsigned char *buf, unsigned char *buffer, int size, int quality){
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];
  unsigned char *line_buffer, *yuyv;
  int z;
  static int written;
  //int count = 0;
  //printf("%s\n", buf);
  line_buffer = calloc(WIDTH * 3, 1);
  yuyv = buf;//将YUYV格式的图片数据赋给YUYV指针
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  /* jpeg_stdio_dest (&cinfo, file); */
  dst_buffer(&cinfo, buffer, size, &written);
  cinfo.image_width = WIDTH;
  cinfo.image_height = HEIGHT;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);
  jpeg_start_compress(&cinfo, TRUE);

  z = 0;

  while(cinfo.next_scanline < HEIGHT){
    int x;
    unsigned char *ptr = line_buffer;
    for(x = 0; x < WIDTH; x++){
      int r, g, b;
      int y, u, v;
      if(!z)
        y = yuyv[0] << 8;
      else
        y = yuyv[2] << 8;
      u = yuyv[1] - 128;
      v = yuyv[3] - 128;
      r = (y + (359 * v)) >> 8;
      g = (y - (88 * u) - (183 * v)) >> 8;
      b = (y + (454 * u)) >> 8;
      *(ptr++) = (r > 255) ? 255 : ((r < 0) ? 0 : r);
      *(ptr++) = (g > 255) ? 255 : ((g < 0) ? 0 : g);
      *(ptr++) = (b > 255) ? 255 : ((b < 0) ? 0 : b);

      if(z++) {
        z = 0;
        yuyv += 4;
      }
    } 

    row_pointer[0] = line_buffer;
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  free(line_buffer);

  return (written);
}
