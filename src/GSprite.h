#ifndef __GSPRITE_H__
#define __GSPRITE_H__

#include <SDL.h>
#include "GEvent.h"

typedef struct _GSprite GSprite;

typedef void (*GSpriteRender)(GSprite *spr, SDL_Renderer *renderer, int offsx, int offsy);
typedef int (*GSpriteAction)(GSprite *spr, GEvent *event);
typedef int (*GSpriteIsInside)(GSprite *spr, int x, int y);
typedef void (*GSpriteFree)(GSprite *spr);

struct _GSprite {
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
  GSpriteAction action;
  GSpriteIsInside is_inside;
  GSpriteFree free;
};

// Intended for subclasses, since you have to provide subclass struct size and methods
GSprite *GSprite_new (int size, GSpriteRender render, GSpriteAction action, GSpriteIsInside is_inside, GSpriteFree free);
// Free sprite
void GSprite_free (GSprite *spr);
// Remove sprite from parent's hierarchy. The sprite will end up completely isolated.
void GSprite_unparent (GSprite *spr);
// First unparent, and then add sprite to new parent's hierarchy, after all other children.
void GSprite_add_child (GSprite *parent, GSprite *child);
// Render sprite and then all its children (if any), using the given offset.
void GSprite_render (GSprite *spr, SDL_Renderer *renderer, int offsx, int offsy);
// Perform the given action on the sprite iff the action's position is inside the sprite (checked using GSprite_is_inside).
// Returns 1 if an action was performed.
// Children of the sprite are tested first, and if any returns 1, the parent does not perform any action.
int GSprite_action (GSprite *spr, GEvent *event);
// Returns 1 if the given position is over the sprite. By default (no is_inside method provided by the subclass)
// the sprites are rectangular areas.
int GSprite_is_inside (GSprite *spr, int x, int y);

#endif