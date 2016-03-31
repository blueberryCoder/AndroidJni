package com.apress.aviplayer;

import java.io.IOException;

import android.app.Activity;
import android.app.AlertDialog;

public class AbstractPlayerActivity extends Activity{

	public static final String EXTRA_FILE_NAME = "com.apress.aviplayer.EXTRA_FILE_NAME";
	
	/**AVI 视频文件描述符 */
	protected long avi =0;
	
	protected void onStart(){
		super.onStart();
		try{
			avi = open(getFileName());
		}catch(IOException e){
			new AlertDialog.Builder(this).setTitle("error").setMessage(e.getMessage()).show();
		}
	}

	protected void onStop(){
		super.onStop();
		//如果AVI视频时打开的
		if(0!=avi){
			close(avi);
			avi =0;
		}
	}
	private String getFileName() {
		return getIntent().getExtras().getString(EXTRA_FILE_NAME);
	}
	/**
	 * 打开指定AVI文件并且返回一个文件描述符
	 * @param fileName
	 * @return
	 * @throws IOException
	 */
	protected native static long open(String fileName) throws IOException ;
	
	/**
	 * 获得视频的宽度
	 * @param avi
	 * @return
	 */
	protected native static int getWidth(long avi);
	
	/**
	 * 获得视频高度
	 * @param avi
	 * @return
	 */
	protected native static int getHeith(long avi);
	
	/**
	 * 获得帧率
	 * @param avi
	 * @return
	 */
	protected native static double getFrameRate(long avi);
	
	/**
	 * 基于给定的文件描述符，关闭指定的AVI文件
	 * @param avi
	 */
	protected native static void close(long avi) ;
	
	static{
		System.loadLibrary("AVIPlayer");
	}
	
}
