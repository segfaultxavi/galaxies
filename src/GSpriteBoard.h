#ifndef __GSPRITE_BOARD_H__
#define __GSPRITE_BOARD_H__

typedef struct _GSpriteBoard GSpriteBoard;

#include "GSprite.h"
#include "GResources.h"
#include "GSpriteTile.h"
#include "GSpriteCore.h"

#ifdef __cplusplus
extern "C" {
#endif

GSprite *GSpriteBoard_new (GResources *res, int editing);
void GSpriteBoard_start (GSpriteBoard *spr, int mapSizeX, int mapSizeY, int numInitialCores, const float *initialCores, const int *initialTiles);
int GSpriteBoard_load (GSpriteBoard *spr, const char *desc);
char *GSpriteBoard_save (GSpriteBoard *spr, int includeTileColors);
int GSpriteBoard_check_description (const char *desc);
GSpriteTile *GSpriteBoard_get_tile (GSpriteBoard *spr, int x, int y);
GSpriteCore *GSpriteBoard_get_core (GSpriteBoard *spr, int id);
int GSpriteBoard_get_num_cores (GSpriteBoard *spr);
GSpriteCore *GSpriteBoard_get_selected_core (GSpriteBoard *spr);
int GSpriteBoard_is_tile_selectable (GSpriteBoard *spr, int x, int y);
void GSpriteBoard_reset (GSpriteBoard *spr);
int GSpriteBoard_get_map_size_x (GSpriteBoard *spr);
int GSpriteBoard_get_map_size_y (GSpriteBoard *spr);
int GSpriteBoard_is_empty (GSpriteBoard *spr);
int GSpriteBoard_has_no_manual_tiles (GSpriteBoard *spr);
void GSpriteBoard_set_tiles (GSpriteBoard *spr, char *tiles);
void GSpriteBoard_set_finished (GSpriteBoard *spr, int finished);
void GSpriteBoard_size_from_desc (const char *desc, int *size_x, int *size_y);
int GSpriteBoard_check_completion (GSpriteBoard *spr);
void GSpriteBoard_set_core_cursor (GSpriteBoard *spr, GSpriteCoreType type);

#ifdef __cplusplus
}
#endif

#endif
