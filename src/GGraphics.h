#ifndef __GGRAPHICS_H__
#define __GGRAPHICS_H__

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SQR(x) ((x) * (x))

void GGraphics_free_surface (SDL_Surface *suf);
SDL_Surface *GGraphics_circle (int w, int h, int R1, int R2);
void GGraphics_add_glow (SDL_Surface **surf, int strength, Uint32 color);
void GGraphics_get_content_rect (SDL_Surface *surf, SDL_Rect *content_rect);

#ifdef __cplusplus
}
#endif

#endif
