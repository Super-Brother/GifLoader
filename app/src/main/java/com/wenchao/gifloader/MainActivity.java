package com.wenchao.gifloader;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private ImageView imageView;
    private String path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "demo.gif";
    private GifHandle gifHandle;
    private Bitmap bitmap;

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {

                case 1:
                    final long nextFrameTime = gifHandle.renderFrame(bitmap);
                    imageView.setImageBitmap(bitmap);
                    handler.sendEmptyMessageDelayed(1, nextFrameTime);
                    break;

                default:
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imageView = findViewById(R.id.imageView);

        gifHandle = new GifHandle(path);

        int width = gifHandle.getWidth();
        int height = gifHandle.getHeight();
        bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        //图片渲染
        final long nextFrameTime = gifHandle.renderFrame(bitmap);
        if (handler != null) {
            handler.sendEmptyMessageDelayed(1, nextFrameTime);
        }
        imageView.setImageBitmap(bitmap);
    }
}
