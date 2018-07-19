
#include <hardware/hardware.h>
#include <hardware/led.h>

#include <fcntl.h>

int close_led(struct hw_device_t* device)
{
	if(device)
		free(device);
	return 0;
}

int set_led(struct led_device_t* dev,int ledNO,int val)
{
	ioctl(dev->fd,val,ledNO);
	return 0;
}


//从模块module里将id（name）对应的硬件hw_device_t结构体找出，传给device
int led_open(const struct hw_module_t* module, const char* id,struct hw_device_t** device)
{
	struct led_device_t *dev = malloc(sizeof(led_device_t));
	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t*)module;
	dev->common.close = (int (*)(struct hw_device_t* device))close_led;
	dev->set_led = (int (*)(struct led_device_t* dev,int ledNO,int val))set_led;
	*device = (struct led_device_t *)dev;
	return 0;
	
}


/*2,实现led_module_t里的methods成员*/

static struct hw_module_methods_t led_module_methods = {
    .open = led_open,
};


/*1,hal层一般要构造一个hw_module_t结构体*/

struct led_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
		.version_major = 1,
        .version_minor = 0,
        .id = LED_HARDWARE_MODULE_ID,
        .name = "tiny4412_led",
        .author = "1459525376@qq.com",
        .methods = &led_module_methods,
    },
};


