#include <SDL.h>
#include "GSprite.h"
#include <memory.h>

GSprite *GSprite_new (int size, GSpriteRender render, GSpriteEvent event, GSpriteIsInside is_inside, GSpriteFree _free)
{
  GSprite *spr = malloc (size);
  memset (spr, 0, sizeof (GSprite));

  spr->visible = 1;
  spr->render = render;
  spr->event = event;
  spr->is_inside = is_inside;
  spr->free = _free;

  return spr;
}

void GSprite_free (GSprite *spr) {
  GSprite *ptr;
  if (!spr) return;
  if (spr->free)
    spr->free (spr);
  ptr = spr->children;
  free (spr);
  while (ptr) {
    spr = ptr->next;
    GSprite_free (ptr);
    ptr = spr;
  }
}

void GSprite_unparent (GSprite *spr) {
  if (!spr || !spr->parent) return;
  if (spr->parent->children == spr)
    spr->parent->children = spr->next;
  if (spr->prev)
    spr->prev->next = spr->next;
  if (spr->next)
    spr->next->prev = spr->prev;
  spr->parent = spr->next = spr->prev = NULL;
}

void GSprite_add_child (GSprite *parent, GSprite *child) {
  GSprite_unparent (child);
  if (parent->children) {
    GSprite *ptr = parent->children;
    while (ptr->next) ptr = ptr->next;
    ptr->next = child;
    child->prev = ptr;
  } else {
    parent->children = child;
  }
  child->parent = parent;
}

void GSprite_render (GSprite *spr, SDL_Renderer *renderer, int offsx, int offsy) {
  GSprite *ptr = spr->children;
  if (!spr->visible)
    return;
  if (spr->render)
    spr->render (spr, renderer, offsx, offsy);
  while (ptr) {
    GSprite_render (ptr, renderer, spr->x + offsx, spr->y + offsy);
    ptr = ptr->next;
  }
}

GSprite *GSprite_topmost_event_receiver (GSprite *parent, int x, int y) {
  GSprite *ptr = parent->children;
  if (!parent->visible)
    return NULL;
  if (!GSprite_is_inside (parent, x, y))
    return NULL;
  x -= parent->x;
  y -= parent->y;
  while (ptr) {
    GSprite *ret = GSprite_topmost_event_receiver (ptr, x, y);
    if (ret != NULL) return ret;
    ptr = ptr->next;
  }
  if (parent->event)
    return parent;
  return NULL;
}

void GSprite_event (GSprite *spr, GEvent *event) {
  if (spr && spr->event)
    spr->event (spr, event);
}

int GSprite_is_inside (GSprite *spr, int x, int y) {
  if (!spr->visible)
    return 0;
  if (spr->is_inside)
    return spr->is_inside (spr, x, y);
  if (spr->w == -1) return 1;
  return x >= spr->x && y >= spr->y && x <= spr->x + spr->w && y <= spr->y + spr->h;
}
