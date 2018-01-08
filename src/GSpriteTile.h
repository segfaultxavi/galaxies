#ifndef __GSPRITE_TILE_H__
#define __GSPRITE_TILE_H__

#include "GSprite.h"
#include "GEvent.h"

typedef struct _GSpriteTile GSpriteTile;

typedef int (*GSpriteTileCallback)(int x, int y, GEvent *event, void *userdata);

#define GTILE_FLAG_VISITED 1
#define GTILE_FLAG_FIXED 2

GSprite *GSpriteTile_new (GResources *res, int x, int y, int tileSizeX, int tileSizeY, GSpriteTileCallback callback, void *userdata);
void GSpriteTile_set_id (GSpriteTile *spr, int id, Uint32 color);
int GSpriteTile_get_id (const GSpriteTile *spr);
void GSpriteTile_set_flags (GSpriteTile *spr, int flags);
int GSpriteTile_get_flags (const GSpriteTile *spr);


#endif

