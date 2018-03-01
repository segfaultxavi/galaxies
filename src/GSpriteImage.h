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
  GSPRITE_IMAGE_FIT_MODE_STRETCH, // Does not preserve Aspect Ratio
  GSPRITE_IMAGE_FIT_MODE_INSIDE,  // Preserves AR, whole image fits inside dest rectangle leaving blank areas (letterbox)
  GSPRITE_IMAGE_FIT_MODE_OUTSIDE  // Preserves AR, zooms image to fill dest rectangle completely
} GSpriteImageFitMode;

GSprite *GSpriteImage_new (GResources *res, int x, int y, int w, int h, const char *filename, GSpriteImageFitMode fit_mode);

#ifdef __cplusplus
}
#endif

#endif
