#ifndef __GEVENT_H__
#define __GEVENT_H__

#define GEVENT_POSITION_NONE -1000000

typedef enum _GEventType {
  GEVENT_TYPE_NONE,
  GEVENT_TYPE_MOVE,
  GEVENT_TYPE_SPRITE_ACTIVATE,
  GEVENT_TYPE_SPRITE_ACTIVATE_SECONDARY,
  GEVENT_TYPE_SPRITE_IN,
  GEVENT_TYPE_SPRITE_OUT,
  GEVENT_TYPE_KEY,
  GEVENT_TYPE_TIMER
} GEventType;

typedef struct _GEvent {
  GEventType type; // Type of event
  int x, y;        // Coordinates for position-based events
  int keycode;     // SDL_Keycode for keyboard events
  void *userdata;  // For GEvents carried over SDL events
} GEvent;

#endif
