package com.todoroo.aacenc;

import java.io.File;
import java.io.IOException;

import android.app.Activity;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;

public class Main extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        findViewById(R.id.write).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                write();
            }
        });

        findViewById(R.id.play).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                play();
            }
        });
    }

    private void play() {
        String file = "/sdcard/audio.m4a";

        MediaPlayer mediaPlayer = new MediaPlayer();

        try {
            mediaPlayer.setDataSource(file);
            mediaPlayer.prepare();
            mediaPlayer.start();
        } catch (IllegalArgumentException e) {
            throw new RuntimeException(e);
        } catch (IllegalStateException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        Toast.makeText(Main.this, "Playing Audio", Toast.LENGTH_LONG).show();
    }

    private void write() {

        try {
            File dir = getFilesDir();
            String outaac = dir.toString() + "/audio.aac";

            System.err.println("Writing AAC: " + outaac);
            AACEncoder encoder = new AACEncoder();
            encoder.init(64000, 1, 44100, 16, outaac);

            byte[] input = new byte[44100];

            for(int j = 10; j < 100; j += 10) {
                for(int i = 0; i < input.length; i++)
                    input[i] = (byte) Math.round(255 * Math.sin(i * 1.0 / j));

                encoder.encode(input);
            }


            encoder.uninit();

            String outm4a = "/sdcard/audio.m4a";
            System.err.println("Writing M4A: " + outm4a);
            new AACToM4A().convert(this, outaac, outm4a);

            Toast.makeText(Main.this, "WE DID IT", Toast.LENGTH_LONG).show();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}