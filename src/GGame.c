#include <SDL.h>
#include <SDL_ttf.h>
#include "GGame.h"
#include "GResources.h"
#include "GSprite.h"
#include "GSpriteMainMenu.h"
#include "GSpriteNull.h"

struct _GGame {
  SDL_Window *sdl_window;
  GResources resources;
};

extern unsigned const char ___BA_TTF[];
extern unsigned int ___BA_TTF_len;
extern unsigned const char spincycle_otf[];
extern unsigned int spincycle_otf_len;
extern unsigned const char days_otf[];
extern unsigned int days_otf_len;

GGame *GGame_new () {
  GGame *game = SDL_malloc (sizeof (GGame));
  GResources *res = &game->resources;
  SDL_DisplayMode sdpm;
  SDL_RWops *font_rwops;
  SDL_memset (game, 0, sizeof (GGame));

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
  SDL_SetWindowTitle (game->sdl_window, "tentai show");
  SDL_SetRenderDrawBlendMode (res->sdl_renderer, SDL_BLENDMODE_BLEND);

  // Fonts
  font_rwops = SDL_RWFromConstMem (___BA_TTF, ___BA_TTF_len);
  res->font_title_big = TTF_OpenFontRW (font_rwops, 1, res->game_height / 5);
  font_rwops = SDL_RWFromConstMem (___BA_TTF, ___BA_TTF_len);
  res->font_title_med = TTF_OpenFontRW (font_rwops, 1, res->game_height / 10);
  font_rwops = SDL_RWFromConstMem (spincycle_otf, spincycle_otf_len);
  res->font_med = TTF_OpenFontRW (font_rwops, 1, res->game_height / 10);
  font_rwops = SDL_RWFromConstMem (spincycle_otf, spincycle_otf_len);
  res->font_small = TTF_OpenFontRW (font_rwops, 1, res->game_height / 20);
  font_rwops = SDL_RWFromConstMem (days_otf, days_otf_len);
  res->font_text = TTF_OpenFontRW (font_rwops, 1, res->game_height / 20);
  if (!res->font_title_big || !res->font_title_med || !res->font_med || !res->font_small || !res->font_text) {
    SDL_Log ("TTF_OpenFontRW: %s", SDL_GetError ());
    goto error;
  }

  // Cursors
  res->pointer_cur = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_ARROW);
  res->hand_cur = SDL_CreateSystemCursor (SDL_SYSTEM_CURSOR_HAND);
  SDL_SetCursor(res->pointer_cur);

  // Content
  res->root = GSpriteNull_new (res, 0, 0);
  GSprite_add_child (res->root, GSpriteMainMenu_new (res));

  // Load preferences
  GPrefs_load (&res->preferences);

  return game;

error:
  GGame_free (game);
  return NULL;
}

void GGame_free (GGame *game) {
  if (!game) return;
  GSprite_free (game->resources.root);
  TTF_CloseFont (game->resources.font_title_big);
  TTF_CloseFont (game->resources.font_title_med);
  TTF_CloseFont (game->resources.font_med);
  TTF_CloseFont (game->resources.font_small);
  TTF_CloseFont (game->resources.font_text);
  SDL_FreeCursor (game->resources.pointer_cur);
  SDL_FreeCursor (game->resources.hand_cur);
  GPrefs_free (&game->resources.preferences);
  SDL_DestroyRenderer (game->resources.sdl_renderer);
  SDL_DestroyWindow (game->sdl_window);

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

  // Some events must be handled as fast as possible
  SDL_SetEventFilter (GGame_event_filter, game);

  while (!quit) {
    SDL_Event event;
    GEvent gevent = { 0 };
    SDL_WaitEventTimeout (NULL, 40);
    while (SDL_PollEvent (&event)) {
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
        gevent.userdata = event.user.data1;
        break;
      default:
        break;
      }
      if (gevent.type != GEVENT_TYPE_NONE) {
        int spr_destroyed = 0;
        GSprite *receiver = GSprite_hierarchical_event (game->resources.root, &gevent, &spr_destroyed);
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

    SDL_SetRenderDrawColor (game->resources.sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear (game->resources.sdl_renderer);
    GSprite_render (game->resources.root, 0, 0);
    SDL_RenderPresent (game->resources.sdl_renderer);
  }
  GPrefs_save (&game->resources.preferences);
}
