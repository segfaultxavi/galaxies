#include "GJustify.h"
#include "GSprite.h"

void GJustify_apply (GSprite *spr, int x, int y, GJustify justify_hor, GJustify justify_ver) {
  switch (justify_hor) {
    case GJUSTIFY_BEGIN:
      spr->x = x;
      break;
    case GJUSTIFY_CENTER:
      spr->x = x - spr->w / 2;
      break;
    case GJUSTIFY_END:
      spr->x = x - spr->w;
      break;
  }
  switch (justify_ver) {
    case GJUSTIFY_BEGIN:
      spr->y = y;
      break;
    case GJUSTIFY_CENTER:
      spr->y = y - spr->h / 2;
      break;
    case GJUSTIFY_END:
      spr->y = y - spr->h;
      break;
  }
}
