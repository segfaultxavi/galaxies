#ifndef __GSPRITE_CORE_H__
#define __GSPRITE_CORE_H__

#include "GSprite.h"
#include "GEvent.h"
#include "GResources.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _GSpriteCoreType {
  GCORE_TYPE_BLOCKER = 3,
  GCORE_TYPE_2_FOLD = 0
} GSpriteCoreType;

typedef struct _GSpriteCore GSpriteCore;

typedef int (*GSpriteCoreCallback)(int id, GEvent *event, void *userdata, int *destroyed);

SDL_Texture *GSpriteCore_create_texture (GResources *res, int w, int h);
SDL_Texture *GSpriteCore_create_highlight_texture (GResources *res, int w, int h);

GSprite *GSpriteCore_new (GResources *res, GSpriteCoreType type, float x, float y, int id,
    int radiusX, int radiusY, GSpriteCoreCallback callback, void *board);
int GSpriteCore_get_id (GSpriteCore *spr);
void GSpriteCore_set_id (GSpriteCore *spr, int id);
Uint32 GSpriteCore_get_color (GSpriteCore *spr);
void GSpriteCore_get_corner (GSpriteCore *spr, int *x, int *y);
void GSpriteCore_get_opposite (GSpriteCore *spr, int x, int y, int *x2, int *y2);
GSpriteCore *GSpriteCore_clone (GSpriteCore *spr);
GSpriteCoreType GSpriteCore_get_type (GSpriteCore *spr);
void GSpriteCore_set_type (GSpriteCore *spr, GSpriteCoreType type);

#ifdef __cplusplus
}
#endif

#endif

