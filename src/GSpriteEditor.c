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
#include "GSpriteProgress.h"
#include "GSolver.h"
#include "GIcons.h"

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
  GSprite *num_sols_spr;
  GSprite *curr_sol_spr;
  GSprite *size_spr;
  GSpriteCore *core_spr[4];
  GSpriteProgress *progress_spr;
  GSpriteEditorSizeChange size_change_plus;
  GSpriteEditorSizeChange size_change_minus;
  GSpriteEditorCopyFromClipboard copy_from_clipboard;
  int selected_core_type;

  // Menu construction
  GSprite *margin;
  int margin_width;
  int line_height;

  // Solver
  GSolver *solver;
  int num_solutions;
  int current_solution;
  SDL_TimerID solver_timer_id;
  Uint32 solver_start_timestamp;
};

void GSpriteEditor_cores_changed (GSpriteEditor *spr);

static int GSpriteEditor_help (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Help");

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "HELP", GICON_HELP,
      "Click on a core to SELECT it. Click again to UNSELECT it.\n"
      "With no selection, click on an empty spot to CREATE a core.\n"
      "Double-click a core to REMOVE it.\n"
      " \n"
      "Solutions are calculated as you edit your map.\n"
      "GOOD PUZZLES HAVE ONLY ONE SOLUTION.\n"
      "Calculating solutions can take a long time on big maps.\n"
      " \n"
      "Have fun!",
      "OK", GSpritePopup_dismiss, NULL, NULL, NULL, NULL, spr));

  return 1;
}

static void GSpriteEditor_set_size_spr (GSpriteEditor *spr) {
  char text[16];
  if (spr->size_spr)
    GSprite_free (spr->size_spr);
  SDL_snprintf (text, sizeof (text), "SIZE %d", GSpriteBoard_get_map_size_x (spr->board));
  spr->size_spr = GSpriteLabel_new (spr->base.res, spr->margin_width / 2, 4 * spr->line_height,
    GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER, spr->base.res->font_small,
    0xFF000000, 0xFFFFFFFF, text);
  GSprite_add_child (spr->margin, spr->size_spr);
}

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
    GSpritePopup_new (spr->base.res, "RESET", GICON_RESET,
      "Are you sure?\n"
      "This will remove all tile colors\n"
      "and preserve cores.",
      "YES", GSpriteEditor_reset_yes, NULL, "NO", GSpritePopup_dismiss, NULL, spr));
  return 1;
}

static void GSpriteEditor_restart_yes (void *userdata) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Restart:Yes");
  GSprite_free ((GSprite *)spr->board);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (spr->base.res, 1);
  GSpriteBoard_start (spr->board, GEDITOR_DEFAULT_SIZE, GEDITOR_DEFAULT_SIZE, 0, NULL, NULL);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);
  GSpriteEditor_cores_changed (spr);
  GSpriteEditor_set_size_spr (spr);
}

static int GSpriteEditor_restart(void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;
  SDL_Log ("Editor:Restart");

  if (GSpriteBoard_is_empty (spr->board))
    return 1;

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "RESTART", GICON_RESTART,
      "Are you sure?\n"
      "This will remove everything\n"
      "from the map.",
      "YES", GSpriteEditor_restart_yes, NULL, "NO", GSpritePopup_dismiss, NULL, spr));
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
  GSpriteEditor_cores_changed (sc->spr);
  GSpriteEditor_set_size_spr (sc->spr);
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
    GSpritePopup_new (spr->base.res, "SIZE CHANGE", GICON_WARNING,
      "Are you sure?\n"
      "This will remove everything from\n"
      "the map.",
      "YES", GSpriteEditor_size_change_yes, NULL, "NO", GSpritePopup_dismiss, NULL, &spr->size_change_plus));
  return 1;
}

