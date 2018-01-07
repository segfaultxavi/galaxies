#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteGalaxies.h"
#include "GSpriteLevelSelect.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteNull.h"
#include "GSpriteBoard.h"

struct _GSpriteGalaxies {
  GSprite base;
  GSprite *level_select;
  void *level_data;
  GSprite *reset;
  GSprite *solution;
  GSprite *completed;
  GSpriteBoard *board;
};

static int GSpriteGalaxies_reset (void *userdata, int *destroyed) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Reset");
  return 1;
}

static int GSpriteGalaxies_solution (void *userdata, int *destroyed) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Solution");
  return 1;
}

static int GSpriteGalaxies_back (void *userdata, int *destroyed) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Back");
  spr->level_select->visible = 1;
  GSprite_free ((GSprite *)spr);
  if (destroyed) *destroyed = 1;
  return 1;
}

static int GSpriteGalaxies_event (GSpriteGalaxies *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        ret = GSpriteGalaxies_back (spr, destroyed);
      }
      break;
    default:
      break;
  }
  return ret;
}

GSprite *GSpriteGalaxies_new (GResources *res, GSprite *level_select, const char *level_description, void *level_data) {
  int line = res->game_height / 10;
  int mwidth = res->game_width - res->game_height;
  GSpriteGalaxies *spr = (GSpriteGalaxies *)GSprite_new (res, sizeof (GSpriteGalaxies),
      NULL, (GSpriteEvent)GSpriteGalaxies_event, NULL, NULL);
  GSprite *margin = GSpriteNull_new (res, res->game_height, 0);
  spr->level_select = level_select;
  spr->level_data = level_data;
  GSpriteLevelSelect_update_level (spr->level_data, GSPRITE_LEVEL_SELECT_LEVEL_STATUS_IN_PROGRESS);
  spr->base.w = spr->base.h = -1;
  GSprite_add_child (margin,
    GSpriteLabel_new (res, mwidth / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_title_med,
      0xFF000000, 0xFFFFFFFF, "tentai show"));
  spr->reset = GSpriteButton_new (res, mwidth / 2, 2 * line, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
    res->font_small, 0xFFFFFFFF, 0xFF000000, "Reset", GSpriteGalaxies_reset, spr);
  GSprite_add_child (margin, spr->reset);
  spr->solution = GSpriteButton_new (res, mwidth / 2, 3 * line, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
    res->font_small, 0xFFFFFFFF, 0xFF000000, "Solution", GSpriteGalaxies_solution, spr);
  GSprite_add_child (margin, spr->solution);
  spr->completed = GSpriteLabel_new_multiline (res, mwidth / 2, 6 * line, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN,
    res->font_med, 0xFFFFFFFF, 0xFFFF0000, "level\ncomplete");
  GSprite_add_child (margin, spr->completed);
  spr->completed->visible = 0;
  GSprite_add_child (margin,
    GSpriteButton_new (res, mwidth / 2, res->game_height, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_END,
      res->font_small, 0xFFFFFFFF, 0xFF000000, "Back", GSpriteGalaxies_back, spr));
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
  GSpriteLevelSelect_update_level (spr->level_data, GSPRITE_LEVEL_SELECT_LEVEL_STATUS_DONE);
}
