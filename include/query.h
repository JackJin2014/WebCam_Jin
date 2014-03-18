/**
 * @file query.h
 * @brief 摄像头查询模块接口文件
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#ifndef __QUERY_H__
#define __QUERY_H__

#include "common.h"
/**
 * @brief query_cap 
 * 查询摄像头参数及驱动信息:打印struct v4l2_capability
 * @param fd	摄像头文件描述符
 * @return Cap *
 */
//static Cap * query_cap(int fd);
extern void query_cap(int fd);

/**
 * @brief query_priority 
 * 罗列程序优先级enum v4l2_priority
 * @param fd
 */
extern void query_priority(int fd);

/**
 * @brief query_input 
 *	查询当前摄像头输入设备的信息,G_INPUT和S_INPUT用来查询和选则当前的input
 *	打印struct v4l2_input 信息
 * @param fd
 */
extern void query_input(int fd);


/**
 * @brief query_fps 
 *	使用VIDIOC_G_PARM查询帧率信息
 * @param fd
 */
extern void query_fps(int fd);

/**
 * @brief query_fmt 
 *	使用VIDIOC_G_FMT 查询摄像头格式信息
 * @param fd
 */
extern void query_fmt(int fd);

/**
 * @brief query_fmt2 
 *	使用VIDIOC_ENUM_FMT查询摄像头信息
 * @param fd
 */
extern void query_fmt2(int fd);

/**
 * @brief enumerate_ctrl 
 * 使用VIDIOC_QUERYCTRL查询摄像头可控制信息
 * @param fd
 */
extern void enumerate_ctrl(int fd);


#endif
