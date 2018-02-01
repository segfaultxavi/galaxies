#include <SDL.h>
#include "GGame.h"
#include "GResources.h"
#include "GSpriteGalaxies.h"
#include "GSpriteEditor.h"
#include "GSpriteBoard.h"
#include "GSpriteTile.h"
#include "GSpriteBoardGrid.h"
#include "GSpriteCore.h"
#include "GAudio.h"
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
  GSpriteCore *coreCursor;
  char *last_code;
  int finished;
};

#define GBOARD_TILE(spr,x,y) spr->tiles[(y) * spr->mapSizeX + (x)]

static void GSpriteBoard_deploy_core (GSpriteBoard *spr, float sx, float sy, GSpriteCoreType type, int id);

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

static void GSpriteBoard_set_curr_core_id (GSpriteBoard *spr, int id) {
  if (id > -1)
    GAudio_play_sample (spr->base.res->audio, spr->base.res->wav_ping);
  else
    GAudio_play_sample (spr->base.res->audio, spr->base.res->wav_pong);
  spr->currCoreId = id;
}

static void GSpriteBoard_remove_core (GSpriteBoard *spr, int id) {
  int i, x, y;
  GSprite_free ((GSprite *)spr->cores[id]);
  for (i = id; i < spr->numCores - 1; i++) {
    spr->cores[i] = spr->cores[i + 1];
    GSpriteCore_set_id (spr->cores[i], i);
  }
  spr->numCores--;
  spr->cores = SDL_realloc (spr->cores, spr->numCores * sizeof (GSpriteCore *));

  for (y = 0; y < spr->mapSizeY; y++) {
    for (x = 0; x < spr->mapSizeX; x++) {
      GSpriteTile *tile = GBOARD_TILE (spr, x, y);
      int tile_id = GSpriteTile_get_id (tile);
      if (tile_id == id) {
        GSpriteTile_set_id (tile, -1, 0x00000000);
        GSpriteTile_set_flags (tile, 0);
      } else if (tile_id > id)
        GSpriteTile_set_id (tile, tile_id - 1, GSpriteCore_get_color (spr->cores[tile_id - 1]));
    }
  }

  GSpriteBoard_set_curr_core_id (spr, -1);

  GSpriteEditor_cores_changed ((GSpriteEditor *)spr->base.parent);
}

static int GSpriteBoard_core_event (int id, GEvent *event, void *userdata, int *destroyed) {
  int res = 0;
  GSpriteBoard *spr = userdata;
  switch (event->type) {
    case GEVENT_TYPE_MOVE:
      res = 1;
      break;
    case GEVENT_TYPE_SPRITE_ACTIVATE:
      res = 1;
      if (spr->finished) break;
      if (spr->currCoreId != id)
        GSpriteBoard_set_curr_core_id (spr, id);
      else
        GSpriteBoard_set_curr_core_id (spr, -1);
      spr->currTileX = -1;
      spr->currTileY = -1;
      break;
    case GEVENT_TYPE_SPRITE_ACTIVATE_SECONDARY:
      res = 1;
      if (!spr->editing) break;
      if (spr->finished) break;
      GSpriteBoard_remove_core (spr, id);
      if (destroyed) *destroyed = 1;
      break;
    default:
      break;
  }
  return res;
}

