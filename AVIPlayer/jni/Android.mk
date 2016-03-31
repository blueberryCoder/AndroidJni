LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := AVIPlayer
LOCAL_SRC_FILES := \
	Common.cpp \
	com_apress_aviplayer_AbstractPlayerActivity.cpp \
	com_apress_aviplayer_BitmapPlayerActivity.cpp
#使用AVILib 静态库
LOCAL_STATIC_LIBRARIES += avilib_static
#连接jnigraphics库
LOCAL_LDLIBS += -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
#引入AVILib库模块
$(call import-module, transcode-1.1.7/avilib)
