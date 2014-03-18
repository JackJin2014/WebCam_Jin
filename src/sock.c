/**
 * @file sock.c
 * @brief webserver接口实现【视频传输模块及http协议处理】
 *  实现将视频流发送到浏览器上,服务器一次处理一个客户端请求
 *		1、先建立浏览器和本服务器的socket通信
 *		2、增加http协议，实现发送视频流/图片
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#include "common.h"
#include "sock.h"

/**
 * @brief open_listenfd 
 *  创建并返回监听socket描述付，setsockopt设置IP可重用
 * @param port	端口号
 * @return 成功返回监听socket描述符，失败-1
 */
int Open_listenfd(int port)
{
	int listenfd, optval=1;
	SA_in serveraddr;
	CLEAR(serveraddr);
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	if( setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
				(const void *)&optval, sizeof(int)) < 0)
		return -1;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	serveraddr.sin_port = htons((unsigned short)port);
	CLEAR(serveraddr.sin_zero);
	if(bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
		return -1;

	if(listen(listenfd, LISTENQ))
		return -1;
	return listenfd;
}

/**
 * @brief Accept 
 *  等待客户端的链接请求，
 * @param listenfd	监听socket描述付
 * @param addr		客户端地址指针
 * @param addrlen	客户端地址长度
 * @return 链接socket描述付
 */
int Accept(int listenfd, SA *addr, socklen_t *addrlen)
{
	int rc;
	if ((rc = accept(listenfd, addr, addrlen)) < 0){
		perror("Accept error");
		exit(EXIT_FAILURE);
	}
	return rc;
}

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
void send_picture(int confd, Cam_conf* conf)
{
	char readbuf[MAX]; CLEAR(readbuf);
	char writebuf[MAX]; CLEAR(writebuf);

	//接受浏览器URL信息或客户端信息
	recv(confd, readbuf, MAX, 0);	
	printf("Client Info:%s.\n", readbuf);
	if(strstr(readbuf, "http") != NULL || strstr(readbuf, "HTTP") != NULL){//浏览器请求
		//向浏览器响应http 报文头信息
		if(write(confd, HEADER, strlen(HEADER)) != strlen(HEADER)){
			perror("send_picture(): Response Error");
			exit(EXIT_FAILURE);
		}

		//浏览器请求一张图片
		if(strstr(readbuf, "snapshot") != NULL){
			sprintf(writebuf, "--www.briup.com\nContent-type:image/jpeg\nContent-Length: %d\n\n", tmp_buf.len+ DHT_SIZE);		
			if(write(confd, writebuf, strlen(writebuf)) != strlen(writebuf)) {
				perror("send_picture: write1()");
				exit(EXIT_FAILURE);
			}
			print_picture(confd, tmp_buf.start, tmp_buf.len);
			/*
			if(write(confd, frame_buf.start, frame_buf.len) < 0){
				perror("write a frame");
				exit(EXIT_FAILURE);
			}*/
		}else{//浏览器请求视频
			while(1){
				sprintf(writebuf, "--www.briup.com\nContent-type:image/jpeg\nContent-Length: %d\n\n", tmp_buf.len+ DHT_SIZE);		
				if(write(confd, writebuf, strlen(writebuf)) != strlen(writebuf))	{
					perror("send_picture: write2()");
					exit(EXIT_FAILURE);
				}
				get_frame(conf);
				//print_picture(confd, frame_buf.start, frame_buf.len);
				if(write(confd, tmp_buf.start, tmp_buf.len) <= 0){
					perror("write a frame");
					exit(EXIT_FAILURE);
				}
				free(tmp_buf.start);
			}
		}
	}
	else{
	/*	后期扩展Client端使用 */
	}

}

/**
 * @brief Client_Info 
 *	 打印客户端的IP地址和域名
 * @param client_addr
 */
void Client_Info(SA_in *client_addr)
{	
	struct hostent  *hp;
	char *haddrp;
	/*获取客户端的域名和IP地址*/
	hp = gethostbyaddr((const char *)&client_addr->sin_addr.s_addr,sizeof(client_addr->sin_addr.s_addr), AF_INET);
	if(hp == NULL){
		fprintf(stderr, "gethostbyaddr error");
		exit(EXIT_FAILURE);
	}
	haddrp = (char *)inet_ntoa(client_addr->sin_addr);
	printf("connnet to %s(%s).\n",hp->h_name, haddrp);
}

