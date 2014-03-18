/**
 * @file webcam.c
 * @brief 摄像头配置及操作接口实现文件
 * @author Jack Jin - gjinjian@gmail.com
 * @version v0.1
 * @date 2013-09-11
 */

#include "webcam.h"
#include "huffman.h"


#if 0
ReqBuf	reqbuf;		//内核空间
Vbuf	vbuf;		//队列
Buf		*bufs;		//用户映射空间
Buf     tmp_buf;	//一帧图片空间
#endif

/**
 * @brief cam_open 
 * 打开摄像头
 * @param argc
 * @param argv
 * @return 文件描述符
 */
int  cam_open(int argc,char *argv[])
{
	int fd;
	if(argc < 2){
		fprintf(stderr, "Usage: %s [path].\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	fd = open(argv[1], O_RDWR, 0);
	if(fd < 0 ){
		fprintf(stderr, "open: %s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return fd;
}

/**
 * @brief cam_close 
 *  	关闭摄像头、释放用户缓冲空间
 * @param fd
 */
void cam_close(int fd)
{
	close(fd);
	fd = -1;
	cam_free();

}

/**
 * @brief cam_init 
 *	查询并设置设备属性、格式、帧率,内存映射设置	
 * @param fd		摄像头
 */
void cam_init(Cam_conf *conf)
{
	set_fmt(conf);
	set_fps(conf),
	set_frame_size(conf);
	init_mmap(conf);
}

/**
 * @brief cam_free 释放用户缓冲空间
 */
static void cam_free(void)
{
	int i;
	for(i=0; i < reqbuf.count; i++){
		munmap(bufs[i].start, bufs[i].len);
	}
}

/**
 * @brief start_capturing 
 *	开启视频流
 * @param fd
 */
void start_capturing(int fd)
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(fd, VIDIOC_STREAMON, &type)){
		perror("VIDIOC_STREAMON");	
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief stop_capturing 
 *	关闭视频流
 * @param fd
 */
void stop_capturing(int fd)
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(fd, VIDIOC_STREAMOFF, &type)){
		perror("VIDIOC_STREAMOFF");	
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief set_fmt  设置视频的帧格式
 * 根据摄像头像素参数设置Logitech C210:640*480
 * @param conf
 */
static void set_fmt(Cam_conf *conf )
{
	Fmt cam_fmt;
	CLEAR(cam_fmt);
	cam_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	cam_fmt.fmt.pix.width = conf->width;
	cam_fmt.fmt.pix.height= conf->height;
	//MJPEG 压缩格式
	cam_fmt.fmt.pix.pixelformat= conf->fmt ;
	cam_fmt.fmt.pix.field = V4L2_FIELD_ANY;
 
	if(ioctl(conf->cam_fd, VIDIOC_S_FMT, &cam_fmt) < 0){
		fprintf(stderr, "set_fmt():VIDIOC_S_FMT error:%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

}


/**
 * @brief set_fps 
 *	 设置帧率，C210 最大帧率30fps
 * @param fd
 * @param fps
 */
static void set_fps(Cam_conf *conf)
{
	Fps cam_fps;
	CLEAR(cam_fps);
	cam_fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	cam_fps.parm.capture.capturemode = 1;	//设置为高清模式
	cam_fps.parm.capture.timeperframe.numerator = 1;
	cam_fps.parm.capture.timeperframe.denominator = conf->fps;
	if(-1 == ioctl(conf->cam_fd, VIDIOC_S_PARM, &cam_fps)){
		perror("VIDIOC_S_PARM");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief init_mmap 
 *	在内核空间申请帧缓冲、映射到用户空间、并入缓冲队列
 * @param fd
 * @param num
 */
static void init_mmap(Cam_conf *conf )
{
	CLEAR(reqbuf);
    reqbuf.count = conf->n_buf;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	if(-1 == ioctl(conf->cam_fd, VIDIOC_REQBUFS, &reqbuf)){
		if (errno == EINVAL)
			printf ("Video capturing or mmap-streaming is not supported\n");
		else
			perror ("VIDIOC_REQBUFS");

		exit (EXIT_FAILURE);
 	}
	/* We want at least five buffers. */
	if (reqbuf.count < 5) {
		/* You may need to free the buffers here. */
		printf ("Not enough buffer memory\n");
		exit (EXIT_FAILURE);
	}  
#if 0
	printf("----------------------------------\n");
	printf("reqbuf.count = %d\n", reqbuf.count);
	printf("----------------------------------\n");
#endif
	bufs = calloc(reqbuf.count, sizeof(Buf));
	assert(bufs != NULL);
	
	//映射及入队【VIDIOC_QUERYBUF 来获得设备内存中的这些个 buffer 物理地址】
	unsigned int i;
	for(i=0; i< reqbuf.count; i++){
		CLEAR(vbuf);
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;	
		vbuf.index = i;

		if(-1 == ioctl(conf->cam_fd, VIDIOC_QUERYBUF, &vbuf)){
			perror("VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}
		/* remember for munmap() */
		bufs[i].len = vbuf.length;	
		bufs[i].start = mmap(NULL, vbuf.length, 
						PROT_READ | PROT_WRITE,
						MAP_SHARED,
						conf->cam_fd, vbuf.m.offset);
		if(MAP_FAILED == bufs[i].start){
			/* If you do not exit here you should unmap() and free()  the buffers mapped so far. */
			perror ("mmap");
			exit (EXIT_FAILURE);
		}
//		printf("bufs[%d].len = %d\n", i, bufs[i].len);
				
		//入队
		if(-1 == ioctl(conf->cam_fd, VIDIOC_QBUF, &vbuf)){
			perror("VIDIOC_QBUF");
			exit(EXIT_FAILURE);
		}
	}

}


/**
 * @brief set_frame_size 
 * 设置图片大小
 * @param conf
 * @return 
 */
static int set_frame_size(Cam_conf *conf)
{
	if(conf == NULL){
		return -1;
	}
	tmp_buf.len = WIDTH*HEIGHT<<1;
	switch(conf->fmt){
		case MJPEG:
				tmp_buf.start = (unsigned char *)calloc(1, tmp_buf.len);   
				if(!tmp_buf.start)
					return -1;
				break;
		case YUYV:
				tmp_buf.start = (unsigned char *)calloc(1, tmp_buf.len*3 / 2);   
				if(!tmp_buf.start)
					return -1;
				break;
		default:	
			perror("set_frame_size");		
			return -1;
			break;
	}
	return tmp_buf.len;
}
#define HEADERFREAM	0xaf
/**
 * @brief get_frame 
 *		出队一帧数据，并入队
 * @param conf
 * @return 成功0，失败-1 
 */
int  get_frame(Cam_conf *conf)
{
	CLEAR(vbuf);
	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuf.memory = V4L2_MEMORY_MMAP;
	if(ioctl(conf->cam_fd, VIDIOC_DQBUF, &vbuf) < 0){
		perror("get_frame() VIDIOC_DQBUF");
		exit(EXIT_FAILURE);
	}	 
//	printf("byteused:%d.\n", vbuf.bytesused);

	if(vbuf.bytesused <= HEADERFREAM){
		return -1;
	}
#if 0	
	if(set_frame_size(conf) < 0){
		perror("get_frame");
		return -1;
	}
#endif

#if 0
	tmp_buf.start = (char *)malloc(sizeof(char)*(vbuf.bytesused));
	memset(tmp_buf.start, 0, sizeof(char)*(vbuf.bytesused));
	tmp_buf.len = vbuf.bytesused;
#endif
	switch(conf->fmt){
		case MJPEG:
			memcpy(tmp_buf.start, bufs[vbuf.index].start, vbuf.bytesused);
			break;
		case YUYV:
			yuv_to_jpeg(bufs[vbuf.index].start, tmp_buf.start, vbuf.bytesused, 62);	//?? 
			break;
		default:
			printf("Unknown Format.\n");
			break;
	}
	if(ioctl(conf->cam_fd, VIDIOC_QBUF, &vbuf) < 0){
		perror("VIDIOC_QBUF");
		free(tmp_buf.start);
		return -1;
	} 
	return 0;
}

/**
 * @brief process 
 *		保存一帧图片到本地目录
 * @param conf
 */
void process(Cam_conf *conf)
{
	int pic_fd = open("webcam.jpeg", O_RDWR|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
	if(pic_fd < 0){
		perror("open()");
		exit(EXIT_FAILURE);
	}
	get_frame(conf);
	print_picture(pic_fd, tmp_buf.start, tmp_buf.len);	
	free(tmp_buf.start);
	close(pic_fd);
}

/**
 * @brief is_huffman 
 * 判断图片是否为huffman编码
 * @param buf
 * @return 
 */
static int is_huffman(unsigned char *buf)
{
  unsigned char *ptbuf;
  int i = 0;
  ptbuf = buf;
  while (((ptbuf[0] << 8) | ptbuf[1]) != 0xffda) {
    if (i++ > 2048)
      return 0;
    if (((ptbuf[0] << 8) | ptbuf[1]) == 0xffc4)
      return 1;
    ptbuf++;
  }
  return 0;

}

/**
 * @brief print_picture 
 *  将图片输出到fd文件中
 * @param fd
 * @param buf
 * @param size
 * @return 
 */
int print_picture(int fd, unsigned char *buf, int size)
{
    unsigned char *ptdeb, *ptcur = buf;
    int sizein;
	
    if (!is_huffman(buf)) {
		printf("not huffman.\n");
        ptdeb = ptcur = buf;
        while (((ptcur[0] << 8) | ptcur[1]) != 0xffc0)
            ptcur++;
        sizein = ptcur - ptdeb;
        if( write(fd, buf, sizein) <= 0) return -1;
        if( write(fd, dht_data, DHT_SIZE) <= 0) return -1;
        if( write(fd, ptcur, size - sizein) <= 0) return -1;
    } else {
        if( write(fd, ptcur, size) <= 0) return -1;
    }

    return 0;


}



