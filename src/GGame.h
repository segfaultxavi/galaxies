#ifndef __GGAME_H__
#define __GGAME_H__

#include "GResources.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GGame GGame;

GGame *GGame_new ();
void GGame_free (GGame *game);
void GGame_run (GGame *game);

SDL_RWops *GGame_openAsset (GResources *res, const char *name);

#ifdef __cplusplus
}
#endif

#endif