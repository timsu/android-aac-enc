package com.todoroo.aacenc;

import java.io.FileOutputStream;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class Main extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        Button button = (Button) findViewById(R.id.button);
        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                AACEncoder encoder = new AACEncoder();

                byte[] input = new byte[1000];
                for(int i = 0; i < input.length; i++)
                    input[i] = (byte) Math.round(255 * Math.sin(i * 1.0 / 100));

                byte[] output = encoder.encode(64000, 1, 16000, 16, input);

                try {
                    FileOutputStream fos = new FileOutputStream("/sdcard/output.aac");
                    fos.write(output);
                    fos.close();

                    Toast.makeText(Main.this, "WE DID IT", Toast.LENGTH_LONG).show();
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        });

    }
}