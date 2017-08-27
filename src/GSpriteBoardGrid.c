#include <SDL.h>
#include "GSpriteBoardGrid.h"
#include "GSpriteBoard.h"
#include "GSpriteTile.h"

struct _GSpriteBoardGrid {
  GSprite base;
  int mapSizeX, mapSizeY;
  int tileSizeX, tileSizeY;
  GSpriteBoard *board;
};

static SDL_Color gcolor_grid_inactive = { 0x80, 0x80, 0x80, 0xFF };

void GSpriteBoardGrid_render (GSpriteBoardGrid *spr, SDL_Renderer *renderer, int offsx, int offsy) {
  int x, y;
  Uint32 color = 0xFFFFFF00;

  // Grid separating galaxies
  SDL_SetRenderDrawColor (renderer, gcolor_grid_inactive.r, gcolor_grid_inactive.g, gcolor_grid_inactive.b, gcolor_grid_inactive.a);
  for (y = 0; y <= spr->mapSizeY; y++) {
    for (x = 0; x < spr->mapSizeX; x++) {
      GSpriteTile *tile_up = GSpriteBoard_get_tile (spr->board, x, y - 1);
      GSpriteTile *tile_down = GSpriteBoard_get_tile (spr->board, x, y);
      if (y == 0 || y == spr->mapSizeY || GSpriteTile_getID (tile_up) != GSpriteTile_getID (tile_down)) {
        SDL_RenderDrawLine (renderer, offsx + x * spr->tileSizeX, offsy + y * spr->tileSizeY, offsx + (x + 1) * spr->tileSizeX, offsy + y * spr->tileSizeY);
      }
    }
  }
  for (x = 0; x <= spr->mapSizeX; x++) {
    for (y = 0; y < spr->mapSizeY; y++) {
      GSpriteTile *tile_left = GSpriteBoard_get_tile (spr->board, x - 1, y);
      GSpriteTile *tile_right = GSpriteBoard_get_tile (spr->board, x, y);
      if (x == 0 || x == spr->mapSizeX || GSpriteTile_getID (tile_left) != GSpriteTile_getID (tile_right)) {
        SDL_RenderDrawLine (renderer, offsx + x * spr->tileSizeX, offsy + y * spr->tileSizeY, offsx + x * spr->tileSizeX, offsy + (y + 1) * spr->tileSizeY);
      }
    }
  }
}

GSprite *GSpriteBoardGrid_new (int mapSizeX, int mapSizeY, int tileSizeX, int tileSizeY, GSpriteBoard *board) {
  GSpriteBoardGrid *spr = (GSpriteBoardGrid *)GSprite_new (sizeof (GSpriteBoardGrid),
    (GSpriteRender)GSpriteBoardGrid_render, NULL, NULL, NULL);
  spr->base.w = mapSizeX * tileSizeX;
  spr->base.h = mapSizeY * tileSizeY;
  spr->mapSizeX = mapSizeX;
  spr->mapSizeY = mapSizeY;
  spr->tileSizeX = tileSizeX;
  spr->tileSizeY = tileSizeY;
  spr->board = board;
  return (GSprite *)spr;
}
