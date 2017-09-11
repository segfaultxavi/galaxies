#ifndef __GSPRITE_BUTTON_H__
#define __GSPRITE_BUTTON_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"

typedef struct _GSpriteButton GSpriteButton;

typedef int (* GSpriteButtonCallback)(void *userdata);

GSprite *GSpriteButton_new (GResources *res, int x, int y, int w, int h, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    TTF_Font *font, Uint32 color, const char *text, GSpriteButtonCallback callback, void *userdata);

#endif
