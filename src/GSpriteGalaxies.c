﻿#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteGalaxies.h"
#include "GSpriteLevelSelect.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteNull.h"
#include "GSpriteBoard.h"
#include "GSpritePopup.h"

struct _GSpriteGalaxies {
  GSprite base;
  GSprite *level_select;
  void *level_data;
  GSprite *reset;
  GSprite *completed;
  GSpriteBoard *board;
};

static int GSpriteGalaxies_help (void *userdata, int *destroyed) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Help");

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "HELP",
      "Click on a circle (a CORE) to select it.\n"
      "Click neighbor tiles to link them to that core.\n"
      " \n"
      "• All tiles must be linked to a core.\n"
      "• All tiles linked to the same core must form a single group.\n"
      "• Linking a tile automatically links the opposite one.\n"
      " \n"
      "Good luck!",
      "OK", GSpritePopup_dismiss, NULL, NULL, spr));

  return 1;
}

static void GSpriteGalaxies_reset_yes (void *userdata) {
  GSpriteGalaxies *spr = userdata;

  SDL_Log ("Galaxies:Reset:Yes");
  GSpriteBoard_reset (spr->board);
}

static int GSpriteGalaxies_reset (void *userdata, int *destroyed) {
  GSpriteGalaxies *spr = userdata;
  SDL_Log ("Galaxies:Reset");

  if (GSpriteBoard_has_no_manual_tiles (spr->board))
    return 1;

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "RESET",
      "Are you sure?\n"
      "This will remove all tile colors.",
      "YES", GSpriteGalaxies_reset_yes, "NO", GSpritePopup_dismiss, spr));

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

GSprite *GSpriteGalaxies_new (GResources *res, GSprite *level_select, int level_number, const char *level_description, void *level_data) {
  int line = res->game_height / 10;
  int mwidth = res->game_width - res->game_height;
  GSpriteGalaxies *spr = (GSpriteGalaxies *)GSprite_new (res, sizeof (GSpriteGalaxies),
      NULL, (GSpriteEvent)GSpriteGalaxies_event, NULL, NULL);
  GSprite *margin = GSpriteNull_new (res, res->game_height, 0);
  char str[16];
  spr->level_select = level_select;
  spr->level_data = level_data;
  spr->base.w = spr->base.h = -1;
  GSprite_add_child (margin,
    GSpriteLabel_new (res, mwidth / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_med,
      0xFF000000, 0xFFFFFFFF, "TENTAI SHOW"));
  SDL_snprintf (str, sizeof (str), "Level %d", level_number);
  GSprite_add_child (margin,
    GSpriteLabel_new (res, mwidth / 2, 1 * line, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_small,
      0xFFFFFFFF, 0x00000000, str));
  GSprite_add_child (margin, GSpriteButton_new (res, mwidth / 2, 3 * line, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
    res->font_small, 0xFFFFFFFF, 0xFF000000, "Help", GSpriteGalaxies_help, spr));
  spr->reset = GSpriteButton_new (res, mwidth / 2, 4 * line, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
    res->font_small, 0xFFFFFFFF, 0xFF000000, "Reset", GSpriteGalaxies_reset, spr);
  GSprite_add_child (margin, spr->reset);
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
  spr->completed->visible = 1;
  GSpriteLevelSelect_update_level_status (spr->level_data, GSPRITE_LEVEL_SELECT_LEVEL_STATUS_DONE, GSpriteBoard_save (spr->board, 1));
  GSpriteBoard_galaxy_size_highlight (spr->board);
}

void GSpriteGalaxies_update_level_status (GSpriteGalaxies *spr, GSpriteLevelSelectLevelStatus status, char *desc) {
  GSpriteLevelSelect_update_level_status (spr->level_data, status, desc);
}