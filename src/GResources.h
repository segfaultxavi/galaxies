#ifndef __GRESOURCES__
#define __GRESOURCES__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"

typedef struct _GResources {
  SDL_Renderer *sdl_renderer;
  TTF_Font *font_big, *font_med, *font_small;
  GSprite *root;
  int game_width;
  int game_height;
} GResources;

#endif