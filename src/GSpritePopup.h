#ifndef __GSPRITE_POPUP_H__
#define __GSPRITE_POPUP_H__

#include <SDL.h>
#include "GSprite.h"
#include "GResources.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpritePopup GSpritePopup;

typedef void (* GSpritePopupCallback)(void *userdata);

GSprite *GSpritePopup_new (GResources *res, const char *title_text, const char *icon_text, const char *body_text,
  const char *button1_text, GSpritePopupCallback callback1, const char *button1_icon,
  const char *button2_text, GSpritePopupCallback callback2, const char *button2_icon,
  void *userdata);

// You can use this as a NOP callback
void GSpritePopup_dismiss (void *userdata);

#ifdef __cplusplus
}
#endif

#endif
