LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := AVIPlayer
LOCAL_SRC_FILES := \
	Common.cpp \
	com_apress_aviplayer_AbstractPlayerActivity.cpp \
	com_apress_aviplayer_BitmapPlayerActivity.cpp \
	com_apress_aviplayer_OpenGLPlayerActivity.cpp \
	com_apress_aviplayer_NativeWindowPlayerActivity.cpp
#使用AVILib 静态库
LOCAL_STATIC_LIBRARIES += avilib_static
#连接jnigraphics库  
#连接 openGL ES 1.0 库 ,如果想使用openGL ES 2.0 则使用 -lGLESv2
#连接原生WINDOW API库  -landroid
LOCAL_LDLIBS += -ljnigraphics  -lGLESv1_CM -landroid -llog

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES
include $(BUILD_SHARED_LIBRARY)
#引入AVILib库模块
$(call import-module, transcode-1.1.7/avilib)
