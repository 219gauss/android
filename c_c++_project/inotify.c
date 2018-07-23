
/*
*功能：监控某文件里，有无文件被创建，删除
*执行：./inotify inotify_test/ &
*/

#include <stdio.h>
#include <sys/inotify.h>

int watch_inotify_handle(int fd)
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
											

	while(res >= sizeof(struct inotify_event))
	{
		event = (struct inotify_event*)(buf + pos);

		if(event->mask == IN_CREATE)		
			printf("create a file,name is %s\n",event->name);
		else if(event->mask == IN_DELETE)
			printf("delete a file,name is %s\n",event->name);

		event_size = sizeof(struct inotify_event) + event->len;  /*每个event事件的具体大小*/
		pos += event_size;
		res -+ event_size;
	}
	
}


int main(int argc,char** argv)
{
	int fd,newfd; 
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

	watch_inotify_handle(fd);

	inotify_rm_watch(fd,newfd);
	if(newfd == -1)
	{
		printf("inotify_rm_watch fault\n");
		return -1;
	}

	close(fd);
	return 0;

	
}

