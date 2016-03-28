#include <jni.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <com_example_threads_MainActivity.h>


static jmethodID gOnNativeMessage =NULL;

/**
 * java 虚拟机的指针
 */
static JavaVM* gJvm  =NULL;

/**
 * 对象的全局引用
 */
static jobject gObj=NULL;

//互斥实例
static pthread_mutex_t mutex;
//信号量
static sem_t sem ;

//原生worker 线程的参数
struct NativeWorkerArgs
{
	jint id ;
	jint iterations;
};
jint JNI_OnLoad(JavaVM* vm,void* reserved){
	gJvm = vm ;
	return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL Java_com_example_threads_MainActivity_nativeInit
  (JNIEnv * env, jobject obj){
//	if(0!=pthread_mutex_init(&mutex,NULL)){
//		//获取异常
//		jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
//		env->ThrowNew(exceptionClazz,"Uable to initialize mutext.");
//		goto exit;
//	}

	if(0!=sem_init(&sem,0,1)){
		//获取异常
				jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
				env->ThrowNew(exceptionClazz,"Uable to initialize mutext.");
	}
	if(NULL==gObj){
		gObj = env->NewGlobalRef(obj);
		if(NULL ==gObj){
			goto exit ;
		}
	}
	if(NULL==gOnNativeMessage){
		jclass clazz = env->GetObjectClass(obj);
		gOnNativeMessage = env->GetMethodID(clazz,"onNativeMessage","(Ljava/lang/String;)V");
		if(gOnNativeMessage==NULL){
			jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
			//抛出异常
			env->ThrowNew(exceptionClazz,"Uable to find method");
		}
	}
	exit:
	return ;
}
static void* nativeWorkerThread(void* args){


	JNIEnv* env=NULL;
	//将当前线程附加到Java虚拟机上
	//并且获得JNIEnv接口的指针
	if(0==gJvm->AttachCurrentThread(&env,NULL)){
		//获得原生worker线程参数
		NativeWorkerArgs* nativeWorkerArgs = (NativeWorkerArgs*) args ;
		Java_com_example_threads_MainActivity_nativeWorker(env,gObj,nativeWorkerArgs->id,nativeWorkerArgs->iterations);
		delete nativeWorkerArgs ;
		gJvm->DetachCurrentThread();
	}
	return (void *)1;
}

JNIEXPORT void JNICALL Java_com_example_threads_MainActivity_posixThreads
  (JNIEnv * env, jobject obj, jint threads, jint iterations){
	//线程句柄
	pthread_t* handles = new pthread_t[threads];
	for(jint i=0; i<threads;i++){
		NativeWorkerArgs* nativeWorkerArgs = new NativeWorkerArgs();
		nativeWorkerArgs->id =i ;
		nativeWorkerArgs->iterations = iterations ;
		//创建一个新线程
		int result = pthread_create(&handles[i],NULL,nativeWorkerThread,(void *)nativeWorkerArgs);
		if(0!=result){
			jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
			env->ThrowNew(exceptionClazz,"Unable to create thread.");
			goto exit ;
		}
	}

	//等待线程终止
//	for(jint i=0;i<threads;i++){
//		void* result = NULL;
//		//连接每个线程的句柄
//		if(0!=pthread_join(handles[i],&result)){
//			//获取异常类
//			jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
//			//抛出异常
//			env->ThrowNew(exceptionClazz,"Unable to join thread");
//		}else{
//			char message[26];
//			sprintf(message,"Worker %d returned %d",i,result);
//			jstring messageString = env->NewStringUTF(message);
//			//调用原生消息方法
//			env->CallVoidMethod(gObj,gOnNativeMessage,messageString);
//			//检查是否产生异常
//			if(NULL !=env->ExceptionOccurred()){
//				goto exit;
//			}
//		}
//	}

	exit:
	return ;
}


JNIEXPORT void JNICALL Java_com_example_threads_MainActivity_nativeWorker
  (JNIEnv * env, jobject obj, jint id, jint iterations){

//	//锁定互斥锁
//		if(0!= pthread_mutex_lock(&mutex)){
//			jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
//			//抛出异常
//			env->ThrowNew(exceptionClazz,"Unable to lock mutext");
//		}
	if(0!=sem_wait(&sem)){
			//获取异常
					jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
					env->ThrowNew(exceptionClazz,"Uable to initialize mutext.");
		}


	 for(jint i=0;i<iterations;i++){
		 char message[40];
		 sprintf(message,"Worker %d: Iterations %d",id,i);
		 jstring messageString = env->NewStringUTF(message);
		 env->CallVoidMethod(obj,gOnNativeMessage,messageString);
		 //检查是否产生异常
		 if(NULL !=env->ExceptionOccurred()) break;
		 sleep(1);
	 }

//	 //解锁互斥锁
//	 if(0!=pthread_mutex_unlock(&mutex)){
//		 jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
//		 			//抛出异常
//		 			env->ThrowNew(exceptionClazz,"Unable to unlock mutext");
//	 }
	 if(0!=sem_post(&sem)){
	 		//获取异常
	 				jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
	 				env->ThrowNew(exceptionClazz,"Uable to initialize mutext.");
	 	}
}

JNIEXPORT void JNICALL Java_com_example_threads_MainActivity_nativeFree
  (JNIEnv *env, jobject obj){
	if(NULL !=gObj){
		env->DeleteGlobalRef(gObj);
		gObj =NULL;
	}
//	if(0!=pthread_mutex_destroy(&mutex)){
//		jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
//		env->ThrowNew(exceptionClazz,"Unable to destroy mutext.");
//	}
	if(0!=sem_destroy(&sem)){
			//获取异常
					jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
					env->ThrowNew(exceptionClazz,"Uable to initialize mutext.");
		}
}
