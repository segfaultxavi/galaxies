#ifndef __GGRAPHICS_H__
#define __GGRAPHICS_H__

#include <SDL.h>

#define SQR(x) ((x) * (x))

void GGraphics_free_surface (SDL_Surface *suf);
SDL_Surface *GGraphics_circle (int w, int h, int R1, int R2);
void GGraphics_glow (SDL_Surface *surf, int strength, Uint32 color);

#endif
