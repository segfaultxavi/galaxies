#ifndef __GSPRITE_BOARD_H__
#define __GSPRITE_BOARD_H__

#include "GSprite.h"
#include "GResources.h"
#include "GSpriteTile.h"
#include "GSpriteCore.h"

typedef struct _GSpriteBoard GSpriteBoard;

GSprite *GSpriteBoard_new (GResources *res, int editing);
void GSpriteBoard_start (GSpriteBoard *spr, int mapSizeX, int mapSizeY, int numInitialCores, const float *initialCores, const int *initialTiles);
int GSpriteBoard_load (GSpriteBoard *spr, const char *desc);
char *GSpriteBoard_save (GSpriteBoard *spr, int includeTileColors);
GSpriteTile *GSpriteBoard_get_tile (GSpriteBoard *spr, int x, int y);
GSpriteCore *GSpriteBoard_get_selected_core (GSpriteBoard *spr);
int GSpriteBoard_is_tile_selectable (GSpriteBoard *spr, int x, int y);
void GSpriteBoard_reset (GSpriteBoard *spr);
int GSpriteBoard_get_map_size_x (GSpriteBoard *spr);
int GSpriteBoard_get_map_size_y (GSpriteBoard *spr);
int GSpriteBoard_is_empty (GSpriteBoard *spr);

#endif
