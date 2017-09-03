#include <stdio.h> // For NULL ¬_¬'
#include "GSpriteNull.h"

GSprite *GSpriteNull_new (GResources *res, int x, int y) {
  GSprite *spr = GSprite_new (res, sizeof (GSprite), NULL, NULL, NULL, NULL);
  spr->x = x;
  spr->y = y;
  spr->w = spr->h = -1;
  return spr;
}
