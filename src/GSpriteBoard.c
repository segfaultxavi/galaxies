#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteGalaxies.h"
#include "GSpriteBoard.h"
#include "GSpriteTile.h"
#include "GSpriteBoardGrid.h"
#include "GSpriteCore.h"
#include <string.h>

struct _GSpriteBoard {
  GSprite base;
  int mapSizeX, mapSizeY;
  int tileSizeX, tileSizeY;
  GSpriteTile **tiles;
  int numCores;
  GSpriteCore **cores;
  GSpriteBoardGrid *grid;
  int currCoreId;
  int currTileX, currTileY;
  int editing;
  char *last_code;
};

#define GBOARD_TILE(spr,x,y) spr->tiles[(y) * spr->mapSizeX + (x)]

static void GSpriteBoard_render (GSpriteBoard *spr, int offsx, int offsy) {
  int i;
  SDL_Renderer *renderer = spr->base.res->sdl_renderer;
  SDL_SetRenderDrawColor (renderer, 0x40, 0x40, 0x40, 0xFF);
  offsx += spr->base.x;
  offsy += spr->base.y;
  for (i = 0; i <= spr->mapSizeY; i++) {
    SDL_RenderDrawLine (renderer, offsx, offsy + i * spr->tileSizeY, offsx + spr->tileSizeX * spr->mapSizeX, offsy + i * spr->tileSizeY);
  }
  for (i = 0; i <= spr->mapSizeX; i++) {
    SDL_RenderDrawLine (renderer, offsx + i * spr->tileSizeX, offsy, offsx + i * spr->tileSizeX, offsy + spr->tileSizeY * spr->mapSizeY);
  }
}

static int GSpriteBoard_check_completion (GSpriteBoard *spr) {
  int x, y;
  for (y = 0; y < spr->mapSizeY; y++) {
    for (x = 0; x < spr->mapSizeX; x++) {
      if (GSpriteTile_get_id (GBOARD_TILE (spr, x, y)) == -1)
        return 0;
    }
  }
  return 1;
}

static int GSpriteBoard_valid_tile_position (GSpriteBoard *spr, int x, int y, int id) {
  int x2, y2;
  if (id == -1) return 0;
  if (x < 0 || x >= spr->mapSizeX || y < 0 || y >= spr->mapSizeY) return 0;

  GSpriteCore_get_opposite (spr->cores[id], x, y, &x2, &y2);

  if (x2 < 0 || x2 >= spr->mapSizeX || y2 < 0 || y2 >= spr->mapSizeY) return 0;

  if (GSpriteTile_get_flags (GBOARD_TILE (spr, x, y)) & GTILE_FLAG_FIXED) return 0;
  if (GSpriteTile_get_flags (GBOARD_TILE (spr, x2, y2)) & GTILE_FLAG_FIXED) return 0;

  if (GSpriteTile_get_id (GBOARD_TILE (spr, x2, y2)) == id) return 0;

  if (x > 0 && GSpriteTile_get_id (GBOARD_TILE (spr, x - 1, y)) == id) return 1;
  if (x < spr->mapSizeX - 1 && GSpriteTile_get_id (GBOARD_TILE (spr, x + 1, y)) == id) return 1;
  if (y > 0 && GSpriteTile_get_id (GBOARD_TILE (spr, x, y - 1)) == id) return 1;
  if (y < spr->mapSizeY - 1 && GSpriteTile_get_id (GBOARD_TILE (spr, x, y + 1)) == id) return 1;

  return 0;
}

int GSpriteBoard_is_tile_selectable (GSpriteBoard *spr, int x, int y) {
  if (x < 0 || y < 0 || x >= spr->mapSizeX || y >= spr->mapSizeY) return 0;
  if (GSpriteTile_get_id (GBOARD_TILE (spr, x, y)) == spr->currCoreId) return 1;
  if (GSpriteBoard_valid_tile_position (spr, x, y, spr->currCoreId)) {
    int x2, y2;
    if (x == spr->currTileX && y == spr->currTileY) return 1;
    GSpriteCore_get_opposite (spr->cores[spr->currCoreId], spr->currTileX, spr->currTileY, &x2, &y2);
    if (x == x2 && y == y2) return 1;
  }
  return 0;
}

