package com.apress.aviplayer;

import java.util.concurrent.atomic.AtomicBoolean;

import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

public class NativeWindowPlayerActivity extends AbstractPlayerActivity{
	
	private SurfaceHolder mSurfaceHolder ;
	
	private final AtomicBoolean isPlaying =new  AtomicBoolean() ;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.act_native_window_player);
		SurfaceView mSurfaceView = (SurfaceView) findViewById(R.id.surface_view);
		mSurfaceHolder = mSurfaceView.getHolder();
		mSurfaceHolder.addCallback(surfaceHolderCallback);
	}
	
	private Callback surfaceHolderCallback = new Callback() {
		
		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			isPlaying.set(false);
		}
		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			isPlaying.set(true);
			//在一个单独的线程中启动渲染器
			new Thread(renderer).start();
		}
		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width,
				int height) {
		}
	};
	public final Runnable renderer = new Runnable() {
		
		@Override
		public void run() {
			//获得surface 实例
			Surface surface =mSurfaceHolder.getSurface();
			//初始化原生的window
			init(avi,surface);
			//使用帧速计算延迟
			long frameDelay = (long) (1000/getFrameRate(avi));
			//播放时开始渲染
			while(isPlaying.get()){
				//将帧渲染至surface
				render(avi,surface);
				try {
					Thread.sleep(frameDelay);
				} catch (InterruptedException e) {
					e.printStackTrace();
					break;
				}
			}
			
		}
	};
	
	private native static boolean render(long avi,Surface surface);
	private native static void init(long avi,Surface surface);
}
