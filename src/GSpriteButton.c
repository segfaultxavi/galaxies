#include <SDL.h>
#include "GSpriteButton.h"
#include "GSpriteLabel.h"
#include "GResources.h"

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

static SDL_Color gcolor_button_normal = { 0x40, 0x40, 0x40, 0xFF };
static SDL_Color gcolor_button_hover = { 0x80, 0x80, 0x80, 0xFF };
static SDL_Color gcolor_button_active = { 0xC0, 0xC0, 0xC0, 0xFF };

#define GBUTTON_MARGIN 10

void GSpriteButton_render (GSpriteButton *spr, SDL_Renderer *renderer, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Color col;
  rect.x = spr->base.x + offsx;
  rect.y = spr->base.y + offsy;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  switch (spr->state) {
    case GBUTTON_STATE_NORMAL: col = gcolor_button_normal; break;
    case GBUTTON_STATE_HOVER: col = gcolor_button_hover; break;
    case GBUTTON_STATE_ACTIVE: col = gcolor_button_active; break;
  }
  SDL_SetRenderDrawColor (renderer, col.r, col.g, col.b, col.a);
  if (SDL_RenderFillRect (renderer, &rect) != 0) {
    SDL_Log ("SDL_RenderFillRect: %s", SDL_GetError ());
  }
}

int GSpriteButton_event (GSpriteButton *spr, GEvent *event) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_SPRITE_IN:
      spr->state = GBUTTON_STATE_HOVER;
      break;
    case GEVENT_TYPE_SPRITE_OUT:
      spr->state = GBUTTON_STATE_NORMAL;
      break;
    case GEVENT_TYPE_SPRITE_ACTIVATE:
      spr->state = GBUTTON_STATE_ACTIVE;
      ret = spr->callback (spr->userdata);
      break;
    default:
      break;
  }
  return ret;
}

GSprite *GSpriteButton_new (int x, int y, int w, int h, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    GResources *res, TTF_Font *font, Uint32 color, const char *text, GSpriteButtonCallback callback, void *userdata) {
  GSprite *label;
  GSpriteButton *spr;
  label = GSpriteLabel_new (0, 0, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN, res, font, color, text);
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
  spr = (GSpriteButton *)GSprite_new (sizeof (GSpriteButton),
      (GSpriteRender)GSpriteButton_render, (GSpriteEvent)GSpriteButton_event, NULL, NULL);
  spr->base.w = w;
  spr->base.h = h;
  GSprite_justify ((GSprite *)spr, x, y, justify_hor, justify_ver);
  spr->callback = callback;
  spr->userdata = userdata;
  spr->state = GBUTTON_STATE_NORMAL;
  GSprite_add_child ((GSprite *)spr, label);
  return (GSprite *)spr;
}
