#ifndef __GSPRITE_LABEL_H__
#define __GSPRITE_LABEL_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"

typedef enum _GSpriteLabelJustify {
  GLABEL_JUSTIFY_BEGIN,
  GLABEL_JUSTIFY_CENTER,
  GLABEL_JUSTIFY_END
} GSpriteLabelJustify;

GSprite *GSpriteLabel_new (int x, int y, GSpriteLabelJustify justify_hor, GSpriteLabelJustify justify_ver,
  SDL_Renderer *renderer, TTF_Font *font, Uint32 color, const char *text);

#endif
