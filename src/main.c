#include <SDL.h>
#include <stdio.h>
#include "GGame.h"

int main (int argc, char *argv[]) {
  GGame *game;
  if (SDL_Init (SDL_INIT_VIDEO) != 0) {
    fprintf (stderr, "SDL_Init: %s\n", SDL_GetError());
  }
  game = GGame_new ();
  if (!game) {
    SDL_Quit ();
    return -1;
  }
  SDL_Log ("Game running\n");

  GGame_run (game);
  GGame_free (game);
  SDL_Quit ();

  return 0;
}