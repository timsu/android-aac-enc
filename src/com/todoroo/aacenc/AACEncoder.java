package com.todoroo.aacenc;

public class AACEncoder {

    /**
     * Native JNI
     *
     */
    public native byte[] encode(int bitrate, int channels,
            int sampleRate, int bitsPerSample, byte[] inputArray);

    static {
        System.loadLibrary("aac-encoder");
    }

}
