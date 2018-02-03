#include <SDL.h>
#include "GGame.h"
#include "GSpriteLevelSelect.h"
#include "GSpriteGalaxies.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteBoard.h"
#include "GIcons.h"
#include <stdio.h>

#define GLEVEL_BUTTONS_PER_LINE 6

typedef struct _GSpriteLevelSelectButtonData {
  GSpriteLevelSelect *level_spr;
  GSpriteButton *button_spr;
  int level;
} GSpriteLevelSelectButtonData;

struct _GSpriteLevelSelect {
  GSprite base;
  GSprite *main_menu;
  GSpriteLevelSelectButtonData *buttons;
};

static const char *initial_level_descriptions[] = {
  "1dddccbade",
  "1ffeeedcdghg",
  "1gggfdcceahgjjcj",
  "1hhmeebfakekgckbkimgmmhmikcm",
  "1hhkggkakeljilceecafaldm",
  "1hhkhgcgillgkecmajabjafc",
  "1hhnggkdiemfcickagjkhmiiecgacc",
  "1iigehkhcgmiooaa",
  "1iioeekgkbmeiaoeilmkfnboakmoceag",
  "1jjmiiggcckahmnikkqeammeiqon",
  "1jjliidnddaindnngqlqiaqgql",
  "1kkurcoeflakdreoiscksfdejceclakomgqmnpdaqrss",
  "1llpkkmmeocrnsqpupskrdqhjcefiimgae",
  "1llpkkfmnirispqqasdulskcucqakadaru",
  "1mmxhhphpphpiedjalcmeaedaescwdugusvwmupwkubtawguew",
  "1mmrgkbdeicmnpbterptowuqwoqmpcsfmajqkt"
};

static Uint32 GSpriteLevelSelect_get_button_color (GSpriteLevelSelectButtonData *button) {
  GSpriteLevelSelect *spr = button->level_spr;
  Uint32 color = 0;
  switch (spr->base.res->preferences.level_status[button->level]) {
    case GSPRITE_LEVEL_SELECT_LEVEL_STATUS_UNTRIED:
      color = 0xFFC0C0C0;
      break;
    case GSPRITE_LEVEL_SELECT_LEVEL_STATUS_IN_PROGRESS:
      color = 0xFFC0C000;
      break;
    case GSPRITE_LEVEL_SELECT_LEVEL_STATUS_DONE:
      color = 0xFF00C000;
      break;
    default:
      break;
  }
  return color;
}

void GSpriteLevelSelect_update_level_status (void *userdata, GSpriteLevelSelectLevelStatus status, char *desc) {
  GSpriteLevelSelectButtonData *button = userdata;
  GSpriteLevelSelect *spr = button->level_spr;
  GPrefs *prefs = &spr->base.res->preferences;
  prefs->level_status[button->level] = status;
  if (desc) {
    if (prefs->level_desc[button->level]) {
      SDL_free (prefs->level_desc[button->level]);
    }
    prefs->level_desc[button->level] = desc;
  }
  GSpriteButton_set_color (button->button_spr, GSpriteLevelSelect_get_button_color (button));
}

static int GSpriteLevelSelect_back (void *userdata, int *destroyed) {
  GSpriteLevelSelect *spr = userdata;
  GSprite *main_menu = spr->main_menu;
  SDL_Log ("Back");
  GSprite_free ((GSprite *)spr);
  main_menu->visible = 1;
  if (destroyed) *destroyed = 1;
  return 1;
}

static int GSpriteLevelSelect_selection (void *userdata, int *destroyed) {
  GSpriteLevelSelectButtonData *button = userdata;
  GPrefs *prefs = &button->level_spr->base.res->preferences;
  const char *desc;
  SDL_Log ("Start level %d", button->level);
  button->level_spr->base.visible = 0;
  desc = prefs->level_desc[button->level];
  if (!desc)
    desc = initial_level_descriptions[button->level];
  GSprite_add_child (button->level_spr->base.parent, GSpriteGalaxies_new (button->level_spr->base.res, (GSprite *)button->level_spr, button->level, desc, button));
  return 1;
}

