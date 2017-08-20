#ifndef __GSPRITE_LABEL_H__
#define __GSPRITE_LABEL_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"

GSprite *GSpriteLabel_new (int x, int y, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
  GResources *res, TTF_Font *font, Uint32 color, const char *text);

#endif
