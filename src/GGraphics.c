#include <SDL.h>
#include "GGraphics.h"
#include "string.h"

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

void GGraphics_glow (SDL_Surface *surf, int strength, Uint32 color) {
  int w, h, x, y, s;
  Uint32 *colors;
  unsigned char *alphas, *alphas2;

  SDL_LockSurface (surf);
  w = surf->w;
  h = surf->h;
  colors = (Uint32 *)surf->pixels;
  alphas = (unsigned char *)malloc (w * h);
  alphas2 = (unsigned char *)malloc (w * h);
  memset (alphas, 0, w * h);
  // Copy Alpha values
  for (x = 0; x < w * h; x++) {
    alphas2[x] = (colors[x] >> 24) & 0xFF;
  }
  // Diffuse (Horribly naive approach: filter is separable)
  for (s = 0; s < strength; s++) {
    unsigned char *tmp = alphas2;
    alphas2 = alphas;
    alphas = tmp;
    for (y = 1; y < h - 1; y++) {
      for (x = 1; x < w - 1; x++) {
        int i = y * w + x;
        alphas2[i] = (4 * (int)alphas[i] + alphas[i-1] + alphas[i+1] + alphas[i+w] + alphas[i-w]) / 8;
      }
    }
  }
  if (strength & 1) {
    unsigned char *tmp = alphas2;
    alphas2 = alphas;
    alphas = tmp;
  }
  // Generate output image
  for (x = 0; x < w * h; x++) {
    unsigned char a = (colors[x] >> 24) & 0xFF;
    if (a == 0)
      colors[x] = (color & 0x00FFFFFF) | (alphas[x] << 24);
  }
  free (alphas);
  free (alphas2);

  SDL_UnlockSurface (surf);
}
