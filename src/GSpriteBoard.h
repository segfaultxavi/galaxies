#ifndef __GSPRITE_BOARD_H__
#define __GSPRITE_BOARD_H__

#include "GSprite.h"
#include "GResources.h"
#include "GSpriteTile.h"

typedef struct _GSpriteBoard GSpriteBoard;

GSprite *GSpriteBoard_new (int editing, GResources *res);
void GSpriteBoard_start (GSpriteBoard *spr, int mapSizeX, int mapSizeY, float *cores);
GSpriteTile *GSpriteBoard_get_tile (GSpriteBoard *spr, int x, int y);

#endif
