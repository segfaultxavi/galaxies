#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteGalaxies.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteNull.h"

typedef struct _GSpriteGalaxies {
  GSprite base;
  GResources *res;
  GSprite *main_menu;
} GSpriteGalaxies;

int GSpriteGalaxies_reset (void *userdata) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Reset");
  return 0;
}

int GSpriteGalaxies_solution (void *userdata) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Solution");
  return 0;
}

int GSpriteGalaxies_back (void *userdata) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Back");
  spr->main_menu->visible = 1;
  GSprite_free ((GSprite *)spr);
  return 1;
}

GSprite *GSpriteGalaxies_new (GResources *res, GSprite *main_menu) {
  int line = res->game_height / 16;
  int mwidth = res->game_width - res->game_height;
  GSpriteGalaxies *spr = (GSpriteGalaxies *)GSprite_new (sizeof (GSpriteGalaxies),
      NULL, NULL, NULL, NULL);
  GSprite *margin = GSpriteNull_new (res->game_height, 0);
  spr->main_menu = main_menu;
  spr->res = res;
  spr->base.w = spr->base.h = -1;
  spr->res = res;
  GSprite_add_child (margin,
    GSpriteLabel_new (mwidth / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res, res->font_med, 0xFFFFFF00, "galaxies"));
  GSprite_add_child (margin,
    GSpriteButton_new (mwidth / 2, 3 * line, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res, res->font_small, 0xFF0000FF, "reset", GSpriteGalaxies_reset, spr));
  GSprite_add_child (margin,
    GSpriteButton_new (mwidth / 2, 4 * line, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res, res->font_small, 0xFF0000FF, "solution", GSpriteGalaxies_solution, spr));
  GSprite_add_child (margin,
    GSpriteButton_new (mwidth / 2, res->game_height, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_END,
      res, res->font_small, 0xFF0000FF, "back", GSpriteGalaxies_back, spr));
  GSprite_add_child ((GSprite *)spr, margin);
  return (GSprite *)spr;
}