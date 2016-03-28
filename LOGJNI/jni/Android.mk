# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hello-jni
LOCAL_SRC_FILES := my_log.h hello-jni.c 

#定义日志标签
MY_LOG_TAG := \"hello-jni\"

#定义基于构建类型的默认日志等级
ifneq ($(APP_OPTIM),release)
	MY_LOG_LEVEL := MY_LOG_LEVEL_ERROR
else 
	MY_LOG_LEVEL := MY_LOG_LEVEL_VERBOSE
endif

#追加编译标记
LOCAL_CFLAGS += -DMY_LOG_TAG=$(MY_LOG_TAG)
LOCAL_CFLAGS += -DMY_LOG_LEVEL=$(MY_LOG_LEVEL)
LOCAL_CFLAGS += -g
#动态链接原生模块与日志库
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)
