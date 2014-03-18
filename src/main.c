#include "webcam.h"
#include "query.h"
#include "sock.h"
#include "th.h"

int cam_fd = -1;

void open_cam(int argc, char *argv[]);
void query_cam(int fd);

int main(int argc, char *argv[])
{
/* ignore SIGPIPE (send if transmitting to closed sockets) */
	signal(SIGPIPE, SIG_IGN);
	if (signal(SIGINT, signal_handler) == SIG_ERR){
		fprintf(stderr, "Register signal ERR\n");
		exit(1);
	}

	open_cam(argc, argv);
	//query_cam(cam_fd);

	int listenfd, connfd, port, clientlen;
	SA_in  clientaddr;
	port = atoi(argv[2]);
	clientlen = sizeof(clientaddr);
	listenfd = Open_listenfd(port);
	if(listenfd < 0){	
		perror("open_lisenfd error");	
		exit(EXIT_FAILURE);
	}


	pthread_t	camera_th, client_th;
	pthread_create(&camera_th, NULL, &cam_th, NULL);
	pthread_detach(camera_th);

	while(1){
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);	
		Client_Info(&clientaddr);
		//send_picture(connfd, &conf);	
		pthread_create(&client_th, NULL, &cli_th, &connfd);
		pthread_detach(client_th);
	}

	stop_capturing(cam_fd);
	cam_close(cam_fd);
	return 0;
}


void open_cam(int argc, char *argv[])
{
	if(argc != 3){
		fprintf(stderr, "usage: %s <video> <port>\n",argv[0]);
		exit(0);
	} 

	cam_fd = cam_open(argc, argv);
	conf.cam_fd = cam_fd;
	//conf.fmt = MJPEG;
	conf.fmt = YUYV;
	conf.width = WIDTH;
	conf.height = HEIGHT;
	conf.fps = 30;
	conf.n_buf = 5;

	cam_init(&conf);
	share_buf.len = tmp_buf.len;
	share_buf.start = (char*)malloc(sizeof(char)*tmp_buf.len);
	memset(share_buf.start, 0, sizeof(char)*share_buf.len);
	printf("share_buf.len=%d\n", share_buf.len);


	start_capturing(cam_fd);
}
void query_cam(int fd)
{
	query_cap(fd);	
	query_fps(fd);
	query_fmt(fd);
	query_fmt2(fd);
}
