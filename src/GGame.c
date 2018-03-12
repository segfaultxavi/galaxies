#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "GGame.h"
#include "GAudio.h"
#include "GResources.h"
#include "GSprite.h"
#include "GSpriteMainMenu.h"
#include "GSpriteNull.h"

struct _GGame {
  SDL_Window *sdl_window;
  GResources resources;
};

SDL_RWops *GGame_openAsset (GResources *res, const char *name) {
  char full_path[256];
  SDL_RWops *rwops;

  if (res->data_path)
    SDL_snprintf (full_path, sizeof (full_path), "%s%s", res->data_path, name);
  else
    SDL_snprintf (full_path, sizeof (full_path), "%s", name);
  rwops = SDL_RWFromFile (full_path, "rb");
  if (!rwops) {
    SDL_Log ("SDL_RWFromFile: %s", SDL_GetError ());
  }
  return rwops;
}

GGame *GGame_new () {
  GGame *game = SDL_malloc (sizeof (GGame));
  GResources *res = &game->resources;
  GSprite *main_menu;
  SDL_DisplayMode sdpm;
  SDL_RWops *font_rwops;
  SDL_memset (game, 0, sizeof (GGame));

  res->data_path = SDL_GetBasePath ();
  if (res->data_path) {
    char assets_path[256];
    SDL_snprintf (assets_path, sizeof (assets_path), "%s%s", res->data_path, "assets/");
    SDL_free (res->data_path);
    res->data_path = SDL_strdup (assets_path);
  }
  SDL_Log ("Data folder: %s", res->data_path);

  // Window
  SDL_GetCurrentDisplayMode (0, &sdpm);
  res->game_width = sdpm.w;
  res->game_height = sdpm.h;
  SDL_Log ("Screen size is %dx%d", res->game_width, res->game_height);
  SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8); // Workaround for Samsung Galaxy S3
  SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
  if (SDL_CreateWindowAndRenderer (res->game_width, res->game_height, SDL_WINDOW_SHOWN, &game->sdl_window, &res->sdl_renderer) != 0) {
    SDL_Log ("SDL_CreateWindowAndRenderer: %s", SDL_GetError ());
    goto error;
  }
  SDL_SetWindowTitle (game->sdl_window, "Tentai Show");
  SDL_SetRenderDrawBlendMode (res->sdl_renderer, SDL_BLENDMODE_BLEND);

  // Images
  if ((IMG_Init (IMG_INIT_JPG) & IMG_INIT_JPG) == 0) {
    SDL_Log ("IMG_Init: %s", IMG_GetError ());
  }

  // Fonts
  font_rwops = GGame_openAsset (res, "spincycle.otf");
  res->font_big = TTF_OpenFontRW (font_rwops, 1, res->game_height / 5);
  font_rwops = GGame_openAsset (res, "spincycle.otf");
  res->font_med = TTF_OpenFontRW (font_rwops, 1, res->game_height / 10);
  font_rwops = GGame_openAsset (res, "spincycle.otf");
  res->font_small = TTF_OpenFontRW (font_rwops, 1, res->game_height / 20);
  font_rwops = GGame_openAsset (res, "comfortaa.ttf");
  res->font_text = TTF_OpenFontRW (font_rwops, 1, res->game_height / 20);
  font_rwops = GGame_openAsset (res, "telegrama.ttf");
  res->font_mono = TTF_OpenFontRW (font_rwops, 1, res->game_height / 30);
  font_rwops = GGame_openAsset (res, "webhostinghub.ttf");
  res->font_icons_med = TTF_OpenFontRW (font_rwops, 1, res->game_height / 10);
  font_rwops = GGame_openAsset (res, "webhostinghub.ttf");
  res->font_icons_small = TTF_OpenFontRW (font_rwops, 1, res->game_height / 20);
  if (!res->font_big || !res->font_med || !res->font_small || !res->font_text || !res->font_mono || !res->font_icons_med || !res->font_icons_small) {
    SDL_Log ("TTF_OpenFontRW: %s", SDL_GetError ());
    goto error;
  }

  // Load preferences
  GPrefs_load (&res->preferences);

  // Audio files
  res->audio = GAudio_new (res);
  if (!res->audio)
    SDL_Log ("Audio not available");

  // Cursors
  res->pointer_cur = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_ARROW);
  res->hand_cur = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_HAND);
  SDL_SetCursor(res->pointer_cur);

  // Content
  res->root = GSpriteNull_new (res, 0, 0);
  main_menu = GSpriteMainMenu_new (res);
  if (!main_menu) goto error;
  GSprite_add_child (res->root, main_menu);

  return game;

error:
  GGame_free (game);
  return NULL;
}

