
/*
*���ܣ����ĳ�ļ�������ļ���������ɾ��
*ִ�У�./inotify inotify_test/ &
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
	result = epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event); /*���opfd��EPOLLIN�¼����ļ��Ƿ������룩*/
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
	
	
	res = read(fd,buf,sizeof(buf));          /*��ȡ�ļ�,����¼���������buf�ᱣ��һ��inotify_event�ṹ�壨���ڸ�����Ϣ��
											*����¼�û������������
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
				int opfd = open(name,O_RDWR);    /*��Ҫ��ص��ļ�*/
			
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
		event_size = sizeof(struct inotify_event) + event->len;  /*ÿ��event�¼��ľ����С*/
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
	
	epfd = epoll_create(8);  /*����һ��epoll�ľ����size���������ں������������Ŀһ���ж��*/
	if(epfd == -1)
			printf("epoll_create error\n");

	fd = inotify_init();   /*����һ��inotify��ʵ����Ȼ�󷵻�inotify�¼����е��ļ�������*/
	if(fd == -1)
	{
		printf("inotify_init fault\n");
		return -1;
	}

	newfd = inotify_add_watch(fd,argv[1],IN_CREATE | IN_DELETE);  /*���һ������б�*/
	if(newfd == -1)
	{
		printf("file add watch fault\n");
		return -1;
	}

	add_to_epoll(fd,epfd);

	while(1)
	{
		event_num = epoll_wait(epfd,ret_event,16,-1);  /*���epfd����¼�������¼�������
														�򱣴��¼����ͣ������¼����ļ���fd��
														���ز����¼��ĸ���*/
		for(i=0;i<event_num;i++)
		{
			if(ret_event[i].data.fd == fd)
				watch_inotify_handle(fd,epfd);
			else
				{
					int len =read(ret_event[i].data.fd,buf,EPOLL_MAX_DATA);      /*ͨ��ret_event�����fd���ҵ������¼����ļ����Ӷ���ȡ����*/
					buf[len] = '\0';
					printf("data is %s\n",buf);
				}

		}
	}

	
	
	return 0;
}