static void GSpriteBoard_flood_fill (GSpriteBoard *spr, int sx, int sy, int id) {
  GSpriteTile *tile = GBOARD_TILE (spr, sx, sy);
  int flags;
  if (GSpriteTile_get_id (tile) != id) return;
  flags = GSpriteTile_get_flags (tile);
  if ((flags & GTILE_FLAG_VISITED) != 0) return;
  GSpriteTile_set_flags (tile, flags | GTILE_FLAG_VISITED);

  if (sx > 0) GSpriteBoard_flood_fill (spr, sx - 1, sy, id);
  if (sx < spr->mapSizeX - 1) GSpriteBoard_flood_fill (spr, sx + 1, sy, id);
  if (sy > 0) GSpriteBoard_flood_fill (spr, sx, sy - 1, id);
  if (sy < spr->mapSizeY - 1) GSpriteBoard_flood_fill (spr, sx, sy + 1, id);
}

static void GSpriteBoard_check_core (GSpriteBoard *spr, int id) {
  GSpriteCore *core = spr->cores[id];
  int sx = (int)(((GSprite *)core)->x / spr->tileSizeX);
  int sy = (int)(((GSprite *)core)->y / spr->tileSizeY);
  int x, y;

  // Clear flag
  for (y = 0; y < spr->mapSizeY; y++)
    for (x = 0; x < spr->mapSizeX; x++) {
      GSpriteTile *tile = GBOARD_TILE (spr, x, y);
      GSpriteTile_set_flags (tile, GSpriteTile_get_flags (tile) & ~GTILE_FLAG_VISITED);
    }

  // Fill all tiles connected to the core
  GSpriteBoard_flood_fill (spr, sx, sy, id);

  // Remove unconnected tiles
  for (y = 0; y < spr->mapSizeY; y++)
    for (x = 0; x < spr->mapSizeX; x++) {
      GSpriteTile *tile = GBOARD_TILE (spr, x, y);
      if (GSpriteTile_get_id (tile) == id && (GSpriteTile_get_flags (tile) & GTILE_FLAG_VISITED) == 0) {
        GSpriteTile_set_id (tile, -1, 0);
      }
    }
}

static void GSpriteBoard_handle_click (GSpriteBoard *spr, int x, int y) {
  GSpriteTile *tile = GBOARD_TILE (spr, x, y);
  int tile_id = GSpriteTile_get_id (tile);
  int x2, y2;
  GSpriteTile *tile2;
  int tile_id2;

  if (spr->currCoreId == -1) {
    // Without selected core, clicked on an owned tile 
    spr->currCoreId = tile_id;
    return;
  } else if (spr->currCoreId == tile_id) {
    // Clicked on tile already owned: deselect core
    spr->currCoreId = -1;
    return;
  }

  if (GSpriteBoard_valid_tile_position (spr, x, y, spr->currCoreId) == 0) {
    // Clicked on an owned tile, which is invalid for the current core
    spr->currCoreId = tile_id;
    return;
  }

  GSpriteCore_get_opposite (spr->cores[spr->currCoreId], x, y, &x2, &y2);
  if (GSpriteBoard_valid_tile_position (spr, x2, y2, spr->currCoreId) == 0) {
    // The opposite position is invalid
    return;
  }

  if ((tile_id != -1) && (tile_id != spr->currCoreId)) {
    // Clicked on a valid tile which was owned by another core:
    int x3, y3;
    GSpriteTile *tile3;
    // Remove tile from previous core
    GSpriteTile_set_id (tile, -1, 0x00000000);
    GSpriteCore_get_opposite (spr->cores[tile_id], x, y, &x3, &y3);
    tile3 = GBOARD_TILE (spr, x3, y3);
    // And remove tile opposite to previous core
    GSpriteTile_set_id (tile3, -1, 0x00000000);
    GSpriteBoard_check_core (spr, tile_id);
  }
  tile2 = GBOARD_TILE (spr, x2, y2);
  tile_id2 = GSpriteTile_get_id (tile2);
  if ((tile_id2 != -1) && (tile_id2 != spr->currCoreId)) {
    // The tile opposite to the current core is also owned by another core
    int x3, y3;
    GSpriteTile *tile3;
    // Remove tile from previous core
    GSpriteTile_set_id (tile2, -1, 0x00000000);
    GSpriteCore_get_opposite (spr->cores[tile_id2], x2, y2, &x3, &y3);
    tile3 = GBOARD_TILE (spr, x3, y3);
    // And remove tile opposite to previous core
    GSpriteTile_set_id (tile3, -1, 0x00000000);
    GSpriteBoard_check_core (spr, tile_id2);
  }

  // Assign tile to current core
  GSpriteTile_set_id (tile, spr->currCoreId, GSpriteCore_get_color (spr->cores[spr->currCoreId]));
  GSpriteTile_set_id (tile2, spr->currCoreId, GSpriteCore_get_color (spr->cores[spr->currCoreId]));

  if (GSpriteBoard_check_completion (spr)) {
    GSpriteGalaxies_complete ((GSpriteGalaxies *)spr->base.parent);
  }
}

