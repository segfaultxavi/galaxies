#include <SDL.h>
#include "GSpriteBoardGrid.h"
#include "GSpriteBoard.h"
#include "GSpriteTile.h"

struct _GSpriteBoardGrid {
  GSprite base;
  int mapSizeX, mapSizeY;
  int tileSizeX, tileSizeY;
  GSpriteBoard *board;
  Uint32 color;
};

static void GSpriteBoardGrid_render (GSpriteBoardGrid *spr, int offsx, int offsy) {
  int x, y;
  int a, r, g, b;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;

  // Grid separating galaxies
  a = (spr->color >> 24) & 0xFF;
  r = (spr->color >> 16) & 0xFF;
  g = (spr->color >>  8) & 0xFF;
  b = (spr->color >>  0) & 0xFF;
  SDL_SetRenderDrawColor (renderer, r, g, b, a);
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

  // Selected Galaxy
  if (GSpriteBoard_get_selected_core (spr->board) != NULL) {
    SDL_SetRenderDrawColor (renderer, 0xFF, 0xFF, 0x00, 0xFF);
    for (y = 0; y <= spr->mapSizeY; y++) {
      for (x = 0; x < spr->mapSizeX; x++) {
        int up = GSpriteBoard_is_tile_selectable (spr->board, x, y - 1);
        int down = GSpriteBoard_is_tile_selectable (spr->board, x, y);
        if (up ^ down) {
          SDL_Rect r;
          r.x = offsx + x * spr->tileSizeX;
          r.y = offsy + y * spr->tileSizeY - 2;
          r.w = spr->tileSizeX;
          r.h = 5;
          SDL_RenderFillRect (renderer, &r);
        }
      }
    }
    for (x = 0; x <= spr->mapSizeX; x++) {
      for (y = 0; y < spr->mapSizeY; y++) {
        int left = GSpriteBoard_is_tile_selectable (spr->board, x - 1, y);
        int right = GSpriteBoard_is_tile_selectable (spr->board, x, y);
        if (left ^ right) {
          SDL_Rect r;
          r.x = offsx + x * spr->tileSizeX - 2;
          r.y = offsy + y * spr->tileSizeY;
          r.w = 5;
          r.h = spr->tileSizeY;
          SDL_RenderFillRect (renderer, &r);
        }
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
  spr->color = 0xFF808080;
  return (GSprite *)spr;
}

void GSpriteBoardGrid_set_color (GSpriteBoardGrid *spr, Uint32 color) {
  spr->color = color;
}