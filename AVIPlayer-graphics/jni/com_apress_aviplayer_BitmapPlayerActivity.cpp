extern "C"{
#include <avilib.h>
}
#include <Common.h>
#include <android/bitmap.h>

#include <com_apress_aviplayer_BitmapPlayerActivity.h>

JNIEXPORT jboolean JNICALL Java_com_apress_aviplayer_BitmapPlayerActivity_render
  (JNIEnv * env, jclass clazz, jlong avi, jobject bitmap){
	jboolean isFrameRead = JNI_FALSE ;
	char* frameBuffer = 0;
	long frameSize = 0;
	int keyFrame=0;
	//锁定bitmap得到的raw byte
	if(0>AndroidBitmap_lockPixels(env,bitmap,(void**) &frameBuffer)){
		ThrowException(env,"java/io/IOException","Unable to lock pixels.");
		goto exit;
	}

	//将AVI帧byte读到bitmap中
	frameSize = AVI_read_frame((avi_t*)avi,frameBuffer,&keyFrame);
	//解锁bitmap
	if(0>AndroidBitmap_unlockPixels(env,bitmap)){
		ThrowException(env,"java/io/IOException","Uable to unlock pixels.");
		goto exit;
	}

	//检查帧是否成功读取
	if(0<frameSize){
		isFrameRead = JNI_TRUE;
	}
	exit:
	return isFrameRead ;
}

