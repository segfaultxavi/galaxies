#ifndef __GSPRITE_H__
#define __GSPRITE_H__

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

  GSpriteRender render;
  GSpriteAction action;
  GSpriteIsInside is_inside;
  GSpriteFree free;
};

GSprite *GSprite_new (int size, GSpriteRender render, GSpriteAction action, GSpriteIsInside is_inside, GSpriteFree free);
void GSprite_free (GSprite *spr);
void GSprite_unparent (GSprite *spr);
void GSprite_add_child (GSprite *parent, GSprite *child);
void GSprite_render (GSprite *spr, SDL_Renderer *renderer, int x, int y);
int GSprite_action (GSprite *spr, GEvent *event);
int GSprite_is_inside (GSprite *spr, int x, int y);

#endif