#ifndef __GAUDIO__
#define __GAUDIO__

#include <SDL.h>

typedef struct _GAudio GAudio;
typedef struct _GAudioSample GAudioSample;
typedef struct _GAudioMusic GAudioMusic;

#include "GResources.h"

GAudio *GAudio_new (GResources *res);
void GAudio_free (GAudio *audio);
void GAudio_play_sample (GAudio *audio, GAudioSample *sample);
void GAudio_play_music (GAudio *audio, GAudioMusic *music);
void GAudio_set_audio_volume (GAudio *audio, float vol);
void GAudio_set_music_volume (GAudio *audio, float vol);

#endif