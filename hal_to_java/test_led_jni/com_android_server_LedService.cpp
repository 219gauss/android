
#define LOG_TAG "LedService"

#include "jni.h"
#include "JNIHelp.h"

#include <utils/Log.h>
#include <hardware/led.h>

#include <stdio.h>

namespace android
{
	struct led_device_t *led_device = NULL;
	
	static jboolean initled(JNIEnv *env, jobject clazz)
	{
		struct hw_module_t *module;
		struct hw_device_t *device;
		
		jint err;

	    //获取模块 hw_get_module
	    err = hw_get_module(LED_HARDWARE_MODULE_ID, (const struct hw_module_t * *) module);
		if(err == 0)
		{
			//获取设备 
			err = module->methods->open(module,NULL,&device);
			if(err == 0)
			{
				led_device = (struct led_device_t *)device;
				return 0;
			}
			return -1;
		}
		return 0;
	}

	static jint setled(JNIEnv *env, jobject clazz, jint ledNo,jint val)
	{
		int no = ledNo;
		int value = val;
		led_device->set_led(led_device,no,value);
		return 0;
	}

	static JNINativeMethod method_table[] = {
	    { "openLed", "()Z", (void*)initled },
	    { "setLed", "(II)I", (void*)setled },
	};

	int register_android_server_LedService(JNIEnv *env)
	{
	    return jniRegisterNativeMethods(env, "com/android/server/LedService",
	            method_table, NELEM(method_table));
	}

	};
