
/*
*���ܣ����ĳ�ļ�������ļ���������ɾ��
*ִ�У�./inotify inotify_test/ &
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
	
	res = read(fd,buf,sizeof(buf));          /*��ȡ�ļ�,����¼���������buf�ᱣ��һ��inotify_event�ṹ�壨���ڸ�����Ϣ��
											*����¼�û������������
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

		event_size = sizeof(struct inotify_event) + event->len;  /*ÿ��event�¼��ľ����С*/
		pos += event_size;
		res -+ event_size;
	}
	
}


int main(int argc,char** argv)
{
	int fd,newfd; 
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

