#include <jni.h>
#include <com_apress_echo_EchoServerActivity.h>
#include <com_apress_echo_EchoClientActivity.h>
#include <stdio.h>
//va_list , vsnprintf
#include <stdarg.h>
// errno
#include <errno.h>
// strerror_r, memset
#include <string.h>
// socekt,bind,getsockname,listen,accept,recv,send,connect
#include <sys/types.h>
#include <sys/socket.h>

//sockaddr_un
#include <netinet/in.h>
//inet_ntop
#include <arpa/inet.h>
//close ,unlink
#include <unistd.h>
//offetof
#include <stddef.h>

//�����־��Ϣ����
#define MAX_LOG_MESSAGE_LENGTH 256
//������ݻ����С
#define MAX_BUFFER_SIZE 80

/**
 * ����������Ϣ��¼��Ӧ�ó���.
 * @param env JNIEnv interface
 * @param env object instance
 * @param format message message format and arguments.
 */
static void LogMessage(JNIEnv* env ,jobject obj,const char* format,...){
	//������־�ķ��� ID
	static jmethodID methodID = NULL;
	//�������IDδ����
	if(NULL==methodID){
		jclass clazz = env->GetObjectClass(obj);
		methodID =env->GetMethodID(clazz,"logMessage","(Ljava/lang/String;)V");
		//�ͷ�������
		env->DeleteLocalRef(clazz);
	}

	if(NULL!= methodID){
		char buffer[MAX_LOG_MESSAGE_LENGTH];
		va_list ap; //������
		va_start(ap,format);
		vsnprintf(buffer,MAX_LOG_MESSAGE_LENGTH,format,ap);
		va_end(ap);
		//��������ת��ΪJava �ַ���
		jstring message = env->NewStringUTF(buffer);
		//����ַ���������ȷ
		if(NULL!=message){
			//��¼��Ϣ
			env->CallVoidMethod(obj,methodID,message);
			//�ͷ���Ϣ����
			env->DeleteLocalRef(message);
		}
	}
}

/**
 * @param env JNIEnv interface
 * @param className class name
 * @param message exception message
 */
static void ThrowException(JNIEnv* env,const char* className,const char* message){
	//��ȡ�쳣��
	jclass clazz = env->FindClass(className);
	if(NULL!=clazz){
		//�׳��쳣
		env->ThrowNew(clazz,message);
		//�ͷ�ԭ���������
		env->DeleteLocalRef(clazz);
	}
}

static void ThrowErrorException(JNIEnv* env,const char * className,int errnum){
	char buffer[MAX_LOG_MESSAGE_LENGTH];
	//��ȡ�������Ϣ
	if(-1==strerror_r(errnum,buffer,MAX_LOG_MESSAGE_LENGTH)){
		strerror_r(errno,buffer,MAX_LOG_MESSAGE_LENGTH);
	}
	//�׳��쳣
	ThrowException(env,className,buffer);
}

static int NewTcpSocket(JNIEnv* env ,jobject obj){
	//����socket
	LogMessage(env,obj,"Constructing a new Tcp socket...");
	/**
	 *  ��һ������: ��ѡPF_LOGCAL:�����ڲ�ͨ��Э���壬��Э��ʹ������������ͬһ̫�豸�ϵ�Ӧ�ó��������SocketAPIs�˴�ͨ��
	 * 			  ��ѡPF_INET:Internet��4��Э���壬��Э����ʹӦ�ó�������������������ط�����Ӧ�ó������ͨ��
	 * 	�ڶ�������: ��ѡSOCK_STREAM:�ṩʹ��TCPЭ��ġ���������ͨ�ŵ�Stream Socket����
	 * 			 ��ѡSOCK_DGRAM���ṩʹ��UPDЭ��ģ������ӵ�ͨ��Datagram socket����
	 * 	������������ ָ�����õ���Э�飬���ڴ����Э�����Э��������˵��ֻ��ʹ��һ��Э�飬Ϊ��ѡ��Ĭ�ϵ�Э�飬�ò�������Ϊ��
	 */
	int tcpSocket = socket(PF_INET,SOCK_STREAM,0);
	//���socket�����Ƿ���ȷ
	if(-1 ==tcpSocket){
		//�׳�������ŵ��쳣
		ThrowErrorException(env,"java/io/IOException",errno);
	}
	return tcpSocket;
}

static void BindSocketToPort(JNIEnv* env,jobject obj,int sd,unsigned short port){
	struct sockaddr_in address;
	//��socket�ĵ�ַ
	memset(&address,0,sizeof(address));
	address.sin_family = PF_INET;

	//�󶨵����е�ַ
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	//���˿�ת��Ϊ�����ֽ�˳��
	address.sin_port = htons(port);

	//�� socket
	LogMessage(env,obj,"Binding to port %hu.",port);

	// ��һ������:socket ��������ָ�����󶨵�ָ����ַ��socketʵ��
	// �ڶ�������:ָ��socket ���󶨵�Э���ַ
	// ����������:ָ�����ݸ�������Э���ַ�Ľṹ��С
	if(-1 ==bind(sd,((struct sockaddr*) &address),sizeof(address))){
		//�׳�������ŵ��쳣
		ThrowErrorException(env,"java/io/IOException",errno);
	}
}

