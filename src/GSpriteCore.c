#include <SDL.h>
#include <SDL_ttf.h>
#include "GSprite.h"
#include "GSpriteCore.h"
#include "GResources.h"
#include "GGraphics.h"
#include "GSpriteBoard.h"
#include <stdlib.h>

struct _GSpriteCore {
  GSprite base;
  int id;
  Uint32 color;
  GSpriteCoreType type;
  GSpriteCoreCallback callback;
  void *callback_userdata;
  GSpriteBoard *board;
};

#define GSPRITECORE_RADIUS 0.75f

static void GSpriteCore_render (GSpriteCore *spr, int offsx, int offsy) {
  SDL_Rect dst;
  GResources *res = spr->base.res;
  SDL_Renderer *renderer = res->sdl_renderer;
  SDL_Texture *tex;
  dst.x = spr->base.x + offsx - spr->base.w / 2;
  dst.y = spr->base.y + offsy - spr->base.h / 2;
  dst.w = spr->base.w;
  dst.h = spr->base.h;
  tex = res->core_texture[spr->type];
  if (spr->board) {
    // This core is in a board, color is handled specially
    SDL_SetTextureColorMod (tex, (0x40 + (spr->color >> 16)) & 0xFF, (0x40 + (spr->color >> 8)) & 0xFF, (0x40 + (spr->color >> 0)) & 0xFF);
  } else {
    // This is a basic core sprite, color works normally
    SDL_SetTextureColorMod (tex, (spr->color >> 16) & 0xFF, (spr->color >> 8) & 0xFF, (spr->color >> 0) & 0xFF);
  }
  SDL_RenderCopy (renderer, tex, NULL, &dst);
}

static int GSpriteCore_event (GSpriteCore *spr, GEvent *event, int *destroyed) {
  if (spr->callback)
    return spr->callback (spr->id, event, spr->callback_userdata, destroyed);
  return 0;
}

static int GSpriteCore_is_inside (GSprite *spr, int x, int y) {
  int r2, R2;
  x += spr->w / 2;
  y += spr->h / 2;
  if (x < spr->x || y < spr->y || x > spr->x + spr->w || y > spr->y + spr->h)
    return 0;
  r2 = SQR (x - (spr->x + spr->w / 2)) + SQR (y - (spr->y + spr->h / 2));
  R2 = (int)SQR (GSPRITECORE_RADIUS * spr->w / 2);
  return (r2 < R2);
}

SDL_Texture *GSpriteCore_create_texture (GResources *res, int w, int h, TTF_Font *icon_font, const char *icon_text) {
  SDL_Surface *surf = GGraphics_circle (w, h, 0, (int)(GSPRITECORE_RADIUS * w / 2));
  SDL_Texture *tex;


  SDL_Surface *icon_surf;
  SDL_Rect rect = { 0 };
  SDL_Color col = { 0, 0, 0, 0xFF }; // Opaque black
  icon_surf = TTF_RenderUTF8_Blended (icon_font, icon_text, col);
  if (!icon_surf) {
    SDL_Log ("TTF_RenderUTF8_Blended: %s", TTF_GetError ());
  }
  GGraphics_get_content_rect (icon_surf, &rect);
  rect.x = (surf->w - rect.w) / 2 - rect.x;
  rect.y = (surf->h - rect.h) / 2 - rect.y;
  SDL_BlitSurface (icon_surf, NULL, surf, &rect);
  SDL_FreeSurface (icon_surf);



  tex = SDL_CreateTextureFromSurface (res->sdl_renderer, surf);
  GGraphics_free_surface (surf);
  return tex;
}

GSprite *GSpriteCore_new (GResources *res, GSpriteCoreType type, float x, float y, int id, Uint32 color,
    int radiusX, int radiusY, GSpriteCoreCallback callback, void *callback_userdata, GSpriteBoard *board) {
  GSpriteCore *spr = (GSpriteCore *)GSprite_new (res, sizeof (GSpriteCore),
    (GSpriteRender)GSpriteCore_render, (GSpriteEvent)GSpriteCore_event, (GSpriteIsInside)GSpriteCore_is_inside, NULL);
  int r, g, b;
  spr->base.x = (int)(x * radiusX);
  spr->base.y = (int)(y * radiusY);
  spr->base.w = radiusX;
  spr->base.h = radiusY;
  spr->type = type;
  if (color == 0) {
    if (id > -1) {
      r = rand () % 0x40;
      g = rand () % 0x40;
      b = 0x40 - (r + g) / 2;
      r += 0x40;
      g += 0x40;
      b += 0x40;
    } else {
      r = 0x80;
      g = 0x80;
      b = 0x00;
    }
    spr->color = 0xFF000000 | (r << 16) + (g << 8) + b;
  } else {
    spr->color = color;
  }
  spr->id = id;
  spr->callback = callback;
  spr->callback_userdata = callback_userdata;
  spr->board = board;
  return (GSprite *)spr;
}

int GSpriteCore_get_id (GSpriteCore *spr) {
  return spr->id;
}

void GSpriteCore_set_id (GSpriteCore *spr, int id) {
  spr->id = id;
}

Uint32 GSpriteCore_get_color (GSpriteCore *spr) {
  return spr->color;
}

void GSpriteCore_set_color (GSpriteCore *spr, Uint32 color) {
  spr->color = color;
}

void GSpriteCore_get_corner (GSpriteCore *spr, int *x, int *y) {
  *x = (int)(spr->base.x / spr->base.w);
  *y = (int)(spr->base.y / spr->base.h);
}

void GSpriteCore_get_opposite (GSpriteCore *spr, int x, int y, int *x2, int *y2) {
  int cx = (int)(spr->base.x * 2 / spr->base.w);
  int cy = (int)(spr->base.y * 2 / spr->base.h);
  if (spr->type == GCORE_TYPE_BLOCKER) {
    *x2 = *y2 = -1;
    return;
  }
  *x2 = cx - x - 1;
  *y2 = cy - y - 1;
}

GSpriteCore *GSpriteCore_clone (GSpriteCore *spr) {
  GSpriteCore *clone = (GSpriteCore *)GSprite_new (spr->base.res, sizeof (GSpriteCore),
    NULL, NULL, NULL, NULL);
  clone->base.x = spr->base.x;
  clone->base.y = spr->base.y;
  clone->base.w = spr->base.w;
  clone->base.h = spr->base.h;
  clone->id = spr->id;
  clone->type = spr->type;
  clone->callback = NULL;
  clone->board = NULL;
  return clone;
}

GSpriteCoreType GSpriteCore_get_type (GSpriteCore *spr) {
  return spr->type;
}

void GSpriteCore_set_type (GSpriteCore *spr, GSpriteCoreType type) {
  spr->type = type;
}
