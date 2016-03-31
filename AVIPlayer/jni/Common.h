#pragma once
#include <jni.h>
void ThrowException(JNIEnv* env,const char* className,const char* message);
