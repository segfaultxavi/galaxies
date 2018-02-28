#include <SDL.h>
#include <SDL_image.h>
#include "GSpriteImage.h"
#include "GResources.h"
#include "GGame.h"

struct _GSpriteImage {
  GSprite base;
  SDL_Texture *tex;
  int w, h;
};

static void GSpriteImage_render (GSpriteImage *spr, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Color col;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;

  rect.x = spr->base.x + offsx;
  rect.y = spr->base.y + offsy;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  SDL_RenderCopy (renderer, spr->tex, NULL, &rect);
}

void GSpriteImage_free (GSpriteImage *spr) {
  SDL_DestroyTexture (spr->tex);
}

GSprite *GSpriteImage_new (GResources *res, int x, int y, int w, int h, const char *filename) {
  GSpriteImage *spr;
  SDL_Surface *surf;
  SDL_RWops *rwops;

  spr = (GSpriteImage *)GSprite_new (res, sizeof (GSpriteImage),
      (GSpriteRender)GSpriteImage_render, NULL, NULL, (GSpriteFree)GSpriteImage_free);

  rwops = GGame_openAsset (res, filename);
  surf = IMG_Load_RW (rwops, 1);
  spr->tex = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  spr->w = surf->w;
  spr->h = surf->h;
  SDL_FreeSurface (surf);

  spr->base.x = x;
  spr->base.y = y;
  spr->base.w = w;
  spr->base.h = h;
  return (GSprite *)spr;
}
