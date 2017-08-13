#include <SDL.h>
#include "GGame.h"
#include "GSprite.h"
#include "GSpriteTestBox.h"

struct _GGame {
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
  GSprite *root;
};

GGame *GGame_new () {
  GGame *game = malloc (sizeof (GGame));
  if (SDL_CreateWindowAndRenderer (500, 500, SDL_WINDOW_SHOWN, &game->sdl_window, &game->sdl_renderer) != 0) {
    SDL_Log ("SDL_CreateWindowAndRenderer: %s", SDL_GetError ());
    GGame_free (game);
    game = NULL;
  }
  SDL_SetWindowTitle (game->sdl_window, "Galaxies");
  game->root = GSpriteTestBox_new (100, 100, 100, 100);
  return game;
}

void GGame_free (GGame *game) {
  if (!game) return;
  free (game);
}

void GGame_run (GGame *game) {
  int quit = 0;

  while (!quit) {
    SDL_Event event;
    GEvent gevent;
    SDL_WaitEventTimeout (NULL, 40);
    while (SDL_PollEvent (&event)) {
      gevent.type = event.type;
      switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;
      case SDL_MOUSEBUTTONDOWN:
        gevent.x = event.button.x;
        gevent.y = event.button.y;
        GSprite_action (game->root, &gevent);
        break;
      case SDL_MOUSEMOTION:
        gevent.x = event.motion.x;
        gevent.y = event.motion.y;
        GSprite_action (game->root, &gevent);
        break;
      default:
        break;
      }
    }

    SDL_SetRenderDrawColor (game->sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear (game->sdl_renderer);
    GSprite_render (game->root, game->sdl_renderer);
    SDL_RenderPresent (game->sdl_renderer);
  }
}