static int GSpriteLevelSelect_event (GSpriteLevelSelect *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        ret = GSpriteLevelSelect_back (spr, destroyed);
      }
      break;
    default:
      break;
  }
  return ret;
}

static void GSpriteLevelSelect_free (GSpriteLevelSelect *spr) {
  SDL_free (spr->buttons);
}

GSprite *GSpriteLevelSelect_new (GResources *res, GSprite *main_menu) {
  GSpriteLevelSelect *spr = (GSpriteLevelSelect *)GSprite_new (res, sizeof (GSpriteLevelSelect),
      NULL, (GSpriteEvent)GSpriteLevelSelect_event, NULL, (GSpriteFree)GSpriteLevelSelect_free);
  int l;
  int num_levels = sizeof (initial_level_descriptions) / sizeof (initial_level_descriptions[0]);
  int line = res->game_height / 8;
  int bstride = res->game_width / GLEVEL_BUTTONS_PER_LINE;
  int bmargin = bstride / 10;
  int bsize = bstride - bmargin;
  spr->base.w = spr->base.h = -1;
  spr->main_menu = main_menu;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, res->game_width / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_med,
      0xFF000000, 0xFFFFFFFF, "Level Selection"));

  if (res->preferences.num_levels < num_levels) {
    // Initialize preferences
    for (l = res->preferences.num_levels; l < num_levels; l++) {
      res->preferences.level_status[l] = GSPRITE_LEVEL_SELECT_LEVEL_STATUS_UNTRIED;
    }
    res->preferences.num_levels = num_levels;
  }

  // Check if level descriptions have changed
  for (l = 0; l < num_levels; l++) {
    if (res->preferences.level_desc[l] &&
        SDL_strncmp (res->preferences.level_desc[l], initial_level_descriptions[l], SDL_strlen (initial_level_descriptions[l])) != 0) {
      SDL_free (res->preferences.level_desc[l]);
      res->preferences.level_desc[l] = NULL;
      res->preferences.level_status[l] = GSPRITE_LEVEL_SELECT_LEVEL_STATUS_UNTRIED;
    }
  }

  spr->buttons = SDL_malloc (sizeof (GSpriteLevelSelectButtonData) * num_levels);

  for (l = 0; l < num_levels; l++) {
    int x = l % GLEVEL_BUTTONS_PER_LINE;
    int y = l / GLEVEL_BUTTONS_PER_LINE;
    char text[4];
    int size;
    sprintf (text, "%d", l + 1);
    spr->buttons[l].level_spr = spr;
    spr->buttons[l].level = l;
    spr->buttons[l].button_spr = (GSpriteButton *)
      GSpriteButton_new (res, x * bstride + bmargin / 2, 1 * line + y * bstride, bsize, bsize, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN,
        res->font_med, GSpriteLevelSelect_get_button_color (&spr->buttons[l]), 0xFF000000, text, GSpriteLevelSelect_selection, &spr->buttons[l]);
    GSprite_add_child ((GSprite *)spr, (GSprite *)spr->buttons[l].button_spr);

    GSpriteBoard_size_from_desc (initial_level_descriptions[l], &size, NULL);
    text[1] = '\0';
    if (size <= 7) text[0] = 'S';
    else if (size <= 10) text[0] = 'M';
    else if (size <= 15) text[0] = 'L';
    else text[0] = 'X';
    GSprite_add_child ((GSprite *)spr->buttons[l].button_spr,
        GSpriteLabel_new (res, bsize, bsize, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_END, res->font_text, 0xFF000000, 0xFF808080, text));
  }
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new_with_icon (res, res->game_width, res->game_height, -1, -1, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_END,
      res->font_med, 0xFFFFFFFF, 0xFF000000, "Back", GSpriteLevelSelect_back, spr, res->font_icons_med, GICON_BACK));
  return (GSprite *)spr;
}

void GSpriteLevelSelect_play_next_level (void *userdata) {
  GSpriteLevelSelectButtonData *button = userdata;
  GSpriteLevelSelect *spr = button->level_spr;
  int num_levels = sizeof (initial_level_descriptions) / sizeof (initial_level_descriptions[0]);
  int level = button->level;
  if (level < num_levels) level++;

  GSpriteLevelSelect_selection (&spr->buttons[level], NULL);
}