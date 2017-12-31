#ifndef __GSPRITE_GALAXIES_H__
#define __GSPRITE_GALAXIES_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"

typedef struct _GSpriteGalaxies GSpriteGalaxies;

GSprite *GSpriteGalaxies_new (GResources *resources, GSprite *level_select, const char *level_description, void *level_data);
void GSpriteGalaxies_complete (GSpriteGalaxies *spr);

#endif
