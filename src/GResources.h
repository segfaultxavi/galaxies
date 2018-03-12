#ifndef __GRESOURCES__
#define __GRESOURCES__

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GResources GResources;

#include "GSprite.h"
#include "GPrefs.h"
#include "GAudio.h"

struct _GResources {
  // Owned by GGame
  SDL_Renderer *sdl_renderer;
  char *data_path;
  TTF_Font *font_big, *font_med, *font_small, *font_text, *font_mono;
  TTF_Font *font_icons_med, *font_icons_small;
  SDL_Cursor *pointer_cur, *hand_cur;
  GSprite *root;
  int game_width;
  int game_height;
  Uint32 game_time; // milliseconds since game started
  Uint32 frame_time; // milliseconds since last rendered frame
  GAudio *audio;
  GAudioSample *wav_ping, *wav_pong, *wav_woosh, *ogg_applause;
  GAudioMusic *ogg_music;

  // Owned by GSpriteBoard
  SDL_Texture *core_texture;
  SDL_Texture *core_highlight_texture;

  GPrefs preferences;
};

#ifdef __cplusplus
}
#endif

#endif