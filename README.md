Android AAC Encoder project
============================

Extraction of Android Stagefright VO AAC encoder with a nice Java API.

In addition, includes a patched
(MP4Parser)[http://code.google.com/p/mp4parser/] Java library for wrapping
AAC files in an MP4 container to produce M4A audio files playable by Google
Chrome and Apple QuickTime.

This project is set up as a single Eclipse project with a Main.java example
activity. AAC encoding logic is found in jni/aac-enc.c, which needs to be
built with the [Android
NDK](http://developer.android.com/sdk/ndk/index.html). I used NDK r7c, but
any version should work.


