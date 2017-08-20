#ifndef __GJUSTIFY__
#define __GJUSTIFY__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"

typedef enum _GJustify {
  GJUSTIFY_BEGIN,
  GJUSTIFY_CENTER,
  GJUSTIFY_END
} GJustify;

// Changes the position of the sprite depending on its w and h and the requested justification
void GJustify_apply (GSprite *spr, int x, int y, GJustify justify_hor, GJustify justify_ver);

#endif