static unsigned short GetSocketPort(JNIEnv* env,jobject obj,int sd){
	unsigned short port=0;
	struct sockaddr_in address;
	socklen_t addressLength = sizeof(address);

	//��ȡsocket ��ַ
	if(-1== getsockname(sd,(struct sockaddr*)&address,&addressLength )){
		//�׳�������õ��쳣
		ThrowErrorException(env,"java/io/IOException",errno);
	}else{
		//���˿�ת��Ϊ�����ֽ�˳��
		port =ntohs(address.sin_port);
		LogMessage(env,obj,"Binded to random port %hu.",port);
	}
	return port ;
}

/**
 * ����ָ���Ĵ��������ӵ�backlog��socket,��backlog����ʱ�ܾ��µ�����
 *
 * @param env JNIEnv interface.
 * @param obj object instance
 * @param sd socket descriptor
 * @param backlog backlog size
 * @throws IOException
 */
static void ListenOnSocket(JNIEnv* env,jobject obj,int sd,int backlog){
	//�������� backlog ��socket
	LogMessage(env,obj,"Listening on socket with a backlog of %d pending connections.",backlog);
	if(-1==listen(sd,backlog)){
		//�׳��������źŵ��쳣
		ThrowErrorException(env,"java/io/IOException",errno);
	}
}

static void LogAddress(JNIEnv* env,jobject obj,const char* message,const struct sockaddr_in* address){
	char ip[INET_ADDRSTRLEN];
	//��IP��ַת��Ϊ�ַ���
	if(NULL == inet_ntop(PF_INET,&(address->sin_addr),ip,INET_ADDRSTRLEN)){
		//�׳�������ŵ��쳣
		ThrowErrorException(env,"java/io/IOException",errno);
	}else{
		//���˿�ת��Ϊ�����ֽ�˳��
		unsigned short port = ntohs(address->sin_port);
		//��¼��ַ
		LogMessage(env,obj,"%s %s:%hu.",message,ip,port);
	}
}

/**
 * �ڸ�����socket �������͵ȴ������Ŀͻ�����
 */
static int AcceptOnSocket(JNIEnv* env,jobject obj,int sd){
	struct sockaddr_in address;
	socklen_t addressLength = sizeof(address);

	//�����͵ȴ������Ŀͻ�����
	//���ҽ�����
	LogMessage(env,obj,"Waiting for a client connection...");
	int clientSocket = accept(sd,(struct sockaddr*)& address,&addressLength);

	//����ͻ�socket ��Ч
	if(-1 == clientSocket){
		//�׳�������ŵ��쳣
		ThrowErrorException(env,"java/io/IOException",errno);
	}
	else{
		//��¼��ַ
		LogAddress(env,obj,"Client connnetion from ",&address);
	}
	return clientSocket;
}

/**
 * ��������������socket�����ݷŵ�������
 */
static ssize_t ReceiveFromSocket(JNIEnv* env,jobject obj,int sd,char* buffer,size_t bufferSize){
	//��������������socket�����ݷŵ�������
	LogMessage(env,obj,"Receiving from the socket...");
	/**
	 * ��socket��������
	 * ��һ������:ָ��������Ҫ���н������ݵ�socketʵ��
	 * �ڶ�������:ָ���ڴ��ַ��ָ�룬���ڴ���������socket���յ�����
	 * ����������:ָ���������Ĵ�С��
	 * ���ĸ�����:ָ����������Ҫ���صĶ����־
	 */
	ssize_t recvSize = recv(sd,buffer,bufferSize-1,0);
	//�������ʧ��
	if(-1==recvSize){
		//�׳���������쳣
		ThrowErrorException(env,"java/io/IOException",errno);
	}else{
		//��NULL��β�������γ�һ���ַ���
		buffer[recvSize] = NULL;
		//������ݽ��ճɹ�
		if(recvSize>0){
			LogMessage(env,obj,"Received %d bytes: %s",recvSize,buffer);
		}else{
			LogMessage(env,obj,"Client disconnected.");
		}
	}
	return recvSize ;
}

/**
 * �������������ݷ��͵�socket.
 */
static ssize_t SendToSocket(JNIEnv* env,jobject obj,int sd,const char* buffer,size_t bufferSize){
	//�����ݻ�����������socket
	LogMessage(env,obj,"Sending to the socket...");
	ssize_t sentSize = send(sd,buffer,bufferSize,0);
	if(-1 == sentSize){
		ThrowErrorException(env,"java/io/Exception",errno);
	}else{
		if(sentSize>0){
			LogMessage(env,obj,"Sent %d bytes: %s",sentSize,buffer);
		}else{
			LogMessage(env,obj,"Client disconnected.");
		}

	}
	return sentSize ;
}


