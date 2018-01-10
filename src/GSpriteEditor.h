#ifndef __GSPRITE_EDITOR_H__
#define __GSPRITE_EDITOR_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"
#include "GSpriteLevelSelect.h"

typedef struct _GSpriteEditor GSpriteEditor;

GSprite *GSpriteEditor_new (GResources *resources, GSprite *main_menu);
void GSpriteEditor_board_changed (GSpriteEditor *editor);

#endif
