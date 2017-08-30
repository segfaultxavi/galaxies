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
  GSprite_unparent (spr);
  ptr = spr->children;
  while (ptr) {
    GSprite *ptr2;
    ptr2 = ptr->next;
    GSprite_free (ptr);
    ptr = ptr2;
  }
  if (spr->free)
    spr->free (spr);
  free (spr);
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
  GSprite *ptr = parent->children, *ptr_prev = NULL;
  if (!parent->visible)
    return NULL;
  if (!GSprite_is_inside (parent, x, y))
    return NULL;
  x -= parent->x;
  y -= parent->y;
  // Traverse children in reverse order
  while (ptr) {
    ptr_prev = ptr;
    ptr = ptr->next;
  }
  ptr = ptr_prev;
  while (ptr) {
    GSprite *ret = GSprite_topmost_event_receiver (ptr, x, y);
    if (ret != NULL) return ret;
    ptr = ptr->prev;
  }
  if (parent->event)
    return parent;
  return NULL;
}

int GSprite_event (GSprite *spr, GEvent *event) {
  if (spr && spr->event)
    return spr->event (spr, event);
  return 0;
}

int GSprite_is_inside (GSprite *spr, int x, int y) {
  if (!spr->visible)
    return 0;
  if (spr->is_inside)
    return spr->is_inside (spr, x, y);
  if (spr->w == -1) return 1;
  return x >= spr->x && y >= spr->y && x <= spr->x + spr->w && y <= spr->y + spr->h;
}

void GSprite_justify (GSprite *spr, int x, int y, GSpriteJustify justify_hor, GSpriteJustify justify_ver) {
  switch (justify_hor) {
    case GSPRITE_JUSTIFY_BEGIN:
      spr->x = x;
      break;
    case GSPRITE_JUSTIFY_CENTER:
      spr->x = x - spr->w / 2;
      break;
    case GSPRITE_JUSTIFY_END:
      spr->x = x - spr->w;
      break;
  }
  switch (justify_ver) {
    case GSPRITE_JUSTIFY_BEGIN:
      spr->y = y;
      break;
    case GSPRITE_JUSTIFY_CENTER:
      spr->y = y - spr->h / 2;
      break;
    case GSPRITE_JUSTIFY_END:
      spr->y = y - spr->h;
      break;
  }
}
