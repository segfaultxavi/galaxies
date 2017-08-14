#include <SDL.h>
#include "GGame.h"
#include "GSpriteMainMenu.h"
#include "GSpriteLabel.h"

typedef struct _GSpriteMainMenu {
  GSprite base;
} GSpriteMainMenu;

GSprite *GSpriteMainMenu_new (SDL_Renderer *renderer, TTF_Font *font_big, TTF_Font *font_med) {
  GSpriteMainMenu *spr = (GSpriteMainMenu *)GSprite_new (sizeof (GSpriteMainMenu),
      NULL, NULL, NULL, NULL);
  spr->base.w = spr->base.h = -1;
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (ggame_width / 2, 0, GLABEL_JUSTIFY_CENTER, GLABEL_JUSTIFY_BEGIN, renderer, font_big, 0xFFFFFF00, "galaxies"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (ggame_width / 2, 100, GLABEL_JUSTIFY_CENTER, GLABEL_JUSTIFY_BEGIN, renderer, font_med, 0xFF0000FF, "play"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (ggame_width / 2, 150, GLABEL_JUSTIFY_CENTER, GLABEL_JUSTIFY_BEGIN, renderer, font_med, 0xFF0000FF, "editor"));
  GSprite_add_child ((GSprite *)spr,
    GSpriteLabel_new (ggame_width / 2, 200, GLABEL_JUSTIFY_CENTER, GLABEL_JUSTIFY_BEGIN, renderer, font_med, 0xFF0000FF, "credits"));
  return (GSprite *)spr;
}
