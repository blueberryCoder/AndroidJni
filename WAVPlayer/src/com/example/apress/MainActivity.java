package com.example.apress;

import java.io.File;
import java.io.IOException;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends Activity implements OnClickListener{

	/**文件名编辑文本.*/
	private EditText fileNameEdit;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		fileNameEdit =(EditText) findViewById(R.id.fileNameEdit);
		Button playButton = (Button) findViewById(R.id.playButton);
		playButton.setOnClickListener(this);
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.playButton:
			onPlayButtonClick();
			break;
		}
	}

	private void onPlayButtonClick() {
		//位于外部存储器
		File file = new File("/sdcard",fileNameEdit.getText().toString());
		//开始播放
		PlayTast playTast = new PlayTast();
		playTast.execute(file.getAbsolutePath());
		
	}
	
	private class PlayTast extends AsyncTask<String, Void, Exception>{

		@Override
		protected Exception doInBackground(String... file) {
			Exception result = null;
			try{
				Log.i("TAG","file="+file[0]);
				play(file[0]) ;
			}catch(IOException ex){
				ex.printStackTrace();
				result = ex ;
			}
			return result;
		}
		
		@Override
		protected void onPostExecute(Exception ex) {
			//如果播放失败则显示错误信息
			if(ex!=null){
				new AlertDialog.Builder(MainActivity.this).setTitle("Exception")
				.setMessage(ex.getMessage()).show();
			}
		}
		
	}

	private native void play(String fileName) throws IOException;
	
	static{
		System.loadLibrary("WAVPlayer");
	}
}
