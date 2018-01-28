#include <SDL.h>
#include <SDL_mixer.h>
#include "GResources.h"
#include "GPrefs.h"
#include "GAudio.h"
#include "GGame.h"

struct _GAudio {
  GResources *res;
};

void GAudio_free (GAudio *audio) {
  if (!audio) return;

  Mix_FreeChunk ((Mix_Chunk *)audio->res->wav_ping);
  Mix_FreeChunk ((Mix_Chunk *)audio->res->wav_pong);
  Mix_FreeChunk ((Mix_Chunk *)audio->res->wav_woosh);
  Mix_FreeChunk ((Mix_Chunk *)audio->res->ogg_applause);
  if (audio->res->ogg_music)
    Mix_FreeMusic ((Mix_Music *)audio->res->ogg_music);
  Mix_Quit ();
  Mix_CloseAudio ();
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
  if (Mix_Init (MIX_INIT_OGG) != MIX_INIT_OGG) {
    SDL_Log ("Mix_Init: %s (continuing)", Mix_GetError ());
  }

  rwops = GGame_openAsset (res, "ping.wav");
  res->wav_ping = (GAudioSample *)Mix_LoadWAV_RW (rwops, 1);
  rwops = GGame_openAsset (res, "pong.wav");
  res->wav_pong = (GAudioSample *)Mix_LoadWAV_RW (rwops, 1);
  rwops = GGame_openAsset (res, "woosh.wav");
  res->wav_woosh = (GAudioSample *)Mix_LoadWAV_RW (rwops, 1);
  rwops = GGame_openAsset (res, "applause.ogg");
  res->ogg_applause = (GAudioSample *)Mix_LoadWAV_RW (rwops, 1);
  if (!res->wav_ping || !res->wav_pong || !res->wav_woosh || !res->ogg_applause) {
    SDL_Log ("Mix_LoadWAV_RW: %s", Mix_GetError ());
    goto error;
  }
  rwops = GGame_openAsset (res, "music.ogg");
  res->ogg_music = (GAudioMusic *)Mix_LoadMUS_RW (rwops, 1);
  if (!res->ogg_music) {
    SDL_Log ("Mix_LoadMUS_RW: %s (continuing)", Mix_GetError ());
  }

  GAudio_set_audio_volume (audio, (float)res->preferences.audio);
  GAudio_set_music_volume (audio, (float)res->preferences.music);

  return audio;

error:
  GAudio_free (audio);
  return NULL;
}

void GAudio_play_sample (GAudio *audio, GAudioSample *sample) {
  if (!audio || !sample) return;
  if (audio->res->preferences.audio == 0) return;

  if (Mix_PlayChannel (-1, (Mix_Chunk *)sample, 0) == -1) {
    SDL_Log ("Mix_PlayChannel: %s", Mix_GetError ());
  }
}

void GAudio_play_music (GAudio *audio, GAudioMusic *music) {
  if (!audio || !music) return;

  if (Mix_PlayMusic ((Mix_Music *)music, -1) == -1) {
    SDL_Log ("Mix_PlayMusic: %s", Mix_GetError ());
  }
}

void GAudio_set_audio_volume (GAudio *audio, float vol) {
  if (!audio) return;
  Mix_Volume (-1, (int)(128 * vol));
}

void GAudio_set_music_volume (GAudio *audio, float vol) {
  if (!audio) return;
  Mix_VolumeMusic ((int)(128 * vol));
}
