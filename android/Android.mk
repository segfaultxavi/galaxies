LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES :=  bajoran_font.c GGame.c GGraphics.c GSpriteBoard.c GSpriteBoardGrid.c GSpriteButton.c GSprite.c GSpriteCore.c GSpriteCredits.c GSpriteGalaxies.c GSpriteLabel.c GSpriteLevelSelect.c GSpriteMainMenu.c GSpriteNull.c GSpriteTile.c main.c spincycle_font.c days_font.c GPrefs.c GSpriteEditor.c GSpritePopup.c GSolver.c GSpriteProgress.c telegrama_font.c GAudio.c ping_wav.c pong_wav.c woosh_wav.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
