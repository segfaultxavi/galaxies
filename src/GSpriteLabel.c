#include <SDL.h>
#include "GResources.h"
#include "GSpriteLabel.h"
#include "GGraphics.h"

struct _GSpriteLabel {
  GSprite base;
  SDL_Texture *texture;
};

static void GSpriteLabel_render (GSpriteLabel *spr, int offsx, int offsy) {
  SDL_Rect dst;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;
  dst.x = spr->base.x + offsx;
  dst.y = spr->base.y + offsy;
  dst.w = spr->base.w;
  dst.h = spr->base.h;
  SDL_RenderCopy (renderer, spr->texture, NULL, &dst);
}

static void GSpriteLabel_free (GSpriteLabel *spr) {
  SDL_DestroyTexture (spr->texture);
}

GSprite *GSpriteLabel_new (GResources *res, int x, int y, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    TTF_Font *font, Uint32 text_color, Uint32 glow_color, const char *text) {
  SDL_Surface *surf;
  GSpriteLabel *spr = (GSpriteLabel *)GSprite_new (res, sizeof (GSpriteLabel),
      (GSpriteRender)GSpriteLabel_render, NULL, NULL, (GSpriteFree)GSpriteLabel_free);
  SDL_Color col = { (text_color >> 16) & 0xFF, (text_color >> 8) & 0xFF, (text_color >> 0) & 0xFF, (text_color >> 24) & 0xFF};
  surf = TTF_RenderText_Blended (font, text, col);
  if (!surf) {
    SDL_Log ("TTF_RenderText_Blended: %s", TTF_GetError ());
    GSprite_free ((GSprite *)spr);
    return NULL;
  }
  if (glow_color > 0)
    GGraphics_add_glow (&surf, 10, glow_color);
  spr->base.w = surf->w;
  spr->base.h = surf->h;
  GSprite_justify ((GSprite *)spr, x, y, justify_hor, justify_ver);
  spr->texture = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  GGraphics_free_surface (surf);
  if (!spr->texture) {
    SDL_Log ("SDL_CreateTextureFromSurface: %s", SDL_GetError ());
    GSprite_free ((GSprite *)spr);
    return NULL;
  }
  return (GSprite *)spr;
}
