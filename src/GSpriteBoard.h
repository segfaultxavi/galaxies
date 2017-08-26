#ifndef __GSPRITE_BOARD_H__
#define __GSPRITE_BOARD_H__

#include "GSprite.h"
#include "GResources.h"

typedef struct _GSpriteBoard GSpriteBoard;

#define GBOARD_TILE(spr,x,y) spr->tiles[(y) * spr->mapSizeX + (x)]

GSprite *GSpriteBoard_new (int editing, GResources *res);
void GSpriteBoard_start (GSpriteBoard *spr, int mapSizeX, int mapSizeY, float *cores);

#endif
