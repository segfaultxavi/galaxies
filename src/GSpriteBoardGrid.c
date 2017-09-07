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

static void GSpriteBoardGrid_render (GSpriteBoardGrid *spr, int offsx, int offsy) {
  int x, y;
  Uint32 color = 0xFFFFFF00;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;

  // Grid separating galaxies
  SDL_SetRenderDrawColor (renderer, gcolor_grid_inactive.r, gcolor_grid_inactive.g, gcolor_grid_inactive.b, gcolor_grid_inactive.a);
  for (y = 0; y <= spr->mapSizeY; y++) {
    for (x = 0; x < spr->mapSizeX; x++) {
      GSpriteTile *tile_up = GSpriteBoard_get_tile (spr->board, x, y - 1);
      GSpriteTile *tile_down = GSpriteBoard_get_tile (spr->board, x, y);
      if (y == 0 || y == spr->mapSizeY || GSpriteTile_get_id (tile_up) != GSpriteTile_get_id (tile_down)) {
        SDL_Rect r;
        r.x = offsx + x * spr->tileSizeX;
        r.y = offsy + y * spr->tileSizeY - 1;
        r.w = spr->tileSizeX;
        r.h = 3;
        SDL_RenderFillRect (renderer, &r);
      }
    }
  }
  for (x = 0; x <= spr->mapSizeX; x++) {
    for (y = 0; y < spr->mapSizeY; y++) {
      GSpriteTile *tile_left = GSpriteBoard_get_tile (spr->board, x - 1, y);
      GSpriteTile *tile_right = GSpriteBoard_get_tile (spr->board, x, y);
      if (x == 0 || x == spr->mapSizeX || GSpriteTile_get_id (tile_left) != GSpriteTile_get_id (tile_right)) {
        SDL_Rect r;
        r.x = offsx + x * spr->tileSizeX - 1;
        r.y = offsy + y * spr->tileSizeY;
        r.w = 3;
        r.h = spr->tileSizeY;
        SDL_RenderFillRect (renderer, &r);
      }
    }
  }
}

GSprite *GSpriteBoardGrid_new (GResources *res, int mapSizeX, int mapSizeY, int tileSizeX, int tileSizeY, GSpriteBoard *board) {
  GSpriteBoardGrid *spr = (GSpriteBoardGrid *)GSprite_new (res, sizeof (GSpriteBoardGrid),
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
