#include <SDL.h>
#include "GGame.h"
#include "GSpriteMainMenu.h"
#include "GSpriteLabel.h"
#include "GSpriteButton.h"
#include "GSpriteCredits.h"

typedef struct _GSpriteMainMenu {
  GSprite base;
  SDL_Renderer *renderer;
  TTF_Font *font_big;
  TTF_Font *font_med;
} GSpriteMainMenu;

int GSpriteMainMenu_play (void *userdata) {
  GSpriteMainMenu *spr = userdata;
  SDL_Log ("Play");
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
  GSprite_add_child (spr->base.parent, GSpriteCredits_new (spr->renderer, spr->font_big, spr->font_med, (GSprite *)spr));
  return 0;
}

GSprite *GSpriteMainMenu_new (SDL_Renderer *renderer, TTF_Font *font_big, TTF_Font *font_med) {
  GSpriteMainMenu *spr = (GSpriteMainMenu *)GSprite_new (sizeof (GSpriteMainMenu),
      NULL, NULL, NULL, NULL);
  spr->base.w = spr->base.h = -1;
  spr->renderer = renderer;
  spr->font_big = font_big;
  spr->font_med = font_med;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (ggame_width / 2, 0, GLABEL_JUSTIFY_CENTER, GLABEL_JUSTIFY_BEGIN, renderer, font_big, 0xFFFFFF00, "galaxies"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (ggame_width / 4, 100, ggame_width / 2, 50, renderer, font_med, 0xFF0000FF, "play", GSpriteMainMenu_play, spr));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (ggame_width / 4, 150, ggame_width / 2, 50, renderer, font_med, 0xFF0000FF, "editor", GSpriteMainMenu_editor, spr));
  GSprite_add_child ((GSprite *)spr,
    GSpriteButton_new (ggame_width / 4, 200, ggame_width / 2, 50, renderer, font_med, 0xFF0000FF, "credits", GSpriteMainMenu_credits, spr));
  return (GSprite *)spr;
}
