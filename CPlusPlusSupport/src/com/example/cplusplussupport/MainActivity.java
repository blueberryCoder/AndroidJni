package com.example.cplusplussupport;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map.Entry;
import java.util.Set;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		List<String> mList = new ArrayList<String>();
		String value = "value" ;
		mList.add(value);
		mList.add(value);
		mList.add(value);
		mList.add(value);
		
		Set<String> hashSet = new HashSet<String>();
		hashSet.add(value);
		hashSet.add(value);
		hashSet.add(value);
		hashSet.add(value);
		hashSet.add(value);
		
		for(String str: mList){
			Log.i("LOG_MainActivity","List str: "+str);
		}
		
		for(String str: hashSet){
			Log.i("LOG_MainActivity","Set str: "+str);
		}
	}
}
