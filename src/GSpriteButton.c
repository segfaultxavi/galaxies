#include <SDL.h>
#include "GSpriteButton.h"
#include "GSpriteLabel.h"

typedef enum _GButtonState {
  GBUTTON_STATE_NORMAL,
  GBUTTON_STATE_HOVER,
  GBUTTON_STATE_ACTIVE
} GButtonState;

typedef struct _GSpriteButton {
  GSprite base;
  GSpriteButtonCallback callback;
  void *userdata;
  GButtonState state;
} GSpriteButton;

SDL_Color gcolor_normal = { 0x40, 0x40, 0x40, 0xFF };
SDL_Color gcolor_hover = { 0x80, 0x80, 0x80, 0xFF };
SDL_Color gcolor_active = { 0xC0, 0xC0, 0xC0, 0xFF };

void GSpriteButton_render (GSpriteButton *spr, SDL_Renderer *renderer, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Color col;
  rect.x = spr->base.x + offsx;
  rect.y = spr->base.y + offsy;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  switch (spr->state) {
    case GBUTTON_STATE_NORMAL: col = gcolor_normal; break;
    case GBUTTON_STATE_HOVER: col = gcolor_hover; break;
    case GBUTTON_STATE_ACTIVE: col = gcolor_active; break;
  }
  SDL_SetRenderDrawColor (renderer, col.r, col.g, col.b, col.a);
  if (SDL_RenderFillRect (renderer, &rect) != 0) {
    SDL_Log ("SDL_RenderFillRect: %s", SDL_GetError ());
  }
}

int GSpriteButton_event (GSpriteButton *spr, GEvent *event) {
  switch (event->type) {
    case GEVENT_TYPE_SPRITE_IN:
      spr->state = GBUTTON_STATE_HOVER;
      break;
    case GEVENT_TYPE_SPRITE_OUT:
      spr->state = GBUTTON_STATE_NORMAL;
      break;
    case GEVENT_TYPE_SPRITE_ACTIVATE:
      spr->state = GBUTTON_STATE_ACTIVE;
      spr->callback (spr->userdata);
      break;
  }
  return 1;
}

GSprite *GSpriteButton_new (int x, int y, int w, int h, SDL_Renderer *renderer, TTF_Font *font,
  Uint32 color, const char *text, GSpriteButtonCallback callback, void *userdata) {
  GSpriteButton *spr = (GSpriteButton *)GSprite_new (sizeof (GSpriteButton),
      (GSpriteRender)GSpriteButton_render, (GSpriteEvent)GSpriteButton_event, NULL, NULL);
  spr->base.x = x;
  spr->base.y = y;
  spr->base.w = w;
  spr->base.h = h;
  spr->callback = callback;
  spr->userdata = userdata;
  spr->state = GBUTTON_STATE_NORMAL;
  GSprite_add_child ((GSprite *)spr, GSpriteLabel_new (w / 2, h / 2, GLABEL_JUSTIFY_CENTER, GLABEL_JUSTIFY_CENTER, renderer, font, color, text));
  return (GSprite *)spr;
}
