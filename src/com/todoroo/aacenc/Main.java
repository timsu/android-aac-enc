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

            String outm4a = dir.toString() + "/audio.m4a";
            System.err.println("Writing M4A: " + outm4a);
            new AACToM4A().convert(outaac, outm4a);

            Toast.makeText(Main.this, "WE DID IT", Toast.LENGTH_LONG).show();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}