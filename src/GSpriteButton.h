#ifndef __GSPRITE_BUTTON_H__
#define __GSPRITE_BUTTON_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"

typedef int (* GSpriteButtonCallback)(void *userdata);

GSprite *GSpriteButton_new (int x, int y, int w, int h, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    GResources *resources, TTF_Font *font, Uint32 color, const char *text, GSpriteButtonCallback callback, void *userdata);

#endif
