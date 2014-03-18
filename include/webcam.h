/**
 * @file webcam.h
 * @brief 摄像头配置及操作接口文件
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#ifndef __WEBCAM_H__
#define __WEBCAM_H__

#include "common.h"
#include "yuv2jpeg.h"
/**
 * @brief cam_open 
 * 打开摄像头
 * @param argc
 * @param argv
 * @return 文件描述符
 */
extern int  cam_open(int argc,char *argv[]);

/**
 * @brief cam_close 
 *  	关闭摄像头、释放用户缓冲空间
 * @param fd
 */
extern void cam_close(int fd);


/**
 * @brief cam_init 
 *	查询并设置设备属性、格式、帧率,内存映射设置	
 * @param fd		摄像头
 */
void cam_init(Cam_conf *conf);
/**
 * @brief cam_free 释放用户缓冲空间
 */
static void cam_free(void);

/**
 * @brief start_capturing 
 *	开启视频流
 * @param fd
 */
void start_capturing(int fd);

/**
 * @brief stop_capturing 
 *	关闭视频流
 * @param fd
 */
void stop_capturing(int fd);

/**
 * @brief set_fmt  设置视频的帧格式
 * 根据摄像头像素参数设置Logitech C210:640*480
 * @param conf
 */
static void set_fmt(Cam_conf *conf );


/**
 * @brief set_fps 
 *	 设置帧率，C210 最大帧率30fps
 * @param fd
 * @param fps
 */
static void set_fps(Cam_conf *conf );

/**
 * @brief init_mmap 
 *	在内核空间申请帧缓冲、映射到用户空间、并入缓冲队列
 * @param fd
 * @param num
 */
static void init_mmap(Cam_conf *conf );

/**
 * @brief set_frame_size 
 * 设置图片大小
 * @param conf
 * @return 
 */
static int set_frame_size(Cam_conf *conf);
/**
 * @brief get_frame 
 *		出队一帧数据，拷贝到temp_buf，并入队
 * @param conf
 * @return 成功0，失败-1 
 */
int  get_frame(Cam_conf *conf);

/**
 * @brief process 
 *		保存一帧图片到本地目录
 * @param conf
 */ 
void process(Cam_conf *conf);


/**
 * @brief is_huffman 
 * 判断图片是否为huffman编码
 * @param buf
 * @return 
 */
static int is_huffman(unsigned char *buf);


/**
 * @brief print_picture 
 *  将图片输出到fd文件中
 * @param fd
 * @param buf
 * @param size
 * @return 
 */
int print_picture(int fd, unsigned char *buf, int size);





#endif
