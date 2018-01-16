#include <SDL.h>
#include "GSpriteCore.h"
#include "GResources.h"
#include "GGraphics.h"
#include "GSpriteBoard.h"
#include <stdlib.h>

struct _GSpriteCore {
  GSprite base;
  int id;
  Uint32 color;
  int solid;
  GSpriteCoreCallback callback;
  GSpriteBoard *board;
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
  if (spr->solid) {
    if (GSpriteBoard_get_selected_core (spr->board) != spr) {
      SDL_SetTextureColorMod (res->core_texture, (0x40 + (spr->color >> 16)) & 0xFF, (0x40 + (spr->color >> 8)) & 0xFF, (0x40 + (spr->color >> 0)) & 0xFF);
      SDL_RenderCopy (renderer, res->core_texture, NULL, &dst);
    } else {
      SDL_SetTextureColorMod (res->core_texture, 0xC0, 0xC0, 0x00);
      SDL_RenderCopy (renderer, res->core_texture, NULL, &dst);
    }
  } else {
    SDL_SetTextureColorMod (res->core_highlight_texture, 0xFF, 0xFF, 0x00);
    SDL_RenderCopy (renderer, res->core_highlight_texture, NULL, &dst);
  }
}

static int GSpriteCore_event (GSpriteCore *spr, GEvent *event, int *destroyed) {
  if (spr->callback)
    return spr->callback (spr->id, event, spr->board, destroyed);
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

GSprite *GSpriteCore_new (GResources *res, float x, float y, int id, int radiusX, int radiusY, int solid, GSpriteCoreCallback callback, void *board) {
  GSpriteCore *spr = (GSpriteCore *)GSprite_new (res, sizeof (GSpriteCore),
    (GSpriteRender)GSpriteCore_render, (GSpriteEvent)GSpriteCore_event, (GSpriteIsInside)GSpriteCore_is_inside, NULL);
  int r, g, b;
  spr->base.x = (int)(x * radiusX);
  spr->base.y = (int)(y * radiusY);
  spr->base.w = radiusX;
  spr->base.h = radiusY;
  spr->solid = solid;
  r = rand () % 0x40;
  g = rand () % 0x40;
  b = 0x40 - (r + g) / 2;
  r += 0x40;
  g += 0x40;
  b += 0x40;
  spr->id = id;
  spr->color = 0x80000000 | (r << 16) + (g << 8) + b;
  spr->callback = callback;
  spr->board = board;
  return (GSprite *)spr;
}

int GSpriteCore_get_id (GSpriteCore *spr) {
  return spr->id;
}

void GSpriteCore_set_id (GSpriteCore *spr, int id) {
  spr->id = id;
}

Uint32 GSpriteCore_get_color (GSpriteCore *spr) {
  return spr->color;
}

void GSpriteCore_get_corner (GSpriteCore *spr, int *x, int *y) {
  *x = (int)(spr->base.x / spr->base.w);
  *y = (int)(spr->base.y / spr->base.h);
}

void GSpriteCore_get_opposite (GSpriteCore *spr, int x, int y, int *x2, int *y2) {
  int cx = (int)(spr->base.x * 2 / spr->base.w);
  int cy = (int)(spr->base.y * 2 / spr->base.h);
  *x2 = cx - x - 1;
  *y2 = cy - y - 1;
}

GSpriteCore *GSpriteCore_clone (GSpriteCore *spr) {
  GSpriteCore *clone = (GSpriteCore *)GSprite_new (spr->base.res, sizeof (GSpriteCore),
    NULL, NULL, NULL, NULL);
  clone->base.x = spr->base.x;
  clone->base.y = spr->base.y;
  clone->base.w = spr->base.w;
  clone->base.h = spr->base.h;
  clone->id = spr->id;
  clone->callback = NULL;
  clone->board = NULL;
  return clone;
}