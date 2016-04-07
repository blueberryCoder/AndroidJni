package com.apress.aviplayer;

import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicBoolean;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Bundle;

public class OpenGLPlayerActivity extends AbstractPlayerActivity{
	private static final String TAG ="OpenGLPlayerActivity";
	/**正在播放*/
	private final AtomicBoolean isPlaying = new AtomicBoolean();
	/**原生渲染器*/
	private long instance ;
	/** GL surface view 实例 */
	private GLSurfaceView glSurfaceView ;
	
	/**
	 * On create
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.act_open_gl_player);
		
		glSurfaceView = (GLSurfaceView) findViewById(R.id.gl_surface_view);
		//设置渲染器
		glSurfaceView.setRenderer(renderer);
		
		//请求时渲染帧
		glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
	}
	
	@Override
	protected void onStart() {
		super.onStart();
		//初始化原生渲染器
		instance = init(avi);
	}
	@Override
	protected void onResume() {
		super.onResume();
		glSurfaceView.onResume();
	}
	@Override
	protected void onPause() {
		super.onPause();
		glSurfaceView.onPause();
	}
	@Override
	protected void onStop() {
		super.onStop();
		free(instance);
		instance = 0;
	}
	
	private final Runnable player = new Runnable() {
		
		@Override
		public void run() {
			//使用帧速计算延迟
			long frameDelay = (long) (1000/getFrameRate(avi));
			//播放时开始渲染
			while(isPlaying.get()){
				//请求渲染
				glSurfaceView.requestRender(); 
				//等待下一帧
				try {
					Thread.sleep(frameDelay);
				} catch (InterruptedException e) {
					break;
				}
			}
		}
	};
	private Renderer renderer = new Renderer() {
		
		@Override
		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			//初始化OpenGL surface
			initSurface(instance,avi);
			//surface 准备好开始播放
			isPlaying.set(true);
			//启动播放器
			new Thread(player).start();
		}
		
		@Override
		public void onSurfaceChanged(GL10 gl, int width, int height) {
			
		}
		
		@Override
		public void onDrawFrame(GL10 gl) {
			//渲染下一帧
			if(!render(instance,avi)){
				isPlaying.set(false);
			}
		}
	};
	
	private static native void initSurface(long instance,long avi);
	private static native long init(long avi);
	private static native void free(long instance);
	private static native boolean render(long instance,long avi);
	
}
