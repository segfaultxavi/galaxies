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
  {"programming", "XAVI ARTIGAS"},
  {"original idea", "TENTAI SHOW by nikoli"},
  {"font", "BAJORAN by kiwi media"},
  {"music", "?"},
  {"sound", "?"},
  {"libraries", "sdl, sdl_ttf"}
};

GSprite *GSpriteCredits_new (GResources *res, GSprite *main_menu) {
  GSpriteCredits *spr = (GSpriteCredits *)GSprite_new (sizeof (GSpriteCredits),
      NULL, NULL, NULL, NULL);
  int l;
  int line = res->game_height / 16;
  spr->base.w = spr->base.h = -1;
  spr->main_menu = main_menu;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res->game_width / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res, res->font_big, 0xFFFFFF00, "galaxies"));
  for (l = 0; l < sizeof (credits) / sizeof (credits[0]); l++) {
    if (credits[l][0][0] != '\0')
    GSprite_add_child ((GSprite *)spr,
      GSpriteLabel_new (res->game_width * 0.4f, (l + 4) * line, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_BEGIN,
        res, res->font_small, 0xFFFFFFFF, credits [l][0]));
    GSprite_add_child ((GSprite *)spr,
      GSpriteLabel_new (res->game_width * 0.45f, (l + 4) * line, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN,
        res, res->font_small, 0xFFFFFF00, credits [l][1]));
  }
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (res->game_width, res->game_height, -1, -1, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_END,
      res, res->font_med, 0xFF0000FF, "back", GSpriteCredits_back, spr));
  return (GSprite *)spr;
}
