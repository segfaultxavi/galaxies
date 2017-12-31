#ifndef __GSPRITE_LEVEL_SELECT_H__
#define __GSPRITE_LEVEL_SELECT_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GResources.h"
#include "GSprite.h"

typedef enum _GSpriteLevelSelectLevelStatus {
  GSPRITE_LEVEL_SELECT_LEVEL_STATUS_UNTRIED,
  GSPRITE_LEVEL_SELECT_LEVEL_STATUS_IN_PROGRESS,
  GSPRITE_LEVEL_SELECT_LEVEL_STATUS_DONE
} GSpriteLevelSelectLevelStatus;

typedef struct _GSpriteLevelSelect GSpriteLevelSelect;

GSprite *GSpriteLevelSelect_new (GResources *resources, GSprite *main_menu);

void GSpriteLevelSelect_update_level (void *userdata, GSpriteLevelSelectLevelStatus status);

#endif
