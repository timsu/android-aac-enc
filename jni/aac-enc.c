#include <string.h>
#include <jni.h>
#include <inc/voAAC.h>
#include <inc/cmnMemory.h>

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

jbyteArray
Java_com_todoroo_aacenc_LibAacEnc_encode( JNIEnv* env,
                                          jobject thiz,
                                          int bitrate,
                                          int channels,
                                          int sampleRate,
                                          int bitsPerSample,
                                          jbyteArray inputArray)
{
  int ch;

  VO_AUDIO_CODECAPI codec_api = { 0 };
  VO_HANDLE handle = 0;
  VO_MEM_OPERATOR mem_operator = { 0 };
  VO_CODEC_INIT_USERDATA user_data;
  AACENC_PARAM params = { 0 };

  if (bitsPerSample != 16) {
    throwException(env, "java/lang/IllegalArgumentException", 
                   "Unsupported WAV sample depth. Only 16 bits per sample is supported");
    return NULL;
  }
  
  jbyte* buffer = (*env)->GetByteArrayElements(env, inputArray, (jboolean) 0);
  int inputSize = (*env)->GetArrayLength(env, inputArray);

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
    return NULL;
  }

  VO_CODECBUFFER input = { 0 }, output = { 0 };
  VO_AUDIO_OUTPUTINFO output_info = { 0 };
  int read, i;

  unsigned char* outbuf = (unsigned char*) malloc(inputSize);

  input.Buffer = buffer;
  input.Length = inputSize;
  codec_api.SetInputData(handle, &input);

  output.Buffer = outbuf;
  output.Length = sizeof(outbuf);

  if (codec_api.GetOutputData(handle, &output, &output_info) != VO_ERR_NONE) {
    throwException(env, "java/lang/RuntimeException", 
                   "Unable to encode frame");
    return NULL;
  }

  jbyteArray result = (*env)->NewByteArray(env, output.Length);
  if (result == NULL)
    return NULL;

  (*env)->SetByteArrayRegion(env, result, 0, output.Length, outbuf);

  free(outbuf);
  codec_api.Uninit(handle);

  return result;
}

