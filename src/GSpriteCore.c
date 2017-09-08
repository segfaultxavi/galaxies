#include <SDL.h>
#include "GSpriteCore.h"
#include "GResources.h"
#include "GGraphics.h"
#include <stdlib.h>

struct _GSpriteCore {
  GSprite base;
  int id;
  int radius;
  Uint32 color;
  int highlighted;
  GSpriteCoreCallback callback;
  void *userdata;
};

#define GSPRITECORE_RADIUS 0.75f

static void GSpriteCore_render (GSpriteCore *spr, int offsx, int offsy) {
  SDL_Rect dst;
  GResources *res = spr->base.res;
  SDL_Renderer *renderer = res->sdl_renderer;
  dst.x = spr->base.x + offsx - spr->base.w / 2;
  dst.y = spr->base.y + offsy - spr->base.h / 2;
  dst.w = spr->base.w;
  dst.h = spr->base.h;
  if (spr->highlighted == 0) {
    SDL_SetTextureColorMod (res->core_texture, (0x40 + (spr->color >> 16)) & 0xFF, (0x40 + (spr->color >> 8)) & 0xFF, (0x40 + (spr->color >> 0)) & 0xFF);
    SDL_RenderCopy (renderer, res->core_texture, NULL, &dst);
  } else {
    SDL_SetTextureColorMod (res->core_texture, 0xC0, 0xC0, 0x00);
    SDL_RenderCopy (renderer, res->core_texture, NULL, &dst);
    SDL_SetTextureColorMod (res->core_highlight_texture, 0xFF, 0xFF, 0x00);
    SDL_RenderCopy (renderer, res->core_highlight_texture, NULL, &dst);
  }
}

static int GSpriteCore_event (GSpriteCore *spr, GEvent *event) {
  if (spr->callback)
    return spr->callback (spr->id, event, spr->userdata);
  return 0;
}

static int GSpriteCore_is_inside (GSprite *spr, int x, int y) {
  int r2, R2;
  x += spr->w / 2;
  y += spr->h / 2;
  if (x < spr->x || y < spr->y || x > spr->x + spr->w || y > spr->y + spr->h)
    return 0;
  r2 = SQR (x - (spr->x + spr->w / 2)) + SQR (y - (spr->y + spr->h / 2));
  R2 = (int)SQR (GSPRITECORE_RADIUS * spr->w / 2);
  return (r2 < R2);
}

SDL_Texture *GSpriteCore_create_texture (GResources *res, int w, int h) {
  SDL_Surface *surf = GGraphics_circle (w, h, 0, (int)(GSPRITECORE_RADIUS * w / 2));
  SDL_Texture *tex;
  tex = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  GGraphics_free_surface (surf);
  return tex;
}

SDL_Texture *GSpriteCore_create_highlight_texture (GResources *res, int w, int h) {
  int r = (int)(GSPRITECORE_RADIUS * w / 2);
  SDL_Surface *surf = GGraphics_circle (w, h, (int)(r * 0.9f), (int)(r * 1.1f));
  SDL_Texture *tex;
  tex = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  GGraphics_free_surface (surf);
  return tex;
}

GSprite *GSpriteCore_new (GResources *res, float x, float y, int id, int radiusX, int radiusY, GSpriteCoreCallback callback, void *userdata) {
  GSpriteCore *spr = (GSpriteCore *)GSprite_new (res, sizeof (GSpriteCore),
    (GSpriteRender)GSpriteCore_render, (GSpriteEvent)GSpriteCore_event, (GSpriteIsInside)GSpriteCore_is_inside, NULL);
  int r, g, b;
  spr->base.x = (int)(x * radiusX);
  spr->base.y = (int)(y * radiusY);
  spr->base.w = radiusX;
  spr->base.h = radiusY;
  r = rand () % 0x40 + 0x40;
  g = rand () % 0x40 + 0x40;
  b = rand () % 0x40 + 0x40;
  spr->id = id;
  spr->color = 0x80000000 | (r << 16) + (g << 8) + b;
  spr->callback = callback;
  spr->userdata = userdata;
  spr->highlighted = 0;
  return (GSprite *)spr;
}

void GSpriteCore_set_highlight (GSpriteCore *spr, int highlighted) {
  spr->highlighted = highlighted;
}

Uint32 GSpriteCore_get_color (GSpriteCore *spr) {
  return spr->color;
}

void GSpriteCore_get_opposite (GSpriteCore *spr, int x, int y, int *x2, int *y2) {
  int cx = (int)(spr->base.x * 2 / spr->base.w);
  int cy = (int)(spr->base.y * 2 / spr->base.h);
  *x2 = cx - x;
  *y2 = cy - y;
}
