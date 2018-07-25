
/*
*功能：监控某文件里，有无文件被创建，删除
*执行：./inotify inotify_test/ &
*mkfifo inotify_epoll_test/17
*echo 123 > inotify_epoll_test/17
*/

#include <stdio.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


#define EPOLL_MAX_EVENT 16
#define EPOLL_MAX_DATA 500

char *base_dir;
static char *epoll_files[1000];


int add_to_epoll(int fd,int epfd)
{
	int result;
	struct epoll_event event;
	memset(&event, 0, sizeof(event));

	event.data.fd = fd;
	event.events = EPOLLIN;
	result = epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event); /*添加opfd的EPOLLIN事件（文件是否有输入）*/
	return result;
}

void rm_from_epoll(int fd, int epfd)
{
	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
}

int get_epoll_fd_for_name(char *name)
{
	int i;
	char name_to_find[500];
	sprintf(name_to_find, "%s/%s", base_dir, name);

	for (i = 0; i < 1000; i++)
	{
		if (!epoll_files[i])
			continue;
		
		if (!strcmp(epoll_files[i], name_to_find))
			return i;
	}
	return -1;
}


int watch_inotify_handle(int fd,int epfd)
{
	int res;
	char buf[512];
	struct inotify_event *event;
	int pos = 0; 
	int event_size;
	
	
	res = read(fd,buf,sizeof(buf));          /*读取文件,如果事件发生，则buf会保存一个inotify_event结构体（存在各种信息）
											*如果事件没发生，则阻塞
											*/
	if(res < sizeof(struct inotify_event))
	{
		printf("read fault\n");
		return -1;
	}
											

	while(res >= (int)sizeof(*event))
	{	
		event = (struct inotify_event*)(buf + pos);

		if(event->len)
		{
			if(event->mask & IN_CREATE)
			{
				char *name = malloc(512);
			
				printf("create a file,name is %s\n",event->name);

				sprintf(name,"%s%s",base_dir,event->name);
				printf("create a file,name is %s\n",name);
				int opfd = open(name,O_RDWR);    /*打开要监控的文件*/
			
				add_to_epoll(opfd,epfd);
				epoll_files[opfd] = name;
			}
			else
			{
				int tmpFd = get_epoll_fd_for_name(event->name);
				if(tmpFd >= 0)
				{
					printf("delete a file,name is %s\n",event->name);
					rm_from_epoll(tmpFd,epfd);
					free(epoll_files[tmpFd]);
				}
			}
		}
		event_size = sizeof(struct inotify_event) + event->len;  /*每个event事件的具体大小*/
		pos += event_size;
		res -= event_size;
	}
	return 0;
}


int main(int argc,char** argv)
{

	int epfd,event_num;
	struct epoll_event ret_event[EPOLL_MAX_EVENT];
	char buf[EPOLL_MAX_DATA];

	int fd,newfd;
	int i;

	base_dir = argv[1];
	
	epfd = epoll_create(8);  /*创建一个epoll的句柄，size用来告诉内核这个监听的数目一共有多大*/
	if(epfd == -1)
			printf("epoll_create error\n");

	fd = inotify_init();   /*创建一个inotify的实例，然后返回inotify事件队列的文件描述符*/
	if(fd == -1)
	{
		printf("inotify_init fault\n");
		return -1;
	}

	newfd = inotify_add_watch(fd,argv[1],IN_CREATE | IN_DELETE);  /*添加一个检测列表*/
	if(newfd == -1)
	{
		printf("file add watch fault\n");
		return -1;
	}

	add_to_epoll(fd,epfd);

	while(1)
	{
		event_num = epoll_wait(epfd,ret_event,16,-1);  /*监控epfd里的事件，如果事件产生，
														则保存事件类型，产生事件的文件的fd，
														返回产生事件的个数*/
		for(i=0;i<event_num;i++)
		{
			if(ret_event[i].data.fd == fd)
				watch_inotify_handle(fd,epfd);
			else
				{
					int len =read(ret_event[i].data.fd,buf,EPOLL_MAX_DATA);      /*通过ret_event保存的fd，找到产生事件的文件，从而读取数据*/
					buf[len] = '\0';
					printf("data is %s\n",buf);
				}

		}
	}

	
	
	return 0;
}

