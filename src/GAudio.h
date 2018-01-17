#ifndef __GAUDIO__
#define __GAUDIO__

#include <SDL.h>

typedef struct _GAudio GAudio;
typedef struct _GAudioSample GAudioSample;

#include "GResources.h"

GAudio *GAudio_new (GResources *res);
void GAudio_free (GAudio *audio);
void GAudio_play (GAudio *audio, GAudioSample *sample);

#endif