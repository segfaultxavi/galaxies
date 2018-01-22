#ifndef __GSPRITE_GALAXIES_H__
#define __GSPRITE_GALAXIES_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"
#include "GSpriteLevelSelect.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpriteGalaxies GSpriteGalaxies;

GSprite *GSpriteGalaxies_new (GResources *resources, GSprite *level_select, int level_number, const char *level_description, void *level_data);
void GSpriteGalaxies_complete (GSpriteGalaxies *spr);
void GSpriteGalaxies_update_level_status (GSpriteGalaxies *spr, GSpriteLevelSelectLevelStatus status, char *desc);

#ifdef __cplusplus
}
#endif

#endif
