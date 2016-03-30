#include <jni.h>
#include <com_apress_echo_EchoServerActivity.h>
#include <com_apress_echo_EchoClientActivity.h>
#include <com_apress_echo_LocalEchoActivity.h>
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



#include <stdlib.h>
#include <sys/wait.h>
//address_un
#include <sys/un.h>
#include <android/log.h>

//最大日志消息长度
#define MAX_LOG_MESSAGE_LENGTH 256
//最大数据缓冲大小
#define MAX_BUFFER_SIZE 80

/**
 * 将给定的消息记录到应用程序.
 * @param env JNIEnv interface
 * @param env object instance
 * @param format message message format and arguments.
 */
static void LogMessage(JNIEnv* env ,jobject obj,const char* format,...){
	//缓存日志的方法 ID
	static jmethodID methodID = NULL;
	//如果方法ID未缓存
	if(NULL==methodID){
		jclass clazz = env->GetObjectClass(obj);
		methodID =env->GetMethodID(clazz,"logMessage","(Ljava/lang/String;)V");
		//释放类引用
		env->DeleteLocalRef(clazz);
	}

	if(NULL!= methodID){
		char buffer[MAX_LOG_MESSAGE_LENGTH];
		va_list ap; //处理变参
		va_start(ap,format);
		vsnprintf(buffer,MAX_LOG_MESSAGE_LENGTH,format,ap);
		va_end(ap);
		//将缓冲区转换为Java 字符串
		jstring message = env->NewStringUTF(buffer);
		//如果字符串构造正确
		if(NULL!=message){
			//记录消息
			env->CallVoidMethod(obj,methodID,message);
			//释放消息引用
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
	//获取异常类
	jclass clazz = env->FindClass(className);
	if(NULL!=clazz){
		//抛出异常
		env->ThrowNew(clazz,message);
		//释放原生类的引用
		env->DeleteLocalRef(clazz);
	}
}

static void ThrowErrorException(JNIEnv* env,const char * className,int errnum){
	char buffer[MAX_LOG_MESSAGE_LENGTH];
	//获取错误号信息
	if(-1==strerror_r(errnum,buffer,MAX_LOG_MESSAGE_LENGTH)){
		strerror_r(errno,buffer,MAX_LOG_MESSAGE_LENGTH);
	}
	//抛出异常
	ThrowException(env,className,buffer);
}

/**
 * 构造一个新的TCP Socket
 */
static int NewTcpSocket(JNIEnv* env ,jobject obj){
	//构造socket
	LogMessage(env,obj,"Constructing a new Tcp socket...");
	/**
	 *  第一个参数: 可选PF_LOGCAL:主机内部通信协议族，该协议使物理上运行在同一太设备上的应用程序可以用SocketAPIs彼此通信
	 * 			  可选PF_INET:Internet第4版协议族，该协议族使应用程序可以在网络上其他地方运行应用程序进行通信
	 * 	第二个参数: 可选SOCK_STREAM:提供使用TCP协议的、面向连接通信的Stream Socket类型
	 * 			 可选SOCK_DGRAM：提供使用UPD协议的，无连接的通信Datagram socket类型
	 * 	第三个参数： 指定会用到的协议，对于大多数协议族和协议类型来说，只能使用一个协议，为了选择默认的协议，该参数可设为零
	 */
	int tcpSocket = socket(PF_INET,SOCK_STREAM,0);
	//检查socket构造是否正确
	if(-1 ==tcpSocket){
		//抛出带错误号的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}
	return tcpSocket;
}



static void BindSocketToPort(JNIEnv* env,jobject obj,int sd,unsigned short port){
	struct sockaddr_in address;
	//绑定socket的地址
	memset(&address,0,sizeof(address));
	address.sin_family = PF_INET;

	//绑定到所有地址
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	//将端口转换为网络字节顺序
	address.sin_port = htons(port);

	//绑定 socket
	LogMessage(env,obj,"Binding to port %hu.",port);

	// 第一个参数:socket 描述符：指定将绑定到指定地址的socket实例
	// 第二个参数:指定socket 被绑定的协议地址
	// 第三个参数:指定传递给函数的协议地址的结构大小
	if(-1 ==bind(sd,((struct sockaddr*) &address),sizeof(address))){
		//抛出带错误号的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}
}

static unsigned short GetSocketPort(JNIEnv* env,jobject obj,int sd){
	unsigned short port=0;
	struct sockaddr_in address;
	socklen_t addressLength = sizeof(address);

	//获取socket 地址
	if(-1== getsockname(sd,(struct sockaddr*)&address,&addressLength )){
		//抛出带错误好的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}else{
		//将端口转换为主机字节顺序
		port =ntohs(address.sin_port);
		LogMessage(env,obj,"Binded to random port %hu.",port);
	}
	return port ;
}

/**
 * 监听指定的待处理连接的backlog的socket,当backlog以满时拒绝新的连接
 *
 * @param env JNIEnv interface.
 * @param obj object instance
 * @param sd socket descriptor
 * @param backlog backlog size
 * @throws IOException
 */
static void ListenOnSocket(JNIEnv* env,jobject obj,int sd,int backlog){
	//监听给定 backlog 的socket
	LogMessage(env,obj,"Listening on socket with a backlog of %d pending connections.",backlog);
	if(-1==listen(sd,backlog)){
		//抛出带错误信号的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}
}

static void LogAddress(JNIEnv* env,jobject obj,const char* message,const struct sockaddr_in* address){
	char ip[INET_ADDRSTRLEN];
	//将IP地址转换为字符串
	if(NULL == inet_ntop(PF_INET,&(address->sin_addr),ip,INET_ADDRSTRLEN)){
		//抛出带错误号的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}else{
		//将端口转换为主机字节顺序
		unsigned short port = ntohs(address->sin_port);
		//记录地址
		LogMessage(env,obj,"%s %s:%hu.",message,ip,port);
	}
}

/**
 * 在给定的socket 上阻塞和等待进来的客户连接
 */
static int AcceptOnSocket(JNIEnv* env,jobject obj,int sd){
	struct sockaddr_in address;
	socklen_t addressLength = sizeof(address);

	//阻塞和等待进来的客户连接
	//并且接受它
	LogMessage(env,obj,"Waiting for a client connection...");
	int clientSocket = accept(sd,(struct sockaddr*)& address,&addressLength);

	//如果客户socket 无效
	if(-1 == clientSocket){
		//抛出带错误号的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}
	else{
		//记录地址
		LogAddress(env,obj,"Client connnetion from ",&address);
	}
	return clientSocket;
}

/**
 * 阻塞并接收来自socket的数据放到缓存区
 */
static ssize_t ReceiveFromSocket(JNIEnv* env,jobject obj,int sd,char* buffer,size_t bufferSize){
	//阻塞并接受来自socket的数据放到缓冲区
	LogMessage(env,obj,"Receiving from the socket...");
	/**
	 * 从socket接收数据
	 * 第一个参数:指定程序想要从中接收数据的socket实例
	 * 第二个参数:指向内存地址的指针，该内存用来保存socket接收的数据
	 * 第三个参数:指定缓冲区的大小，
	 * 第四个参数:指定接收所需要返回的额外标志
	 */
	ssize_t recvSize = recv(sd,buffer,bufferSize-1,0);
	//如果接收失败
	if(-1==recvSize){
		//抛出带错误的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}else{
		//以NULL结尾缓冲区形成一个字符串
		buffer[recvSize] = NULL;
		//如果数据接收成功
		if(recvSize>0){
			LogMessage(env,obj,"Received %d bytes: %s",recvSize,buffer);
		}else{
			LogMessage(env,obj,"Client disconnected.");
		}
	}
	return recvSize ;
}

/**
 * 将缓冲区的数据发送到socket.
 */
static ssize_t SendToSocket(JNIEnv* env,jobject obj,int sd,const char* buffer,size_t bufferSize){
	//将数据缓存区发动到socket
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
	//连接到给定的IP地址和给定的端口号
	LogMessage(env,obj,"Connect to %s:%uh...",ip,port);
	struct sockaddr_in address ;
	memset(&address,0,sizeof(address));
	address.sin_family = PF_INET;
	//将IP地址字符串转换为网络地址
	if(0==inet_aton(ip,&(address.sin_addr))){
		//抛出异常
		ThrowErrorException(env,"java/io/Exception",errno);
	}
	else
	{
		//将端口号转化为网络字节顺序
		address.sin_port = htons(port);
		//转化为地址
		if(-1 ==connect(sd,(const sockaddr*)&address,sizeof(address))){
			//抛出带错误号的异常
			ThrowErrorException(env,"java/io/IOException",errno);
		}else{
			LogMessage(env,obj,"Connected.");
		}
	}

}

/**
 * 核心 jni TCP 服务端
 */
JNIEXPORT void JNICALL Java_com_apress_echo_EchoServerActivity_nativeStartTcpServer
  (JNIEnv * env, jobject obj, jint port){
	//构造新的TCP socket
	int serverSocket  = NewTcpSocket(env,obj);
	if(NULL == env->ExceptionOccurred()){
		//将socket 绑定到某端口号
		BindSocketToPort(env,obj,serverSocket,(unsigned short)port );
		if(NULL!= env->ExceptionOccurred()){
			goto exit;
		}
		//如果请求了随机端口号
		if(0==port){
			//获取当前绑定的端口号socket
			GetSocketPort(env,obj,serverSocket);
			if(NULL!=env->ExceptionOccurred())
				goto exit ;
		}
		//监听有4个等待连接的backlog 的socket
		ListenOnSocket(env,obj,serverSocket,4);
		if(NULL!=env->ExceptionOccurred()) goto exit;
		//接收socket 的一个客户连接
		int clientSocket = AcceptOnSocket(env,obj,serverSocket);
		if(NULL!=env->ExceptionOccurred()) goto exit ;
		char buffer[MAX_BUFFER_SIZE];
		ssize_t recvSize ;
		ssize_t sentSize ;
		//接收并发送回数据
		while(1){
			//从socket 中接收
			recvSize = ReceiveFromSocket(env,obj,clientSocket,buffer,MAX_BUFFER_SIZE);
			if((0==recvSize) || (NULL != env->ExceptionOccurred()))
				break;
			//发送给socket
			sentSize = SendToSocket(env,obj,clientSocket,buffer,(size_t)recvSize);
			if((0==sentSize)|| (NULL!=env->ExceptionOccurred()) )
				break;
		}
		//关闭客户端socket
		close(clientSocket);

	}

	exit:
	if(serverSocket>0){
		close(serverSocket);
	}
}
JNIEXPORT void JNICALL Java_com_apress_echo_EchoClientActivity_nativeStartTcpClient
  (JNIEnv * env, jobject obj, jstring ip, jint port, jstring message){
	//构造新的TCP socket
	int clientSocket  = NewTcpSocket(env,obj);
	if(NULL==env->ExceptionOccurred()){
		//以C字符串形式获取IP地址
		const char* ipAddress  = env->GetStringUTFChars(ip,NULL);
		if(NULL == ipAddress) goto exit ;
		//连接到IP地址和端口
		ConnectToAddress(env,obj,clientSocket,ipAddress,(unsigned short) port);
		//释放IP地址
		env->ReleaseStringUTFChars(ip,ipAddress);

		//如果连接成功
		if(NULL!=env->ExceptionOccurred()) goto exit ;
		//以C字符串形式获取信息
		const char* messageText  = env->GetStringUTFChars(message,NULL);
		if(NULL== message) goto exit ;
		//获取消息大小
		jsize  messageSize = env->GetStringUTFLength(message);
		//发送给socket
		SendToSocket(env,obj,clientSocket,messageText,messageSize);

		//释放消息文本
		env->ReleaseStringUTFChars(message,messageText);
		//如果发送未成功
		if(NULL!=env->ExceptionOccurred())goto exit ;
		char buffer[MAX_BUFFER_SIZE];
		//从socket 接收
		ReceiveFromSocket(env,obj,clientSocket,buffer,MAX_BUFFER_SIZE);
	}

	exit:
	if(clientSocket<-1)
	close(clientSocket);
}
/**
 * *************************************************************************************************
 * ****************************UDP sample***********************************************************
 * *************************************************************************************************
 */

/**
 * 构造一个新的UDP Socket
 * @param env JNIEnv interface
 * @param obj object instance
 * @param socket descriptor
 * @throws IOException
 */
static int NewUdpSocket(JNIEnv* env,jobject obj){
	//构造socket
	LogMessage(env,obj,"Constructing a new UDP socket...");
	int udpSocket = socket(PF_INET,SOCK_DGRAM,0);
	//检查socket构造是否正确
	if(-1 == udpSocket){
		//抛出带错误的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}
	return udpSocket;
}

/**
 * 从socket 中阻塞并接收数据保存到缓冲区，填充客户端地址
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket descriptor
 * @param address client address
 * @param buffer data buffer
 * @param bufferSize buffer size
 * @return receive size
 * @throws IOException
 */
static ssize_t ReceiveDatagramFromSocket(JNIEnv* env,jobject obj,int sd,
		struct sockaddr_in* address,char* buffer, size_t bufferSize){
	socklen_t addressLength = sizeof(struct sockaddr_in);
	//从socket中接收数据
	LogMessage(env,obj,"Receiving from the socket...");
	ssize_t recvSize = recvfrom(sd,buffer,bufferSize,0,(struct sockaddr*)address,&addressLength);
	//如果接收失败
	if(-1 ==recvSize){
		//抛出异常
		ThrowErrorException(env,"java/io/Exception",errno);
	}else{
		//记录地址
		LogAddress(env,obj,"Receiver from",address);
		//以NULL终止缓冲区使其为一个字符
		buffer[recvSize] = NULL;
		//如果数据已经修改
		if(recvSize>0){
			LogMessage(env,obj,"Received %d bytes: %s",recvSize,buffer);
		}
	}
	return  recvSize;
}

/**
 * 用给定的socket发送数据报到给定的地址
 * @param env JNIEnv interface
 * @param obj object instance
 * @param sd socket description
 * @param address remote address
 * @param buffer data buffer
 * @param bufferSize buffer size
 * @param sent Size.
 * @throws IOException.
 */
static ssize_t SendDatagramToSocket(JNIEnv* env,jobject obj,int sd,const struct sockaddr_in* address,
		const char* buffer,size_t bufferSize){
	//向socket发送数据缓冲区
	LogMessage(env,obj,"Send to",address);
	ssize_t sentSize = sendto(sd,buffer,bufferSize,0,(const sockaddr* )address,sizeof(struct sockaddr_in));
	//如果发送失败
	if(-1 == sentSize){
		//抛出带错误信号的异常
		ThrowErrorException(env,"java/io/Exception",errno);
	}
	else if(sentSize>0){
		LogMessage(env,obj,"Sent %d bytes: %s",sentSize,buffer);
	}
	return sentSize ;
}


JNIEXPORT void JNICALL Java_com_apress_echo_EchoServerActivity_nativeStartUdpServer
  (JNIEnv * env, jobject obj, jint port){
	//构建一个新的UDP socket
	int serverSocket = NewUdpSocket(env,obj);
	if(NULL==env->ExceptionOccurred()){
		//将socket 绑定到某一个端口上
		BindSocketToPort(env,obj,serverSocket,(unsigned short)port);
		if(NULL!=env->ExceptionOccurred()) goto exit ;
		//如果请求随机端口号
		if(0==port){
			//获取当绑定的端口号socket
			GetSocketPort(env,obj,serverSocket);
			if(NULL!=env->ExceptionOccurred()) goto exit ;
		}

		//客户端地址
		struct sockaddr_in address;
		memset(&address,0,sizeof(address));
		char buffer[MAX_BUFFER_SIZE];
		ssize_t recvSize ;
		ssize_t sentSize ;
		//从socket 中接收
		recvSize = ReceiveDatagramFromSocket(env,obj,serverSocket,&address,buffer,MAX_BUFFER_SIZE);
		if((0==recvSize)|| (NULL!=env->ExceptionOccurred())) goto exit;
		//发送给socket
		sentSize= SendDatagramToSocket(env,obj,serverSocket,&address,buffer,(size_t)recvSize);
	}

	exit:
	if(serverSocket>0){
		close(serverSocket);
	}
}

/**
 * 发送udp
 */
JNIEXPORT void JNICALL Java_com_apress_echo_EchoClientActivity_nativeStartUdpClient
  (JNIEnv * env, jobject obj, jstring ip, jint port, jstring message){
	//构造一个新的UDP socket
	int clientSocket  = NewUdpSocket(env,obj);
	if(NULL == env->ExceptionOccurred()){
		struct sockaddr_in address ;
		memset(&address,0,sizeof(address));
		address.sin_family = PF_INET ;
		//以C字符串形式获取IP地址
		const char* ipAddress = env->GetStringUTFChars(ip,NULL);
		if(NULL == ipAddress)
			goto exit ;
		//将IP地址字符串转换为网络地址
		int result = inet_aton(ipAddress,&(address.sin_addr));
		//释放IP地址
		env->ReleaseStringUTFChars(ip,ipAddress);
		//如果转换失败
		if(0==result){
			//抛出带错误的异常
			ThrowErrorException(env,"java/io/IOException",errno);
			goto exit ;
		}
		//将端口转换为网络字节顺序
		address.sin_port = htons(port);
		//以C字符串形式获取信息
		const char* messageText = env->GetStringUTFChars(message,NULL);
		if(NULL ==messageText) goto exit ;
		//获取信息大小
		jsize messageSize = env->GetStringUTFLength(message);
		//发送消息给socket
		SendDatagramToSocket(env,obj,clientSocket,&address,messageText,messageSize);
		//释放消息文本
		env->ReleaseStringUTFChars(message,messageText);

		//如果发送未成功
		if(NULL!=env->ExceptionOccurred()) goto exit ;
		char buffer[MAX_BUFFER_SIZE] ;
		//清除地址
		memset(&address,0,sizeof(address));
		//从 socket 接收
		ReceiveDatagramFromSocket(env,obj,clientSocket,&address,buffer,MAX_BUFFER_SIZE);
	}
	exit:
	if(clientSocket>0){
		close(clientSocket);
	}
}
/**
 * ***************************************************************************************************
 * ***********************本地socket*******************************************************************
 * ***************************************************************************************************
 */

/**
 * 构造一个新的原生UNIX socket
 */
static int NewLocalSocket(JNIEnv* env,jobject obj){
	//构造socket
	LogMessage(env,obj,"Constructing a new Local UNIX socket...");
	int localSocket = socket(PF_LOCAL,SOCK_STREAM,0);
	//检查socket构造是否正确
	if(-1==localSocket){
		//抛出带错误号的异常
		ThrowErrorException(env,"java/io/Exception",errno);
	}
	return localSocket ;
}
/**
 * 将本地UNIX socket与 某一名称绑定
 */
static void BindLocalSocketToName(JNIEnv* env,jobject obj,int sd,const char* name){
	struct sockaddr_un address;
	//名字长度
	const size_t nameLength = strlen(name);
	//路径长度初始化与名字长度相等
	size_t pathLength = nameLength ;
	//如果名字不是'/'开头，即它在抽象命名空间里
	bool abstractNamespace = ('/'!=name[0]);

	//抽象命名空间要求目录的第一个字节是0字节,更新路径长度包括0字节
	if(abstractNamespace){
		pathLength++;
	}
	//检查路径长度
	if(pathLength > sizeof(address.sun_path)){
		//抛出的常
		ThrowException(env,"java/io/IOException","Name is too big.");
	}else{
		//清楚地址字节
		memset(&address,0,sizeof(address));
		address.sun_family = PF_LOCAL ;
		//Socket路径
		char* sunPath = address.sun_path ;
		//第一字节必须是0以使用抽象命名空间
		if(abstractNamespace){
			*sunPath++ =NULL;
		}
		//追加本地名字
		strcpy(sunPath,name);
		//地址长度
		socklen_t addressLength = (offsetof(struct sockaddr_un,sun_path))+pathLength ;
		//如果socket名已经绑定，取消连接
		unlink(address.sun_path);
		//绑定socket
		LogMessage(env,obj,"Bind to local name %s%s.",(abstractNamespace)?"(null)":"",name);
		if(-1==bind(sd,(struct sockaddr* ) &address,addressLength)){
			//抛出带错误号的遗产
			ThrowErrorException(env,"java/io/IOExcepion",errno);
		}

	}
}


static int AcceptOnLocalSocket(JNIEnv* env,jobject obj,int sd){
	//阻塞并等待即将到来的客户端连接并且接收它
	LogMessage(env,obj,"Waiting for a client connectoion...");
	int clientSocket = accept(sd,NULL,NULL);
	//如果客户端socket无效
	if(-1==clientSocket){
		//抛出带错误号的异常
		ThrowErrorException(env,"java/io/IOException",errno);
	}
	return clientSocket ;
}
JNIEXPORT void JNICALL Java_com_apress_echo_LocalEchoActivity_nativeStartLocalServer
  (JNIEnv * env, jobject obj, jstring name){
	//构造一个新的本地UNIX socket
	int serverSocket = NewLocalSocket(env,obj);
	if(NULL==env->ExceptionOccurred()){
		//以C字符串形式获取名称
		const char* nameText = env->GetStringUTFChars(name,NULL);
		if(NULL ==nameText) goto exit ;
		// 绑定socket 到某一端口号
		BindLocalSocketToName(env,obj,serverSocket,nameText);
		//释放name文本
		env->ReleaseStringUTFChars(name,nameText);
		//如果绑定失败
		if(NULL !=env->ExceptionOccurred()) goto exit ;
		//监听有4个挂起连接的带backlog的socket
		ListenOnSocket(env,obj,serverSocket,4);
		if(NULL!=env->ExceptionOccurred()) goto exit ;
		//接收socket 的一个客户连接
		int clientSocket = AcceptOnLocalSocket(env,obj,serverSocket);
		if(NULL!=env->ExceptionOccurred())goto exit;
		char buffer[MAX_BUFFER_SIZE];
		ssize_t recvSize;
		ssize_t sentSize ;
		//接收并发送回数据
		while(1){
			//从socket 中接收
			recvSize = ReceiveFromSocket(env,obj,clientSocket,buffer,MAX_BUFFER_SIZE);
			if((0==recvSize)||(NULL!=env->ExceptionOccurred()))break ;
			//发送给socket
			sentSize = SendToSocket(env,obj,clientSocket,buffer,recvSize);
			if((0==sentSize) || (NULL!=env->ExceptionOccurred())) break;
		}
		close(clientSocket);
	}
	exit:
	if(serverSocket>0){
		close(serverSocket);
	}
}



