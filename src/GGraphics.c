#include <SDL.h>
#include "GGraphics.h"

void GGraphics_free_surface (SDL_Surface *surf) {
  free (surf->pixels);
  SDL_FreeSurface (surf);
}

SDL_Surface *GGraphics_circle (int w, int h, int R1, int R2) {
  unsigned char *data = malloc (w * h * 4);
  Uint32 *colors = (Uint32 *)data;
  SDL_Surface *surf;
  int x, y;
  R1 = SQR (R1);
  R2 = SQR (R2);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      float r = (float)(SQR (x - w / 2) + SQR (y - h / 2));
      if ( r <= R2 && r >= R1)
        colors[y * w + x] = 0xFFFFFFFF;
      else
        colors[y * w + x] = 0x00000000;
    }
  }
  surf = SDL_CreateRGBSurfaceFrom (data, w, h, 32, w * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  return surf;
}
