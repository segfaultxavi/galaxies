#ifndef __GRESOURCES__
#define __GRESOURCES__

#include <SDL.h>
#include <SDL_ttf.h>

typedef struct _GResources GResources;

#include "GSprite.h"
#include "GPrefs.h"
#include "GAudio.h"

struct _GResources {
  // Owned by GGame
  SDL_Renderer *sdl_renderer;
  TTF_Font *font_title_big, *font_title_med, *font_med, *font_small, *font_text, *font_mono;
  SDL_Cursor *pointer_cur, *hand_cur;
  GSprite *root;
  int game_width;
  int game_height;
  GAudio *audio;
  GAudioSample *wav_ping, *wav_pong, *wav_woosh;
  GAudioSample *mp3_music;

  // Owned by GSpriteBoard
  SDL_Texture *core_texture;
  SDL_Texture *core_highlight_texture;

  GPrefs preferences;
};

#endif