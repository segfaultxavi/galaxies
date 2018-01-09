#ifndef __GSPRITE_POPUP_H__
#define __GSPRITE_POPUP_H__

#include <SDL.h>
#include "GSprite.h"
#include "GResources.h"

typedef struct _GSpritePopup GSpritePopup;

typedef void (* GSpritePopupCallback)(void *userdata);

GSprite *GSpritePopup_new (GResources *res, const char *title, const char *text,
  const char *button1, GSpritePopupCallback callback1,
  const char *button2, GSpritePopupCallback callback2,
  void *userdata);

// You can use this as a NOP callback
void GSpritePopup_dismiss (void *userdata);

#endif
