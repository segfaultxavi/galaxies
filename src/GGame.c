#include <SDL.h>
#include <SDL_ttf.h>
#include "GGame.h"
#include "GSprite.h"
#include "GSpriteMainMenu.h"
#include "GSpriteNull.h"

struct _GGame {
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
  TTF_Font *font_big, *font_med, *font_small;
  SDL_RWops *font_rwops;
  GSprite *root;
};

extern unsigned const char ___BA_TTF[];
extern unsigned int ___BA_TTF_len;

int ggame_width = 640;
int ggame_height = 480;

GGame *GGame_new () {
  GGame *game = malloc (sizeof (GGame));

  // Window
  if (SDL_CreateWindowAndRenderer (ggame_width, ggame_height, SDL_WINDOW_SHOWN, &game->sdl_window, &game->sdl_renderer) != 0) {
    SDL_Log ("SDL_CreateWindowAndRenderer: %s", SDL_GetError ());
    goto error;
  }
  SDL_SetWindowTitle (game->sdl_window, "Galaxies");

  // Font
  game->font_rwops = SDL_RWFromConstMem (___BA_TTF, ___BA_TTF_len);
  game->font_big = TTF_OpenFontRW (game->font_rwops, 0, 64);
  game->font_med = TTF_OpenFontRW (game->font_rwops, 0, 48);
  game->font_small = TTF_OpenFontRW (game->font_rwops, 0, 32);
  if (!game->font_big || !game->font_med || !game->font_small) {
    SDL_Log ("TTF_OpenFontRW: %s", SDL_GetError ());
    goto error;
  }

  // Content
  game->root = GSpriteNull_new (0, 0);
  GSprite_add_child (game->root, GSpriteMainMenu_new (game->sdl_renderer, game->font_big, game->font_med));
  return game;

error:
  GGame_free (game);
  return NULL;
}

void GGame_free (GGame *game) {
  if (!game) return;
  TTF_CloseFont (game->font_big);
  TTF_CloseFont (game->font_med);
  TTF_CloseFont (game->font_small);
  SDL_RWclose (game->font_rwops);
  free (game);
}

void GGame_run (GGame *game) {
  int quit = 0;
  GSprite *focus = NULL;

  while (!quit) {
    SDL_Event event;
    GEvent gevent;
    SDL_WaitEventTimeout (NULL, 40);
    while (SDL_PollEvent (&event)) {
      gevent.type = GEVENT_TYPE_NONE;
      switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;
      case SDL_MOUSEBUTTONDOWN:
        gevent.type = GEVENT_TYPE_SPRITE_ACTIVATE;
        gevent.x = event.button.x;
        gevent.y = event.button.y;
        break;
      case SDL_MOUSEMOTION:
        gevent.type = GEVENT_TYPE_MOVE;
        gevent.x = event.motion.x;
        gevent.y = event.motion.y;
        break;
      default:
        break;
      }
      if (gevent.type != GEVENT_TYPE_NONE) {
        GSprite *topmost = GSprite_topmost_event_receiver (game->root, gevent.x, gevent.y);
        if (topmost != focus) {
          GEvent focus_out_event = { GEVENT_TYPE_SPRITE_OUT, gevent.x, gevent.y };
          GEvent focus_in_event = { GEVENT_TYPE_SPRITE_IN, gevent.x, gevent.y };
          GSprite_event (focus, &focus_out_event);
          GSprite_event (topmost, &focus_in_event);
          focus = topmost;
        }
        GSprite_event (topmost, &gevent);
      }
    }

    SDL_SetRenderDrawColor (game->sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear (game->sdl_renderer);
    GSprite_render (game->root, game->sdl_renderer, 0, 0);
    SDL_RenderPresent (game->sdl_renderer);
  }
}
