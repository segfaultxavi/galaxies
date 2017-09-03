#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteBoard.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteNull.h"
#include "GSpriteTile.h"
#include "GSpriteBoardGrid.h"
#include "GSpriteCore.h"

struct _GSpriteBoard {
  GSprite base;
  int mapSizeX, mapSizeY;
  int tileSizeX, tileSizeY;
  GSpriteTile **tiles;
  GSpriteBoardGrid *grid;
  int editing;
};

#define GBOARD_TILE(spr,x,y) spr->tiles[(y) * spr->mapSizeX + (x)]

int GSpriteBoard_tile_event (int x, int y, GEvent *event, void *userdata) {
  int res = 0;
  GSpriteBoard *spr = userdata;
  switch (event->type) {
    case GEVENT_TYPE_SPRITE_IN:
      GSpriteTile_setID (GBOARD_TILE (spr, x, y), 0, 0xFFFFFFFF);
      break;
    case GEVENT_TYPE_SPRITE_OUT:
      GSpriteTile_setID (GBOARD_TILE (spr, x, y), 0, 0xFF202020);
      break;
    default:
      break;
  }
  return res;
}

void GSpriteBoard_free (GSpriteBoard *spr) {
  free (spr->tiles);
}

GSprite *GSpriteBoard_new (GResources *res, int editing) {
  GSpriteBoard *spr = (GSpriteBoard *)GSprite_new (res, sizeof (GSpriteBoard), NULL, NULL, NULL, (GSpriteFree)GSpriteBoard_free);
  spr->editing = editing;
  spr->base.w = spr->base.h = res->game_height;
  return (GSprite *)spr;
}

void GSpriteBoard_start (GSpriteBoard *spr, int mapSizeX, int mapSizeY, float *cores) {
  int x, y;
  spr->mapSizeX = mapSizeX;
  spr->mapSizeY = mapSizeY;
  spr->tileSizeX = (spr->base.w - 1) / spr->mapSizeX;
  spr->tileSizeY = (spr->base.h - 1) / spr->mapSizeY;
  spr->base.w = spr->tileSizeX * spr->mapSizeX;
  spr->base.h = spr->tileSizeY * spr->mapSizeY;
  spr->base.x = (spr->base.res->game_height - spr->base.w) / 2;
  spr->base.y = (spr->base.res->game_height - spr->base.h) / 2;
  spr->tiles = malloc (mapSizeX * mapSizeY * sizeof (GSpriteTile *));
  for (y = 0; y < mapSizeY; y++) {
    for (x = 0; x < mapSizeX; x++) {
      GSpriteTile *tile = (GSpriteTile *)GSpriteTile_new (spr->base.res, x, y, spr->tileSizeX, spr->tileSizeY, GSpriteBoard_tile_event, spr);
      GBOARD_TILE (spr, x, y) = tile;
      GSprite_add_child ((GSprite *)spr, (GSprite *)tile);
      GSpriteTile_setID (tile, -1, 0xFF202020);
    }
  }
  spr->grid = (GSpriteBoardGrid *)GSpriteBoardGrid_new (spr->base.res, spr->mapSizeX, spr->mapSizeY, spr->tileSizeX, spr->tileSizeY, spr);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->grid);

  GSprite_add_child ((GSprite *)spr, GSpriteCore_new (spr->base.res, 5, 5, 0, spr->tileSizeX, spr->tileSizeY, NULL, NULL));
}

GSpriteTile *GSpriteBoard_get_tile (GSpriteBoard *spr, int x, int y) {
  if (x < 0 || x >= spr->mapSizeX || y < 0 || y >= spr->mapSizeY)
    return NULL;
  return GBOARD_TILE (spr, x, y);
}