static int GSpriteBoard_tile_event (int x, int y, GEvent *event, void *userdata) {
  int res = 0;
  GSpriteBoard *spr = userdata;
  GSpriteTile *tile = GBOARD_TILE (spr, x, y);
  switch (event->type) {
    case GEVENT_TYPE_MOVE:
      res = 1;
      break;
    case GEVENT_TYPE_SPRITE_IN:
      spr->currTileX = x;
      spr->currTileY = y;
      res = 1;
      break;
    case GEVENT_TYPE_SPRITE_ACTIVATE:
      GSpriteBoard_handle_click (spr, x, y);
      res = 1;
      break;
    default:
      break;
  }
  return res;
}

static int GSpriteBoard_core_event (int id, GEvent *event, void *userdata) {
  int res = 0;
  GSpriteBoard *spr = userdata;
  switch (event->type) {
    case GEVENT_TYPE_MOVE:
      res = 1;
      break;
    case GEVENT_TYPE_SPRITE_ACTIVATE:
      if (spr->currCoreId != id)
        spr->currCoreId = id;
      else
        spr->currCoreId = -1;
      res = 1;
      break;
    default:
      break;
  }
  return res;
}

void GSpriteBoard_free (GSpriteBoard *spr) {
  if (spr->last_code)
    free (spr->last_code);
  free (spr->tiles);
  free (spr->cores);
  SDL_DestroyTexture (spr->base.res->core_texture);
  SDL_DestroyTexture (spr->base.res->core_highlight_texture);
}

GSprite *GSpriteBoard_new (GResources *res, int editing) {
  GSpriteBoard *spr = (GSpriteBoard *)GSprite_new (res, sizeof (GSpriteBoard), (GSpriteRender)GSpriteBoard_render, NULL, NULL, (GSpriteFree)GSpriteBoard_free);
  spr->editing = editing;
  spr->base.w = spr->base.h = res->game_height;
  return (GSprite *)spr;
}

static void GSpriteBoard_deploy_core(GSpriteBoard *spr, float sx, float sy, int id) {
  int x, y, lenx, leny;
  ((GSprite *)spr->cores[id])->x = (int)(sx * spr->tileSizeX);
  ((GSprite *)spr->cores[id])->y = (int)(sy * spr->tileSizeY);
  
  // Tiles under the core
  lenx = (int)sx != sx ? 1 : 2;
  leny = (int)sy != sy ? 1 : 2;
  for (x = (int)(sx - 0.5f); x < (int)(sx - 0.5f) + lenx; x++) {
    for (y = (int)(sy - 0.5f); y < (int)(sy - 0.5f) + leny; y++) {
      GSpriteTile *tile = GBOARD_TILE (spr, x, y);
      int tile_id = GSpriteTile_get_id (tile);
      
      if (tile_id != -1) {
        int x2, y2;
        GSpriteCore_get_opposite (spr->cores[tile_id], x, y, &x2, &y2);
        GSpriteTile_set_id (GBOARD_TILE(spr, x2, y2), -1, 0x00000000);
        GSpriteBoard_check_core (spr, tile_id);
      }
      
      GSpriteTile_set_id (tile, id, GSpriteCore_get_color (spr->cores[id]));
      GSpriteTile_set_flags (tile, GSpriteTile_get_flags (tile) | GTILE_FLAG_FIXED);
    }
  }
}

