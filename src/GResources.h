#ifndef __GRESOURCES__
#define __GRESOURCES__

#include <SDL.h>
#include <SDL_ttf.h>

typedef struct _GResources GResources;

#include "GSprite.h"

struct _GResources {
  SDL_Renderer *sdl_renderer;
  TTF_Font *font_big, *font_med, *font_small;
  GSprite *root;
  int game_width;
  int game_height;
};

#endif