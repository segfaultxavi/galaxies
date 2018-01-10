#include <SDL.h>
#include "GSpriteTile.h"
#include "GSpriteBoard.h"
#include "GSpriteCore.h"

struct _GSpriteTile {
  GSprite base;
  int id;
  Uint32 color;
  int flags;
  GSpriteTileCallback callback;
  GSpriteBoard *board;
};

static void GSpriteTile_render (GSpriteTile *spr, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;
  Uint32 color = spr->color;
  GSpriteCore *curr_core = GSpriteBoard_get_selected_core (spr->board);
  if (curr_core && GSpriteCore_get_id (curr_core) == spr->id)
    color = 0x80FFFF00;
  SDL_SetRenderDrawColor (renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF, (color >> 24) & 0xFF);
  rect.x = offsx + spr->base.x;
  rect.y = offsy + spr->base.y;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  SDL_RenderFillRect (renderer, &rect);
}

static int GSpriteTile_event (GSpriteTile *spr, GEvent *event, int *destroyed) {
  int x = spr->base.x / spr->base.w;
  int y = spr->base.y / spr->base.h;

  if (spr->callback)
    return spr->callback (x, y, event, spr->board);
  return 0;
}

GSprite *GSpriteTile_new (GResources *res, int x, int y, int tileSizeX, int tileSizeY, GSpriteTileCallback callback, void *board) {
  GSpriteTile *spr = (GSpriteTile *)GSprite_new (res, sizeof (GSpriteTile),
    (GSpriteRender)GSpriteTile_render, (GSpriteEvent)GSpriteTile_event, NULL, NULL);
  spr->base.x = x * tileSizeX;
  spr->base.y = y * tileSizeY;
  spr->base.w = tileSizeX;
  spr->base.h = tileSizeY;
  spr->color = 0xFFFFFFFF; // Unused color
  spr->flags = 0;
  spr->callback = callback;
  spr->board = board;
  return (GSprite *)spr;
}

void GSpriteTile_set_id (GSpriteTile *spr, int id, Uint32 color) {
  spr->id = id;
  spr->color = color;
}

int GSpriteTile_get_id (const GSpriteTile *spr) {
  return spr->id;
}

void GSpriteTile_set_flags (GSpriteTile *spr, int flags) {
  spr->flags = flags;
}

int GSpriteTile_get_flags (const GSpriteTile *spr) {
  return spr->flags;
}
