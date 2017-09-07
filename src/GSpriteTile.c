#include <SDL.h>
#include "GSpriteTile.h"

struct _GSpriteTile {
  GSprite base;
  int id;
  Uint32 color;
  int flags;
  int highlighted;
  GSpriteTileCallback callback;
  void *userdata;
};

void GSpriteTile_render (GSpriteTile *spr, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;
  Uint32 color = spr->color;
  if (spr->highlighted != 0)
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

  switch (event->type) {
    case GEVENT_TYPE_SPRITE_IN:
      spr->highlighted = 1;
      break;
    case GEVENT_TYPE_SPRITE_OUT:
      spr->highlighted = 0;
      break;
    default:
      break;
  }

  if (spr->callback)
    return spr->callback (x, y, event, spr->userdata);
  return 0;
}

GSprite *GSpriteTile_new (GResources *res, int x, int y, int tileSizeX, int tileSizeY, GSpriteTileCallback callback, void *userdata) {
  GSpriteTile *spr = (GSpriteTile *)GSprite_new (res, sizeof (GSpriteTile),
    (GSpriteRender)GSpriteTile_render, (GSpriteEvent)GSpriteTile_event, NULL, NULL);
  spr->base.x = x * tileSizeX;
  spr->base.y = y * tileSizeY;
  spr->base.w = tileSizeX;
  spr->base.h = tileSizeY;
  spr->color = 0xFFFFFFFF; // Unused color
  spr->flags = 0;
  spr->highlighted = 0;
  spr->callback = callback;
  spr->userdata = userdata;
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
