#include <SDL.h>
#include "GSpriteButton.h"
#include "GSpriteLabel.h"
#include "GResources.h"
#include "GAudio.h"

typedef enum _GButtonState {
  GBUTTON_STATE_NORMAL,
  GBUTTON_STATE_HOVER,
  GBUTTON_STATE_ACTIVE
} GButtonState;

struct _GSpriteButton {
  GSprite base;
  GSpriteButtonCallback callback;
  void *userdata;
  GButtonState state;
  Uint32 color;
};

#define GBUTTON_MARGIN 10

static void GSpriteButton_render (GSpriteButton *spr, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Color col;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;
  rect.x = spr->base.x + offsx;
  rect.y = spr->base.y + offsy;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  col.a = (spr->color >> 24) & 0xFF;
  col.r = (spr->color >> 16) & 0xFF;
  col.g = (spr->color >>  8) & 0xFF;
  col.b = (spr->color >>  0) & 0xFF;
  switch (spr->state) {
    case GBUTTON_STATE_NORMAL: col.r >>= 2; col.g >>= 2; col.b >>= 2; break;
    case GBUTTON_STATE_HOVER: col.r >>= 1; col.g >>= 1; col.b >>= 1; break;
    case GBUTTON_STATE_ACTIVE: col.r >>= 0; col.g >>= 0; col.b >>= 0; break;
  }
  SDL_SetRenderDrawColor (renderer, col.r, col.g, col.b, col.a);
  if (SDL_RenderFillRect (renderer, &rect) != 0) {
    SDL_Log ("SDL_RenderFillRect: %s", SDL_GetError ());
  }
}

static int GSpriteButton_event (GSpriteButton *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_MOVE:
      ret = 1;
      break;
    case GEVENT_TYPE_SPRITE_IN:
      spr->state = GBUTTON_STATE_HOVER;
      ret = 1;
      break;
    case GEVENT_TYPE_SPRITE_OUT:
      spr->state = GBUTTON_STATE_NORMAL;
      ret = 1;
      break;
    case GEVENT_TYPE_SPRITE_ACTIVATE:
    case GEVENT_TYPE_SPRITE_ACTIVATE_SECONDARY:
      spr->state = GBUTTON_STATE_ACTIVE;
      GAudio_play (spr->base.res->audio, spr->base.res->wav_ping);
      ret = spr->callback (spr->userdata, destroyed);
      break;
    default:
      break;
  }
  return ret;
}

GSprite *GSpriteButton_new (GResources *res, int x, int y, int w, int h, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    TTF_Font *font, Uint32 button_color, Uint32 label_color, const char *text, GSpriteButtonCallback callback, void *userdata) {
  GSprite *label;
  GSpriteButton *spr;
  label = GSpriteLabel_new (res, 0, 0, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN, font, label_color, 0xFFFFFFFF, text);
  if (w == -1) {
    label->x = GBUTTON_MARGIN;
    w = label->w + 2 * GBUTTON_MARGIN;
  } else {
    label->x = w / 2 - label->w / 2;
  }
  if (h == -1) {
    label->y = GBUTTON_MARGIN;
    h = label->h + 2 * GBUTTON_MARGIN;
  } else {
    label->y = h / 2 - label->h / 2;
  }
  spr = (GSpriteButton *)GSprite_new (res, sizeof (GSpriteButton),
      (GSpriteRender)GSpriteButton_render, (GSpriteEvent)GSpriteButton_event, NULL, NULL);
  spr->base.w = w;
  spr->base.h = h;
  GSprite_justify ((GSprite *)spr, x, y, justify_hor, justify_ver);
  spr->callback = callback;
  spr->userdata = userdata;
  spr->state = GBUTTON_STATE_NORMAL;
  spr->color = button_color;
  GSprite_add_child ((GSprite *)spr, label);
  return (GSprite *)spr;
}

void GSpriteButton_set_color (GSpriteButton *spr, Uint32 color) {
  spr->color = color;
}
