#ifndef __GANDROID__
#define __GANDROID__

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "GResources.h"

typedef struct _GAndroid GAndroid;

GAndroid *GAndroid_new (GResources *res);
void GAndroid_free (GAndroid *android);

#ifdef __cplusplus
}
#endif

#endif