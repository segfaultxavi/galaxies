#include <SDL.h>
#include "GSpriteTestBox.h"

typedef struct _GSpriteTestBox {
  GSprite base;
  int color;
} GSpriteTestBox;

void GSpriteTestBox_render (GSpriteTestBox *spr, SDL_Renderer *renderer) {
  SDL_Rect rect;
  rect.x = spr->base.x;
  rect.y = spr->base.y;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  SDL_SetRenderDrawColor (renderer,
    (spr->color >> 16) & 0xFF,
    (spr->color >>  8) & 0xFF,
    (spr->color >>  0) & 0xFF,
    (spr->color >> 24) & 0xFF);
  if (SDL_RenderFillRect (renderer, &rect) != 0) {
    SDL_Log ("SDL_RenderFillRect: %s", SDL_GetError ());
  }
}

int GSpriteTestBox_action (GSpriteTestBox *spr, GEvent *event) {
  if (event->type == SDL_MOUSEBUTTONDOWN) {
    spr->color ^= 0x0000FF00;
    SDL_Log ("Clicked");
  }
  return 1;
}

GSprite *GSpriteTestBox_new (int x, int y, int w, int h) {
  GSpriteTestBox *spr = (GSpriteTestBox *)GSprite_new (sizeof (GSpriteTestBox),
      (GSpriteRender)GSpriteTestBox_render, (GSpriteAction)GSpriteTestBox_action, NULL);
  spr->base.x = x;
  spr->base.y = y;
  spr->base.w = w;
  spr->base.h = h;
  spr->color = 0xFFFF0000;
  return (GSprite *)spr;
}
