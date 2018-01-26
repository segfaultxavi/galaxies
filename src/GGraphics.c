#include <SDL.h>
#include "GGraphics.h"
#include "string.h"

void GGraphics_free_surface (SDL_Surface *surf) {
  if (surf->userdata)
    SDL_free (surf->userdata);
  SDL_FreeSurface (surf);
}

SDL_Surface *GGraphics_circle (int w, int h, int R1, int R2) {
  unsigned char *data = SDL_malloc (w * h * 4);
  Uint32 *colors = (Uint32 *)data;
  SDL_Surface *surf;
  int x, y;
  R1 = SQR (R1);
  R2 = SQR (R2);
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      float r = (float)(SQR (x - w / 2) + SQR (y - h / 2));
      if (r > R2 && r < (R2 + w)) {
        // Not actual antialias, but close enough
        int a = (int)(255 * (R2 + w - r) / w);
        a = SDL_max (SDL_min (255, a), 0);
        colors[y * w + x] = (a << 24) | 0x00FFFFFF;
      } else if (r <= R2 && r >= R1) {
        colors[y * w + x] = 0xFFFFFFFF;
      } else if (r > (R1 - w) && r < R1) {
        int a = (int)(255 * (r - R1 + w) / w);
        a = SDL_max (SDL_min (255, a), 0);
        colors[y * w + x] = (a << 24) | 0x00FFFFFF;
      } else {
        colors[y * w + x] = 0x00000000;
      }
    }
  }
  surf = SDL_CreateRGBSurfaceFrom (data, w, h, 32, w * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  surf->userdata = data;
  return surf;
}

void GGraphics_add_glow (SDL_Surface **org_surf, int strength, Uint32 color) {
  SDL_Surface *surf;
  int w, h, x, y, s;
  int org_w, org_h;
  unsigned char *colors, *org_colors;
  unsigned char *alphas, *alphas2;
  int gr = (color >> 16) & 0xFF;
  int gg = (color >>  8) & 0xFF;
  int gb = (color >>  0) & 0xFF;
  int margin = 8;

  SDL_LockSurface (*org_surf);
  org_w = (*org_surf)->w;
  org_h = (*org_surf)->h;
  w = org_w + margin * 2;
  h = org_h + margin * 2;
  org_colors = (unsigned char *)(*org_surf)->pixels;

  surf = SDL_CreateRGBSurface (0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
  SDL_LockSurface (surf);
  colors = (unsigned char *)surf->pixels;
  alphas = (unsigned char *)SDL_malloc (w * h);
  alphas2 = (unsigned char *)SDL_malloc (w * h);
  memset (alphas, 0, w * h);
  memset (alphas2, 0, w * h);
  // Copy Alpha values
  for (y = 0; y < org_h; y++) {
    for (x = 0; x < org_w; x++) {
      alphas2[(y + margin) * w + x + margin] = org_colors[(y * org_w + x) * 4 + 3];
      ((Uint32 *)colors)[(y + margin) * w + x + margin] = ((Uint32 *)org_colors)[y * org_w + x];
    }
  }
  SDL_UnlockSurface (*org_surf);
  GGraphics_free_surface (*org_surf);
  // Diffuse (Horribly naive approach)
  for (s = 0; s < strength; s++) {
    for (y = 0; y < h; y++) {
      for (x = 2; x < w - 2; x++) {
        int i = y * w + x;
        alphas[i] = ((int)alphas2[i - 2] + alphas2[i - 1] + alphas2[i] + alphas2[i + 1] + alphas2[i + 2]) / 5;
      }
    }
    for (x = 0; x < w; x++) {
      for (y = 2; y < h - 2; y++) {
        int i = y * w + x;
        alphas2[i] = ((int)alphas[i - 2 * w] + alphas[i - w] + alphas[i] + alphas[i + w] + alphas[i + 2 * w]) / 5;
      }
    }
  }
  // Generate output image
  for (x = 0; x < w * h; x++) {
    int a = colors[x * 4 + 3];
    int r = colors[x * 4 + 2];
    int g = colors[x * 4 + 1];
    int b = colors[x * 4 + 0];
    int a2 = (int)(alphas2[x] * 1.5f);
    a2 = a2 < 255 ? a2 : 255;
    colors[x * 4 + 3] = a + (255 - a) * a2 / 255;
    colors[x * 4 + 2] = (a * r + (255 - a) * gr) / 255;
    colors[x * 4 + 1] = (a * g + (255 - a) * gg) / 255;
    colors[x * 4 + 0] = (a * b + (255 - a) * gb) / 255;
  }
  SDL_free (alphas);
  SDL_free (alphas2);

  SDL_UnlockSurface (surf);
  *org_surf = surf;
}
