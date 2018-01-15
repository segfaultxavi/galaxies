#include <SDL.h>
#include "GSpriteProgress.h"
#include "GSpriteLabel.h"
#include "GResources.h"

#define GPROGRESS_MARGIN 10
#define GPROGRESS_DEFAULT_BG_COLOR 0xFF000040
#define GPROGRESS_DEFAULT_FG_COLOR 0xFF808000

struct _GSpriteProgress {
  GSprite base;
  int progress;
  GSprite *text;
  TTF_Font *text_font;
  Uint32 text_color;
  Uint32 glow_color;
};

static void GSpriteProgress_render (GSpriteProgress *spr, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Color col;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;

  // Progress background
  rect.x = spr->base.x + offsx + spr->progress;
  rect.y = spr->base.y + offsy;
  rect.w = spr->base.w - spr->progress;
  rect.h = spr->base.h;
  col.a = (GPROGRESS_DEFAULT_BG_COLOR >> 24) & 0xFF;
  col.r = (GPROGRESS_DEFAULT_BG_COLOR >> 16) & 0xFF;
  col.g = (GPROGRESS_DEFAULT_BG_COLOR >>  8) & 0xFF;
  col.b = (GPROGRESS_DEFAULT_BG_COLOR >>  0) & 0xFF;
  SDL_SetRenderDrawColor (renderer, col.r, col.g, col.b, col.a);
  if (SDL_RenderFillRect (renderer, &rect) != 0) {
    SDL_Log ("SDL_RenderFillRect: %s", SDL_GetError ());
  }
  // Progress foreground
  rect.x = spr->base.x + offsx;
  rect.y = spr->base.y + offsy;
  rect.w = spr->progress;
  rect.h = spr->base.h;
  col.a = (GPROGRESS_DEFAULT_FG_COLOR >> 24) & 0xFF;
  col.r = (GPROGRESS_DEFAULT_FG_COLOR >> 16) & 0xFF;
  col.g = (GPROGRESS_DEFAULT_FG_COLOR >> 8) & 0xFF;
  col.b = (GPROGRESS_DEFAULT_FG_COLOR >> 0) & 0xFF;
  SDL_SetRenderDrawColor (renderer, col.r, col.g, col.b, col.a);
  if (SDL_RenderFillRect (renderer, &rect) != 0) {
    SDL_Log ("SDL_RenderFillRect: %s", SDL_GetError ());
  }
}

GSprite *GSpriteProgress_new (GResources *res, int x, int y, int w, int h, const char *text, TTF_Font *font, Uint32 text_color, Uint32 glow_color, float progress) {
  GSpriteProgress *spr;

  // Progress
  spr = (GSpriteProgress *)GSprite_new (res, sizeof (GSpriteProgress),
      (GSpriteRender)GSpriteProgress_render, NULL, NULL, NULL);

  // Text
  spr->text = GSpriteLabel_new (res, w / 2, h  / 2, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER, font, text_color, glow_color, text);
  GSprite_add_child ((GSprite *)spr, spr->text);
  spr->text_font = font;
  spr->text_color = text_color;
  spr->glow_color = glow_color;

  spr->base.x = x;
  spr->base.y = y;
  spr->base.w = w;
  spr->base.h = h;
  return (GSprite *)spr;
}

void GSpriteProgress_set_progress (GSpriteProgress *spr, float progress) {
  spr->progress = (int)(progress * spr->base.w);
}

void GSpriteProgress_set_text (GSpriteProgress *spr, const char *text) {
  GSprite_free (spr->text);
  spr->text = GSpriteLabel_new (spr->base.res, spr->base.w / 2, spr->base.h / 2,
      GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER, spr->text_font, spr->text_color, spr->glow_color, text);
  GSprite_add_child ((GSprite *)spr, spr->text);
}
