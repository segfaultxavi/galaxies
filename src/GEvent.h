#ifndef __GEVENT_H__
#define __GEVENT_H__

typedef enum _GEventType {
  GEVENT_TYPE_NONE,
  GEVENT_TYPE_MOVE,
  GEVENT_TYPE_SPRITE_ACTIVATE,
  GEVENT_TYPE_SPRITE_IN,
  GEVENT_TYPE_SPRITE_OUT,
  GEVENT_TYPE_KEY
} GEventType;

typedef struct _GEvent {
  GEventType type; // Type of event
  int x, y;        // Coordinates for position-based events
  int keycode;     // SDL_Keycode for keyboard events
} GEvent;

#endif
