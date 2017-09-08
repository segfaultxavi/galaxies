#include <SDL.h>
#include <SDL_ttf.h>
#include "GGame.h"
#include "GResources.h"
#include "GSprite.h"
#include "GSpriteMainMenu.h"
#include "GSpriteNull.h"

struct _GGame {
  SDL_Window *sdl_window;
  SDL_RWops *font_rwops;
  GResources resources;
};

extern unsigned const char ___BA_TTF[];
extern unsigned int ___BA_TTF_len;

GGame *GGame_new () {
  GGame *game = malloc (sizeof (GGame));
  GResources *res = &game->resources;
  SDL_DisplayMode sdpm;

  // Window
  SDL_GetCurrentDisplayMode (0, &sdpm);
  res->game_width = sdpm.w;
  res->game_height = sdpm.h;
  SDL_Log ("Screen size is %dx%d", res->game_width, res->game_height);
  SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8); // Workaround for Samsung Galaxy S3
  SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
  if (SDL_CreateWindowAndRenderer (res->game_width, res->game_height, SDL_WINDOW_SHOWN, &game->sdl_window, &res->sdl_renderer) != 0) {
    SDL_Log ("SDL_CreateWindowAndRenderer: %s", SDL_GetError ());
    goto error;
  }
  SDL_SetWindowTitle (game->sdl_window, "Galaxies");
  SDL_SetRenderDrawBlendMode (res->sdl_renderer, SDL_BLENDMODE_BLEND);

  // Font
  game->font_rwops = SDL_RWFromConstMem (___BA_TTF, ___BA_TTF_len);
  res->font_big = TTF_OpenFontRW (game->font_rwops, 0, res->game_height / 5);
  res->font_med = TTF_OpenFontRW (game->font_rwops, 0, res->game_height / 10);
  res->font_small = TTF_OpenFontRW (game->font_rwops, 0, res->game_height / 20);
  if (!res->font_big || !res->font_med || !res->font_small) {
    SDL_Log ("TTF_OpenFontRW: %s", SDL_GetError ());
    goto error;
  }

  // Cursors
  res->pointer_cur = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_ARROW);
  res->hand_cur = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_HAND);
  SDL_SetCursor(res->pointer_cur);

  // Content
  res->root = GSpriteNull_new (res, 0, 0);
  GSprite_add_child (res->root, GSpriteMainMenu_new (res));
  return game;

error:
  GGame_free (game);
  return NULL;
}

void GGame_free (GGame *game) {
  if (!game) return;
  GSprite_free (game->resources.root);
  TTF_CloseFont (game->resources.font_big);
  TTF_CloseFont (game->resources.font_med);
  TTF_CloseFont (game->resources.font_small);
  SDL_FreeCursor (game->resources.pointer_cur);
  SDL_FreeCursor (game->resources.hand_cur);
  SDL_DestroyRenderer (game->resources.sdl_renderer);
  SDL_DestroyWindow (game->sdl_window);

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
        GSprite *topmost = GSprite_topmost_event_receiver (game->resources.root, gevent.x, gevent.y);
        if (topmost != focus) {
          GEvent focus_out_event = { GEVENT_TYPE_SPRITE_OUT, gevent.x, gevent.y };
          GEvent focus_in_event = { GEVENT_TYPE_SPRITE_IN, gevent.x, gevent.y };
          GSprite_event (focus, &focus_out_event);
          GSprite_event (topmost, &focus_in_event);
          focus = topmost;
        }
        if (GSprite_event (topmost, &gevent) == 1) {
          focus = NULL;
        }
        SDL_SetCursor (topmost != NULL ? game->resources.hand_cur : game->resources.pointer_cur);
      }
    }

    SDL_SetRenderDrawColor (game->resources.sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear (game->resources.sdl_renderer);
    GSprite_render (game->resources.root, 0, 0);
    SDL_RenderPresent (game->resources.sdl_renderer);
  }
}
