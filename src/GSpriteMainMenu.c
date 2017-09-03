#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteMainMenu.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteGalaxies.h"
#include "GSpriteCredits.h"

typedef struct _GSpriteMainMenu {
  GSprite base;
} GSpriteMainMenu;

int GSpriteMainMenu_play (void *userdata) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Play");
  spr->base.visible = 0;
  GSprite_add_child (spr->base.parent, GSpriteGalaxies_new (spr->base.res, (GSprite *)spr));
  return 0;
}

int GSpriteMainMenu_editor (void *userdata) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Editor");
  return 0;
}

int GSpriteMainMenu_credits (void *userdata) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Credits");
  spr->base.visible = 0;
  GSprite_add_child (spr->base.parent, GSpriteCredits_new (spr->base.res, (GSprite *)spr));
  return 0;
}

GSprite *GSpriteMainMenu_new (GResources *res) {
  int line = res->game_height / 8;
  GSpriteMainMenu *spr = (GSpriteMainMenu *)GSprite_new (res, sizeof (GSpriteMainMenu),
      NULL, NULL, NULL, NULL);
  spr->base.w = spr->base.h = -1;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (res, res->game_width / 2, 0, GSPRITE_JUSTIFY_CENTER, GSPRITE_JUSTIFY_BEGIN, res->font_big, 0xFFFFFF00, "galaxies"));
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
