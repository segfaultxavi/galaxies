LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := \
  GGame.c \
  GGraphics.c \
  GSpriteBoard.c \
  GSpriteBoardGrid.c \
  GSpriteButton.c \
  GSprite.c \
  GSpriteCore.c \
  GSpriteCredits.c \
  GSpriteGalaxies.c \
  GSpriteLabel.c \
  GSpriteLevelSelect.c \
  GSpriteMainMenu.c \
  GSpriteNull.c \
  GSpriteTile.c \
  main.c \
  GPrefs.c \
  GSpriteEditor.c \
  GSpritePopup.c \
  GSolver.c \
  GSpriteProgress.c \
  GSpriteOptions.c \
  GSpriteImage.c \
  GAudio.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer SDL2_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
