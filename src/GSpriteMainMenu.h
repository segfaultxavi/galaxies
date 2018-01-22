#ifndef __GSPRITE_MAIN_MENU_H__
#define __GSPRITE_MAIN_MENU_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpriteMainMenu GSpriteMainMenu;

GSprite *GSpriteMainMenu_new (GResources *resources);

#ifdef __cplusplus
}
#endif

#endif
