#ifndef __GSPRITE_TILE_H__
#define __GSPRITE_TILE_H__

#include "GSprite.h"
#include "GEvent.h"

typedef struct _GSpriteTile GSpriteTile;

typedef int (*GSpriteTileCallback)(int x, int y, GEvent *event, void *userdata);

GSprite *GSpriteTile_new (int x, int y, int tileSizeX, int tileSizeY, GSpriteTileCallback callback, void *userdata);
void GSpriteTile_setID (GSpriteTile *spr, int id, Uint32 color);
int GSpriteTile_getID (GSpriteTile *spr);

#endif

