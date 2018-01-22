#ifndef __GSPRITE_CREDITS_H__
#define __GSPRITE_CREDITS_H__

#include <SDL.h>
#include "GResources.h"
#include "GSprite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSpriteCredits GSpriteCredits;

GSprite *GSpriteCredits_new (GResources *resources, GSprite *main_menu);

#ifdef __cplusplus
}
#endif

#endif
