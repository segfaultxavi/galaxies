#ifndef __GSPRITE_CREDITS_H__
#define __GSPRITE_CREDITS_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GResources.h"
#include "GSprite.h"

typedef struct _GSpriteCredits GSpriteCredits;

GSprite *GSpriteCredits_new (GResources *resources, GSprite *main_menu);

#endif
