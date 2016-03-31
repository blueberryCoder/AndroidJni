package com.apress.aviplayer;

import java.util.concurrent.atomic.AtomicBoolean;




import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;

public class BitmapPlayerActivity extends AbstractPlayerActivity{
	
	private static final String TAG ="BitmapPlayerActivity";
	/** 正在播放*/
	private final AtomicBoolean isPlaying = new AtomicBoolean() ;
	private SurfaceHolder surfaceHolder ;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.act_bitmap_player);
		SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface_view);
		surfaceHolder = surfaceView.getHolder();
		surfaceHolder.addCallback(surfaceHolderCallback);
		surfaceView.setKeepScreenOn(true);
	}
	private final Callback surfaceHolderCallback = new Callback() {
		
		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			isPlaying.set(false);
		}
		
		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			// surface 准备好后开始播放
			isPlaying.set(true);
			//在一个单独的线程中渲染
			new Thread(renderer).start();
		}
		
		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width,
				int height) {
		}
	};
	private final Runnable renderer = new Runnable() {
		
		@Override
		public void run() {
			//创建一个新的bitmap来保存所有的帧
			Bitmap bitmap = Bitmap.createBitmap(getWidth(avi),getHeith(avi),Bitmap.Config.RGB_565);
			//使用帧速来计算延迟
			long frameDelay = (long)(1000/getFrameRate(avi));
			Log.i(TAG, "frameDelay:"+frameDelay +" width: "+getWidth(avi)+" height:"+getHeith(avi));
			
			//播放的时候开始渲染
			while(isPlaying.get()){
				render(avi, bitmap);
				//锁定canvas
				Canvas canvas = surfaceHolder.lockCanvas();
				canvas.drawColor(Color.WHITE);
				//将Bitmap绘制到canvas上
				Log.i(TAG, "result,btimap's width:"+bitmap.getWidth()+"height:"+bitmap.getHeight());
				canvas.drawBitmap(bitmap, 0, 0,null);
				//canvas 准备显示
				surfaceHolder.unlockCanvasAndPost(canvas);
				try {
					Thread.sleep(frameDelay);
				} catch (InterruptedException e) {
					break ;
				}
			}
		}
	};
	
	private  native static boolean render(long avi,Bitmap bitmap);
}
