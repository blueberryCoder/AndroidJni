LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)


LOCAL_MODULE    := WAVPlayer
LOCAL_SRC_FILES := WAVPlayer.cpp 


#使用WAVLib 静态库
LOCAL_STATIC_LIBRARIES += wavlib_static

#与OpenSL ES 连接
LOCAL_LDLIBS += -lOpenSLES -llog
LOCAL_CPP_FEATURES += exceptions

include $(BUILD_SHARED_LIBRARY)

#引入 WAVLib 库模块
$(call import-module, transcode-1.1.7/avilib)
