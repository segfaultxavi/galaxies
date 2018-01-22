#ifndef __GSPRITE_BOARD_GRID_H__
#define __GSPRITE_BOARD_GRID_H__

#include "GSprite.h"
#include "GSpriteBoard.h"
#include "GSpriteTile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpriteBoardGrid GSpriteBoardGrid;

GSprite *GSpriteBoardGrid_new (GResources *res, int mapSizeX, int mapSizeY, int tileSizeX, int tileSizeY, GSpriteBoard *board);
void GSpriteBoardGrid_set_color (GSpriteBoardGrid *spr, Uint32 color);

#ifdef __cplusplus
}
#endif

#endif

