/**
 * @file th.h
 * @brief 多线程的并发处理接口
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#ifndef __TH_H__
#define __TH_H__
#include <pthread.h>
#include <signal.h>

/**
 * @brief cam_th 
 *	摄像头采集线程
 * @param arg
 * @return 
 */
extern void *cam_th(void *arg);
/**
 * @brief cli_th 
 *	客户端处理线程
 * @param arg
 * @return 
 */
extern void *cli_th(void *arg);

/**
 * @brief signal_handler 
 *	SIGINT 处理函数：停止捕获、关闭摄像头、释放动态分配的内存、释放锁和释放条件变量
 * @param signo
 */
extern void signal_handler(int signo);

#endif
