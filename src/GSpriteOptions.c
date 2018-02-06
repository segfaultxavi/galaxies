#include <SDL.h>
#include "GGame.h"
#include "GSpriteOptions.h"
#include "GPrefs.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GIcons.h"
#include "GAudio.h"

struct _GSpriteOptions {
  GSprite base;
  GSprite *main_menu;
  GSprite *audio_spr;
  GSprite *music_spr;
};

int GSpriteOptions_back (void *userdata, int *destroyed) {
  GSpriteOptions *spr = userdata;
  GSprite *main_menu = spr->main_menu;
  SDL_Log ("Back");
  GSprite_free ((GSprite *)spr);
  main_menu->visible = 1;
  if (destroyed) *destroyed = 1;
  return 1;
}

static int GSpriteOptions_event (GSpriteOptions *spr, GEvent *event, int *destroyed) {
  int ret = 0;
  switch (event->type) {
    case GEVENT_TYPE_KEY:
      if (event->keycode == SDLK_ESCAPE || event->keycode == SDLK_AC_BACK) {
        ret = GSpriteOptions_back (spr, destroyed);
      }
      break;
    default:
      break;
  }
  return ret;
}

static GSprite *GSpriteOptions_new_bool_button (GSprite *spr, int x, int y, const char *name, int value, GSpriteButtonCallback callback, const char *icon) {
  char buff[32];
  int w = spr->res->game_width / 2;
  int h = spr->res->game_height / 6;
  SDL_snprintf (buff, sizeof (buff), "%s: %s", name, value ? "ON" : "OFF");
  return GSpriteButton_new_with_text_and_icon (spr->res, w / 2 + x * w, y * h, 9 * w / 10, h, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN,
    spr->res->font_med, 0xFFFFFFFF, 0xFF000000, buff, callback, spr, spr->res->font_icons_med, icon);
}

static void GSpriteOptions_audio (GSpriteOptions *spr);

static int GSpriteOptions_audio_toggle (void *userdata, int *destroyed) {
  GSpriteOptions *spr = userdata;
  GPrefs *prefs = &((GSprite *)spr)->res->preferences;
  SDL_Log ("Options:Audio");
  if (destroyed) *destroyed = 1;
  prefs->audio = 1 - prefs->audio;
  GSpriteOptions_audio (spr);
  GAudio_set_audio_volume (((GSprite *)spr)->res->audio, (float)prefs->audio);
  return 1;
}

static void GSpriteOptions_audio (GSpriteOptions *spr) {
  GPrefs *prefs = &((GSprite *)spr)->res->preferences;
  if (spr->audio_spr)
    GSprite_free (spr->audio_spr);
  spr->audio_spr = GSpriteOptions_new_bool_button ((GSprite *)spr, 0, 2, "AUDIO", prefs->audio, GSpriteOptions_audio_toggle, GICON_AUDIO);
  GSprite_add_child ((GSprite *)spr, spr->audio_spr);
}

static void GSpriteOptions_music (GSpriteOptions *spr);

static int GSpriteOptions_music_toggle (void *userdata, int *destroyed) {
  GSpriteOptions *spr = userdata;
  GPrefs *prefs = &((GSprite *)spr)->res->preferences;
  SDL_Log ("Options:Music");
  if (destroyed) *destroyed = 1;
  prefs->music = 1 - prefs->music;
  GSpriteOptions_music (spr);
  GAudio_set_music_volume (((GSprite *)spr)->res->audio, (float)prefs->music);
  return 1;
}

static void GSpriteOptions_music (GSpriteOptions *spr) {
  GPrefs *prefs = &((GSprite *)spr)->res->preferences;
  if (spr->music_spr)
    GSprite_free (spr->music_spr);
  spr->music_spr = GSpriteOptions_new_bool_button ((GSprite *)spr, 1, 2, "MUSIC", prefs->music, GSpriteOptions_music_toggle, GICON_MUSIC);
  GSprite_add_child ((GSprite *)spr, spr->music_spr);
}

GSprite *GSpriteOptions_new (GResources *res, GSprite *main_menu) {
  GSpriteOptions *spr = (GSpriteOptions *)GSprite_new (res, sizeof (GSpriteOptions),
      NULL, (GSpriteEvent)GSpriteOptions_event, NULL, NULL);
  int w = res->game_width / 3;
  int h = res->game_height / 6;

  spr->base.w = spr->base.h = -1;
  spr->main_menu = main_menu;

  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, res->game_width / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_big,
      0xFF000000, 0xFFFFFFFF, "TENTAI SHOW"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, 3 * res->game_width / 4, 3 * res->game_height / 30, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_med,
      0xFFFFFFFF, 0xFFFFFFFF, "OPTIONS"));

  GSpriteOptions_audio (spr);
  GSpriteOptions_music (spr);

  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new_with_text_and_icon (res, res->game_width, res->game_height, -1, -1, GSPRITE_JUSTIFY_END, GSPRITE_JUSTIFY_END,
      res->font_med, 0xFFFFFFFF, 0xFF000000, "Back", GSpriteOptions_back, spr, res->font_icons_med, GICON_BACK));
  return (GSprite *)spr;
}
