#ifndef __GSPRITE_CORE_H__
#define __GSPRITE_CORE_H__

#include "GSprite.h"
#include "GEvent.h"
#include "GResources.h"

typedef struct _GSpriteCore GSpriteCore;

typedef int (*GSpriteCoreCallback)(int id, GEvent *event, void *userdata);

GSprite *GSpriteCore_new (float x, float y, int id, int radiusX, int radiusY, GSpriteCoreCallback callback, void *userdata, GResources *res);

#endif

