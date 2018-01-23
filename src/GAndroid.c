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
  jint ret;

#if 0
  ret = (*android->vm)->DetachCurrentThread (android->vm);
  if (ret != JNI_OK) {
    SDL_Log ("DetachCurrentThread: %d", ret);
  }
#endif
  SDL_free (android);
}

GAndroid *GAndroid_new (GResources *res) {
  GAndroid *android = SDL_malloc (sizeof (GAndroid));
  jint ret;

  android->res = res;
  android->vm = GAndroid_vm;
  SDL_Log ("GAndroid_new: Using VM %p", android->vm);

  ret = (*android->vm)->AttachCurrentThread (android->vm, &android->env, NULL);
  if (ret != JNI_OK) {
    SDL_Log ("AttachCurrentThread: %d", ret);
    goto error;
  }
  SDL_Log ("GAndroid_new: Using JNIEnv %p", android->env);

  // options = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN);
  jclass siob_class = (*android->env)->FindClass (android->env,
      "com/google/android/gms/auth/api/signin/GoogleSignInOptions$Builder");
  SDL_Log ("GAndroid_new: siob_class %p", siob_class);
  jmethodID siob_init_mid = (*android->env)->GetMethodID (android->env, siob_class, "<init>", "()V");
  SDL_Log ("GAndroid_new: siob_init_mid %p", siob_init_mid);
  jobject siob = (*android->env)->NewObject (android->env, siob_class, siob_init_mid);
  SDL_Log ("GAndroid_new: siob %p", siob);
  jclass sio_class = (*android->env)->FindClass (android->env,
      "com/google/android/gms/auth/api/signin/GoogleSignInOptions");
  SDL_Log ("GAndroid_new: sio_class %p", sio_class);
  jfieldID sio_player_login_fid = (*android->env)->GetStaticFieldID (android->env, sio_class,
      "DEFAULT_GAMES_SIGN_IN", "com/google/android/gms/auth/api/signin/GoogleSignInOptions");
  SDL_Log ("GAndroid_new: sio_player_login_fid %p", sio_player_login_fid);
  jobject sio_player_login = (*android->env)->GetStaticObjectField (android->env, sio_class, sio_player_login_fid);
  SDL_Log ("GAndroid_new: sio_player_login %p", sio_player_login);
  // options.build();
  jmethodID siob_build_mid = (*android->env)->GetMethodID (android->env, siob_class,
      "build", "()Lcom/google/android/gms/auth/api/signin/GoogleSignInOptions;");
  SDL_Log ("GAndroid_new: siob_build_mid %p", siob_build_mid);
  jobject sio = (*android->env)->CallObjectMethod (android->env, siob, siob_build_mid);
  SDL_Log ("GAndroid_new: sio %p", sio);

  // GoogleApiClient.Builder clientBuilder = new GoogleApiClient.Builder(getActivity());
  jclass gacb_class = (*android->env)->FindClass (android->env,
      "com/google/android/gms/common/api/GoogleApiClient$Builder");
  SDL_Log ("GAndroid_new: gacb_class %p", gacb_class);
  jmethodID gacb_init_mid = (*android->env)->GetMethodID (android->env, gacb_class, "<init>", "(Landroid/content/Context;)V");
  SDL_Log ("GAndroid_new: gacb_init_mid %p", gacb_init_mid);
  jobject activity = (jobject)SDL_AndroidGetActivity();
  SDL_Log ("GAndroid_new: activity %p", activity);
  jobject gacb = (*android->env)->NewObject (android->env, gacb_class, gacb_init_mid, activity);
  SDL_Log ("GAndroid_new: gacb %p", gacb);

  // clientBuilder.addApi(Auth.GOOGLE_SIGN_IN_API, options);
  jmethodID gacb_addApi_mid = (*android->env)->GetMethodID (android->env, gacb_class, "addApi",
      "(Lcom/google/android/gms/common/api/Api;)Lcom/google/android/gms/common/api/GoogleApiClient$Builder;");
  SDL_Log ("GAndroid_new: gacb_addApi_mid %p", gacb_addApi_mid);
  jclass auth_class = (*android->env)->FindClass (android->env,
      "com/google/android/gms/auth/api/Auth");
  SDL_Log ("GAndroid_new: auth_class %p", auth_class);
  jfieldID auth_GSIAPI_fid = (*android->env)->GetStaticFieldID (android->env, auth_class,
      "GOOGLE_SIGN_IN_API", "com/google/android/gms/common/api/Api");
  SDL_Log ("GAndroid_new: auth_GSIAPI_fid %p", auth_GSIAPI_fid);
  jobject auth_GSIAPI = (*android->env)->GetStaticObjectField (android->env, auth_class, auth_GSIAPI_fid);
  SDL_Log ("GAndroid_new: auth_GSIAPI %p", auth_GSIAPI);
  jobject gacb2 = (*android->env)->CallObjectMethod (android->env, gacb, gacb_addApi_mid, auth_GSIAPI);
  SDL_Log ("GAndroid_new: gacb2 %p", gacb2);

  // clientBuilder.addApi(getGamesAPI());
  jclass games_class = (*android->env)->FindClass (android->env,
      "com/google/android/gms/games/Games");
  SDL_Log ("GAndroid_new: games_class %p", games_class);
  jfieldID games_API_fid = (*android->env)->GetStaticFieldID (android->env, games_class,
      "API", "com/google/android/gms/common/api/Api");
  SDL_Log ("GAndroid_new: games_API_fid %p", games_API_fid);
  jobject games_API = (*android->env)->GetStaticObjectField (android->env, games_class, games_API_fid);
  SDL_Log ("GAndroid_new: games_API %p", games_API);
  jobject gacb3 = (*android->env)->CallObjectMethod (android->env, gacb2, gacb_addApi_mid, games_API);
  SDL_Log ("GAndroid_new: gacb3 %p", gacb3);

  // mGoogleApiClient = clientBuilder.build();
  jmethodID gacb_build_mid = (*android->env)->GetMethodID (android->env, gacb_class, "build",
      "()Lcom/google/android/gms/common/api/GoogleApiClient;");
  SDL_Log ("GAndroid_new: gacb_build_mid %p", gacb_build_mid);
  jobject gac = (*android->env)->CallObjectMethod (android->env, gacb3, gacb_build_mid);
  SDL_Log ("GAndroid_new: gac %p", gac);

  // mGoogleApiClient.connect(GoogleApiClient.SIGN_IN_MODE_OPTIONAL);
  jclass gac_class = (*android->env)->FindClass (android->env,
      "com/google/android/gms/common/api/GoogleApiClient");
  SDL_Log ("GAndroid_new: gac_class %p", gac_class);
  jfieldID gac_SIMO_fid = (*android->env)->GetStaticFieldID (android->env, gac_class,
      "SIGN_IN_MODE_OPTIONAL", "I");
  SDL_Log ("GAndroid_new: gac_SIMO_fid %p", gac_SIMO_fid);
  int gac_SIMO = (*android->env)->GetStaticIntField (android->env, gac_class, gac_SIMO_fid);
  SDL_Log ("GAndroid_new: gac_SIMO %d", gac_SIMO);
  jmethodID gac_connect_mid = (*android->env)->GetMethodID (android->env, gac_class, "connect", "(I)V");
  SDL_Log ("GAndroid_new: gac_connect_mid %p", gac_connect_mid);
  (*android->env)->CallVoidMethod (android->env, gac, gac_connect_mid, gac_SIMO);


  jmethodID gac_hasConnected_mid = (*android->env)->GetMethodID (android->env, gac_class, "hasConnectedApi",
      "(Lcom/google/android/gms/common/api/Api;)Z");
  SDL_Log ("GAndroid_new: gac_hasConnected_mid %p", gac_hasConnected_mid);
  jboolean hasConnected = (*android->env)->CallBooleanMethod (android->env, gac, gac_hasConnected_mid, auth_GSIAPI);
  SDL_Log ("GAndroid_new: hasConnected %d", hasConnected);
  if (!hasConnected) {
    // Auth.GoogleSignInApi.silentSignIn(mGoogleApiClient)
    jfieldID auth_GSI_fid = (*android->env)->GetStaticFieldID (android->env, auth_class,
      "GoogleSignInApi", "com/google/android/gms/auth/api/signin/GoogleSignInApi");
    SDL_Log ("GAndroid_new: auth_GSI_fid %p", auth_GSI_fid);
    jobject auth_GSI = (*android->env)->GetStaticObjectField (android->env, auth_class, auth_GSI_fid);
    SDL_Log ("GAndroid_new: auth_GSI %p", auth_GSI);
    jclass gsi_class = (*android->env)->FindClass (android->env,
        "com/google/android/gms/auth/api/signin/GoogleSignInApi");
    SDL_Log ("GAndroid_new: gsi_class %p", gsi_class);
    jmethodID gsi_silentSignIn_mid = (*android->env)->GetMethodID (android->env, gsi_class, "silentSignIn",
        "(Lcom/google/android/gms/common/api/GoogleApiClient;)Lcom/google/android/gms/common/api/OptionalPendingResult;");
    SDL_Log ("GAndroid_new: gsi_silentSignIn_mid %p", gsi_silentSignIn_mid);
    jobject pending_result = (*android->env)->CallObjectMethod (android->env, auth_GSI, gsi_silentSignIn_mid, gac);
    SDL_Log ("GAndroid_new: pending_result %p", pending_result);
    // .setResultCallback(
    jclass pr_class = (*android->env)->FindClass (android->env,
        "com/google/android/gms/common/api/OptionalPendingResult");
    SDL_Log ("GAndroid_new: pr_class %p", gsi_class);
    jmethodID pr_setResultCB_mid = (*android->env)->GetMethodID (android->env, pr_class, "setResultCallback",
        "(Lcom/google/android/gms/common/api/ResultCallback;)V");
    SDL_Log ("GAndroid_new: pr_setResultCB_mid %p", pr_setResultCB_mid);
  }

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
