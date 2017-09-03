#include <SDL.h>
#include "GSpriteCore.h"
#include "GResources.h"
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

#define SQR(x) ((x) * (x))
#define GSPRITECORE_RADIUS 0.75f

void GSpriteCore_render (GSpriteCore *spr, int offsx, int offsy) {
  SDL_Rect dst;
  GResources *res = spr->base.res;
  SDL_Renderer *renderer = res->sdl_renderer;
  dst.x = spr->base.x + offsx;
  dst.y = spr->base.y + offsy;
  dst.w = spr->base.w;
  dst.h = spr->base.h;
  if (spr->highlighted == 0) {
    SDL_SetTextureColorMod (res->core_texture, (spr->color >> 16) & 0xFF, (spr->color >> 8) & 0xFF, (spr->color >> 0) & 0xFF);
    SDL_RenderCopy (renderer, res->core_texture, NULL, &dst);
  } else {
    SDL_SetTextureColorMod (res->core_texture, 0xC0, 0xC0, 0x00);
    SDL_RenderCopy (renderer, res->core_texture, NULL, &dst);
    SDL_RenderCopy (renderer, res->core_highlight_texture, NULL, &dst);
  }
}

int GSpriteCore_event (GSpriteCore *spr, GEvent *event) {
  if (spr->callback)
    return spr->callback (spr->id, event, spr->userdata);
  return 0;
}

int GSpriteCore_is_inside (GSprite *spr, int x, int y) {
  int r2, R2;
  if (x < spr->x || y < spr->y || x > spr->x + spr->w || y > spr->y + spr->h)
    return 0;
  r2 = SQR (x - (spr->x + spr->w / 2)) + SQR (y - (spr->y + spr->h / 2));
  R2 = (int)SQR (GSPRITECORE_RADIUS * spr->w / 2);
  return (r2 < R2);
}

SDL_Texture *GSpriteCore_create_texture (GResources *res, int w, int h) {
  unsigned char *data = malloc (w * h * 4);
  Uint32 *colors = (Uint32 *)data;
  SDL_Surface *surf;
  SDL_Texture *tex;
  int x, y, R2;
  R2 = (int)SQR (GSPRITECORE_RADIUS * w / 2);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      if (SQR (x - w / 2) + SQR (y - h / 2) < R2)
        colors[y * w + x] = 0xFFFFFFFF;
      else
        colors[y * w + x] = 0x00000000;
    }
  }
  surf = SDL_CreateRGBSurfaceFrom (data, w, h, 32, w * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  tex = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  SDL_FreeSurface (surf);
  free (data);
  return tex;
}

SDL_Texture *GSpriteCore_create_highlight_texture (GResources *res, int w, int h) {
  unsigned char *data = malloc (w * h * 4);
  Uint32 *colors = (Uint32 *)data;
  SDL_Surface *surf;
  SDL_Texture *tex;
  int x, y, R2;
  R2 = (int)SQR (GSPRITECORE_RADIUS * w / 2);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      float r = (float)(SQR (x - w / 2) + SQR (y - h / 2));
      if ( r < R2 * 1.1f && r > R2 * 0.9f)
        colors[y * w + x] = 0xFFFFFF00;
      else
        colors[y * w + x] = 0x00000000;
    }
  }
  surf = SDL_CreateRGBSurfaceFrom (data, w, h, 32, w * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  tex = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  SDL_FreeSurface (surf);
  free (data);
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
