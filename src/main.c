#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include "GGame.h"

int main (int argc, char *argv[]) {
  GGame *game;
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    fprintf (stderr, "SDL_Init: %s\n", SDL_GetError());
    return -1;
  }
  if (TTF_Init () == -1) {
    fprintf (stderr, "TTF_Init: %s", TTF_GetError ());
    return -1;
  }
  game = GGame_new ();
  if (!game) {
    SDL_Quit ();
    return -1;
  }
  SDL_Log ("Game running");

  GGame_run (game);

  SDL_Log ("Game quitting");
  GGame_free (game);
  TTF_Quit ();
  SDL_Quit ();

  return 0;
}
