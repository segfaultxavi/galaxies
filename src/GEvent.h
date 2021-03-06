#ifndef __GEVENT_H__
#define __GEVENT_H__

#include "GSprite.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GEVENT_POSITION_NONE -1000000

typedef enum _GEventType {
  GEVENT_TYPE_NONE,
  GEVENT_TYPE_MOVE,
  GEVENT_TYPE_SPRITE_ACTIVATE,
  GEVENT_TYPE_SPRITE_ACTIVATE_SECONDARY,
  GEVENT_TYPE_SPRITE_IN,
  GEVENT_TYPE_SPRITE_OUT,
  GEVENT_TYPE_KEY,
  GEVENT_TYPE_FOCUS_MARKER, // This is not an event, just a marker
  // All events which might change the focus appear above
  GEVENT_TYPE_TIMER
} GEventType;

typedef struct _GEvent {
  GEventType type; // Type of event
  int x, y;        // Coordinates for position-based events
  int keycode;     // SDL_Keycode for keyboard events
  GSprite *target; // For targetted GEvents
} GEvent;

#ifdef __cplusplus
}
#endif

#endif
