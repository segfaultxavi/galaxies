#include <SDL.h>
#include <SDL_image.h>
#include "GSpriteImage.h"
#include "GResources.h"
#include "GGame.h"

struct _GSpriteImage {
  GSprite base;
  SDL_Texture *tex;
  int w, h;
  GSpriteImageFitMode fit_mode;
};

static void GSpriteImage_render (GSpriteImage *spr, int offsx, int offsy) {
  SDL_Rect srect, drect;
  SDL_Color col;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;
  int aw, ah;

  switch (spr->fit_mode) {
    case GSPRITE_IMAGE_FIT_MODE_STRETCH:
      drect.x = spr->base.x + offsx;
      drect.y = spr->base.y + offsy;
      drect.w = spr->base.w;
      drect.h = spr->base.h;
      SDL_RenderCopy (renderer, spr->tex, NULL, &drect);
      break;
    case GSPRITE_IMAGE_FIT_MODE_INSIDE:
      if (spr->base.w * spr->h > spr->w * spr->base.h) {
        ah = spr->base.h;
        aw = spr->base.h * spr->w / spr->h;
      } else {
        aw = spr->base.w;
        ah = spr->base.w * spr->h / spr->w;
      }
      drect.x = spr->base.x + offsx + (spr->base.w - aw) / 2;
      drect.y = spr->base.y + offsy + (spr->base.h - ah) / 2;
      drect.w = aw;
      drect.h = ah;
      SDL_RenderCopy (renderer, spr->tex, NULL, &drect);
      break;
    case GSPRITE_IMAGE_FIT_MODE_OUTSIDE:
      if (spr->base.w * spr->h > spr->w * spr->base.h) {
        aw = spr->w;
        ah = spr->h * spr->base.h / spr->base.w;
      } else {
        ah = spr->h;
        aw = spr->w * spr->base.h / spr->base.w;
      }
      srect.x = (spr->w - aw) / 2;
      srect.y = (spr->h - ah) / 2;
      srect.w = aw;
      srect.h = ah;
      drect.x = spr->base.x + offsx;
      drect.y = spr->base.y + offsy;
      drect.w = spr->base.w;
      drect.h = spr->base.h;
      SDL_RenderCopy (renderer, spr->tex, &srect, &drect);
      break;
  }
}

void GSpriteImage_free (GSpriteImage *spr) {
  SDL_DestroyTexture (spr->tex);
}

GSprite *GSpriteImage_new (GResources *res, int x, int y, int w, int h, const char *filename, GSpriteImageFitMode fit_mode) {
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
  spr->fit_mode = fit_mode;
  SDL_FreeSurface (surf);

  spr->base.x = x;
  spr->base.y = y;
  spr->base.w = w;
  spr->base.h = h;
  return (GSprite *)spr;
}
