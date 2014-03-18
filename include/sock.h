/**
 * @file sock.h
 * @brief webserver接口【视频传输模块及http协议处理】
 *  实现将视频流发送到浏览器上,服务器一次处理一个客户端请求
 *		1、先建立浏览器和本服务器的socket通信
 *		2、增加http协议，实现发送视频流/图片
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#ifndef __SOCK_H__
#define __SOCK_H__

#include <netdb.h>

typedef struct sockaddr SA;
typedef struct sockaddr_in SA_in;
#define LISTENQ  1024  /* second argument to listen() */

#define MAX	1024
// 存放接收客户机浏览器请求后的回应：Http head 报文头
static char HEADER[MAX] = "HTTP/1.1 200 OK\r\nConnection: close\r\nServer: Webcam V0.0\r\nCache-Control: no-store, no-cache,must-revalidate, pre-check=0, post-check=0, max-age=0\r\nPragma:no-cache\r\nContent-type: multipart/x-mixed-replace;boundary=www.briup.com\r\n\r\n";

/**
 * @brief open_listenfd 
 *  创建并返回监听socket描述付，setsockopt设置IP可重用
 * @param port	端口号
 * @return 监听socket描述付
 */
extern int Open_listenfd(int port); 

/**
 * @brief Accept 
 *  等待客户端的链接请求，
 * @param listenfd	监听socket描述付
 * @param addr		客户端地址指针
 * @param addrlen	客户端地址长度
 * @return 链接socket描述付
 */
extern int Accept(int listenfd, SA *addr, socklen_t *addrlen);

/**
 * @brief send_picture 
 * 	响应浏览器请求，发送照片或者视频流
 * 1、接受浏览器URL信息或客户端信息
 * 2、向浏览器响应http 报文头信息
 * 3、浏览器请求一张图片
 * 4、浏览器请求视频
 * @param confd		链接描述付
 * @param conf		摄像头配置指针
 */
extern void send_picture(int confd, Cam_conf* conf);

/**
 * @brief Client_Info 
 *	 打印客户端的IP地址和域名
 * @param client_addr
 */
extern void Client_Info(SA_in *client_addr);
#endif
