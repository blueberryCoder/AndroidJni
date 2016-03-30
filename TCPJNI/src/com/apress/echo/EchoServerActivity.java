package com.apress.echo;

public class EchoServerActivity extends AbstractEchoActivity{
	
	 public EchoServerActivity() {
		 super(R.layout.act_main);
	 }

	@Override
	protected void onStartButtonClicked() {
		Integer  port = getInteger();
		if(port!=null){
			ServerTask serverTask = new ServerTask(port);
			serverTask.start();
		}
	}

	private native void nativeStartTcpServer(int port) throws Exception;
	private native void nativeStartUdpServer(int port) throws Exception;
	
	private class ServerTask extends AbstactEchoTast{

		private final int port ;
		
		public ServerTask(int port) {
			this.port = port;
		}
		@Override
		protected void onBackground() {
			logMessage("Starting server.");
			try{
//				nativeStartTcpServer(port);
				nativeStartUdpServer(port);
			}catch(Exception e){
				logMessage(e.getMessage());
			}
			
			logMessage("Server terminated");
		}
		
	}
}
