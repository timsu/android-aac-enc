#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <inc/voAAC.h>
#include <inc/cmnMemory.h>
#include <android/log.h> 

#define DEBUG 1

#if DEBUG
#define LOG(msg, args...) __android_log_print(ANDROID_LOG_ERROR, "aac-enc", msg, ## args)
#else
#define LOG(msg)
#endif

/* utility functions */

void throwException(JNIEnv* env, const char *name, const char *msg)
{
  jclass cls = (*env)->FindClass(env, name);
  /* if cls is NULL, an exception has already been thrown */
  if (cls != NULL) {
    (*env)->ThrowNew(env, cls, msg);
  }
  /* free the local ref */
  (*env)->DeleteLocalRef(env, cls);
}

/* internal storage */

FILE* outfile;

VO_AUDIO_CODECAPI codec_api;
VO_HANDLE handle = 0;
VO_AUDIO_CODECAPI codec_api = { 0 };
VO_MEM_OPERATOR mem_operator = { 0 };
VO_CODEC_INIT_USERDATA user_data;
AACENC_PARAM params = { 0 };

/* java native functions */

void
Java_com_todoroo_aacenc_AACEncoder_init( JNIEnv* env,
                                         jobject thiz,
                                         int bitrate,
                                         int channels,
                                         int sampleRate,
                                         int bitsPerSample,
                                         jstring outputFile)
{

  if (bitsPerSample != 16) {
    throwException(env, "java/lang/IllegalArgumentException", 
                   "Unsupported sample depth. Only 16 bits per sample is supported");
    return;
  }
  
  voGetAACEncAPI(&codec_api);

  mem_operator.Alloc = cmnMemAlloc;
  mem_operator.Copy = cmnMemCopy;
  mem_operator.Free = cmnMemFree;
  mem_operator.Set = cmnMemSet;
  mem_operator.Check = cmnMemCheck;
  user_data.memflag = VO_IMF_USERMEMOPERATOR;
  user_data.memData = &mem_operator;
  codec_api.Init(&handle, VO_AUDIO_CodingAAC, &user_data);

  params.sampleRate = sampleRate;
  params.bitRate = bitrate;
  params.nChannels = channels;
  params.adtsUsed = 1;
  if (codec_api.SetParam(handle, VO_PID_AAC_ENCPARAM, &params) != VO_ERR_NONE) {
    throwException(env, "java/lang/IllegalArgumentException", 
                   "Unable to set encoding parameters");
    return;
  }

  const char* output_file = (*env)->GetStringUTFChars(env, outputFile, (jboolean) 0);
  outfile = fopen(output_file, "wb");
  LOG("writing to %s", output_file);
  (*env)->ReleaseStringUTFChars(env, outputFile, output_file);

  LOG("initialized handle: %x", handle);

}

void
Java_com_todoroo_aacenc_AACEncoder_encode( JNIEnv* env,
                                           jobject thiz,
                                           jbyteArray inputArray)
{

  LOG("writing to handle: %x", handle);

  jbyte* buffer = (*env)->GetByteArrayElements(env, inputArray, (jboolean) 0);
  int inputSize = (*env)->GetArrayLength(env, inputArray);

  VO_CODECBUFFER input = { 0 }, output = { 0 };
  VO_AUDIO_OUTPUTINFO output_info = { 0 };

  unsigned char outbuf[20480];

  LOG("input buffer: %d", inputSize);

  /* SET INPUT DATA */
  input.Buffer = buffer;
  input.Length = inputSize;
  codec_api.SetInputData(handle, &input);

  output.Buffer = outbuf;
  output.Length = sizeof(outbuf);

  LOG("output data time");

  /* GET OUTPUT DATA */
  int i;
  for (i = 0; i < 5; i++) {
    int status = codec_api.GetOutputData(handle, &output, &output_info);
    if (status == VO_ERR_INPUT_BUFFER_SMALL)
      break;

    if (status != VO_ERR_NONE) {
      char message[100];
      sprintf(message, "Unable to encode frame: %x", status);
      throwException(env, "java/lang/RuntimeException", message);
      return;
    }

    LOG("output length: %d (%d)", output.Length, i);

    fwrite(outbuf, 1, output.Length, outfile);

  }

  LOG("finished output");
  (*env)->ReleaseByteArrayElements(env, inputArray, buffer, JNI_ABORT);
}

void
Java_com_todoroo_aacenc_AACEncoder_uninit( JNIEnv* env,
                                           jobject thiz)
{

  fclose(outfile);
  codec_api.Uninit(handle);

}

JNIEXPORT jint JNICALL 
JNI_OnLoad (JavaVM * vm, void * reserved) 
{ 
  return JNI_VERSION_1_6; 
} 
