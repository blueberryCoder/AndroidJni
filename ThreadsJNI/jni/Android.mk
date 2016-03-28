LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Threads
LOCAL_SRC_FILES := Threads.cpp com_example_threads_MainActivity.h

include $(BUILD_SHARED_LIBRARY)
