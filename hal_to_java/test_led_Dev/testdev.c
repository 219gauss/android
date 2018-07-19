
#include <stdio.h>
#include <fcntl.h>

#define DEVICE_NAME "/dev/leds"

int main(void)
{
	int fd = -1;

	fd = open(DEVICE_NAME,O_RDWR);
	if(fd == -1)
	{
		printf("failed to open %s file",DEVICE_NAME);
		return -1;
	}

	ioctl(fd,1,2);
	close(fd);
	return 0;
}



