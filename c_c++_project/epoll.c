/*
*功能：监控某一文件是否有数据写入，如果有则打印出来
*执行：./epoll epoll_test/1 &
*/


#include <stdio.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define EPOLL_MAX_EVENT 4
#define EPOLL_MAX_DATA 500

int main(int argc,char** argv)
{
	int epfd,opfd,i,event_num;
	struct epoll_event event;
	struct epoll_event ret_event[EPOLL_MAX_EVENT];
	char buf[EPOLL_MAX_DATA];

	epfd = epoll_create(5);  /*创建一个epoll的句柄，size用来告诉内核这个监听的数目一共有多大*/
	if(epfd == -1)
		printf("epoll_create error\n");

	opfd = open(argv[1],O_RDWR);    /*打开要监控的文件*/
	if(opfd == -1)
		printf("open file error\n");

	event.data.fd = opfd;
	event.events = EPOLLIN;
	epoll_ctl(epfd,EPOLL_CTL_ADD,opfd,&event); /*添加opfd的EPOLLIN事件（文件是否有输入）*/

	while(1)
	{
		event_num = epoll_wait(epfd,ret_event,1,-1);  /*监控epfd里的事件，如果事件产生，
														则保存事件类型，产生事件的文件的fd，
														返回产生事件的个数*/
		for(i=0;i<event_num;i++)
		{
			read(ret_event[i].data.fd,buf,EPOLL_MAX_DATA);      /*通过ret_event保存的fd，找到产生事件的文件，从而读取数据*/
			printf("data is %s\n",buf);

		}
	}
	return 0;
}


