extern "C"{
#include <avilib.h>
}
#include "Common.h"
#include "com_apress_aviplayer_AbstractPlayerActivity.h"

JNIEXPORT jlong JNICALL Java_com_apress_aviplayer_AbstractPlayerActivity_open
  (JNIEnv * env , jclass clazz, jstring fileName){
	avi_t* avi =0 ;
	//获取文件名赋给C的一个字符串变量
	const char* cFileName = env->GetStringUTFChars(fileName,0);
	if(0==cFileName){
		goto exit;
	}
	//打开AVI文件
	avi = AVI_open_input_file(cFileName,1);
	//释放文件名字
	env->ReleaseStringUTFChars(fileName,cFileName);
	//如果AVI文件不能打开则抛出一个异常
	if(0==avi){
		ThrowException(env,"java/io/IOException",AVI_strerror());
	}
	exit:
	return (jlong)avi ;
}

JNIEXPORT jint JNICALL Java_com_apress_aviplayer_AbstractPlayerActivity_getWidth
  (JNIEnv *, jclass, jlong avi){
	return AVI_video_width((avi_t *) avi);
}

JNIEXPORT jint JNICALL Java_com_apress_aviplayer_AbstractPlayerActivity_getHeith
  (JNIEnv *, jclass, jlong avi){
	return AVI_video_height((avi_t *) avi);
}

JNIEXPORT jdouble JNICALL Java_com_apress_aviplayer_AbstractPlayerActivity_getFrameRate
  (JNIEnv *, jclass, jlong avi){
	return AVI_frame_rate((avi_t*) avi);
}

JNIEXPORT void JNICALL Java_com_apress_aviplayer_AbstractPlayerActivity_close
  (JNIEnv *, jclass, jlong  avi){
	AVI_close((avi_t *) avi);
}
