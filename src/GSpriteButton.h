#ifndef __GSPRITE_BUTTON_H__
#define __GSPRITE_BUTTON_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GResources.h"

typedef struct _GSpriteButton GSpriteButton;

// Return 1 if the event has been handled
// Set *destroyed to 1 if the sprite has been destroyed
typedef int (* GSpriteButtonCallback)(void *userdata, int *destroyed);

GSprite *GSpriteButton_new (GResources *res, int x, int y, int w, int h, GSpriteJustify justify_hor, GSpriteJustify justify_ver,
    TTF_Font *font, Uint32 button_color, Uint32 label_color, const char *text, GSpriteButtonCallback callback, void *userdata);

void GSpriteButton_set_color (GSpriteButton *spr, Uint32 color);

#endif
