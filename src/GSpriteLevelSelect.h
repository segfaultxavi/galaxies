#ifndef __GSPRITE_LEVEL_SELECT_H__
#define __GSPRITE_LEVEL_SELECT_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GResources.h"
#include "GSprite.h"

typedef struct _GSpriteLevelSelect GSpriteLevelSelect;

GSprite *GSpriteLevelSelect_new (GResources *resources, GSprite *main_menu);

#endif
