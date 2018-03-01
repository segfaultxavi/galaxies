#ifndef __GSPRITE_IMAGE_H__
#define __GSPRITE_IMAGE_H__

#include <SDL.h>
#include "GSprite.h"
#include "GResources.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpriteImage GSpriteImage;

typedef enum _GSpriteImageFitMode {
  GSPRITE_IMAGE_FIT_MODE_STRETCH,
  GSPRITE_IMAGE_FIT_MODE_INSIDE,
  GSPRITE_IMAGE_FIT_MODE_OUTSIDE
} GSpriteImageFitMode;

GSprite *GSpriteImage_new (GResources *res, int x, int y, int w, int h, const char *filename, GSpriteImageFitMode fit_mode);

#ifdef __cplusplus
}
#endif

#endif
