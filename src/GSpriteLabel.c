#include <SDL.h>
#include "GResources.h"
#include "GSpriteLabel.h"

typedef struct _GSpriteLabel {
  GSprite base;
  SDL_Texture *texture;
} GSpriteLabel;

void GSpriteLabel_render (GSpriteLabel *spr, SDL_Renderer *renderer, int offsx, int offsy) {
  SDL_Rect dst;
  dst.x = spr->base.x + offsx;
  dst.y = spr->base.y + offsy;
  dst.w = spr->base.w;
  dst.h = spr->base.h;
  SDL_RenderCopy (renderer, spr->texture, NULL, &dst);
}

void GSpriteLabel_free (GSpriteLabel *spr) {
  SDL_DestroyTexture (spr->texture);
}

GSprite *GSpriteLabel_new (int x, int y, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    GResources *res, TTF_Font *font, Uint32 color, const char *text) {
  SDL_Surface *surf;
  GSpriteLabel *spr = (GSpriteLabel *)GSprite_new (sizeof (GSpriteLabel),
      (GSpriteRender)GSpriteLabel_render, NULL, NULL, (GSpriteFree)GSpriteLabel_free);
  SDL_Color col = { (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF, (color >> 24) & 0xFF};
  surf = TTF_RenderText_Blended (font, text, col);
  if (!surf) {
    SDL_Log ("TTF_RenderText_Blended: %s", TTF_GetError ());
    GSprite_free ((GSprite *)spr);
    return NULL;
  }
  spr->base.w = surf->w;
  spr->base.h = surf->h;
  GSprite_justify ((GSprite *)spr, x, y, justify_hor, justify_ver);
  spr->texture = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  SDL_FreeSurface (surf);
  if (!spr->texture) {
    SDL_Log ("SDL_CreateTextureFromSurface: %s", SDL_GetError ());
    GSprite_free ((GSprite *)spr);
    return NULL;
  }
  return (GSprite *)spr;
}
