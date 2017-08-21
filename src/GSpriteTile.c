#include <SDL.h>
#include "GSpriteTile.h"

struct _GSpriteTile {
  GSprite base;
  int id;
  Uint32 color;
  int highlighted;
  GSpriteTileCallback callback;
  void *userdata;
};

void GSpriteTile_render (GSpriteTile *spr, SDL_Renderer *renderer, int offsx, int offsy) {
  SDL_Rect rect;
  Uint32 color = spr->color;
  if (spr->highlighted)
    color = 0xFFFFFF00;
  SDL_SetRenderDrawColor (renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF, (color >> 24) & 0xFF);
  rect.x = offsx + spr->base.x;
  rect.y = offsy + spr->base.y;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  SDL_RenderFillRect (renderer, &rect);
}

int GSpriteTile_event (GSpriteTile *spr, GEvent *event) {
  int x = spr->base.x / spr->base.w;
  int y = spr->base.y / spr->base.h;
  if (spr->callback)
    return spr->callback (x, y, event, spr->userdata);
  return 0;
}

GSprite *GSpriteTile_new (int x, int y, int tileSizeX, int tileSizeY, GSpriteTileCallback callback, void *userdata) {
  GSpriteTile *spr = (GSpriteTile *)GSprite_new (sizeof (GSpriteTile),
    (GSpriteRender)GSpriteTile_render, (GSpriteEvent)GSpriteTile_event, NULL, NULL);
  spr->base.x = x * tileSizeX;
  spr->base.y = y * tileSizeY;
  spr->base.w = tileSizeX;
  spr->base.h = tileSizeY;
  spr->color = 0xFF202020;
  spr->highlighted = 0;
  spr->callback = callback;
  spr->userdata = userdata;
  return (GSprite *)spr;
}

void GSpriteTile_setID (GSpriteTile *spr, int id, Uint32 color) {
  spr->id = id;
  spr->color = color;
}

int GSpriteTile_getID (GSpriteTile *spr) {
  return spr->id;
}
