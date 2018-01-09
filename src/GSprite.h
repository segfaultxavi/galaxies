#ifndef __GSPRITE_H__
#define __GSPRITE_H__

#include <SDL.h>
#include "GEvent.h"

typedef struct _GSprite GSprite;

#include "GResources.h"

typedef void (*GSpriteRender)(GSprite *spr, int offsx, int offsy);
typedef int (*GSpriteEvent)(GSprite *spr, GEvent *event, int *destroyed);
typedef int (*GSpriteIsInside)(GSprite *spr, int x, int y);
typedef void (*GSpriteFree)(GSprite *spr);

struct _GSprite {
  // Global resources
  GResources *res;

  // Area
  int x, y;
  int w, h;

  // Invisible sprites and their children are not rendered and do not receive events
  int visible;

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

typedef enum _GSpriteJustify {
  GSPRITE_JUSTIFY_BEGIN,
  GSPRITE_JUSTIFY_CENTER,
  GSPRITE_JUSTIFY_END
} GSpriteJustify;

// Intended for subclasses, since you have to provide subclass struct size and methods
GSprite *GSprite_new (GResources *res, int size, GSpriteRender render, GSpriteEvent event,
    GSpriteIsInside is_inside, GSpriteFree free);
// Free sprite
void GSprite_free (GSprite *spr);
// Remove sprite from parent's hierarchy. The sprite will end up completely isolated.
void GSprite_unparent (GSprite *spr);
// First unparent, and then add sprite to new parent's hierarchy, after all other children.
void GSprite_add_child (GSprite *parent, GSprite *child);
// Render sprite and then all its children (if any), using the given offset.
void GSprite_render (GSprite *spr, int offsx, int offsy);
// Pass the event to the sprite
// Returns 1 if the event was processed
// Upon return, *destroyed will contain 1 if the sprite has been
// destroyed in the process and is no longer available.
int GSprite_event (GSprite *spr, GEvent *event, int *destroyed);
// Pass the event to the whole hierarchy of the sprite (depth-first)
// Returns the sprite that handled the event (or NULL if no one did).
// Upon return, *destroyed will contain 1 if the sprite has been
// destroyed in the process and is no longer available.
GSprite *GSprite_hierarchical_event (GSprite *spr, GEvent *event, int *destroyed);
// Returns 1 if the given position is over the sprite. By default (no is_inside method provided by the subclass)
// the sprites are rectangular areas.
int GSprite_is_inside (GSprite *spr, int x, int y);
// Changes the position of the sprite depending on its w and h and the requested justification
void GSprite_justify (GSprite *spr, int x, int y, GSpriteJustify justify_hor, GSpriteJustify justify_ver);

#endif