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

typedef struct _GSpriteEditorSizeChange {
  GSpriteEditor *spr;
  int delta;
} GSpriteEditorSizeChange;

typedef struct _GSpriteEditorCopyFromClipboard {
  GSpriteEditor *spr;
  char *new_desc;
} GSpriteEditorCopyFromClipboard;

struct _GSpriteEditor {
  GSprite base;
  GSprite *main_menu;
  GSpriteBoard *board;
  GSpriteEditorSizeChange size_change_plus;
  GSpriteEditorSizeChange size_change_minus;
  GSpriteEditorCopyFromClipboard copy_from_clipboard;
};

static void GSpriteEditor_reset_yes (void *userdata) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Reset:Yes");
  GSpriteBoard_reset (spr->board);
}

static int GSpriteEditor_reset (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Reset");

  if (GSpriteBoard_has_no_manual_tiles (spr->board))
    return 1;

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "RESET",
      "Are you sure?\n"
      "This will remove all tile colors\n"
      "and preserve cores.",
      "YES", GSpriteEditor_reset_yes, "NO", GSpritePopup_dismiss, spr));
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

static int GSpriteEditor_restart(void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Restart");

  if (GSpriteBoard_is_empty (spr->board))
    return 1;

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "RESTART",
      "Are you sure?\n"
      "This will remove everything\n"
      "from the map.",
      "YES", GSpriteEditor_restart_yes, "NO", GSpritePopup_dismiss, spr));
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

static void GSpriteEditor_size_change_yes (void *userdata) {
  GSpriteEditorSizeChange *sc = userdata;
  SDL_Log ("Editor:Size change:Yes");

  int mapSizeX = GSpriteBoard_get_map_size_x (sc->spr->board);
  int mapSizeY = GSpriteBoard_get_map_size_y (sc->spr->board);
  mapSizeX += sc->delta;
  mapSizeY += sc->delta;
  SDL_Log ("Editor:Set Size to %dx%d", mapSizeX, mapSizeY);
  GSprite_free ((GSprite *)sc->spr->board);
  sc->spr->board = (GSpriteBoard *)GSpriteBoard_new (sc->spr->base.res, 1);
  GSpriteBoard_start (sc->spr->board, mapSizeX, mapSizeY, 0, NULL, NULL);
  GSprite_add_child ((GSprite *)sc->spr, (GSprite *)sc->spr->board);
}

static int GSpriteEditor_size_plus (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;

  if (GSpriteBoard_get_map_size_x (spr->board) == 20)
    return 1;

  if (GSpriteBoard_is_empty (spr->board)) {
    GSpriteEditor_size_change_yes (&spr->size_change_plus);
    return 1;
  }

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "SIZE CHANGE",
      "Are you sure?\n"
      "This will remove everything\n"
      "from the map.",
      "YES", GSpriteEditor_size_change_yes, "NO", GSpritePopup_dismiss, &spr->size_change_plus));
  return 1;
}

static int GSpriteEditor_size_minus (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;

  if (GSpriteBoard_get_map_size_x (spr->board) == 5)
    return 1;

  if (GSpriteBoard_is_empty (spr->board)) {
    GSpriteEditor_size_change_yes (&spr->size_change_minus);
    return 1;
  }

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "SIZE CHANGE",
      "Are you sure?\n"
      "This will remove everything\n"
      "from the map.",
      "YES", GSpriteEditor_size_change_yes, "NO", GSpritePopup_dismiss, &spr->size_change_plus));
  return 1;
}

static void GSpriteEditor_copy_from_clipboard_yes (void *userdata) {
  GSpriteEditorCopyFromClipboard *cfc = userdata;
  GSpriteEditor *spr = cfc->spr;
  SDL_Log ("Editor:Copy from clipboard:Yes");
  GSprite_free ((GSprite *)spr->board);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (spr->base.res, 1);
  GSpriteBoard_load (spr->board, cfc->new_desc);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);
  SDL_free (cfc->new_desc);
  cfc->new_desc = NULL;
}

static void GSpriteEditor_copy_from_clipboard_no (void *userdata) {
  GSpriteEditorCopyFromClipboard *cfc = userdata;
  SDL_Log ("Editor:Copy from clipboard:No");
  SDL_free (cfc->new_desc);
  cfc->new_desc = NULL;
}

static int GSpriteEditor_copy_from_clipboard (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  char *desc;
  desc = SDL_GetClipboardText ();
  SDL_Log ("Editor:Copy from clipboard %s", desc);
  if (GSpriteBoard_check (desc) == 0) {
    GSprite_add_child ((GSprite *)spr,
      GSpritePopup_new (spr->base.res, "IMPORT FAILED",
        "The content of the clipboard is\n"
        "not a valid Tentai Show map.",
        "OK", GSpritePopup_dismiss, NULL, NULL, NULL));
    SDL_free (desc);
    return 1;
  }

  spr->copy_from_clipboard.spr = spr;
  spr->copy_from_clipboard.new_desc = desc;
  if (GSpriteBoard_is_empty (spr->board)) {
    GSpriteEditor_copy_from_clipboard_yes (&spr->copy_from_clipboard);
    return 1;
  }

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "IMPORT",
      "Are you sure?\n"
      "The current map will be replaced\n"
      "by the content of the clipboard.",
      "YES", GSpriteEditor_copy_from_clipboard_yes, "NO", GSpriteEditor_copy_from_clipboard_no, &spr->copy_from_clipboard));

  return 1;
}

static int GSpriteEditor_copy_to_clipboard (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  char *desc;
  desc = GSpriteBoard_save (spr->board, 0);
  SDL_Log ("Editor:Copy to clipboard %s", desc);
  SDL_SetClipboardText (desc);
  SDL_free (desc);

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "EXPORTED",
      "The current map has been copied\n"
      "to the clipboard.",
      "OK", GSpritePopup_dismiss, NULL, NULL, NULL));
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
  spr->size_change_plus.spr = spr;
  spr->size_change_plus.delta = 1;
  spr->size_change_minus.spr = spr;
  spr->size_change_minus.delta = -1;
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
