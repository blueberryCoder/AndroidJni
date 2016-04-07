extern "C"{
#include <avilib.h>
}
#include <com_apress_aviplayer_NativeWindowPlayerActivity.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <Common.h>

JNIEXPORT jboolean JNICALL Java_com_apress_aviplayer_NativeWindowPlayerActivity_render
  (JNIEnv * env, jclass clazz, jlong avi, jobject surface){
	jboolean isFrameRead = JNI_FALSE;
	long frameSize =0;
	int keyFrame =0;
	//从surface 中获取原生window
	ANativeWindow* nativeWidnow = ANativeWindow_fromSurface(env,surface);
	if(0== nativeWidnow){
		ThrowException(env,"java/io/RuntiemException","Uable to get native window from surface.");
		goto exit;
	}
	//锁定原生window 并访问原始Buffer
	ANativeWindow_Buffer windowBuffer ;
	if(0>ANativeWindow_lock(nativeWidnow,&windowBuffer,0)){
		ThrowException(env,"java/io/RuntimeException","Uable to lock native widnow.");
		goto release ;
	}
	//将AVI帧的比特流读至原始缓冲区
	frameSize = AVI_read_frame((avi_t*)avi,(char*) windowBuffer.bits,&keyFrame);
	//检查帧是否被成功读取
	if(0<frameSize){
		isFrameRead = JNI_FALSE;
	}
	//解锁并且输出缓冲区显示
	if(0>ANativeWindow_unlockAndPost(nativeWidnow)){
		ThrowException(env,"java/io/RuntimeException","Unable to unlock and post to native window.");
		goto release;
	}
	release:
	//释放原生window
	ANativeWindow_release(nativeWidnow);
	exit:
	return isFrameRead;

}

JNIEXPORT void JNICALL Java_com_apress_aviplayer_NativeWindowPlayerActivity_init
  (JNIEnv * env, jclass clazz, jlong avi, jobject surface){
	//从surface 中获取原生window
	ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env,surface);
	if(0==nativeWindow){
		ThrowException(env,"java/io/RuntimeException","Unable to get native window surface .");
		goto exit;
	}

	//设置buffer大小为AVI 视频帧的分辨率
	//如果和window 的物理大小不一致
	//Buffer 会被缩放来匹配这个大小
	if(0>ANativeWindow_setBuffersGeometry(nativeWindow,AVI_video_width(((avi_t *)avi)),AVI_video_height((avi_t*)avi)
			,WINDOW_FORMAT_RGB_565)){
		ThrowException(env,"java/io/RuntimeException","Uable to set buffers geometry.");
	}
	//释放原生window
	ANativeWindow_release(nativeWindow);
	nativeWindow = 0 ;
	exit:
	return ;
}