void GSpriteBoard_start (GSpriteBoard *spr, int mapSizeX, int mapSizeY, int numInitialCores, const float *initialCores, const char *initialTiles) {
  int x, y;
  spr->mapSizeX = mapSizeX;
  spr->mapSizeY = mapSizeY;
  spr->tileSizeX = ((spr->base.w - 1) / spr->mapSizeX / 2) * 2;
  spr->tileSizeY = ((spr->base.h - 1) / spr->mapSizeY / 2) * 2;
  spr->base.w = spr->tileSizeX * spr->mapSizeX;
  spr->base.h = spr->tileSizeY * spr->mapSizeY;
  spr->base.x = (spr->base.res->game_height - spr->base.w) / 2;
  spr->base.y = (spr->base.res->game_height - spr->base.h) / 2;
  spr->tiles = malloc (mapSizeX * mapSizeY * sizeof (GSpriteTile *));

  spr->base.res->core_texture = GSpriteCore_create_texture (spr->base.res, spr->tileSizeX, spr->tileSizeY);
  spr->base.res->core_highlight_texture = GSpriteCore_create_highlight_texture (spr->base.res, spr->tileSizeX, spr->tileSizeY);

  for (y = 0; y < mapSizeY; y++) {
    for (x = 0; x < mapSizeX; x++) {
      GSpriteTile *tile = (GSpriteTile *)GSpriteTile_new (spr->base.res, x, y, spr->tileSizeX, spr->tileSizeY, GSpriteBoard_tile_event, spr);
      GBOARD_TILE (spr, x, y) = tile;
      GSprite_add_child ((GSprite *)spr, (GSprite *)tile);
      GSpriteTile_set_id (tile, -1, 0x00000000);
    }
  }
  spr->grid = (GSpriteBoardGrid *)GSpriteBoardGrid_new (spr->base.res, spr->mapSizeX, spr->mapSizeY, spr->tileSizeX, spr->tileSizeY, spr);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->grid);

  spr->numCores = numInitialCores;
  spr->cores = malloc (spr->numCores * sizeof (GSpriteCore *));
  for (x = 0; x < numInitialCores; x++) {
    spr->cores[x] = (GSpriteCore *)GSpriteCore_new (spr->base.res, 0, 0, x,
      spr->tileSizeX, spr->tileSizeY, GSpriteBoard_core_event, spr);
    GSpriteBoard_deploy_core (spr, initialCores[x * 2 + 0], initialCores[x * 2 + 1], x);
    GSprite_add_child ((GSprite *)spr, (GSprite *)spr->cores[x]);
  }

  if (initialTiles) {
    for (y = 0; y < mapSizeY; y++) {
      for (x = 0; x < mapSizeX; x++) {
        GSpriteTile *tile = GBOARD_TILE (spr, x, y);
        int id = initialTiles[y * mapSizeX + x];
        GSpriteCore *core = spr->cores[id];
        GSpriteTile_set_id (tile, id, GSpriteCore_get_color (core));
      }
    }
  }

  spr->currCoreId = -1;
}

static int GSpriteBoard_a2i (char a) {
  if (a >= 'a' && a <= 'z') return a - 'a';
  if (a >= 'A' && a <= 'Z') return a - 'A' + 26;
  return 0;
}

static char GSpriteBoard_i2a (int i) {
  if (i == -1) return '.';
  if (i < 26) return 'a' + i;
  return 'A' + i - 26;
}

