#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteMainMenu.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteLevelSelect.h"
#include "GSpriteCredits.h"

struct _GSpriteMainMenu {
  GSprite base;
};

static int GSpriteMainMenu_play (void *userdata, int *destroyed) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Play");
  spr->base.visible = 0;
  GSprite_add_child (spr->base.parent, GSpriteLevelSelect_new (spr->base.res, (GSprite *)spr));
  return 1;
}

static int GSpriteMainMenu_editor (void *userdata, int *destroyed) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Editor");
  return 1;
}

static int GSpriteMainMenu_credits (void *userdata, int *destroyed) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Credits");
  spr->base.visible = 0;
  GSprite_add_child (spr->base.parent, GSpriteCredits_new (spr->base.res, (GSprite *)spr));
  return 1;
}

static int GSpriteMainMenu_event (GSpriteMainMenu *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        SDL_Event event = { SDL_QUIT };
        SDL_PushEvent (&event);
      }
      break;
    default:
      break;
  }
  return ret;
}

GSprite *GSpriteMainMenu_new (GResources *res) {
  int line = res->game_height / 8;
  GSpriteMainMenu *spr = (GSpriteMainMenu *)GSprite_new (res, sizeof (GSpriteMainMenu),
      NULL, (GSpriteEvent)GSpriteMainMenu_event, NULL, NULL);
  spr->base.w = spr->base.h = -1;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, res->game_width / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_title_big,
      0xFF000000, 0xFFFFFFFF, "galaxies"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (res, res->game_width / 2, 3 * line, res->game_width / 2, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res->font_med, 0xFF0000FF, "play", GSpriteMainMenu_play, spr));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (res, res->game_width / 2, 4 * line, res->game_width / 2, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res->font_med, 0xFF0000FF, "editor", GSpriteMainMenu_editor, spr));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (res, res->game_width / 2, 5 * line, res->game_width / 2, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res->font_med, 0xFF0000FF, "credits", GSpriteMainMenu_credits, spr));
  return (GSprite *)spr;
}
