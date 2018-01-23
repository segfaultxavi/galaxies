#include <SDL.h>
#include "GResources.h"
#include "GPrefs.h"
#include "GAndroid.h"

#ifndef __ANDROID__ 

void GAndroid_free (GAndroid *android) { }
GAndroid *GAndroid_new (GResources *res) { return NULL; }

#else

#include <jni.h>

// Global var to store the Java VM until the game instantiates a GAndroid
// object where we can store it.
JavaVM *GAndroid_vm = NULL;

struct _GAndroid {
  GResources *res;
  JavaVM *vm;
  JNIEnv *env;
};

void GAndroid_free (GAndroid *android) {
  if (!android) return;

  SDL_free (android);
}

GAndroid *GAndroid_new (GResources *res) {
  GAndroid *android = SDL_malloc (sizeof (GAndroid));
  jint ret;

  android->res = res;
  android->vm = GAndroid_vm;
  ret = AttachCurrentThread (android->vm, &android->env, NULL);
  if (ret != JNI_OK) {
    SDL_Log ("AttachCurrentThread: %d", ret);
    goto error;
  }

  SDL_Log ("GAndroid_new: Using VM %p", GAndroid_vm);

  return android;

error:
  SDL_free (android);
  return NULL;
}

jint JNI_OnLoad (JavaVM *vm, void *reserved) {

  GAndroid_vm = vm;

  SDL_Log ("JNI_OnLoad: Got VM %p", vm);

  return JNI_VERSION_1_4;
}

#endif // __ANDROID__