//
// Created by 徐安 on 2016/7/11.
//
#include "com_example_speextest_Speex.h"
#include <jni.h>

#include <string.h>
#include <unistd.h>

#include <speex/speex.h>
#include <speex/speex_preprocess.h>
#include <speex/speex_echo.h>

SpeexPreprocessState *preprocess_state;//预处理接口
SpeexEchoState *echo_state;

JNIEXPORT void JNICALL Java_com_example_speextest_Speex_open
  (JNIEnv *env, jclass obj, jint frame_size, jint frequency, jint filter_length) {

    preprocess_state = speex_preprocess_state_init(frame_size, frequency);


    int t = 1;
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_VAD, &t);
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_AGC, &t);
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_DENOISE, &t);
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_DEREVERB, &t);

    int noiseSuppress = -25;//原始值 ： 25
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &noiseSuppress); //设置噪声的dB
    int q = 24000;
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_AGC_LEVEL,&q);
    int vadProbStart = 80;//原始值 ：80
    int vadProbContinue = 65;//原始值 ： 65
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_PROB_START , &vadProbStart);
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_PROB_CONTINUE, &vadProbContinue);



    echo_state = speex_echo_state_init(frame_size, filter_length);
    speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_ECHO_STATE,echo_state);
}

JNIEXPORT void JNICALL Java_com_example_speextest_Speex_AEC
  (JNIEnv *env, jclass obj, jshortArray input_frame, jshortArray echo_frame, jshortArray output_frame){

  jint length = env->GetArrayLength(input_frame);
  jshort buffer1[length];
  jshort buffer2[length];
  jshort buffer3[length];
  env->GetShortArrayRegion(input_frame, 0, length, buffer1);
  env->GetShortArrayRegion(echo_frame, 0, length, buffer2);
  speex_echo_cancellation(echo_state, buffer1, buffer2, buffer3);

  speex_preprocess_run(preprocess_state, buffer3);

  env->SetShortArrayRegion(output_frame, 0, length, buffer3);

}

JNIEXPORT void JNICALL Java_com_example_speextest_Speex_playback
  (JNIEnv *env, jclass obj, jshortArray play_frame){
  jint length = env->GetArrayLength(play_frame);
  jshort buffer[length];
  env->GetShortArrayRegion(play_frame, 0, length, buffer);
  speex_echo_playback(echo_state, buffer);
}

JNIEXPORT void JNICALL Java_com_example_speextest_Speex_capture
  (JNIEnv *env, jclass obj, jshortArray input_frame, jshortArray output_frame){
  jint length = env->GetArrayLength(input_frame);
  jshort buffer1[length];
  jshort buffer2[length];
  env->GetShortArrayRegion(input_frame, 0, length, buffer1);
  speex_echo_capture(echo_state, buffer1, buffer2);
  speex_preprocess_run(preprocess_state, buffer2);
  env->SetShortArrayRegion(output_frame, 0, length, buffer2);
}

JNIEXPORT jint JNICALL Java_com_example_speextest_Speex_test
  (JNIEnv *, jclass, jint a){
  return a*a;
}