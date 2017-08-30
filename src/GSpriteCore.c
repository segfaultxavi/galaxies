#include <SDL.h>
#include "GSpriteCore.h"
#include <stdlib.h>

struct _GSpriteCore {
  GSprite base;
  int id;
  int radius;
  Uint32 color;
  GSpriteCoreCallback callback;
  SDL_Texture *texture;
  void *userdata;
};

#define SQR(x) ((x) * (x))
#define GSPRITECORE_RADIUS 0.75f

void GSpriteCore_render (GSpriteCore *spr, SDL_Renderer *renderer, int offsx, int offsy) {
  SDL_Rect dst;
  dst.x = spr->base.x + offsx;
  dst.y = spr->base.y + offsy;
  dst.w = spr->base.w;
  dst.h = spr->base.h;
  SDL_RenderCopy (renderer, spr->texture, NULL, &dst);
}

int GSpriteCore_event (GSpriteCore *spr, GEvent *event) {
  if (spr->callback)
    return spr->callback (spr->id, event, spr->userdata);
  return 0;
}

void GSpriteCore_free (GSpriteCore *spr) {
  SDL_DestroyTexture (spr->texture);
}

int GSpriteCore_is_inside (GSprite *spr, int x, int y) {
  int r2, R2;
  if (x < spr->x || y < spr->y || x > spr->x + spr->w || y > spr->y + spr->h)
    return 0;
  r2 = SQR (x - (spr->x + spr->w / 2)) + SQR (y - (spr->y + spr->h / 2));
  R2 = (int)SQR (GSPRITECORE_RADIUS * spr->w / 2);
  return (r2 < R2);
}

SDL_Texture *GSpriteCore_create_texture (int w, int h, Uint32 color, GResources *res) {
  unsigned char *data = malloc (w * h * 4);
  Uint32 *colors = (Uint32 *)data;
  SDL_Surface *surf;
  SDL_Texture *tex;
  int x, y, R2;
  R2 = (int)SQR (GSPRITECORE_RADIUS * w / 2);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      if (SQR (x - w / 2) + SQR (y - h / 2) < R2)
        colors[y * w + x] = color;
      else
        colors[y * w + x] = 0x00000000;
    }
  }
  surf = SDL_CreateRGBSurfaceFrom (data, w, h, 32, w * 4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  tex = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  SDL_FreeSurface (surf);
  free (data);
  return tex;
}

GSprite *GSpriteCore_new (float x, float y, int id, int radiusX, int radiusY, GSpriteCoreCallback callback, void *userdata, GResources *res) {
  GSpriteCore *spr = (GSpriteCore *)GSprite_new (sizeof (GSpriteCore),
    (GSpriteRender)GSpriteCore_render, (GSpriteEvent)GSpriteCore_event, (GSpriteIsInside)GSpriteCore_is_inside, (GSpriteFree)GSpriteCore_free);
  int r, g, b;
  spr->base.x = (int)(x * radiusX);
  spr->base.y = (int)(y * radiusY);
  spr->base.w = radiusX;
  spr->base.h = radiusY;
  r = rand () % 0x40 + 0x40;
  g = rand () % 0x40 + 0x40;
  b = rand () % 0x40 + 0x40;
  spr->color = 0xFF000000 | (r << 16) + (g << 8) + b;
  spr->callback = callback;
  spr->userdata = userdata;
  spr->texture = GSpriteCore_create_texture (radiusX, radiusY, spr->color, res);
  return (GSprite *)spr;
}
