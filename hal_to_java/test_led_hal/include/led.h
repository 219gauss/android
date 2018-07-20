
#ifndef _LED_H
#define _LED_H

#include <hardware/hardware.h>


#define LED_HARDWARE_MODULE_ID "led"



struct led_module_t {
    struct hw_module_t common;
} ;

struct led_device_t {
    struct hw_device_t common;
    int (*set_led)(struct led_device_t* dev,int ledNO,int val);
};



#endif