int GSpriteBoard_load (GSpriteBoard *spr, const char *desc) {
  int desc_len;
  int size_x, size_y;
  int i, num_cores;
  float *initial_cores = NULL;
  const char *initial_tiles = NULL;

  if (desc == NULL) return 0;
  desc_len = (int)strlen (desc);

  // Check if anything has changed
  if (spr->last_code != NULL && strcmp (desc, spr->last_code) == 0) return 1;

  // Check format
  if (desc_len < 4) return 0; // Header
  if (desc[0] != '1') return 0; // Version number
  size_x = GSpriteBoard_a2i (desc[1]); // Board size
  size_y = GSpriteBoard_a2i (desc[2]);
  if (size_x < 3 || size_x > 20 || size_y < 3 || size_y > 20) return 0; // Invalid sizes
  num_cores = GSpriteBoard_a2i (desc[3]);
  initial_cores = malloc (num_cores * 2 * sizeof (float));
  for (i = 0; i < num_cores; i++) {
    float x = (GSpriteBoard_a2i (desc[4 + i * 2 + 0]) + 1) / 2.f;
    float y = (GSpriteBoard_a2i (desc[4 + i * 2 + 1]) + 1) / 2.f;
    if (x < 0 || x >= (size_x + 0.5f) || y < 0 || y >= (size_y + 0.5f)) {
      SDL_Log ("%g,%g outside %dx%d", x, y, size_x, size_y);
      free (initial_cores);
      return 0;
    }
    initial_cores[i * 2 + 0] = x;
    initial_cores[i * 2 + 1] = y;
  }
  // Check core collisions
  for (i = 0; i < num_cores; i++) {
    int minx = (int)(initial_cores[i * 2 + 0]);
    int maxx = (int)(initial_cores[i * 2 + 0] + 0.5f);
    int miny = (int)(initial_cores[i * 2 + 1]);
    int maxy = (int)(initial_cores[i * 2 + 1] + 0.5f);
    int j;
    for (j = i + 1; j < num_cores; j++) {
      if ((initial_cores[j * 2 + 0] >= minx) && (initial_cores[j * 2 + 0] <= maxx) &&
          (initial_cores[j * 2 + 1] >= miny) && (initial_cores[j * 2 + 1] <= maxy)) {
        SDL_Log ("Cores %d (%g,%g) and %d (%g,%g) overlap",
          i, initial_cores[i * 2 + 0], initial_cores[i * 2 + 1],
          j, initial_cores[j * 2 + 0], initial_cores[j * 2 + 1]);
        free (initial_cores);
        return 0;
      }
    }
  }
  if (desc_len > 4 + num_cores * 2) {
    if (desc_len != 4 + num_cores * 2 + size_x * size_y) {
      SDL_Log ("Invalid code (code_len:%d sizex:%d sizey:%d num_cores:%d", desc_len, size_x, size_y, num_cores);
      free (initial_cores);
      return 0;
    }
    initial_tiles = desc + 4 + num_cores * 2;
  }

  // Build board: Needs to rebuild whole board in case of size change
  GSpriteBoard_start (spr, size_x, size_x, num_cores, initial_cores, initial_tiles);
  free (initial_cores);
  if (spr->last_code)
    free (spr->last_code);
  spr->last_code = strdup (desc);

  return 1;
}

char *GSpriteBoard_save (GSpriteBoard *spr, int includeTileColors) {
  char *code, *ptr;
  int code_len = 4 + spr->numCores * 2 + 1;
  int i;
  
  if (includeTileColors)
    code_len += spr->mapSizeX * spr->mapSizeY;

  code = malloc (code_len);

  // Version number and board size
  code[0] = '1';
  code[1] = GSpriteBoard_i2a (spr->mapSizeX);
  code[2] = GSpriteBoard_i2a (spr->mapSizeY);
  code[3] = GSpriteBoard_i2a (spr->numCores);
  ptr = code + 4;

  // Coords of all cores
  for (i = 0; i < spr->numCores; i++) {
    GSprite *core = (GSprite *)spr->cores[i];
    ptr[0] = GSpriteBoard_i2a ((core->x * 2 / spr->tileSizeX) - 1);
    ptr[1] = GSpriteBoard_i2a ((core->y * 2 / spr->tileSizeY) - 1);
    ptr += 2;
  }

  if (includeTileColors) {
    // Core IDs for all tiles
    int x, y;
    for (y = 0; y < spr->mapSizeY; y++) {
      for (x = 0; x < spr->mapSizeX; x++) {
        GSpriteTile *tile = GBOARD_TILE (spr, x, y);
        ptr[0] = GSpriteBoard_i2a (GSpriteTile_get_id (tile));
        ptr++;
      }
    }
  }

  ptr[0] = '\0';

  return code;
}

GSpriteTile *GSpriteBoard_get_tile (GSpriteBoard *spr, int x, int y) {
  if (x < 0 || x >= spr->mapSizeX || y < 0 || y >= spr->mapSizeY)
    return NULL;
  return GBOARD_TILE (spr, x, y);
}

GSpriteCore *GSpriteBoard_get_selected_core (GSpriteBoard *spr) {
  if (spr->currCoreId > -1)
    return spr->cores[spr->currCoreId];
  return NULL;
}
