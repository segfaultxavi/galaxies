#include <SDL.h>
#include "GGame.h"
#include "GSpriteCredits.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GIcons.h"

struct _GSpriteCredits {
  GSprite base;
  GSprite *main_menu;
};

int GSpriteCredits_back (void *userdata, int *destroyed) {
  GSpriteCredits *spr = userdata;
  GSprite *main_menu = spr->main_menu;
  SDL_Log ("Back");
  GSprite_free ((GSprite *)spr);
  main_menu->visible = 1;
  if (destroyed) *destroyed = 1;
  return 1;
}

static int GSpriteCredits_event (GSpriteCredits *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        ret = GSpriteCredits_back (spr, destroyed);
      }
      break;
    default:
      break;
  }
  return ret;
}

const char *credits[][2] = {
  {"Programming", "Xavi Artigas"},
  {"Original idea", "TENTAI SHOW by Nikoli"},
  {"Fonts", "SPIN CYCLE by Blue Vinyl fonts"},
  {"", "COMFORTAA by Johan Aakerlund" },
  {"", "TELEGRAMA by YOFonts"},
  {"", "WebHostingHub Glyphs"},
  {"Music", "Patrick Lieberkind"},
  {"Sound FX", "freesound.org"},
  {"Libraries", "sdl, sdl_ttf, sdl_mixer"}
};

GSprite *GSpriteCredits_new (GResources *res, GSprite *main_menu) {
  GSpriteCredits *spr = (GSpriteCredits *)GSprite_new (res, sizeof (GSpriteCredits),
      NULL, (GSpriteEvent)GSpriteCredits_event, NULL, NULL);
  int l;
  int line = res->game_height / 16;
  GSprite *button, *label;

  spr->base.w = spr->base.h = -1;
  spr->main_menu = main_menu;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, res->game_width / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_big,
      0xFF000000, 0xFFFFFFFF, "TENTAI SHOW"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, 3 * res->game_width / 4, 3 * res->game_height / 30, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_med,
      0xFFFFFFFF, 0xFFFFFFFF, "CREDITS"));
  for (l = 0; l < sizeof (credits) / sizeof (credits[0]); l++) {
    if (credits[l][0][0] != '\0')
    GSprite_add_child ((GSprite *)spr,
      GSpriteLabel_new (res, (int)(res->game_width * 0.3f), (l + 4) * line, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_BEGIN,
        res->font_text, 0xFFFFFFFF, 0, credits [l][0]));
    GSprite_add_child ((GSprite *)spr,
      GSpriteLabel_new (res, (int)(res->game_width * 0.35f), (l + 4) * line, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN,
        res->font_text, 0xFFFFFF00, 0, credits [l][1]));
  }

  button = GSpriteButton_new_with_icon (res, res->game_width, res->game_height, -1, -1, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_END,
    res->font_med, 0xFFFFFFFF, 0xFF000000, "Back", GSpriteCredits_back, spr, res->font_icons_med, GICON_BACK);
  GSprite_add_child ((GSprite *)spr, button);

  label = GSpriteLabel_new (res, (res->game_width - button->w) / 2, res->game_height - 20, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_END,
    res->font_icons_small, 0xFF808080, 0, "Made in " GICON_PALMTREE " Barcelona");
  GSprite_add_child ((GSprite *)spr, label);

  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, (res->game_width - button->w) / 2, res->game_height - 40 - label->h, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_END,
      res->font_text, 0xFFC0C0C0, 0, "© 2017 Xavi Artigas"));

  return (GSprite *)spr;
}
