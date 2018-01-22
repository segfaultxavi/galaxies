#ifndef __GSPRITE_OPTIONS_H__
#define __GSPRITE_OPTIONS_H__

#include <SDL.h>
#include "GResources.h"
#include "GSprite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpriteOptions GSpriteOptions;

GSprite *GSpriteOptions_new (GResources *resources, GSprite *main_menu);

#ifdef __cplusplus
}
#endif

#endif