static int GSpriteEditor_size_minus (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;

  if (GSpriteBoard_get_map_size_x (spr->board) == 3)
    return 1;

  if (GSpriteBoard_is_empty (spr->board)) {
    GSpriteEditor_size_change_yes (&spr->size_change_minus);
    return 1;
  }

  GSprite_add_child ((GSprite *)spr,
    GSpritePopup_new (spr->base.res, "SIZE CHANGE", GICON_WARNING,
      "Are you sure?\n"
      "This will remove everything from\n"
      "the map.",
      "YES", GSpriteEditor_size_change_yes, NULL, "NO", GSpritePopup_dismiss, NULL, &spr->size_change_minus));
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
  GSpriteEditor_cores_changed (spr);
  GSpriteEditor_set_size_spr (spr);
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
  if (GSpriteBoard_check_description (desc) == 0) {
    GSprite_add_child ((GSprite *)spr,
      GSpritePopup_new (spr->base.res, "IMPORT FAILED", GICON_IMPORT,
        "The content of the clipboard is\n"
        "not a valid Tentai Show map.",
        "OK", GSpritePopup_dismiss, NULL, NULL, NULL, NULL, NULL));
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
    GSpritePopup_new (spr->base.res, "IMPORT", GICON_IMPORT,
      "Are you sure?\n"
      "The current map will be replaced\n"
      "by the content of the clipboard.",
      "YES", GSpriteEditor_copy_from_clipboard_yes, NULL, "NO", GSpriteEditor_copy_from_clipboard_no, NULL, &spr->copy_from_clipboard));

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
    GSpritePopup_new (spr->base.res, "EXPORTED", GICON_EXPORT,
      "The current map has been copied\n"
      "to the clipboard.",
      "OK", GSpritePopup_dismiss, NULL, NULL, NULL, NULL, NULL));
  return 1;
}

static void GSpriteEditor_free (GSpriteEditor *spr) {
  SDL_RemoveTimer (spr->solver_timer_id);
  if (spr->solver)
    GSolver_free (spr->solver);
}

static void GSpriteEditor_update_solution_labels (GSpriteEditor *spr) {
  char text[16];
  if (spr->num_sols_spr) GSprite_free (spr->num_sols_spr);
  SDL_snprintf (text, sizeof (text), "%d", spr->num_solutions);
  spr->num_sols_spr = GSpriteLabel_new (spr->base.res, spr->margin_width / 2, 8 * spr->line_height, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_END,
    spr->base.res->font_text, 0xFFFFFFFF, 0x00000000, text);
  GSprite_add_child (spr->margin, spr->num_sols_spr);

  if (spr->curr_sol_spr) GSprite_free (spr->curr_sol_spr);
  spr->curr_sol_spr = NULL;
  if (spr->num_solutions > 0) {
    SDL_snprintf (text, sizeof (text), "#%d", spr->current_solution);
    spr->curr_sol_spr = GSpriteLabel_new (spr->base.res, spr->margin_width / 2, 8 * spr->line_height, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN,
      spr->base.res->font_text, 0xFFFFFFFF, 0x00000000, text);
    GSprite_add_child (spr->margin, spr->curr_sol_spr);
  }

  if (spr->solver) {
    float progress = GSolver_get_progress (spr->solver);
    if (progress < 1.f) {
      Uint32 timestamp = SDL_GetTicks ();
      float ellapsed = (timestamp - spr->solver_start_timestamp) / 1000.f;
      if (ellapsed > 5.f && progress > 0.f) {
        char buff[128];
        int eta = (int)((1 - progress) * ellapsed / progress);
        if (eta < 60)
          SDL_snprintf (buff, sizeof (buff), "Time remaining %ds", eta);
        else if (eta < 3600)
          SDL_snprintf (buff, sizeof (buff), "Time remaining %d:%02d", eta / 60, eta % 60);
        else
          SDL_snprintf (buff, sizeof (buff), "Time remaining %d:%02d:%02d", eta / 3600, (eta / 60) % 60, eta % 60);
        GSpriteProgress_set_text (spr->progress_spr, buff);
      } else {
        GSpriteProgress_set_text (spr->progress_spr, "Finding solutions");
      }
      ((GSprite *)spr->progress_spr)->visible = 1;
      GSpriteProgress_set_progress (spr->progress_spr, progress);
    } else {
      ((GSprite *)spr->progress_spr)->visible = 0;
    }
  } else {
    ((GSprite *)spr->progress_spr)->visible = 0;
  }
}

void GSpriteEditor_tiles_changed (GSpriteEditor *spr, char *desc) {
  GPrefs *prefs = &spr->base.res->preferences;
  if (desc) {
    if (prefs->editor_desc) {
      SDL_free (prefs->editor_desc);
    }
    prefs->editor_desc = desc;
  }
}

static int GSpriteEditor_prev_solution (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;

  if (!spr->solver) return 1;
  if (spr->num_solutions == 0) return 1;
  if (spr->current_solution == 0)
    spr->current_solution = spr->num_solutions - 1;
  else
    spr->current_solution--;

  GSpriteBoard_set_tiles (spr->board, GSolver_get_solution (spr->solver, spr->current_solution));
  GSpriteEditor_update_solution_labels (spr);
  GSpriteEditor_tiles_changed (spr, GSpriteBoard_save (spr->board, 1));

  return 1;
}

