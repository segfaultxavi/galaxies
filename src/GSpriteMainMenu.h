#ifndef __GSPRITE_MAIN_MENU_H__
#define __GSPRITE_MAIN_MENU_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"

GSprite *GSpriteMainMenu_new (SDL_Renderer *renderer, TTF_Font *font_big, TTF_Font *font_med);

#endif
