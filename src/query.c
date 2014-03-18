/**
 * @file query.c
 * @brief 摄像头查询模块接口文件
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#include "query.h"

/**
 * @brief query_cap 
 * 查询摄像头参数及驱动信息:打印struct v4l2_capability
 * @param fd	摄像头文件描述符
 */
//static Cap * query_cap(int fd);
void query_cap(int fd)
{
	Cap cap;
	CLEAR(cap);
	 if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0){
		 perror("query_cap()");
		exit(EXIT_FAILURE);
	 }
	printf("=========Camera Info==========\n");
	printf("cap.driver: %s.\n", cap.driver);
	printf("cap.card: %s.\n", cap.card);
	printf("cap.bus_info: %s.\n", cap.bus_info);
	printf("cap.version: %d.\n", cap.version);
	printf("cap.capabilities: 0x%x.\n", cap.capabilities);
	printf("cap.device_caps: 0x%x.\n", cap.device_caps);


}

/**
 * @brief query_priority 
 * 罗列程序优先级enum v4l2_priority
 * @param fd
 */
void query_priority(int fd);

/**
 * @brief query_input 
 *	查询当前摄像头输入设备的信息,G_INPUT和S_INPUT用来查询和选则当前的input
 *	打印struct v4l2_input 信息
 * @param fd
 */
void query_input(int fd)
{



}


/**
 * @brief query_fps 
 *	使用VIDIOC_G_PARM查询帧率信息
 * @param fd
 */ 
void query_fps(int fd)
{
	Fps fps;
	fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if( ioctl(fd, VIDIOC_G_PARM, &fps)){
		perror("query_fps:");
		exit(EXIT_FAILURE);
	}

	struct v4l2_captureparm P = fps.parm.capture;
	printf("=======FPS_Info========\n");
	printf("P.capability:%x \n", P.capability);
	printf("P.capturemode:%x \n", P.capturemode );
	printf("P.timeperframe.numerator:%d \n", P.timeperframe.numerator);
	printf("P.timeperframe.denominator:%d \n", P.timeperframe.denominator);
	printf("P.extendedmode %x \n", P.extendedmode);
	printf("P.readbuffers %x \n", P.readbuffers);
}

/**
 * @brief query_fmt 
 *	使用VIDIOC_G_FMT 查询摄像头格式信息
 * @param fd
 */
void query_fmt(int fd)
{
	Fmt fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(fd, VIDIOC_G_FMT, &fmt) < 0){
		perror("query_fmt");
		exit(EXIT_FAILURE);
	}
	struct v4l2_pix_format P = fmt.fmt.pix;
	printf("=======Format Info========\n");
	printf("width: %d \n", P.width);
	printf("height: %d \n", P.height);
	printf("pixelormat:'%c%c%c%c' \n",P.pixelformat & 0xFF, (P.pixelformat>>8)& 0xFF, (P.pixelformat>>16)& 0xFF, (P.pixelformat>>24)& 0xFF);
	
}

/**
 * @brief query_fmt2 
 *	使用VIDIOC_ENUM_FMT查询摄像头信息
 * @param fd
 */
void query_fmt2(int fd)
{
	Fmtd fmtd;
	CLEAR(fmtd);
	fmtd.index = 0;
	fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while( -1 != ioctl(fd, VIDIOC_ENUM_FMT, &fmtd)){
		printf("query_fmt2: %d %s.\n", fmtd.index, fmtd.description);
		fmtd.index++;
	}

}

/**
 * @brief enumerate_ctrl 
 * 使用VIDIOC_QUERYCTRL查询摄像头可控制信息
 * @param fd
 */
void enumerate_ctrl(int fd);


