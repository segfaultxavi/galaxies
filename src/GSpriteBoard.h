#ifndef __GSPRITE_BOARD_H__
#define __GSPRITE_BOARD_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"

typedef struct _GSpriteBoard GSpriteBoard;

GSprite *GSpriteBoard_new (int editing, GResources *res);
void GSpriteBoard_start (GSpriteBoard *spr, int mapSizeX, int mapSizeY, float *cores);

#endif
