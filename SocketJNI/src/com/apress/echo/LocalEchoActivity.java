package com.apress.echo;
import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Bundle;
import android.widget.EditText;

import com.apress.echo.R;
public class LocalEchoActivity extends AbstractEchoActivity{

	private EditText etMessage ;
	
	public LocalEchoActivity() {
		super(R.layout.local_layout);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		etMessage = (EditText) findViewById(R.id.message_edit);
	}
	@Override
	protected void onStartButtonClicked() {
		String name = etPort.getText().toString();
		String message = etMessage.getText().toString();
		if(name.length()>0 && (message.length()>0)){
			String socketName ;
			//如果是filesystem socket ,预先准备应用程序的文件目录
			if(isFilesystemSocket(name)){
				File file = new File(getFilesDir(),name);
				socketName = file.getAbsolutePath();
			}else{
				socketName = name ;
			}
			//TODO
			ServerTask serverTask = new ServerTask(socketName);
			serverTask.start();
			ClientTask clientTask = new ClientTask(socketName, message);
			clientTask.start();
		}
	}
	private native void nativeStartLocalServer(String name) throws Exception;
	
	private boolean isFilesystemSocket(String name) {
		return name.startsWith("/");
	}
	
	public void startLocalClient(String name, String message) throws Exception{
		//构造一个本地socket
		LocalSocket clientSocket = new LocalSocket();
		try{
			//设置socket名称空间
			LocalSocketAddress.Namespace namespace ;
			if(isFilesystemSocket(name)){
				namespace = LocalSocketAddress.Namespace.FILESYSTEM ;
			}else{
				namespace = LocalSocketAddress.Namespace.ABSTRACT ;
			}
			// 构造本地sokcet地址
			LocalSocketAddress address = new LocalSocketAddress(name,namespace);
			//连接到本地socket
			logMessage("Connecting to "+name);
			clientSocket.connect(address);
			logMessage("Connected.");
			//以字节形式获取信息
			byte[] messageBytes = message.getBytes() ;
			//发送消息字到socket
			logMessage("client Sending to the socket...");
			OutputStream outputStream = clientSocket.getOutputStream() ;
			outputStream.write(messageBytes);
			logMessage(String.format("client Send %d bytes: %s", messageBytes.length,message));
			//从socket 中接收消息返回
			logMessage("client Receiving from the socket...");
			InputStream inputStream = clientSocket.getInputStream();
			int readSize = inputStream.read(messageBytes);
			String receivedMessage = new String(messageBytes,0,readSize);
			logMessage(String.format("client Received %d bytes: %s", readSize,receivedMessage));
			//关闭流
			outputStream.close();
			inputStream.close();
		}finally{
			clientSocket.close();
		}
	}
	
	
	/**
	 * 服务器任务.
	 * @author M.Y.G
	 *
	 */
	private class ServerTask extends AbstactEchoTast{
		private final String name ;
		
		public ServerTask( String name) {
			this.name = name ;
		}
		@Override
		protected void onBackground() {
			logMessage("Starting server.");
			try{
				nativeStartLocalServer(name);
			}catch(Exception e){
				logMessage(e.getMessage());
			}
			logMessage("Server terminated.");
		}
		
	}
	
	private class ClientTask extends Thread{
		private final String name ;
		private final String message ;
		public ClientTask(String name,String message) {
			this.name = name ;
			this.message = message ;
		}
		@Override
		public void run() {
			logMessage("Start client.");
			try{
				startLocalClient(name,message);
			}catch(Exception e){
				logMessage(e.getMessage());
			}
			logMessage("Client terminated");
		}
	}

	
}