static int GSpriteEditor_next_solution (void *userdata, int *destroyed) {
  GSpriteEditor *spr = userdata;

  if (!spr->solver) return 1;
  if (spr->num_solutions == 0) return 1;
  if (spr->current_solution == spr->num_solutions - 1)
    spr->current_solution = 0;
  else
    spr->current_solution++;

  GSpriteBoard_set_tiles (spr->board, GSolver_get_solution (spr->solver, spr->current_solution));
  GSpriteEditor_update_solution_labels (spr);
  GSpriteEditor_tiles_changed (spr, GSpriteBoard_save (spr->board, 1));

  return 1;
}

void GSpriteEditor_cores_changed (GSpriteEditor *spr) {
  if (spr->solver)
    GSolver_free (spr->solver);
  spr->solver = GSolver_new (spr->board);
  spr->solver_start_timestamp = SDL_GetTicks ();
  spr->num_solutions = spr->current_solution = 0;
  GSpriteEditor_update_solution_labels (spr);

  GSpriteEditor_tiles_changed (spr, GSpriteBoard_save (spr->board, 1));
}

static int GSpriteEditor_event (GSpriteEditor *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        ret = GSpriteEditor_back (spr, destroyed);
      }
      break;
    case GEVENT_TYPE_TIMER:
      if (event->target == (GSprite *)spr && spr->solver) {
        spr->num_solutions = GSolver_get_num_solutions (spr->solver);
        GSpriteEditor_update_solution_labels (spr);
      }
      break;
    default:
      break;
  }
  return ret;
}

// We don't know on which thread this is running, so push an event that will be processed
// from the main thread, where we can safely use SDL.
Uint32 GSpriteEditor_solver_timer (Uint32 interval, void *param) {
  GSpriteEditor *spr = param;
  SDL_Event event;
  SDL_UserEvent user_event;

  user_event.type = SDL_USEREVENT;
  user_event.code = GEVENT_TYPE_TIMER;
  user_event.data1 = spr;
  user_event.data2 = NULL;
  event.type = SDL_USEREVENT;
  event.user = user_event;
  SDL_PushEvent (&event);

  return interval;
}

void GSpriteEditor_update_core_buttons (GSpriteEditor *spr) {
  int i;

  for (i = 0; i < 4; i++) {
    if (i == spr->selected_core_type)
      GSpriteCore_set_color (spr->core_spr[i], 0xFFC0C0C0);
    else
      GSpriteCore_set_color (spr->core_spr[i], 0xFF404040);
  }
}

int GSpriteEditor_core_button_event (int id, GEvent *event, void *userdata, int *destroyed) {
  int ret = 0;
  GSpriteEditor *spr = userdata;
  switch (event->type) {
    case GEVENT_TYPE_SPRITE_ACTIVATE:
    case GEVENT_TYPE_SPRITE_ACTIVATE_SECONDARY:
      spr->selected_core_type = id;
      GSpriteEditor_update_core_buttons (spr);
      ret = 1;
      break;
    case GEVENT_TYPE_SPRITE_IN:
    case GEVENT_TYPE_MOVE:
      ret = 1;
      break;
  }
  return ret;
}

#define BUTTON(x,y,name, callback, icon) \
  GSprite_add_child (margin, \
    GSpriteButton_new_with_text_and_icon (res, x * mwidth / 4, y * line, mwidth / 2 - 2, line - 2, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER, \
      res->font_small, 0xFFFFFFFF, 0xFF000000, name, callback, spr, res->font_icons_small, icon))

