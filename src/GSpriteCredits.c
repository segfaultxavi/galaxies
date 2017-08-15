#include <SDL.h>
#include "GGame.h"
#include "GSpriteCredits.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"

typedef struct _GSpriteCredits {
  GSprite base;
  GSprite *main_menu;
} GSpriteCredits;

int GSpriteCredits_back (void *userdata) {
  GSpriteCredits *spr = userdata;
  GSprite *main_menu = spr->main_menu;
  SDL_Log ("Back");
  GSprite_free ((GSprite *)spr);
  main_menu->visible = 1;
  return 1;
}

GSprite *GSpriteCredits_new (SDL_Renderer *renderer, TTF_Font *font_big, TTF_Font *font_med, GSprite *main_menu) {
  GSpriteCredits *spr = (GSpriteCredits *)GSprite_new (sizeof (GSpriteCredits),
      NULL, NULL, NULL, NULL);
  spr->base.w = spr->base.h = -1;
  spr->main_menu = main_menu;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (ggame_width / 2, 0, GLABEL_JUSTIFY_CENTER, GLABEL_JUSTIFY_BEGIN, renderer, font_big, 0xFFFFFF00, "galaxies"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (ggame_width - 150, ggame_height - 50, 150, 50, renderer, font_med, 0xFF0000FF, "back", GSpriteCredits_back, spr));
  return (GSprite *)spr;
}
