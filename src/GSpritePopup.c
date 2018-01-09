#include <SDL.h>
#include "GSpritePopup.h"
#include "GSpriteButton.h"
#include "GSpriteLabel.h"
#include "GResources.h"

#define GPOPUP_MARGIN 10
#define GPOPUP_DEFAULT_COLOR 0xFF404040

struct _GSpritePopup {
  GSprite base;
  GSpritePopupCallback callback1;
  GSpritePopupCallback callback2;
  void *userdata;
};

static void GSpritePopup_render (GSpritePopup *spr, int offsx, int offsy) {
  SDL_Rect rect;
  SDL_Color col;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;
  // Dim screen background
  SDL_SetRenderDrawColor (renderer, 0, 0, 0, 0xC0);
  rect.x = rect.y = 0;
  rect.w = spr->base.res->game_width;
  rect.h = spr->base.res->game_height;
  if (SDL_RenderFillRect (renderer, &rect) != 0) {
    SDL_Log ("SDL_RenderFillRect: %s", SDL_GetError ());
  }
  // Popup background
  rect.x = spr->base.x + offsx;
  rect.y = spr->base.y + offsy;
  rect.w = spr->base.w;
  rect.h = spr->base.h;
  col.a = (GPOPUP_DEFAULT_COLOR >> 24) & 0xFF;
  col.r = (GPOPUP_DEFAULT_COLOR >> 16) & 0xFF;
  col.g = (GPOPUP_DEFAULT_COLOR >>  8) & 0xFF;
  col.b = (GPOPUP_DEFAULT_COLOR >>  0) & 0xFF;
  SDL_SetRenderDrawColor (renderer, col.r, col.g, col.b, col.a);
  if (SDL_RenderFillRect (renderer, &rect) != 0) {
    SDL_Log ("SDL_RenderFillRect: %s", SDL_GetError ());
  }
}

static int GSpritePopup_callback1 (void *userdata, int *destroyed) {
  GSpritePopup *spr = userdata;
  GSpritePopupCallback callback = spr->callback1;
  void *userdata2 = spr->userdata;

  GSprite_free ((GSprite *)spr);
  callback (userdata2);
  if (destroyed) *destroyed = 1;
  return 1;
}

static int GSpritePopup_callback2 (void *userdata, int *destroyed) {
  GSpritePopup *spr = userdata;
  GSpritePopupCallback callback = spr->callback2;
  void *userdata2 = spr->userdata;

  GSprite_free ((GSprite *)spr);
  callback (userdata2);
  if (destroyed) *destroyed = 1;
  return 1;
}

static int GSpritePopup_event (GSpritePopup *spr, GEvent *event, int *destroyed) {
  // By default we say we handle all events, since this is a modal popup
  int ret = 1;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        if (spr->callback2)
          ret = GSpritePopup_callback2 (spr, destroyed);
      }
      if (event->keycode == SDLK_RETURN || event->keycode == SDLK_KP_ENTER) {
        if (spr->callback1)
          ret = GSpritePopup_callback1 (spr, destroyed);
      }
      break;
    default:
      break;
  }
  return ret;
}

static int GSpritePopup_is_inside (GSprite *spr, int x, int y) {
  return 1;
}

GSprite *GSpritePopup_new (GResources *res, const char *title_text, const char *body_text,
    const char *button1_text, GSpritePopupCallback callback1,
    const char *button2_text, GSpritePopupCallback callback2,
    void *userdata) {
  GSprite *title, *label, *but;
  GSpritePopup *spr;

  // Popup
  spr = (GSpritePopup *)GSprite_new (res, sizeof (GSpritePopup),
      (GSpriteRender)GSpritePopup_render, (GSpriteEvent)GSpritePopup_event, GSpritePopup_is_inside, NULL);

  // Title
  title = GSpriteLabel_new (res, 0, 0, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN, res->font_med, 0xFF000000, 0xFFFFFFFF, title_text);
  title->x = GPOPUP_MARGIN;
  title->y = GPOPUP_MARGIN;
  GSprite_add_child ((GSprite *)spr, title);

  // Text
  label = GSpriteLabel_new_multiline (res, 0, 0, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN, res->font_text, 0xFFFFFFFF, 0x00000000, body_text);
  label->x = GPOPUP_MARGIN;
  label->y = 2 * GPOPUP_MARGIN + title->h;
  GSprite_add_child ((GSprite *)spr, label);

  // Buttons
  if (callback1) {
    but = GSpriteButton_new (res, GPOPUP_MARGIN, title->h + label->h + 3 * GPOPUP_MARGIN, -1, -1, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN,
      res->font_med, 0xFFFFFFFF, 0xFF000000, button1_text, GSpritePopup_callback1, spr);
    GSprite_add_child ((GSprite *)spr, but);
  }
  if (callback2) {
    but = GSpriteButton_new (res, label->w + GPOPUP_MARGIN, title->h + label->h + 3 * GPOPUP_MARGIN, -1, -1, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_BEGIN,
      res->font_med, 0xFFFFFFFF, 0xFF000000, button2_text, GSpritePopup_callback2, spr);
    GSprite_add_child ((GSprite *)spr, but);
  }

  spr->base.w = SDL_max (title->w, label->w) + 2 * GPOPUP_MARGIN;
  spr->base.h = title->h + label->h + but->h + 4 * GPOPUP_MARGIN;
  GSprite_justify ((GSprite *)spr, res->game_width / 2, res->game_height / 2, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER);
  spr->callback1 = callback1;
  spr->callback2 = callback2;
  spr->userdata = userdata;
  return (GSprite *)spr;
}

void GSpritePopup_dismiss (void *userdata) {
  SDL_Log ("Dismissed");
}