static void ConnectToAddress(JNIEnv* env,jobject obj,int sd,const char* ip,unsigned short port){
	//���ӵ�������IP��ַ�͸����Ķ˿ں�
	LogMessage(env,obj,"Connect to %s:%uh...",ip,port);
	struct sockaddr_in address ;
	memset(&address,0,sizeof(address));
	address.sin_family = PF_INET;
	//��IP��ַ�ַ���ת��Ϊ�����ַ
	if(0==inet_aton(ip,&(address.sin_addr))){
		//�׳��쳣
		ThrowErrorException(env,"java/io/Exception",errno);
	}
	else
	{
		//���˿ں�ת��Ϊ�����ֽ�˳��
		address.sin_port = htons(port);
		//ת��Ϊ��ַ
		if(-1 ==connect(sd,(const sockaddr*)&address,sizeof(address))){
			//�׳�������ŵ��쳣
			ThrowErrorException(env,"java/io/IOException",errno);
		}else{
			LogMessage(env,obj,"Connected.");
		}
	}

}

/**
 * ���� jni TCP �����
 */
JNIEXPORT void JNICALL Java_com_apress_echo_EchoServerActivity_nativeStartTcpServer
  (JNIEnv * env, jobject obj, jint port){
	//�����µ�TCP socket
	int serverSocket  = NewTcpSocket(env,obj);
	if(NULL == env->ExceptionOccurred()){
		//��socket �󶨵�ĳ�˿ں�
		BindSocketToPort(env,obj,serverSocket,(unsigned short)port );
		if(NULL!= env->ExceptionOccurred()){
			goto exit;
		}
		//�������������˿ں�
		if(0==port){
			//��ȡ��ǰ�󶨵Ķ˿ں�socket
			GetSocketPort(env,obj,serverSocket);
			if(NULL!=env->ExceptionOccurred())
				goto exit ;
		}
		//������4���ȴ����ӵ�backlog ��socket
		ListenOnSocket(env,obj,serverSocket,4);
		if(NULL!=env->ExceptionOccurred()) goto exit;
		//����socket ��һ���ͻ�����
		int clientSocket = AcceptOnSocket(env,obj,serverSocket);
		if(NULL!=env->ExceptionOccurred()) goto exit ;
		char buffer[MAX_BUFFER_SIZE];
		ssize_t recvSize ;
		ssize_t sentSize ;
		//���ղ����ͻ�����
		while(1){
			//��socket �н���
			recvSize = ReceiveFromSocket(env,obj,clientSocket,buffer,MAX_BUFFER_SIZE);
			if((0==recvSize) || (NULL != env->ExceptionOccurred()))
				break;
			//���͸�socket
			sentSize = SendToSocket(env,obj,clientSocket,buffer,(size_t)recvSize);
			if((0==sentSize)|| (NULL!=env->ExceptionOccurred()) )
				break;
		}
		//�رտͻ���socket
		close(clientSocket);

	}

	exit:
	if(serverSocket>0){
		close(serverSocket);
	}
}
JNIEXPORT void JNICALL Java_com_apress_echo_EchoClientActivity_nativeStartTcpClient
  (JNIEnv * env, jobject obj, jstring ip, jint port, jstring message){
	//�����µ�TCP socket
	int clientSocket  = NewTcpSocket(env,obj);
	if(NULL==env->ExceptionOccurred()){
		//��C�ַ�����ʽ��ȡIP��ַ
		const char* ipAddress  = env->GetStringUTFChars(ip,NULL);
		if(NULL == ipAddress) goto exit ;
		//���ӵ�IP��ַ�Ͷ˿�
		ConnectToAddress(env,obj,clientSocket,ipAddress,(unsigned short) port);
		//�ͷ�IP��ַ
		env->ReleaseStringUTFChars(ip,ipAddress);

		//������ӳɹ�
		if(NULL!=env->ExceptionOccurred()) goto exit ;
		//��C�ַ�����ʽ��ȡ��Ϣ
		const char* messageText  = env->GetStringUTFChars(message,NULL);
		if(NULL== message) goto exit ;
		//��ȡ��Ϣ��С
		jsize  messageSize = env->GetStringUTFLength(message);
		//���͸�socket
		SendToSocket(env,obj,clientSocket,messageText,messageSize);

		//�ͷ���Ϣ�ı�
		env->ReleaseStringUTFChars(message,messageText);
		//�������δ�ɹ�
		if(NULL!=env->ExceptionOccurred())goto exit ;
		char buffer[MAX_BUFFER_SIZE];
		//��socket ����
		ReceiveFromSocket(env,obj,clientSocket,buffer,MAX_BUFFER_SIZE);
	}

	exit:
	if(clientSocket<-1)
	close(clientSocket);
}
