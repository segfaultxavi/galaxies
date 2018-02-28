#ifndef __GSPRITE_IMAGE_H__
#define __GSPRITE_IMAGE_H__

#include <SDL.h>
#include "GSprite.h"
#include "GResources.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpriteImage GSpriteImage;

GSprite *GSpriteImage_new (GResources *res, int x, int y, int w, int h, const char *filename);

#ifdef __cplusplus
}
#endif

#endif
