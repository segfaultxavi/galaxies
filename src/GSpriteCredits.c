#include <SDL.h>
#include "GGame.h"
#include "GSpriteCredits.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"

typedef struct _GSpriteCredits {
  GSprite base;
  GSprite *main_menu;
} GSpriteCredits;

int GSpriteCredits_back (void *userdata) {
  GSpriteCredits *spr = userdata;
  GSprite *main_menu = spr->main_menu;
  SDL_Log ("Back");
  GSprite_free ((GSprite *)spr);
  main_menu->visible = 1;
  return 1;
}

const char *credits[][2] = {
  {"programming", "xavi artigas"},
  {"original idea", "tentai show"},
  {"", "by nikoli"},
  {"font", "bajoran"},
  {"", "by kiwi media"},
  {"music", "?"},
  {"sound", "?"},
  {"libraries", "sdl, sdl_ttf"}
};

GSprite *GSpriteCredits_new (GResources *res, GSprite *main_menu) {
  GSpriteCredits *spr = (GSpriteCredits *)GSprite_new (sizeof (GSpriteCredits),
      NULL, NULL, NULL, NULL);
  int l;
  spr->base.w = spr->base.h = -1;
  spr->main_menu = main_menu;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res->game_width / 2, 0, GLABEL_JUSTIFY_CENTER, GLABEL_JUSTIFY_BEGIN, res, res->font_big, 0xFFFFFF00, "galaxies"));
  for (l = 0; l < sizeof (credits) / sizeof (credits[0]); l++) {
    if (credits[l][0][0] != '\0')
    GSprite_add_child ((GSprite *)spr,
      GSpriteLabel_new (res->game_width / 2 - 50, 75 + l * 25, GLABEL_JUSTIFY_END, GLABEL_JUSTIFY_BEGIN, res, res->font_small, 0xFFFFFFFF, credits [l][0]));
    GSprite_add_child ((GSprite *)spr,
      GSpriteLabel_new (res->game_width / 2, 75 + l * 25, GLABEL_JUSTIFY_BEGIN, GLABEL_JUSTIFY_BEGIN, res, res->font_small, 0xFFFFFF00, credits [l][1]));
  }
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (res->game_width - 150, res->game_height - 50, 150, 50, res, res->font_med, 0xFF0000FF, "back", GSpriteCredits_back, spr));
  return (GSprite *)spr;
}
