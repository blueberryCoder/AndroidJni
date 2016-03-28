#include <jni.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <com_example_threads_MainActivity.h>


static jmethodID gOnNativeMessage =NULL;

/**
 * java �������ָ��
 */
static JavaVM* gJvm  =NULL;

/**
 * �����ȫ������
 */
static jobject gObj=NULL;

//����ʵ��
static pthread_mutex_t mutex;
//�ź���
static sem_t sem ;

//ԭ��worker �̵߳Ĳ���
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
//		//��ȡ�쳣
//		jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
//		env->ThrowNew(exceptionClazz,"Uable to initialize mutext.");
//		goto exit;
//	}

	if(0!=sem_init(&sem,0,1)){
		//��ȡ�쳣
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
			//�׳��쳣
			env->ThrowNew(exceptionClazz,"Uable to find method");
		}
	}
	exit:
	return ;
}
static void* nativeWorkerThread(void* args){


	JNIEnv* env=NULL;
	//����ǰ�̸߳��ӵ�Java�������
	//���һ��JNIEnv�ӿڵ�ָ��
	if(0==gJvm->AttachCurrentThread(&env,NULL)){
		//���ԭ��worker�̲߳���
		NativeWorkerArgs* nativeWorkerArgs = (NativeWorkerArgs*) args ;
		Java_com_example_threads_MainActivity_nativeWorker(env,gObj,nativeWorkerArgs->id,nativeWorkerArgs->iterations);
		delete nativeWorkerArgs ;
		gJvm->DetachCurrentThread();
	}
	return (void *)1;
}

JNIEXPORT void JNICALL Java_com_example_threads_MainActivity_posixThreads
  (JNIEnv * env, jobject obj, jint threads, jint iterations){
	//�߳̾��
	pthread_t* handles = new pthread_t[threads];
	for(jint i=0; i<threads;i++){
		NativeWorkerArgs* nativeWorkerArgs = new NativeWorkerArgs();
		nativeWorkerArgs->id =i ;
		nativeWorkerArgs->iterations = iterations ;
		//����һ�����߳�
		int result = pthread_create(&handles[i],NULL,nativeWorkerThread,(void *)nativeWorkerArgs);
		if(0!=result){
			jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
			env->ThrowNew(exceptionClazz,"Unable to create thread.");
			goto exit ;
		}
	}

	//�ȴ��߳���ֹ
//	for(jint i=0;i<threads;i++){
//		void* result = NULL;
//		//����ÿ���̵߳ľ��
//		if(0!=pthread_join(handles[i],&result)){
//			//��ȡ�쳣��
//			jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
//			//�׳��쳣
//			env->ThrowNew(exceptionClazz,"Unable to join thread");
//		}else{
//			char message[26];
//			sprintf(message,"Worker %d returned %d",i,result);
//			jstring messageString = env->NewStringUTF(message);
//			//����ԭ����Ϣ����
//			env->CallVoidMethod(gObj,gOnNativeMessage,messageString);
//			//����Ƿ�����쳣
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

//	//����������
//		if(0!= pthread_mutex_lock(&mutex)){
//			jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
//			//�׳��쳣
//			env->ThrowNew(exceptionClazz,"Unable to lock mutext");
//		}
	if(0!=sem_wait(&sem)){
			//��ȡ�쳣
					jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
					env->ThrowNew(exceptionClazz,"Uable to initialize mutext.");
		}


	 for(jint i=0;i<iterations;i++){
		 char message[40];
		 sprintf(message,"Worker %d: Iterations %d",id,i);
		 jstring messageString = env->NewStringUTF(message);
		 env->CallVoidMethod(obj,gOnNativeMessage,messageString);
		 //����Ƿ�����쳣
		 if(NULL !=env->ExceptionOccurred()) break;
		 sleep(1);
	 }

//	 //����������
//	 if(0!=pthread_mutex_unlock(&mutex)){
//		 jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
//		 			//�׳��쳣
//		 			env->ThrowNew(exceptionClazz,"Unable to unlock mutext");
//	 }
	 if(0!=sem_post(&sem)){
	 		//��ȡ�쳣
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
			//��ȡ�쳣
					jclass exceptionClazz =env->FindClass("java/lang/RuntimeException");
					env->ThrowNew(exceptionClazz,"Uable to initialize mutext.");
		}
}
