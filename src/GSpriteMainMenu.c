#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteMainMenu.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteLevelSelect.h"
#include "GSpriteEditor.h"
#include "GSpriteCredits.h"
#include "GSpritePopup.h"
#include "GIcons.h"

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
  spr->base.visible = 0;
  GSprite_add_child (spr->base.parent, GSpriteEditor_new (spr->base.res, (GSprite *)spr, spr->base.res->preferences.editor_desc));
  return 1;
}

static int GSpriteMainMenu_credits (void *userdata, int *destroyed) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Credits");
  spr->base.visible = 0;
  GSprite_add_child (spr->base.parent, GSpriteCredits_new (spr->base.res, (GSprite *)spr));
  return 1;
}

static int GSpriteMainMenu_quit (void *userdata, int *destroyed) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Quit");
  SDL_Event event = { SDL_QUIT };
  SDL_PushEvent (&event);
  return 1;
}

static int GSpriteMainMenu_event (GSpriteMainMenu *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        GSpriteMainMenu_quit (spr, destroyed);
      }
      break;
    default:
      break;
  }
  return ret;
}

GSprite *GSpriteMainMenu_new (GResources *res) {
  int line = res->game_height / 6;
  GSpriteMainMenu *spr = (GSpriteMainMenu *)GSprite_new (res, sizeof (GSpriteMainMenu),
      NULL, (GSpriteEvent)GSpriteMainMenu_event, NULL, NULL);
  spr->base.w = spr->base.h = -1;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, res->game_width / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_big,
      0xFF000000, 0xFFFFFFFF, "TENTAI SHOW"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, 3 * res->game_width / 4, 3 * line / 4, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_med,
      0xFFFFFFFF, 0xFFFFFFFF, "ALPHA"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new_with_icon (res, res->game_width / 2, 2 * line, res->game_width / 2, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res->font_med, 0xFFFFFFFF, 0xFF000000, "PLAY", GSpriteMainMenu_play, spr, res->font_icons_med, GICON_PLAY));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new_with_icon (res, res->game_width / 2, 3 * line, res->game_width / 2, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res->font_med, 0xFFFFFFFF, 0xFF000000, "EDITOR", GSpriteMainMenu_editor, spr, res->font_icons_med, GICON_EDITOR));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new_with_icon (res, res->game_width / 2, 4 * line, res->game_width / 2, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res->font_med, 0xFFFFFFFF, 0xFF000000, "CREDITS", GSpriteMainMenu_credits, spr, res->font_icons_med, GICON_CREDITS));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new_with_icon (res, res->game_width / 2, 5 * line, res->game_width / 2, -1, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_CENTER,
      res->font_med, 0xFFFFFFFF, 0xFF000000, "QUIT", GSpriteMainMenu_quit, spr, res->font_icons_med, GICON_QUIT));
  return (GSprite *)spr;
}
