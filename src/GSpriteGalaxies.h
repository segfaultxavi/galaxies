#ifndef __GSPRITE_GALAXIES_H__
#define __GSPRITE_GALAXIES_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"
#include "GSpriteLevelSelect.h"

typedef struct _GSpriteGalaxies GSpriteGalaxies;

GSprite *GSpriteGalaxies_new (GResources *resources, GSprite *level_select, const char *level_description, void *level_data);
void GSpriteGalaxies_complete (GSpriteGalaxies *spr);
void GSpriteGalaxies_update_level_status (GSpriteGalaxies *spr, GSpriteLevelSelectLevelStatus status, char *desc);

#endif
