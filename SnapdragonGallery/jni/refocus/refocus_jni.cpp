#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "jni.h"
#include <android/log.h>
#include <include/Refocus.h>

#define TAG "jni_refocus"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__)

#define ALIGN64(x)  ((x + 63)&(~63))

using namespace wa;

Refocus* refocusAlgo = NULL;
int mainWidth = 0;
int mainHeight = 0;

void init(JNIEnv *env, jobject thiz){
    if (refocusAlgo == NULL) {
        LOGD("init Refocus");
        refocusAlgo = new Refocus();
    }
}

void setImages(JNIEnv *env, jobject thiz, jbyteArray main, jint mainW, jint mainH,
        jint mainRotation, jbyteArray depth, jint depthW, jint depthH, jint depthRotation) {
    unsigned char* mainData  = (unsigned char*)env->GetByteArrayElements(main, 0);
    unsigned char* depthData  = (unsigned char*)env->GetByteArrayElements(depth, 0);
    mainWidth = mainW;
    mainHeight = mainH;
    Image mainImage(mainW, mainH, mainData, Image::IMAGETYPE_RGBA, Image::DT_8U,
            (Image::Rotation) mainRotation);
    Image depthImage(depthW, depthH, depthData, Image::IMAGETYPE_GRAY, Image::DT_32F,
            (Image::Rotation) depthRotation);
    refocusAlgo->setImages(mainImage, depthImage);
    env->ReleaseByteArrayElements(main, (jbyte *)mainData, 0);
    env->ReleaseByteArrayElements(depth, (jbyte *)depthData, 0);
}

jint doRefocus(JNIEnv *env, jobject thiz,
        jbyteArray refocus, jint focusX, jint focusY, jfloat fNum) {
    unsigned char* refocusData  = (unsigned char*)env->GetByteArrayElements(refocus, 0);
    Image refocusImage(mainWidth, mainHeight, refocusData,
            Image::IMAGETYPE_RGBA, Image::DT_8U, Image::ROTATION_0);
    int result = refocusAlgo->run(focusX, focusY, fNum, refocusImage);
    env->ReleaseByteArrayElements(refocus, (jbyte *)refocusData, 0);
    LOGD("do refocus end ret:%d", result);
    return result;
}


void unInit(JNIEnv *env, jobject thiz){
    if (refocusAlgo != NULL) {
        LOGD("release refocus");
        delete refocusAlgo;
        refocusAlgo = NULL;
    }
}

jstring getVersion(JNIEnv *env, jobject thiz){
    return  env->NewStringUTF(refocusAlgo->getVersion());
}
//class name
static const char *classPathName = "com/android/gallery3d/refocus/RefocusJni";
static JNINativeMethod methods[] = {
    {"nativeInit", "()V", (void*) init },
    {"nativeSetImages", "([BIII[BIII)V", (void*) setImages },
    {"nativeDoRefocus", "([BIIF)I", (void*) doRefocus },
    {"nativeUnInit", "()V", (void*) unInit },
    {"nativeGetVersion", "()Ljava/lang/String;", (void*) getVersion },
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
        JNINativeMethod* gMethods, int numMethods) {
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env) {
  if (!registerNativeMethods(env, classPathName,
                 methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}


// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    LOGI("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
        LOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;
}