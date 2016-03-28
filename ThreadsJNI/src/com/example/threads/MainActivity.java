package com.example.threads;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
public class MainActivity extends Activity {

	private EditText etThreads;
	private EditText etIterations;
	private Button btnStart;
	private TextView tvLogs;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		nativeInit();
		initView();
		btnStart.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				int threads = getNumbers(etThreads);
				int iterations  = getNumbers(etIterations);
				if(threads>0 && iterations>0){
					startThreads(threads,iterations);
				}
			}
		});
	}
	
	private void initView() {
		etThreads =(EditText)findViewById(R.id.et_threads);
		etIterations =(EditText)findViewById(R.id.et_iterations);
		btnStart =(Button)findViewById(R.id.btn_start);
		tvLogs =(TextView)findViewById(R.id.log_view);
	}
	
	private int getNumbers(EditText et){
		try{
		return Integer.parseInt(et.getText().toString());
		}catch(NumberFormatException e){
			return  0;
		}
	}
	
	/**
	 * java实现启动线程
	 * @param threads
	 * @param iterations
	 */
	protected void startThreads( int threads,final int iterations) {
//		javaThreads(threads, iterations);
		posixThreads(threads, iterations);
	}

	private void javaThreads(int threads, final int iterations) {
		for(int i=0;i<threads;i++){
			final int id = i ;
			new Thread(){
				public void run() {
					nativeWorker(id, iterations);
				};
			}.start();
		}
	}
	
	private void onNativeMessage(final String message){
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				tvLogs.append(message);
				tvLogs.append("\n");
			}
		});
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		nativeFree();
	}
	private native void posixThreads(int threads,int iterations);
	private native void nativeInit();
	private native void nativeFree();
	private native void nativeWorker(int id,int iterations);
	
	static{
		System.loadLibrary("Threads");
	}
}