GSprite *GSpriteEditor_new (GResources *res, GSprite *main_menu, const char *desc) {
  int i;
  int line = res->game_height / 10;
  int mwidth = res->game_width - res->game_height;
  GSpriteEditor *spr = (GSpriteEditor *)GSprite_new (res, sizeof (GSpriteEditor),
      NULL, (GSpriteEvent)GSpriteEditor_event, NULL, (GSpriteFree)GSpriteEditor_free);
  GSprite *margin = GSpriteNull_new (res, res->game_height, 0);
  spr->main_menu = main_menu;
  spr->base.w = spr->base.h = -1;
  spr->size_change_plus.spr = spr;
  spr->size_change_plus.delta = 1;
  spr->size_change_minus.spr = spr;
  spr->size_change_minus.delta = -1;
  spr->margin = margin;
  spr->margin_width = mwidth;
  spr->line_height = line;
  GSprite_add_child (margin,
    GSpriteLabel_new (res, mwidth / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_med,
      0xFF000000, 0xFFFFFFFF, "TENTAI SHOW"));
  GSprite_add_child (margin,
    GSpriteLabel_new (res, mwidth / 2, 1 * line, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_med,
      0xFF000000, 0xFFFFFFFF, "EDITOR"));

  BUTTON (1, 3, "RESET", GSpriteEditor_reset, NULL);
  BUTTON (3, 3, "RESTART", GSpriteEditor_restart, NULL);

  GSprite_add_child (margin,
    GSpriteButton_new_with_text_and_icon (res, 0, 4 * line, mwidth / 3, line - 2, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_CENTER, \
      res->font_small, 0xFFFFFFFF, 0xFF000000, NULL, GSpriteEditor_size_minus, spr, res->font_icons_small, GICON_DOWN));
  GSprite_add_child (margin,
    GSpriteButton_new_with_text_and_icon (res, 2 * mwidth / 3, 4 * line, mwidth / 3, line - 2, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_CENTER, \
      res->font_small, 0xFFFFFFFF, 0xFF000000, NULL, GSpriteEditor_size_plus, spr, res->font_icons_small, GICON_UP));

  BUTTON (1, 5, "IMPORT", GSpriteEditor_copy_from_clipboard, NULL);
  BUTTON (3, 5, "EXPORT", GSpriteEditor_copy_to_clipboard, NULL);
  BUTTON (3, 6, "HELP", GSpriteEditor_help, NULL);

  for (i = 0; i < 4; i++) {
    spr->core_spr[i] = (GSpriteCore *)GSpriteCore_new (res, (GSpriteCoreType)i,
        (float)i, 6, i, 0xFF404040, line, line, GSpriteEditor_core_button_event, spr, NULL);
    ((GSprite *)spr->core_spr[i])->x = mwidth / 16 + i * mwidth / 8;
    GSprite_add_child (margin, (GSprite *)spr->core_spr[i]);
  }
  spr->selected_core_type = 0;
  GSpriteEditor_update_core_buttons (spr);

  spr->progress_spr = (GSpriteProgress *)GSpriteProgress_new (res, 0, 7 * line, mwidth, line / 2,
      "Finding solutions", res->font_mono, 0xFFFFFFFF, 0xFFFFFFFF, 0.f);
  GSprite_add_child (margin, (GSprite *)spr->progress_spr);
  ((GSprite *)spr->progress_spr)->visible = 0;

  GSprite_add_child (margin,
    GSpriteButton_new_with_text_and_icon (res, 0, 8 * line, mwidth / 3, line - 2, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_CENTER, \
      res->font_small, 0xFFFFFFFF, 0xFF000000, NULL, GSpriteEditor_prev_solution, spr, res->font_icons_small, GICON_LEFT));
  GSprite_add_child (margin,
    GSpriteButton_new_with_text_and_icon (res, 2 * mwidth / 3, 8 * line, mwidth / 3, line - 2, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_CENTER, \
      res->font_small, 0xFFFFFFFF, 0xFF000000, NULL, GSpriteEditor_next_solution, spr, res->font_icons_small, GICON_RIGHT));

  spr->num_sols_spr = spr->curr_sol_spr = NULL;
  spr->num_solutions = spr->current_solution = 0;
  GSpriteEditor_update_solution_labels (spr);

  GSprite_add_child (margin,
    GSpriteButton_new_with_text_and_icon (res, mwidth / 2, res->game_height, mwidth, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_END,
      res->font_small, 0xFFFFFFFF, 0xFF000000, "Back", GSpriteEditor_back, spr, res->font_icons_small, GICON_BACK));

  GSprite_add_child ((GSprite *)spr, margin);
  spr->board = (GSpriteBoard *)GSpriteBoard_new (res, 1);
  if (!desc)
    GSpriteBoard_start (spr->board, GEDITOR_DEFAULT_SIZE, GEDITOR_DEFAULT_SIZE, 0, NULL, NULL);
  else
    GSpriteBoard_load (spr->board, desc);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->board);

  GSpriteEditor_set_size_spr (spr);

  spr->solver_timer_id = SDL_AddTimer (100, GSpriteEditor_solver_timer, spr);
  return (GSprite *)spr;
}
