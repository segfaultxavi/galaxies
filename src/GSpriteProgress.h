#ifndef __GSPRITE_PROGRESS_H__
#define __GSPRITE_PROGRESS_H__

#include <SDL.h>
#include "GSprite.h"
#include "GResources.h"

typedef struct _GSpriteProgress GSpriteProgress;

GSprite *GSpriteProgress_new (GResources *res, int x, int y, int w, int h, const char *text, TTF_Font *font, Uint32 text_color, Uint32 glow_color, float progress);

void GSpriteProgress_set_progress (GSpriteProgress *spr, float progress);
void GSpriteProgress_set_text (GSpriteProgress *spr, const char *text);

#endif
