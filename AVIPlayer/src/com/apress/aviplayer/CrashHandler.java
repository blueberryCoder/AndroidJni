package com.apress.aviplayer;


import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.os.Looper;
import android.util.Log;
import android.view.ViewDebug.IntToString;
import android.widget.Toast;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.lang.Thread.UncaughtExceptionHandler;

/**
 * UncaughtException处理类,当程序发生Uncaught异常的时候,
 * 由该类来接管程序,并记录发送错误报告.
 * 需要在Application中注册，为了要在程序启动器就监控整个程序。
 */
public class CrashHandler implements UncaughtExceptionHandler {
    public static final String TAG = "CrashHandler";
    private UncaughtExceptionHandler mDefaultHandler;
    private static volatile CrashHandler instance = new CrashHandler();
    private Context context;

    private CrashHandler() {
    }

    public static CrashHandler getInstance() {
        return instance;
    }

    public void init(Context context) {
        this.context = context;
        mDefaultHandler = Thread.getDefaultUncaughtExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler(this);
    }

    @Override
    public void uncaughtException(Thread thread, Throwable ex) {
        if (!handleException(ex)) {
            mDefaultHandler.uncaughtException(thread, ex);
        } else {
//            new Thread(new Runnable() {
//                @Override
//                public void run() {
//                    Looper.prepare();
//                    Log.w(TAG, "currentThread: " + Thread.currentThread().getName());
//                    Toast.makeText(context, "很抱歉,程序出现异常,即将退出.", Toast.LENGTH_SHORT).show();
//                    Looper.loop();
//                }
//            }).start();
        	
            try {
                Thread.sleep(3000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            ex.printStackTrace();
            
            Intent intent = new Intent(context,MainActivity.class) ;
        	intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        	context.startActivity(intent);
        	
            System.exit(0);
           
        }

    }


    private boolean handleException(Throwable ex) {
        try {
            PrintWriter pw = new PrintWriter(new BufferedOutputStream(new FileOutputStream(
                    Environment.getExternalStorageDirectory().getAbsolutePath()
                            + File.separator + "crash.txt")));
            ex.printStackTrace(pw);
            ex.printStackTrace();
            pw.flush();
            pw.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        return true;
    }

}