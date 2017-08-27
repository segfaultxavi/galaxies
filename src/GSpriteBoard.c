#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteBoard.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteNull.h"
#include "GSpriteTile.h"
#include "GSpriteBoardGrid.h"

struct _GSpriteBoard {
  GSprite base;
  GResources *res;
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

GSprite *GSpriteBoard_new (int editing, GResources *res) {
  GSpriteBoard *spr = (GSpriteBoard *)GSprite_new (sizeof (GSpriteBoard), NULL, NULL, NULL, NULL);
  spr->editing = editing;
  spr->base.w = spr->base.h = res->game_height;
  spr->res = res;
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
  spr->base.x = (spr->res->game_height - spr->base.w) / 2;
  spr->base.y = (spr->res->game_height - spr->base.h) / 2;
  spr->tiles = malloc (mapSizeX * mapSizeY * sizeof (GSpriteTile *));
  for (y = 0; y < mapSizeY; y++) {
    for (x = 0; x < mapSizeX; x++) {
      GSpriteTile *tile = (GSpriteTile *)GSpriteTile_new (x, y, spr->tileSizeX, spr->tileSizeY, GSpriteBoard_tile_event, spr);
      GBOARD_TILE (spr, x, y) = tile;
      GSprite_add_child ((GSprite *)spr, (GSprite *)tile);
      GSpriteTile_setID (tile, -1, 0xFF202020);
    }
  }
  spr->grid = (GSpriteBoardGrid *)GSpriteBoardGrid_new (spr->mapSizeX, spr->mapSizeY, spr->tileSizeX, spr->tileSizeY, spr);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->grid);
}

GSpriteTile *GSpriteBoard_get_tile (GSpriteBoard *spr, int x, int y) {
  if (x < 0 || x >= spr->mapSizeX || y < 0 || y >= spr->mapSizeY)
    return NULL;
  return GBOARD_TILE (spr, x, y);
}