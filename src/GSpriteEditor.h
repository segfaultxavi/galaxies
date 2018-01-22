#ifndef __GSPRITE_EDITOR_H__
#define __GSPRITE_EDITOR_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"
#include "GSpriteLevelSelect.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpriteEditor GSpriteEditor;

GSprite *GSpriteEditor_new (GResources *resources, GSprite *main_menu, const char *desc);
void GSpriteEditor_cores_changed (GSpriteEditor *spr);
void GSpriteEditor_tiles_changed (GSpriteEditor *spr, char *desc);

#ifdef __cplusplus
}
#endif

#endif
