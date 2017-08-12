#include <SDL.h>
#include "GGame.h"

struct _GGame {
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
};

GGame *GGame_new () {
  GGame *game = malloc (sizeof (GGame));
  if (SDL_CreateWindowAndRenderer (500, 500, SDL_WINDOW_SHOWN, &game->sdl_window, &game->sdl_renderer) != 0) {
    SDL_Log ("SDL_CreateWindow: %s\n", SDL_GetError ());
    GGame_free (game);
    game = NULL;
  }
  SDL_SetWindowTitle (game->sdl_window, "Galaxies");
  return game;
}

void GGame_free (GGame *game) {
  if (!game) return;
  free (game);
}

void GGame_run (GGame *game) {
  int quit = 0;
  SDL_Event event;

  while (!quit) {
    while (SDL_PollEvent (&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;
      default:
        break;
      }
    }
  }
}