void GGame_free (GGame *game) {
  if (!game) return;
  GAudio_free (game->resources.audio);
  GSprite_free (game->resources.root);
  TTF_CloseFont (game->resources.font_big);
  TTF_CloseFont (game->resources.font_med);
  TTF_CloseFont (game->resources.font_small);
  TTF_CloseFont (game->resources.font_text);
  TTF_CloseFont (game->resources.font_mono);
  TTF_CloseFont (game->resources.font_icons_med);
  TTF_CloseFont (game->resources.font_icons_small);
  SDL_FreeCursor (game->resources.pointer_cur);
  SDL_FreeCursor (game->resources.hand_cur);
  GPrefs_free (&game->resources.preferences);
  SDL_free (game->resources.data_path);
  SDL_DestroyRenderer (game->resources.sdl_renderer);
  SDL_DestroyWindow (game->sdl_window);

  IMG_Quit ();

  SDL_free (game);
}

// TODO: This might run from a thread different than the main one,
// might need to add guards.
int GGame_event_filter (void *userdata, SDL_Event *event) {
  GGame *game = userdata;
  switch (event->type) {
    case SDL_APP_TERMINATING:
      SDL_Log ("TERMINATING");
      GPrefs_save (&game->resources.preferences);
      break;
    case SDL_APP_WILLENTERBACKGROUND:
      SDL_Log ("WILLENTERBACKGROUND");
      break;
    case SDL_APP_WILLENTERFOREGROUND:
      SDL_Log ("WILLENTERFOREGROUND");
      break;
    default:
      break;
  }
  return 1;
}

void GGame_run (GGame *game) {
  int quit = 0;
  GSprite *focus = NULL;
  Uint32 click_timestamp = 0;
  Uint32 last_frame_timestamp = 0;

  // Some events must be handled as fast as possible
  SDL_SetEventFilter (GGame_event_filter, game);

  GAudio_play_music (game->resources.audio, game->resources.ogg_music);

  while (!quit) {
    SDL_Event event;
    GEvent gevent;
    SDL_WaitEventTimeout (NULL, 40);
    while (SDL_PollEvent (&event)) {
      SDL_memset (&gevent, 0, sizeof (gevent));
      gevent.type = GEVENT_TYPE_NONE;
      switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.timestamp - click_timestamp < 250) // ms
          // Manual detection of double-click for Android
          gevent.type = GEVENT_TYPE_SPRITE_ACTIVATE_SECONDARY;
        else
          gevent.type = GEVENT_TYPE_SPRITE_ACTIVATE;
        gevent.x = event.button.x;
        gevent.y = event.button.y;
        click_timestamp = event.button.timestamp;
        break;
      case SDL_MOUSEMOTION:
        gevent.type = GEVENT_TYPE_MOVE;
        gevent.x = event.motion.x;
        gevent.y = event.motion.y;
        break;
      case SDL_KEYDOWN:
        gevent.type = GEVENT_TYPE_KEY;
        gevent.x = gevent.y = GEVENT_POSITION_NONE;
        gevent.keycode = event.key.keysym.sym;
        break;
      case SDL_USEREVENT:
        // Galaxies events carried through SDL events
        gevent.type = event.user.code;
        gevent.target = event.user.data1;
        break;
      default:
        break;
      }
      if (gevent.type != GEVENT_TYPE_NONE) {
        int spr_destroyed = 0;
        GSprite *receiver = NULL;
        if (gevent.target)
          GSprite_event (gevent.target, &gevent, &spr_destroyed);
        else
          receiver = GSprite_hierarchical_event (game->resources.root, &gevent, &spr_destroyed);
        if (gevent.type < GEVENT_TYPE_FOCUS_MARKER) {
          if (gevent.x > GEVENT_POSITION_NONE && focus && receiver != focus) {
            GEvent focus_out_event = { GEVENT_TYPE_SPRITE_OUT, GEVENT_POSITION_NONE, GEVENT_POSITION_NONE };
            GSprite_event (focus, &focus_out_event, NULL);
            focus = NULL;
          }
          if (receiver) {
            if (!spr_destroyed && receiver != focus) {
              GEvent focus_in_event = { GEVENT_TYPE_SPRITE_IN, GEVENT_POSITION_NONE, GEVENT_POSITION_NONE };
              GSprite_event (receiver, &focus_in_event, NULL);
              focus = receiver;
            }
          }
          if (spr_destroyed) {
            focus = NULL;
          }
          SDL_SetCursor (focus != NULL ? game->resources.hand_cur : game->resources.pointer_cur);
        }
      }
    }

    game->resources.game_time = SDL_GetTicks ();
    game->resources.frame_time = game->resources.game_time - last_frame_timestamp;
    last_frame_timestamp = game->resources.game_time;

    SDL_SetRenderDrawColor (game->resources.sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear (game->resources.sdl_renderer);
    GSprite_render (game->resources.root, 0, 0);
    SDL_RenderPresent (game->resources.sdl_renderer);
  }
  GPrefs_save (&game->resources.preferences);
}
