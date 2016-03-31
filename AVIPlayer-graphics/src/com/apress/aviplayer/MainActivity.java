package com.apress.aviplayer;

import java.io.File;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;

public class MainActivity extends Activity implements OnClickListener {
	
	private EditText etFileName ;
	private RadioGroup rgPlayer;
	private Button btnPlayer;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		etFileName = (EditText) findViewById(R.id.et_file_name);
		rgPlayer = (RadioGroup) findViewById(R.id.player_radio_group);
		btnPlayer  =  (Button) findViewById(R.id.btn_player);
		btnPlayer.setOnClickListener(this);
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.btn_player:
			onPlayButtonClick();
			break;

		default:
			break;
		}
	}

	/**
	 * 处理点击事件
	 */
	private void onPlayButtonClick() {
		Intent intent = null ;
		//获得选择的单选按钮id
		int radioId = rgPlayer.getCheckedRadioButtonId();
		//基于id选择Activity
		switch (radioId) {
		case R.id.bitmap_player_radio:
			intent = new Intent(this,BitmapPlayerActivity.class);
			break ;
		default:
//			throw new UnsupportedOperationException("radioId=" +radioId);
		}
		
		//基于外部存储器
		File file  = new File(Environment.getExternalStorageDirectory(),etFileName.getText().toString());
	
		//将AVI文件的名字作为extra内容
		intent.putExtra(AbstractPlayerActivity.EXTRA_FILE_NAME, file.getAbsolutePath());
		//启动 player activiy
		startActivity(intent);
	}

}
