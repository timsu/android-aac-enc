package com.todoroo.aacenc;

import java.io.File;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Toast;

public class Main extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        try {
            File dir = getFilesDir();
            String output = dir.toString() + "/audio.aac";

            AACEncoder encoder = new AACEncoder();
            encoder.init(64000, 1, 16000, 16, output);

            byte[] input = new byte[16000];
            for(int i = 0; i < input.length; i++)
                input[i] = (byte) Math.round(255 * Math.sin(i * 1.0 / 10));

            encoder.encode(input);

            for(int i = 0; i < input.length; i++)
                input[i] = (byte) Math.round(255 * Math.sin(i * 1.0 / 100));

            encoder.encode(input);

            for(int i = 0; i < input.length; i++)
                input[i] = (byte) Math.round(255 * Math.sin(i * 1.0 / 200));

            encoder.encode(input);

            encoder.uninit();

            Toast.makeText(Main.this, "WE DID IT", Toast.LENGTH_LONG).show();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}