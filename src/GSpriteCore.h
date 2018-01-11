#ifndef __GSPRITE_CORE_H__
#define __GSPRITE_CORE_H__

#include "GSprite.h"
#include "GEvent.h"
#include "GResources.h"

typedef struct _GSpriteCore GSpriteCore;

typedef int (*GSpriteCoreCallback)(int id, GEvent *event, void *userdata, int *destroyed);

SDL_Texture *GSpriteCore_create_texture (GResources *res, int w, int h);
SDL_Texture *GSpriteCore_create_highlight_texture (GResources *res, int w, int h);

GSprite *GSpriteCore_new (GResources *res, float x, float y, int id, int radiusX, int radiusY, int solid, GSpriteCoreCallback callback, void *board);
int GSpriteCore_get_id (GSpriteCore *spr);
void GSpriteCore_set_id (GSpriteCore *spr, int id);
Uint32 GSpriteCore_get_color (GSpriteCore *spr);
void GSpriteCore_get_corner (GSpriteCore *spr, int *x, int *y);
void GSpriteCore_get_opposite (GSpriteCore *spr, int x, int y, int *x2, int *y2);
GSpriteCore *GSpriteCore_clone (GSpriteCore *spr);

#endif

