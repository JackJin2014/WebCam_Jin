/**
 * @file th.c
 * @brief 多线程的并发处理接口实现
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#include "th.h"
#include "webcam.h"
#include "sock.h"


int	cam_stop = 0;
pthread_mutex_t	_db= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	_db_update= PTHREAD_COND_INITIALIZER;


/**
 * @brief cam_th 
 *	摄像头采集线程
 * @param arg
 * @return 
 */
void *cam_th(void *arg)
{
	//printf("cam_th\n");
	while(!cam_stop){
		get_frame(&conf);		
		pthread_mutex_lock(&_db);
			memcpy(share_buf.start, tmp_buf.start, tmp_buf.len);
		pthread_cond_broadcast(&_db_update);
		pthread_mutex_unlock(&_db);
	}

}


/**
 * @brief cli_th 
 *	客户端处理线程
 * @param arg
 * @return 
 */
void *cli_th(void *arg)
{
	printf("cli_th\n");
	int confd = *((int*)arg);
	char readbuf[MAX]; CLEAR(readbuf);
	char writebuf[MAX]; CLEAR(writebuf);
	Buf  frame;
	frame.len = tmp_buf.len;					
	frame.start = (char *)malloc(sizeof(char)*(frame.len));		//不要放在循环里面,否则内存会泄漏

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
			//printf("============snapshot=============\n");
			sprintf(writebuf, "--www.briup.com\nContent-type:image/jpeg\nContent-Length: %d\n\n", share_buf.len+ DHT_SIZE);		
			if(write(confd, writebuf, strlen(writebuf)) != strlen(writebuf)) {
				perror("send_picture: write1()");
				exit(EXIT_FAILURE);
			}
			pthread_cond_wait(&_db_update, &_db);
				memset(frame.start, 0, sizeof(char)*(frame.len));
				memcpy(frame.start, share_buf.start, frame.len);
			pthread_mutex_unlock(&_db);
	
			//print_picture(confd, frame.start, frame.len);
			if(write(confd, frame.start, frame.len) < 0){
				perror("write a frame");
				exit(EXIT_FAILURE);
			}
		}else{//浏览器请求视频
			while(!cam_stop){
		//printf("============stream=============\n");
				sprintf(writebuf, "--www.briup.com\nContent-type:image/jpeg\nContent-Length: %d\n\n", share_buf.len+ DHT_SIZE);		
				if(write(confd, writebuf, strlen(writebuf)) != strlen(writebuf))	{
					perror("send_picture: write2()");
					exit(EXIT_FAILURE);
				}  
				pthread_cond_wait(&_db_update, &_db);
					memset(frame.start, 0, sizeof(char)*(frame.len));
					memcpy(frame.start, share_buf.start, frame.len);
				pthread_mutex_unlock(&_db);

				//print_picture(confd, frame.start, frame.len);
				if(write(confd, frame.start, frame.len) <= 0){
					perror("write a frame");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	else{
	/*	后期扩展Client端使用 */
	}

	free(frame.start);
	frame.start = NULL;
}


/**
 * @brief signal_handler 
 *	SIGINT 处理函数：停止捕获、关闭摄像头、释放动态分配的内存、释放锁和释放条件变量
 * @param signo
 */
void signal_handler(int signo)
{
	cam_stop = 1;
	fprintf(stderr, "\nshutdown...\n");
	usleep(1000 * 1000);

	stop_capturing(conf.cam_fd);
	cam_close(conf.cam_fd);
	free(share_buf.start);
	share_buf.start = NULL;

	pthread_cond_destroy(&_db_update);
	pthread_mutex_destroy(&_db);
	exit(0);
}

