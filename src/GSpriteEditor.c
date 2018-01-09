#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteEditor.h"
#include "GSpriteLevelSelect.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteNull.h"
#include "GSpriteBoard.h"
#include "GSpritePopup.h"

#define GEDITOR_DEFAULT_SIZE 7

struct _GSpriteEditor {
  GSprite base;
  GSprite *main_menu;
  GSpriteBoard *board;
};

static int GSpriteEditor_reset (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Reset");
  GSpriteBoard_reset (spr->board);
  return 1;
}

static void GSpriteEditor_restart_yes (void *userdata) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Restart:Yes");
  GSprite_free ((GSprite *)spr->board);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (spr->base.res, 1);
  GSpriteBoard_start (spr->board, GEDITOR_DEFAULT_SIZE, GEDITOR_DEFAULT_SIZE, 0, NULL, NULL);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);
}

static void GSpriteEditor_restart_no (void *userdata) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Restart:No");
}

static int GSpriteEditor_restart(void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  GSprite *popup;
  SDL_Log ("Editor:Restart");

  popup = GSpritePopup_new (spr->base.res, "Restart",
      "Are you sure?\nThis will remove everything.",
      "YES", GSpriteEditor_restart_yes, "NO", GSpriteEditor_restart_no, spr);
  GSprite_add_child ((GSprite *)spr, popup);
  return 1;
}

static int GSpriteEditor_back (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Back");
  spr->main_menu->visible = 1;
  GSprite_free ((GSprite *)spr);
  if (destroyed) *destroyed = 1;
  return 1;
}

static int GSpriteEditor_size_plus (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  int mapSizeX = GSpriteBoard_get_map_size_x (spr->board);
  int mapSizeY = GSpriteBoard_get_map_size_y (spr->board);
  if (mapSizeX == 20)
    return 1;
  mapSizeX++;
  mapSizeY++;
  SDL_Log ("Editor:Set Size to %dx%d", mapSizeX, mapSizeY);
  GSprite_free ((GSprite *)spr->board);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (spr->base.res, 1);
  GSpriteBoard_start (spr->board, mapSizeX, mapSizeY, 0, NULL, NULL);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);
  return 1;
}

static int GSpriteEditor_size_minus (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  int mapSizeX = GSpriteBoard_get_map_size_x (spr->board);
  int mapSizeY = GSpriteBoard_get_map_size_y (spr->board);
  if (mapSizeX == 5)
    return 1;
  mapSizeX--;
  mapSizeY--;
  SDL_Log ("Editor:Set Size to %dx%d", mapSizeX, mapSizeY);
  GSprite_free ((GSprite *)spr->board);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (spr->base.res, 1);
  GSpriteBoard_start (spr->board, mapSizeX, mapSizeY, 0, NULL, NULL);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);
  return 1;
}

static int GSpriteEditor_copy_from_clipboard (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  char *desc;
  desc = SDL_GetClipboardText ();
  SDL_Log ("Editor:Copy from clipboard %s", desc);
  GSprite_free ((GSprite *)spr->board);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (spr->base.res, 1);
  GSpriteBoard_load (spr->board, desc);
  SDL_free (desc);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);
  return 1;
}

static int GSpriteEditor_copy_to_clipboard (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  char *desc;
  desc = GSpriteBoard_save (spr->board, 0);
  SDL_Log ("Editor:Copy to clipboard %s", desc);
  SDL_SetClipboardText (desc);
  SDL_free (desc);
  return 1;
}

static int GSpriteEditor_event (GSpriteEditor *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        ret = GSpriteEditor_back (spr, destroyed);
      }
      break;
    default:
      break;
  }
  return ret;
}

#define BUTTON(x,y,name, callback) \
  GSprite_add_child (margin, \
    GSpriteButton_new (res, x * mwidth / 4, y * line, mwidth / 2 - 2, line - 2, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER, \
      res->font_small, 0xFFFFFFFF, 0xFF000000, name, callback, spr))

GSprite *GSpriteEditor_new (GResources *res, GSprite *main_menu) {
  int line = res->game_height / 10;
  int mwidth = res->game_width - res->game_height;
  GSpriteEditor *spr = (GSpriteEditor *)GSprite_new (res, sizeof (GSpriteEditor),
      NULL, (GSpriteEvent)GSpriteEditor_event, NULL, NULL);
  GSprite *margin = GSpriteNull_new (res, res->game_height, 0);
  spr->main_menu = main_menu;
  spr->base.w = spr->base.h = -1;
  GSprite_add_child (margin,
    GSpriteLabel_new (res, mwidth / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_title_med,
      0xFF000000, 0xFFFFFFFF, "tentai show"));
  GSprite_add_child (margin,
    GSpriteLabel_new (res, mwidth / 2, 1 * line, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_title_med,
      0xFF000000, 0xFFFFFFFF, "EDITOR"));

  BUTTON (1, 3, "RESET", GSpriteEditor_reset);
  BUTTON (3, 3, "RESTART", GSpriteEditor_restart);
  BUTTON (1, 4, "SIZE +", GSpriteEditor_size_plus);
  BUTTON (3, 4, "SIZE -", GSpriteEditor_size_minus);
  BUTTON (1, 5, "IMPORT", GSpriteEditor_copy_from_clipboard);
  BUTTON (3, 5, "EXPORT", GSpriteEditor_copy_to_clipboard);

  GSprite_add_child (margin,
    GSpriteButton_new (res, mwidth / 2, res->game_height, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_END,
      res->font_small, 0xFFFFFFFF, 0xFF000000, "Back", GSpriteEditor_back, spr));

  GSprite_add_child ((GSprite *)spr, margin);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (res, 1);
  GSpriteBoard_start (spr->board, GEDITOR_DEFAULT_SIZE, GEDITOR_DEFAULT_SIZE, 0, NULL, NULL);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);
  return (GSprite *)spr;
}
