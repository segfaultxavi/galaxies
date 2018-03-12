#include <SDL.h>
#include "GSpriteTile.h"
#include "GSpriteBoard.h"
#include "GSpriteCore.h"

struct _GSpriteTile {
  GSprite base;
  int id;
  int target_id;
  Uint32 color;
  Uint32 target_color;
  int flags;
  GSpriteTileCallback callback;
  GSpriteBoard *board;
};

static void GSpriteTile_render (GSpriteTile *spr, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;
  GSpriteCore *curr_core = GSpriteBoard_get_selected_core (spr->board);
  int curr_core_id = curr_core ? GSpriteCore_get_id (curr_core) : -2; // Unassigned tiles will have id == -1
  Uint32 color = curr_core_id == spr->id ? 0x80FFFF00 : spr->color;

  SDL_SetRenderDrawColor (renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF, (color >> 24) & 0xFF);
  rect.x = offsx + spr->base.x;
  rect.y = offsy + spr->base.y;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  SDL_RenderFillRect (renderer, &rect);
  if (spr->target_color > 0 && spr->target_color != spr->color) {
    rect.x = offsx + spr->base.x + 50;
    rect.y = offsy + spr->base.y + 50;
    rect.w = spr->base.w - 100;
    rect.h = spr->base.h - 100;
    color = curr_core_id == spr->target_id ? 0x80FFFF00 : spr->target_color;
    SDL_SetRenderDrawColor (renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF, (color >> 24) & 0xFF);
    SDL_RenderFillRect (renderer, &rect);
  }
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
  spr->id = spr->target_id = -1;
  spr->color = spr->target_color = 0; // Unused colors
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

void GSpriteTile_set_target_id (GSpriteTile *spr, int target_id, Uint32 target_color) {
  spr->target_id = target_id;
  spr->target_color = target_color;
}