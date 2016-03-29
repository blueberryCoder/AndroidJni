package com.apress.echo;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

public abstract class AbstractEchoActivity extends Activity implements OnClickListener{
	
	private static final String TAG="AbstractEchoActivity";
	
	private EditText etPort;
	private Button btnStart;
	private ScrollView logScrollView;
	private TextView tvLog;
	
	private final int layoutId ;
	public AbstractEchoActivity(int layoutId) {
		this.layoutId = layoutId ;
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(layoutId);
		etPort = (EditText) findViewById(R.id.port_edit);
		btnStart = (Button) findViewById(R.id.start_btn);
		logScrollView = (ScrollView) findViewById(R.id.srcoll_view);
		tvLog  = (TextView) findViewById(R.id.log_tv);
		
		btnStart.setOnClickListener(this);
		
	}

	@Override
	public void onClick(View v) {
			if(v ==btnStart){
				onStartButtonClicked();
			}
	}

	protected abstract void  onStartButtonClicked() ;
		
	protected Integer getInteger(){
		Integer port  ;
		try{
		port = Integer.parseInt(etPort.getText().toString());
		}catch(NumberFormatException e){
			e.printStackTrace();
			port =null;
		}
		return port;
	}
	
	protected void logMessage(final String message){
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				logMessageDirect(message);
			}
		});
	}
	
	protected void logMessageDirect(final String message){
		tvLog.append(message);
		tvLog.append("\n");
		logScrollView.fullScroll(View.FOCUS_DOWN);
	}
	
	/**
	 * 抽象的异步echo 任务
	 */
	protected abstract class AbstactEchoTast extends Thread{
		private final Handler handler ;
		public AbstactEchoTast(){
			handler = new Handler();
		}
		
		protected void onPreExecute(){
			btnStart.setEnabled(false);
			tvLog.setText("");
		}
		
		public synchronized void start(){
			onPreExecute();
			super.start();
		}
		
		public void run(){
			onBackground();
			handler.post(new Runnable() {
				@Override
				public void run() {
					onPostExecute();
				}
			});
		}
		private void onPostExecute() {
			btnStart.setEnabled(true);
		}
		protected abstract void onBackground();
	}

	static {
		System.loadLibrary("Echo");
	}
}
