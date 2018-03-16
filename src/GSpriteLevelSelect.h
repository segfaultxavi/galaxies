#ifndef __GSPRITE_LEVEL_SELECT_H__
#define __GSPRITE_LEVEL_SELECT_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GResources.h"
#include "GSprite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _GSpriteLevelSelectLevelStatus {
  GSPRITE_LEVEL_SELECT_LEVEL_STATUS_UNTRIED,
  GSPRITE_LEVEL_SELECT_LEVEL_STATUS_IN_PROGRESS,
  GSPRITE_LEVEL_SELECT_LEVEL_STATUS_DONE
} GSpriteLevelSelectLevelStatus;

GSprite *GSpriteLevelSelect_new (GResources *resources, GSprite *main_menu);

void GSpriteLevelSelect_update_level_status (void *userdata, GSpriteLevelSelectLevelStatus status, char *desc);
void GSpriteLevelSelect_play_next_level (void *userdata);

#ifdef __cplusplus
}
#endif

#endif
