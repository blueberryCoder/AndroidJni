LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := CPlusPlusSupport
LOCAL_SRC_FILES := CPlusPlusSupport.cpp

#添加异常支持  也可在Application.mk文件中添加 -fexceptions
LOCAL_CPP_FEATURES += exceptions

#启动GNU STL 调试模式 也可以使用  __gnu_debug 命名空间代替std命名空间来为大多数的容器提供调试模式启动副本 \
例如: __gun_debug::vector v; 也可以通过预处理表示_GLIBCXX_DEBUG来控制调试模式 \
\
\
STLport 调试模式: 添加LOCAL_CFLAGS += -D_STLP_DEBUG \
将调试模式的信息重定向到Android 日志 : \
添加: LCOAL_CFLAGS += -D_STLP_DEBUG_MESSAGE \
LOCAL_CFLAGS += -llog \
实现全局函数将__stl_debug_message 将错误信息重定向到Android日志: \
#include <stdarg.h> \
#include <android/log.h> \
... \
void __stl_debug_message(const char* format_str,...) \
{ \
   va_list ap; \
   va_start(ap,format_str); \
   __android_log_vprint(ANDROIR_LOG_FATAL,"STLport",format_str,ap); \
   va_end(ap); \
}



LOCAL_CFLAGS += -D_GLIBCXX_DEBUG

include $(BUILD_SHARED_LIBRARY)
