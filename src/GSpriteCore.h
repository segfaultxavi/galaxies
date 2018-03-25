#ifndef __GSPRITE_CORE_H__
#define __GSPRITE_CORE_H__

typedef struct _GSpriteCore GSpriteCore;

typedef enum _GSpriteCoreType {
  GCORE_TYPE_2_FOLD = 0,
  GCORE_TYPE_BLOCKER = 3,
  GCORE_NUM_TYPES
} GSpriteCoreType;

#include "GSprite.h"
#include "GEvent.h"
#include "GResources.h"
#include "GSpriteBoard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*GSpriteCoreCallback)(int id, GEvent *event, void *userdata, int *destroyed);

SDL_Texture *GSpriteCore_create_texture (GResources *res, int w, int h, const char *icon_filename);

GSprite *GSpriteCore_new (GResources *res, GSpriteCoreType type, float x, float y, int id, Uint32 color,
    int radiusX, int radiusY, GSpriteCoreCallback callback, void *callback_userdata, GSpriteBoard *board);
int GSpriteCore_get_id (GSpriteCore *spr);
void GSpriteCore_set_id (GSpriteCore *spr, int id);
Uint32 GSpriteCore_get_color (GSpriteCore *spr);
void GSpriteCore_set_color (GSpriteCore *spr, Uint32 color);
void GSpriteCore_get_corner (GSpriteCore *spr, int *x, int *y);
void GSpriteCore_get_opposite (GSpriteCore *spr, int x, int y, int *x2, int *y2);
GSpriteCore *GSpriteCore_clone (GSpriteCore *spr);
GSpriteCoreType GSpriteCore_get_type (GSpriteCore *spr);
void GSpriteCore_set_type (GSpriteCore *spr, GSpriteCoreType type);

#ifdef __cplusplus
}
#endif

#endif

