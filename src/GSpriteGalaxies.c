#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteGalaxies.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteNull.h"
#include "GSpriteBoard.h"

struct _GSpriteGalaxies {
  GSprite base;
  GSprite *main_menu;
  GSprite *reset;
  GSprite *solution;
  GSprite *completed;
  GSpriteBoard *board;
};

static int GSpriteGalaxies_reset (void *userdata) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Reset");
  return 0;
}

static int GSpriteGalaxies_solution (void *userdata) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Solution");
  return 0;
}

static int GSpriteGalaxies_back (void *userdata) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Back");
  spr->main_menu->visible = 1;
  GSprite_free ((GSprite *)spr);
  return 1;
}

GSprite *GSpriteGalaxies_new (GResources *res, GSprite *main_menu, const char *level_description) {
  int line = res->game_height / 16;
  int mwidth = res->game_width - res->game_height;
  GSpriteGalaxies *spr = (GSpriteGalaxies *)GSprite_new (res, sizeof (GSpriteGalaxies),
      NULL, NULL, NULL, NULL);
  GSprite *margin = GSpriteNull_new (res, res->game_height, 0);
  spr->main_menu = main_menu;
  spr->base.w = spr->base.h = -1;
  GSprite_add_child (margin,
    GSpriteLabel_new (res, mwidth / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_title_med,
      0xFF000000, 0xFFFFFFFF, "galaxies"));
  spr->reset = GSpriteButton_new (res, mwidth / 2, 3 * line, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
    res->font_small, 0xFF0000FF, "reset", GSpriteGalaxies_reset, spr);
  GSprite_add_child (margin, spr->reset);
  spr->solution = GSpriteButton_new (res, mwidth / 2, 4 * line, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
    res->font_small, 0xFF0000FF, "solution", GSpriteGalaxies_solution, spr);
  GSprite_add_child (margin, spr->solution);
  spr->completed = GSpriteLabel_new_multiline (res, mwidth / 2, 6 * line, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN,
    res->font_med, 0xFFFFFFFF, 0xFFFF0000, "level\ncomplete");
  GSprite_add_child (margin, spr->completed);
  spr->completed->visible = 0;
  GSprite_add_child (margin,
    GSpriteButton_new (res, mwidth / 2, res->game_height, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_END,
      res->font_small, 0xFF0000FF, "back", GSpriteGalaxies_back, spr));
  GSprite_add_child ((GSprite *)spr, margin);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (res, 0);
  GSpriteBoard_load (spr->board, level_description);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);
  return (GSprite *)spr;
}

void GSpriteGalaxies_complete (GSpriteGalaxies *spr) {
  spr->reset->visible = 0;
  spr->solution->visible = 0;
  spr->completed->visible = 1;
}
