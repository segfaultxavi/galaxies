#include <SDL.h>
#include <SDL_mixer.h>
#include "GResources.h"
#include "GAudio.h"

struct _GAudio {
  GResources *res;
};

extern unsigned const char ping_wav[];
extern unsigned int ping_wav_len;
extern unsigned const char pong_wav[];
extern unsigned int pong_wav_len;
extern unsigned const char woosh_wav[];
extern unsigned int woosh_wav_len;

void GAudio_free (GAudio *audio) {
  if (!audio) return;

  Mix_FreeChunk ((Mix_Chunk *)audio->res->wav_ping);
  Mix_Quit ();
  SDL_free (audio);
}

GAudio *GAudio_new (GResources *res) {
  GAudio *audio = SDL_malloc (sizeof (GAudio));
  SDL_RWops *rwops;

  audio->res = res;

  if (Mix_OpenAudio (MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
    SDL_Log ("Mix_OpenAudio: %s", Mix_GetError ());
    goto error;
  }
  Mix_Init (0);

  rwops = SDL_RWFromConstMem (ping_wav, ping_wav_len);
  res->wav_ping = (GAudioSample *)Mix_LoadWAV_RW (rwops, 1);
  rwops = SDL_RWFromConstMem (pong_wav, pong_wav_len);
  res->wav_pong = (GAudioSample *)Mix_LoadWAV_RW (rwops, 1);
  rwops = SDL_RWFromConstMem (woosh_wav, woosh_wav_len);
  res->wav_woosh = (GAudioSample *)Mix_LoadWAV_RW (rwops, 1);
  if (!res->wav_ping || !res->wav_pong || !res->wav_woosh) {
    SDL_Log ("Mix_LoadWAV_RW: %s", SDL_GetError ());
    goto error;
  }
  return audio;

error:
  GAudio_free (audio);
  return NULL;
}

void GAudio_play (GAudio *audio, GAudioSample *sample) {
  if (!audio) return;

  Mix_PlayChannel (-1, (Mix_Chunk *)sample, 0);
}
