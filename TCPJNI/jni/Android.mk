LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Echo
LOCAL_SRC_FILES := Echo.cpp com_apress_echo_EchoClientActivity.h com_apress_echo_EchoServerActivity.h

include $(BUILD_SHARED_LIBRARY)
