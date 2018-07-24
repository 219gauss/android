/*
*���ܣ����ĳһ�ļ��Ƿ�������д�룬��������ӡ����
*ִ�У�./epoll epoll_test/1 &
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

	epfd = epoll_create(5);  /*����һ��epoll�ľ����size���������ں������������Ŀһ���ж��*/
	if(epfd == -1)
		printf("epoll_create error\n");

	opfd = open(argv[1],O_RDWR);    /*��Ҫ��ص��ļ�*/
	if(opfd == -1)
		printf("open file error\n");

	event.data.fd = opfd;
	event.events = EPOLLIN;
	epoll_ctl(epfd,EPOLL_CTL_ADD,opfd,&event); /*���opfd��EPOLLIN�¼����ļ��Ƿ������룩*/

	while(1)
	{
		event_num = epoll_wait(epfd,ret_event,1,-1);  /*���epfd����¼�������¼�������
														�򱣴��¼����ͣ������¼����ļ���fd��
														���ز����¼��ĸ���*/
		for(i=0;i<event_num;i++)
		{
			read(ret_event[i].data.fd,buf,EPOLL_MAX_DATA);      /*ͨ��ret_event�����fd���ҵ������¼����ļ����Ӷ���ȡ����*/
			printf("data is %s\n",buf);

		}
	}
	return 0;
}


