#include <SDL.h>
#include "GGame.h"
#include "GSpriteLevelSelect.h"
#include "GSpriteGalaxies.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include <stdio.h>

#define GLEVEL_BUTTONS_PER_LINE 5

typedef struct _GSpriteLevelSelectButtonData {
  GSpriteLevelSelect *spr;
  int level;
} GSpriteLevelSelectButtonData;

struct _GSpriteLevelSelect {
  GSprite base;
  GSprite *main_menu;
  GSpriteLevelSelectButtonData *buttons;
};

static const char *levels[] = {
  "1hheebfakekgckbkimgmmhmikcm",
  "1hhggkakeljilceecafaldm",
  "1hhhgcgillgkecmajabjafc",
  "1mmhhphpphpiedjalcmeaedaescwdugusvwmupwkubtawguew",
  "1hhggkdiemfcickagjkhmiiecgacc"
};

int GSpriteLevelSelect_back (void *userdata, int *destroyed) {
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
  SDL_Log ("Level %d", button->level);
  button->spr->base.visible = 0;
  GSprite_add_child (button->spr->base.parent, GSpriteGalaxies_new (button->spr->base.res, (GSprite *)button->spr, levels[button->level]));
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
  free (spr->buttons);
}

GSprite *GSpriteLevelSelect_new (GResources *res, GSprite *main_menu) {
  GSpriteLevelSelect *spr = (GSpriteLevelSelect *)GSprite_new (res, sizeof (GSpriteLevelSelect),
      NULL, (GSpriteEvent)GSpriteLevelSelect_event, NULL, (GSpriteFree)GSpriteLevelSelect_free);
  int l;
  int num_levels = sizeof (levels) / sizeof (levels[0]);
  int line = res->game_height / 8;
  int bstride = res->game_width / GLEVEL_BUTTONS_PER_LINE;
  int bmargin = bstride / 10;
  int bsize =bstride - bmargin;
  spr->base.w = spr->base.h = -1;
  spr->main_menu = main_menu;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, res->game_width / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_med,
      0xFF000000, 0xFFFFFFFF, "level selection"));

  spr->buttons = malloc (sizeof (GSpriteLevelSelectButtonData) * num_levels);

  for (l = 0; l < num_levels; l++) {
    int x = l % GLEVEL_BUTTONS_PER_LINE;
    int y = l / GLEVEL_BUTTONS_PER_LINE;
    char text[4];
    sprintf (text, "%d", 1 + l);
    spr->buttons[l].spr = spr;
    spr->buttons[l].level = l;
    GSprite_add_child ((GSprite *)spr,
      GSpriteButton_new (res, x * bstride + bmargin / 2, 1 * line + y * bstride, bsize, bsize, GSPRITE_JUSTIFY_BEGIN, GSPRITE_JUSTIFY_BEGIN,
        res->font_med, 0xFF0000FF, text, GSpriteLevelSelect_selection, &spr->buttons[l]));
  }
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (res, res->game_width, res->game_height, -1, -1, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_END,
      res->font_med, 0xFF0000FF, "back", GSpriteLevelSelect_back, spr));
  return (GSprite *)spr;
}
