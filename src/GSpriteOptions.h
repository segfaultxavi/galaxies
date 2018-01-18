#ifndef __GSPRITE_OPTIONS_H__
#define __GSPRITE_OPTIONS_H__

#include <SDL.h>
#include "GResources.h"
#include "GSprite.h"

typedef struct _GSpriteOptions GSpriteOptions;

GSprite *GSpriteOptions_new (GResources *resources, GSprite *main_menu);

#endif
