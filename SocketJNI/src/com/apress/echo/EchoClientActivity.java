package com.apress.echo;

import android.net.IpPrefix;
import android.os.Bundle;
import android.widget.EditText;

public class EchoClientActivity extends AbstractEchoActivity{
	private static final String TAG ="EchoClientActivity";
	private EditText etIp;
	private EditText etMessage ;

	public EchoClientActivity() {
		super(R.layout.client_layout);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		etIp = (EditText) findViewById(R.id.ip_edit);
		etMessage = (EditText) findViewById(R.id.message_et);
		
	}
	@Override
	protected void onStartButtonClicked() {
		String ip = etIp.getText().toString();
		Integer port = getInteger();
		String message = etMessage.getText().toString() ;
		if(0!=ip.length() && port!=null && 0!=message.length()){
			ClientTask clientTask = new ClientTask(ip, port, message);
			clientTask.start();
		}
	}
	
	
	private native void nativeStartTcpClient(String ip,int port,String message) throws Exception ;
	private native void nativeStartUdpClient(String ip,int port,String message) throws Exception ;
	
	private class ClientTask extends AbstactEchoTast{
		private final int port ;
		private final String ip ;
		private final String message ;

		public ClientTask(String ip,int port,String message) {
			this.ip = ip ;
			this.port = port;
			this.message = message ;
		}
		@Override
		protected void onBackground() {
			logMessage("Starting client.");
			try{
//				nativeStartTcpClient(ip, port, message);
				nativeStartUdpClient(ip, port, message);
			}catch(Throwable e){
				logMessage(e.getMessage());
			}
			logMessage("Client terminated.");
		}
		
	}
}
