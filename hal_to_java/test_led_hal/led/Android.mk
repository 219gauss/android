

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := led.default
LOCAL_MODULE_RELATIVE_PATH := hardware\libhardware
LOCAL_SRC_FILES := led.cs
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)




