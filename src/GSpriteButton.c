#include <SDL.h>
#include "GSpriteButton.h"
#include "GSpriteLabel.h"
#include "GSpriteNull.h"
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
      GAudio_play_sample (spr->base.res->audio, spr->base.res->wav_pong);
      ret = spr->callback (spr->userdata, destroyed);
      break;
    default:
      break;
  }
  return ret;
}

GSprite *GSpriteButton_new_with_sprite (GResources *res, int x, int y, int w, int h, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    GSprite *content, Uint32 button_color, GSpriteButtonCallback callback, void *userdata) {
  GSpriteButton *spr;

  // Content
  if (w == -1) {
    content->x = GBUTTON_MARGIN;
    w = content->w + 2 * GBUTTON_MARGIN;
  } else {
    content->x = w / 2 - content->w / 2;
  }
  if (h == -1) {
    content->y = GBUTTON_MARGIN;
    h = content->h + 2 * GBUTTON_MARGIN;
  } else {
    content->y = h / 2 - content->h / 2;
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
  GSprite_add_child ((GSprite *)spr, content);

  return (GSprite *)spr;
}

GSprite *GSpriteButton_new_with_text_and_icon (GResources *res, int x, int y, int w, int h, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    TTF_Font *font, Uint32 button_color, Uint32 label_color, const char *text, GSpriteButtonCallback callback, void *userdata,
    TTF_Font *icon_font, const char *icon_text) {
  GSprite *group = NULL, *label = NULL, *icon = NULL;

  if (!icon_text)
    return GSpriteButton_new_with_text (res, x, y, w, h, justify_hor, justify_ver, font, button_color, label_color, text, callback, userdata);

  // Icon
  icon = GSpriteLabel_new (res, GBUTTON_MARGIN, 0, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN,
    icon_font, label_color, 0xFFFFFFFF, icon_text);

  if (!text) {
    // Only Icon
    group = icon;
  } else {
    // Text and Icon
    group = GSpriteNull_new(res, 0, 0);
    label = GSpriteLabel_new (res, 0, 0, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN, font, label_color, 0xFFFFFFFF, text);
    GSprite_add_child (group, icon);
    GSprite_add_child (group, label);
    if (w == -1) {
      group->w = icon->w + GBUTTON_MARGIN + label->w;
      label->x = icon->w + GBUTTON_MARGIN;
    } else {
      group->w = w - GBUTTON_MARGIN * 2;
      label->x = icon->w + (group->w - icon->w) / 2 - label->w / 2;
    }
    group->h = SDL_max (label->h, icon->h);
    icon->y = (group->h - icon->h) / 2;
    label->y = (group->h - label->h) / 2;
  }

  return GSpriteButton_new_with_sprite (res, x, y, w, h, justify_hor, justify_ver, group, button_color, callback, userdata);
}

GSprite *GSpriteButton_new_with_text (GResources *res, int x, int y, int w, int h, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    TTF_Font *font, Uint32 button_color, Uint32 label_color, const char *text, GSpriteButtonCallback callback, void *userdata) {

  GSprite *label = GSpriteLabel_new (res, 0, 0, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN, font, label_color, 0xFFFFFFFF, text);
  return GSpriteButton_new_with_sprite (res, x, y, w, h, justify_hor, justify_ver, label, button_color, callback, userdata);
}

void GSpriteButton_set_color (GSpriteButton *spr, Uint32 color) {
  spr->color = color;
}
