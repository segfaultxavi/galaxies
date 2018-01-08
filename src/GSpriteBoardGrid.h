#ifndef __GSPRITE_BOARD_GRID_H__
#define __GSPRITE_BOARD_GRID_H__

#include "GSprite.h"
#include "GSpriteBoard.h"
#include "GSpriteTile.h"

typedef struct _GSpriteBoardGrid GSpriteBoardGrid;

GSprite *GSpriteBoardGrid_new (GResources *res, int mapSizeX, int mapSizeY, int tileSizeX, int tileSizeY, GSpriteBoard *board);

#endif

