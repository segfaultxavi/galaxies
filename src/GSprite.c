#include <SDL.h>
#include "GSprite.h"
#include <memory.h>

GSprite *GSprite_new (int size, GSpriteRender render, GSpriteAction action, GSpriteIsInside is_inside, GSpriteFree _free)
{
  GSprite *spr = malloc (size);
  memset (spr, 0, sizeof (GSprite));

  spr->render = render;
  spr->action = action;
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

void GSprite_render (GSprite *spr, SDL_Renderer *renderer, int x, int y) {
  GSprite *ptr = spr->children;
  if (spr->render)
    spr->render (spr, renderer, x, y);
  while (ptr) {
    GSprite_render (ptr, renderer, spr->x + x, spr->y + y);
    ptr = ptr->next;
  }
}

int GSprite_action (GSprite *spr, GEvent *event) {
  GSprite *ptr = spr->children;
  if (!GSprite_is_inside (spr, event->x, event->y))
    return 0;
  event->x -= spr->x;
  event->y -= spr->y;
  while (ptr) {
    if (GSprite_action (ptr, event))
      return 1;
    ptr = ptr->next;
  }
  event->x += spr->x;
  event->y += spr->y;
  if (spr->action)
    return spr->action (spr, event);
  return 0;
}

int GSprite_is_inside (GSprite *spr, int x, int y) {
  if (spr->is_inside)
    return spr->is_inside (spr, x, y);
  if (spr->w == -1) return 1;
  return x >= spr->x && y >= spr->y && x <= spr->x + spr->w && y <= spr->y + spr->h;
}