static void GSpriteBoard_add_core (GSpriteBoard *spr, float cx, float cy, GSpriteCoreType type) {
  int id = spr->numCores;
  spr->numCores++;
  spr->cores = SDL_realloc (spr->cores, spr->numCores * sizeof (GSpriteCore *));

  spr->cores[id] = (GSpriteCore *)GSpriteCore_new (spr->base.res, GCORE_TYPE_2_FOLD, 0, 0, id,
    spr->tileSizeX, spr->tileSizeY, GSpriteBoard_core_event, spr);
  GSpriteBoard_deploy_core (spr, cx, cy, type, id);
  GSprite_add_child ((GSprite *)spr, (GSprite *)spr->cores[id]);
  GSpriteBoard_set_curr_core_id (spr, id);
  ((GSprite*)spr->coreCursor)->visible = 0;

  GSpriteEditor_cores_changed ((GSpriteEditor *)spr->base.parent);
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

static int GSpriteBoard_valid_core_position (GSpriteBoard *spr, float sx, float sy) {
  if (sx <= 0 || sx >= spr->mapSizeX || sy <= 0 || sy >= spr->mapSizeY) return 0;

  int lenx = (int)sx != sx ? 1 : 2;
  int leny = (int)sy != sy ? 1 : 2;
  int x, y;
  for (x = (int)(sx - 0.5f); x <(int)(sx - 0.5) + lenx; x++) {
    for (y = (int)(sy - 0.5f); y <(int)(sy - 0.5) + leny; y++) {
      if (GSpriteTile_get_flags (GBOARD_TILE (spr, x, y)) & GTILE_FLAG_FIXED) return 0;
    }
  }

  return 1;
}

int GSpriteBoard_is_tile_selectable (GSpriteBoard *spr, int x, int y) {
  if (x < 0 || y < 0 || x >= spr->mapSizeX || y >= spr->mapSizeY) return 0;
  if (GSpriteTile_get_id (GBOARD_TILE (spr, x, y)) == spr->currCoreId) return 1;
  if (GSpriteBoard_valid_tile_position (spr, x, y, spr->currCoreId)) {
#ifdef __ANDROID__
    // On touch devices there is no hovering, so we use the galaxy outline to indicate
    // valid moves instead of the tile opposite to the one currently hovered.
    return 1;
#else
    int x2, y2;
    if (x == spr->currTileX && y == spr->currTileY) return 1;
    GSpriteCore_get_opposite (spr->cores[spr->currCoreId], spr->currTileX, spr->currTileY, &x2, &y2);
    if (x == x2 && y == y2) return 1;
#endif
  }
  return 0;
}

static int GSpriteBoard_flood_fill (GSpriteBoard *spr, int sx, int sy, int id) {
  GSpriteTile *tile = GBOARD_TILE (spr, sx, sy);
  int flags;
  int size = 0;
  if (GSpriteTile_get_id (tile) != id) return size;
  flags = GSpriteTile_get_flags (tile);
  if ((flags & GTILE_FLAG_VISITED) != 0) return size;
  GSpriteTile_set_flags (tile, flags | GTILE_FLAG_VISITED);

  size = 1;
  if (sx > 0) size += GSpriteBoard_flood_fill (spr, sx - 1, sy, id);
  if (sx < spr->mapSizeX - 1) size += GSpriteBoard_flood_fill (spr, sx + 1, sy, id);
  if (sy > 0) size += GSpriteBoard_flood_fill (spr, sx, sy - 1, id);
  if (sy < spr->mapSizeY - 1) size += GSpriteBoard_flood_fill (spr, sx, sy + 1, id);
  return size;
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

static void GSpriteBoard_get_core_coords (GSpriteBoard *spr, int x, int y, int sx, int sy, float *outx, float *outy) {
  float cx = x + ((sx + spr->tileSizeX / 4) * 2 / spr->tileSizeX) / 2.f;
  float cy = y + ((sy + spr->tileSizeX / 4) * 2 / spr->tileSizeY) / 2.f;
  cx = SDL_max (0.5f, cx);
  cx = SDL_min (cx, spr->mapSizeX - 0.5f);
  cy = SDL_max (0.5f, cy);
  cy = SDL_min (cy, spr->mapSizeY - 0.5f);
  *outx = cx;
  *outy = cy;
}

static void GSpriteBoard_handle_click (GSpriteBoard *spr, int x, int y, int sx, int sy) {
  GSpriteTile *tile = GBOARD_TILE (spr, x, y);
  int tile_id = GSpriteTile_get_id (tile);
  int x2, y2;
  GSpriteTile *tile2;
  int tile_id2;

  if (spr->currCoreId == -1) {
    float cx, cy;
    if (!spr->editing) {
      // Without selected core, clicked on an owned tile 
      GSpriteBoard_set_curr_core_id (spr, tile_id);
      return;
    }
    GSpriteBoard_get_core_coords (spr, x, y, sx, sy, &cx, &cy);
    if (!GSpriteBoard_valid_core_position (spr, cx, cy)) {
      GSpriteBoard_set_curr_core_id (spr, tile_id);
      return;
    }
    // Add new core
    GSpriteBoard_add_core (spr, cx, cy, GCORE_TYPE_2_FOLD);
    return;
  } else if (spr->currCoreId == tile_id) {
    // Clicked on tile already owned: deselect core
    GSpriteBoard_set_curr_core_id (spr, -1);
    return;
  }

  if (GSpriteBoard_valid_tile_position (spr, x, y, spr->currCoreId) == 0) {
    // Clicked on an owned tile, which is invalid for the current core
    GSpriteBoard_set_curr_core_id (spr, tile_id);
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
  GAudio_play_sample (spr->base.res->audio, spr->base.res->wav_woosh);

  if (!spr->editing) {
    GSpriteGalaxies_update_level_status ((GSpriteGalaxies *)spr->base.parent, GSPRITE_LEVEL_SELECT_LEVEL_STATUS_IN_PROGRESS, GSpriteBoard_save (spr, 1));

    if (GSpriteBoard_check_completion (spr)) {
      GSpriteGalaxies_complete ((GSpriteGalaxies *)spr->base.parent);
    }
  } else {
    GSpriteEditor_tiles_changed ((GSpriteEditor *)spr->base.parent, GSpriteBoard_save (spr, 1));
  }
}

static int GSpriteBoard_tile_event (int x, int y, GEvent *event, void *userdata) {
  int res = 0;
  GSpriteBoard *spr = userdata;
  GSpriteTile *tile = GBOARD_TILE (spr, x, y);
  switch (event->type) {
    case GEVENT_TYPE_MOVE:
      res = 1;
      if (spr->finished) break;
      if (spr->currCoreId == -1 && spr->editing) {
        // Core placement mode
        float cx, cy;
        GSpriteBoard_get_core_coords (spr, x, y, event->x, event->y, &cx, &cy);
        ((GSprite*)spr->coreCursor)->x = (int)(cx * spr->tileSizeX);
        ((GSprite*)spr->coreCursor)->y = (int)(cy * spr->tileSizeY);
        ((GSprite*)spr->coreCursor)->visible = GSpriteBoard_valid_core_position (spr, cx, cy);
      }
      break;
    case GEVENT_TYPE_SPRITE_IN:
      spr->currTileX = x;
      spr->currTileY = y;
      res = 1;
      break;
    case GEVENT_TYPE_SPRITE_ACTIVATE:
    case GEVENT_TYPE_SPRITE_ACTIVATE_SECONDARY:
      res = 1;
      if (spr->finished) break;
      GSpriteBoard_handle_click (spr, x, y, event->x, event->y);
      break;
    default:
      break;
  }
  return res;
}

void GSpriteBoard_free (GSpriteBoard *spr) {
  if (spr->last_code)
    SDL_free (spr->last_code);
  SDL_free (spr->tiles);
  SDL_free (spr->cores);
  SDL_DestroyTexture (spr->base.res->core_texture);
  SDL_DestroyTexture (spr->base.res->core_highlight_texture);
}

GSprite *GSpriteBoard_new (GResources *res, int editing) {
  GSpriteBoard *spr = (GSpriteBoard *)GSprite_new (res, sizeof (GSpriteBoard), (GSpriteRender)GSpriteBoard_render, NULL, NULL, (GSpriteFree)GSpriteBoard_free);
  spr->editing = editing;
  spr->finished = 0;
  spr->base.w = spr->base.h = res->game_height;
  return (GSprite *)spr;
}

static void GSpriteBoard_deploy_core(GSpriteBoard *spr, float sx, float sy, GSpriteCoreType type, int id) {
  int x, y, lenx, leny;
  ((GSprite *)spr->cores[id])->x = (int)(sx * spr->tileSizeX);
  ((GSprite *)spr->cores[id])->y = (int)(sy * spr->tileSizeY);
  GSpriteCore_set_type (spr->cores[id], type);
  
  // Tiles under the core
  lenx = (int)sx != sx ? 1 : 2;
  leny = (int)sy != sy ? 1 : 2;
  for (x = (int)(sx - 0.5f); x < (int)(sx - 0.5f) + lenx; x++) {
    for (y = (int)(sy - 0.5f); y < (int)(sy - 0.5f) + leny; y++) {
      GSpriteTile *tile = GBOARD_TILE (spr, x, y);
      int tile_id = GSpriteTile_get_id (tile);
      
      if (tile_id != -1) {
        int x2, y2;
        GSpriteTile_set_id (GBOARD_TILE (spr, x, y), -1, 0x00000000);
        GSpriteCore_get_opposite (spr->cores[tile_id], x, y, &x2, &y2);
        GSpriteTile_set_id (GBOARD_TILE(spr, x2, y2), -1, 0x00000000);
        GSpriteBoard_check_core (spr, tile_id);
      }
      
      GSpriteTile_set_id (tile, id, GSpriteCore_get_color (spr->cores[id]));
      GSpriteTile_set_flags (tile, GSpriteTile_get_flags (tile) | GTILE_FLAG_FIXED);
    }
  }
}

void GSpriteBoard_start (GSpriteBoard *spr, int mapSizeX, int mapSizeY, int numInitialCores, const float *initialCores, const int *initialTiles) {
  int x, y;
  spr->mapSizeX = mapSizeX;
  spr->mapSizeY = mapSizeY;
  spr->tileSizeX = ((spr->base.w - 1) / spr->mapSizeX / 2) * 2;
  spr->tileSizeY = ((spr->base.h - 1) / spr->mapSizeY / 2) * 2;
  spr->base.w = spr->tileSizeX * spr->mapSizeX;
  spr->base.h = spr->tileSizeY * spr->mapSizeY;
  spr->base.x = (spr->base.res->game_height - spr->base.w) / 2;
  spr->base.y = (spr->base.res->game_height - spr->base.h) / 2;
  spr->tiles = SDL_malloc (mapSizeX * mapSizeY * sizeof (GSpriteTile *));
  spr->coreCursor = NULL;

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
  spr->cores = SDL_malloc (spr->numCores * sizeof (GSpriteCore *));
  for (x = 0; x < numInitialCores; x++) {
    spr->cores[x] = (GSpriteCore *)GSpriteCore_new (spr->base.res, GCORE_TYPE_2_FOLD, 0, 0, x,
      spr->tileSizeX, spr->tileSizeY, GSpriteBoard_core_event, spr);
    GSpriteBoard_deploy_core (spr, initialCores[x * 3 + 0], initialCores[x * 3 + 1], (int)initialCores[x * 3 + 2], x);
    GSprite_add_child ((GSprite *)spr, (GSprite *)spr->cores[x]);
  }

  if (initialTiles) {
    for (y = 0; y < mapSizeY; y++) {
      for (x = 0; x < mapSizeX; x++) {
        GSpriteTile *tile = GBOARD_TILE (spr, x, y);
        int id = initialTiles[y * mapSizeX + x];
        if (id > -1) {
          GSpriteCore *core = spr->cores[id];
          GSpriteTile_set_id (tile, id, GSpriteCore_get_color (core));
        }
      }
    }
  }

  if (spr->editing) {
    spr->coreCursor = (GSpriteCore *)GSpriteCore_new (spr->base.res, GCORE_TYPE_2_FOLD, 0, 0, -1, spr->tileSizeX, spr->tileSizeY, NULL, spr);
    ((GSprite *)spr->coreCursor)->visible = 0;
    GSprite_add_child ((GSprite *)spr, (GSprite *)spr->coreCursor);
  }

  GSpriteBoard_set_curr_core_id (spr, -1);
}

static int GSpriteBoard_a2i (char a) {
  if (a == '.') return -1;
  if (a >= 'a' && a <= 'z') return a - 'a';
  if (a >= 'A' && a <= 'Z') return a - 'A';
  SDL_Log ("GSpriteBoard_a2i: Invalid format char: %c", a);
  return 0;
}

static char GSpriteBoard_i2a (int i) {
  if (i == -1) return '.';
  if (i < 26) return 'a' + i;
  SDL_Log ("GSpriteBoard_i2a: Invalid int: %d", i);
  return '!';
}

static void GSpriteBoard_save_core (int x, int y, GSpriteCoreType type, char *out) {
  out[0] = GSpriteBoard_i2a (x) - ((type & 1) ? 0x20 : 0);
  out[1] = GSpriteBoard_i2a (y) - ((type & 2) ? 0x20 : 0);
}

static void GSpriteBoard_load_core (const char *in, int *x, int *y, GSpriteCoreType *type) {
  *x = GSpriteBoard_a2i (in[0]);
  *y = GSpriteBoard_a2i (in[1]);
  *type = 0 + (in[0] >= 'a' ? 0 : 1) + (in[1] >= 'a' ? 0 : 2);
}

static int GSpriteBoard_preload (const char *desc, int *p_size_x, int *p_size_y, int *p_num_cores, float **p_initial_cores, int **p_initial_tiles) {
  int size_x, size_y;
  int i, num_cores;
  float *initial_cores = NULL;
  int *initial_tiles = NULL;
  int desc_len;

  *p_size_x = *p_size_y = *p_num_cores = 0;
  *p_initial_cores = NULL;
  *p_initial_tiles = NULL;

  if (desc == NULL) return 0;
  desc_len = (int)SDL_strlen (desc);

  // Check format
  if (desc_len < 4) return 0; // Header
  if (desc[0] != '1') return 0; // Version number
  size_x = GSpriteBoard_a2i (desc[1]); // Board size
  size_y = GSpriteBoard_a2i (desc[2]);
  if (size_x < 3 || size_x > 20 || size_y < 3 || size_y > 20) return 0; // Invalid sizes
  num_cores = GSpriteBoard_a2i (desc[3]);
  initial_cores = SDL_malloc (num_cores * 3 * sizeof (float));
  for (i = 0; i < num_cores; i++) {
    int xi, yi;
    GSpriteCoreType type;
    float x, y;
    GSpriteBoard_load_core (&desc[4 + i * 2 + 0], &xi, &yi, &type);
    x = (xi + 1) / 2.f;
    y = (yi + 1) / 2.f;
    if (x < 0 || x >= (size_x + 0.5f) || y < 0 || y >= (size_y + 0.5f)) {
      SDL_Log ("GSpriteBoard_preload: %g,%g outside %dx%d", x, y, size_x, size_y);
      SDL_free (initial_cores);
      return 0;
    }
    initial_cores[i * 3 + 0] = x;
    initial_cores[i * 3 + 1] = y;
    initial_cores[i * 3 + 2] = (float)type;
  }
  // Check core collisions
  for (i = 0; i < num_cores; i++) {
    int minx = (int)(initial_cores[i * 3 + 0]);
    int maxx = (int)(initial_cores[i * 3 + 0] + 0.5f);
    int miny = (int)(initial_cores[i * 3 + 1]);
    int maxy = (int)(initial_cores[i * 3 + 1] + 0.5f);
    int j;
    for (j = i + 1; j < num_cores; j++) {
      if ((initial_cores[j * 3 + 0] >= minx) && (initial_cores[j * 3 + 0] <= maxx) &&
        (initial_cores[j * 3 + 1] >= miny) && (initial_cores[j * 3 + 1] <= maxy)) {
        SDL_Log ("GSpriteBoard_preload: Cores %d (%g,%g) and %d (%g,%g) overlap",
          i, initial_cores[i * 3 + 0], initial_cores[i * 3 + 1],
          j, initial_cores[j * 3 + 0], initial_cores[j * 3 + 1]);
        SDL_free (initial_cores);
        return 0;
      }
    }
  }
  if (desc_len > 4 + num_cores * 2) {
    if (desc_len != 4 + num_cores * 2 + size_x * size_y) {
      SDL_Log ("GSpriteBoard_preload: Invalid code (code_len:%d sizex:%d sizey:%d num_cores:%d", desc_len, size_x, size_y, num_cores);
      SDL_free (initial_cores);
      return 0;
    }
    initial_tiles = SDL_malloc (size_x * size_y * sizeof (int));
    for (i = 0; i < size_x * size_y; i++)
      initial_tiles[i] = GSpriteBoard_a2i (desc[4 + num_cores * 2 + i]);
  }

  *p_size_x = size_x;
  *p_size_y = size_y;
  *p_num_cores = num_cores;
  *p_initial_cores = initial_cores;
  *p_initial_tiles = initial_tiles;

  return 1;
}

int GSpriteBoard_load (GSpriteBoard *spr, const char *desc) {
  int size_x, size_y, num_cores;
  float *initial_cores = NULL;
  int *initial_tiles = NULL;

  // Check if anything has changed
  if (spr->last_code != NULL && strcmp (desc, spr->last_code) == 0) return 1;

  if (!GSpriteBoard_preload (desc, &size_x, &size_y, &num_cores, &initial_cores, &initial_tiles)) {
    return 0;
  }

  // Build board: Needs to rebuild whole board in case of size change
  GSpriteBoard_start (spr, size_x, size_x, num_cores, initial_cores, initial_tiles);
  SDL_free (initial_cores);
  if (initial_tiles)
    SDL_free (initial_tiles);
  if (spr->last_code)
    SDL_free (spr->last_code);
  spr->last_code = SDL_strdup (desc);

  return 1;
}

// Returns 1 if the string is a valid map description
int GSpriteBoard_check (const char *desc) {
  int size_x, size_y, num_cores;
  float *initial_cores = NULL;
  int *initial_tiles = NULL;

  if (!GSpriteBoard_preload (desc, &size_x, &size_y, &num_cores, &initial_cores, &initial_tiles)) {
    return 0;
  }
  SDL_free (initial_cores);
  SDL_free (initial_tiles);
  return 1;
}

char *GSpriteBoard_save (GSpriteBoard *spr, int includeTileColors) {
  char *code, *ptr;
  int code_len = 4 + spr->numCores * 2 + 1;
  int i;
  
  if (includeTileColors)
    code_len += spr->mapSizeX * spr->mapSizeY;

  code = SDL_malloc (code_len);

  // Version number and board size
  code[0] = '1';
  code[1] = GSpriteBoard_i2a (spr->mapSizeX);
  code[2] = GSpriteBoard_i2a (spr->mapSizeY);
  code[3] = GSpriteBoard_i2a (spr->numCores);
  ptr = code + 4;

  // Coords of all cores
  for (i = 0; i < spr->numCores; i++) {
    GSprite *core = (GSprite *)spr->cores[i];
    GSpriteBoard_save_core ((core->x * 2 / spr->tileSizeX) - 1, (core->y * 2 / spr->tileSizeY) - 1, GSpriteCore_get_type ((GSpriteCore *)core), ptr);
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

void GSpriteBoard_reset (GSpriteBoard *spr) {
  int x, y;
  for (y = 0; y < spr->mapSizeY; y++) {
    for (x = 0; x < spr->mapSizeX; x++) {
      GSpriteTile *tile = GBOARD_TILE (spr, x, y);
      if ((GSpriteTile_get_flags (tile) & GTILE_FLAG_FIXED) == 0) {
        GSpriteTile_set_id (tile, -1, 0x00000000);
      }
    }
  }
  spr->currTileX = spr->currTileY = -1;
  spr->currCoreId = -1;

  if (!spr->editing)
    GSpriteGalaxies_update_level_status ((GSpriteGalaxies *)spr->base.parent, GSPRITE_LEVEL_SELECT_LEVEL_STATUS_UNTRIED, GSpriteBoard_save (spr, 0));
  else
    GSpriteEditor_tiles_changed ((GSpriteEditor *)spr->base.parent, GSpriteBoard_save (spr, 0));
}

int GSpriteBoard_get_map_size_x (GSpriteBoard *spr) {
  return spr->mapSizeX;
}

int GSpriteBoard_get_map_size_y (GSpriteBoard *spr) {
  return spr->mapSizeX;
}

int GSpriteBoard_is_empty (GSpriteBoard *spr) {
  return spr->numCores == 0;
}

int GSpriteBoard_has_no_manual_tiles (GSpriteBoard *spr) {
  int x, y;
  for (y = 0; y < spr->mapSizeY; y++) {
    for (x = 0; x < spr->mapSizeX; x++) {
      GSpriteTile *tile = GBOARD_TILE (spr, x, y);
      if ((GSpriteTile_get_flags (tile) & GTILE_FLAG_FIXED) == 0 &&
          GSpriteTile_get_id (tile) != -1) {
        return 0;
      }
    }
  }
  return 1;
}

GSpriteCore *GSpriteBoard_get_core (GSpriteBoard *spr, int id) {
  return spr->cores[id];
}

int GSpriteBoard_get_num_cores (GSpriteBoard *spr) {
  return spr->numCores;
}

void GSpriteBoard_set_tiles (GSpriteBoard *spr, char *tiles) {
  int x, y;

  if (!tiles) return;
  for (y = 0; y < spr->mapSizeY; y++) {
    for (x = 0; x < spr->mapSizeX; x++) {
      int id = tiles[y * spr->mapSizeX + x];
      GSpriteTile *tile = GBOARD_TILE (spr, x, y);
      if (id > -1) {
        GSpriteCore *core = spr->cores[id];
        GSpriteTile_set_id (tile, id, GSpriteCore_get_color (core));
      } else {
        GSpriteTile_set_id (tile, id, 0x00000000);
      }
    }
  }
  spr->currCoreId = -1;
}

void GSpriteBoard_finish (GSpriteBoard *spr) {
  int id, x, y, max_size = 0;

  spr->currCoreId = -1;

  // Find biggest galaxy
  for (id = 0; id < spr->numCores; id++) {
    GSpriteCore *core = spr->cores[id];
    int sx, sy;
    int size;

    GSpriteCore_get_corner (core, &sx, &sy);

    // Clear flag
    for (y = 0; y < spr->mapSizeY; y++)
      for (x = 0; x < spr->mapSizeX; x++) {
        GSpriteTile *tile = GBOARD_TILE (spr, x, y);
        GSpriteTile_set_flags (tile, GSpriteTile_get_flags (tile) & ~GTILE_FLAG_VISITED);
      }

    // Fill all tiles connected to the core to find out size
    size = GSpriteBoard_flood_fill (spr, sx, sy, id);
    if (size > max_size) max_size = size;
  }

  // Brighten tiles according to their galaxy's size
  for (id = 0; id < spr->numCores; id++) {
    GSpriteCore *core = spr->cores[id];
    int sx, sy;
    int size;
    Uint32 color;
    int r, g, b;

    GSpriteCore_get_corner (core, &sx, &sy);

    // Clear flag
    for (y = 0; y < spr->mapSizeY; y++)
      for (x = 0; x < spr->mapSizeX; x++) {
        GSpriteTile *tile = GBOARD_TILE (spr, x, y);
        GSpriteTile_set_flags (tile, GSpriteTile_get_flags (tile) & ~GTILE_FLAG_VISITED);
      }

    // Fill all tiles connected to the core to find out size
    size = GSpriteBoard_flood_fill (spr, sx, sy, id);

    color = GSpriteCore_get_color (core);
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = (color >> 0) & 0xFF;

    color = ((0x80 + 0x7F * size / max_size) << 24) | (r << 16) + (g << 8) + b;
    for (y = 0; y < spr->mapSizeY; y++) {
      for (x = 0; x < spr->mapSizeX; x++) {
        GSpriteTile *tile = GBOARD_TILE (spr, x, y);
        int tile_id = GSpriteTile_get_id (tile);
        if (tile_id == id) {
          GSpriteTile_set_id (tile, id, color);
        }
      }
    }
  }

  // Finally, change the border lines to black
  GSpriteBoardGrid_set_color (spr->grid, 0xFF000000);

  spr->finished = 1;
}

void GSpriteBoard_size_from_desc (const char *desc, int *size_x, int *size_y) {
  if (size_x)
    *size_x = GSpriteBoard_a2i (desc[1]);
  if (size_y)
    *size_y = GSpriteBoard_a2i (desc[2]);
}
