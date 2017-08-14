#ifndef __GSPRITE_H__
#define __GSPRITE_H__

#include <SDL.h>
#include "GEvent.h"

typedef struct _GSprite GSprite;

typedef void (*GSpriteRender)(GSprite *spr, SDL_Renderer *renderer, int offsx, int offsy);
typedef int (*GSpriteEvent)(GSprite *spr, GEvent *event);
typedef int (*GSpriteIsInside)(GSprite *spr, int x, int y);
typedef void (*GSpriteFree)(GSprite *spr);

struct _GSprite {
  // Area
  int x, y;
  int w, h;

  // Our parent
  GSprite *parent;
  // Pointer to a doubly-linked list of our children
  GSprite *children;
  // Doubly-linked list containing our siblings
  GSprite *prev, *next;

  // For subclases
  GSpriteRender render;
  GSpriteEvent event;
  GSpriteIsInside is_inside;
  GSpriteFree free;
};

// Intended for subclasses, since you have to provide subclass struct size and methods
GSprite *GSprite_new (int size, GSpriteRender render, GSpriteEvent event, GSpriteIsInside is_inside, GSpriteFree free);
// Free sprite
void GSprite_free (GSprite *spr);
// Remove sprite from parent's hierarchy. The sprite will end up completely isolated.
void GSprite_unparent (GSprite *spr);
// First unparent, and then add sprite to new parent's hierarchy, after all other children.
void GSprite_add_child (GSprite *parent, GSprite *child);
// Render sprite and then all its children (if any), using the given offset.
void GSprite_render (GSprite *spr, SDL_Renderer *renderer, int offsx, int offsy);
// Finds the topmost child at the given position which is interested in events (has an event callback),
// or the parent, or NULL.
GSprite *GSprite_topmost_event_receiver (GSprite *parent, int x, int y);
// Pass the event to the sprite
void GSprite_event (GSprite *spr, GEvent *event);
// Returns 1 if the given position is over the sprite. By default (no is_inside method provided by the subclass)
// the sprites are rectangular areas.
int GSprite_is_inside (GSprite *spr, int x, int y);

#endif