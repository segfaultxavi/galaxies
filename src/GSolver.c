#include <SDL.h>
#include "GSolver.h"
#include "GSpriteCore.h"
#include "GSpriteTile.h"

struct _GSolver {
  int num_cores;
  GSpriteCore **cores;
  int map_size_x, map_size_y;
  char *tile_flags;
  char *initial_candidates;
  char *state;
  char *current_option;
  Uint64 total_options;
  Uint64 explored_options;
  int num_solutions;
  char **solutions;
  SDL_Thread *worker_thread;
  int quit;
};

#define GSOLVER_TILE_FLAG(solver,x,y) solver->tile_flags[(y) * solver->map_size_x + (x)]
#define GSOLVER_STATE(solver,x,y) solver->state[(y) * solver->map_size_x + (x)]
#define GSOLVER_CURRENT_OPTION(solver,x,y) solver->current_option[(y) * solver->map_size_x + (x)]
#define GSOLVER_CANDIDATE_RAW(solver,x,y,c) solver->initial_candidates[((y) * solver->map_size_x + (x)) * (1 + solver->num_cores) + (c)]
#define GSOLVER_CANDIDATE_LEN(solver,x,y) GSOLVER_CANDIDATE_RAW(solver,x,y,0)
#define GSOLVER_CANDIDATE(solver,x,y,c) GSOLVER_CANDIDATE_RAW(solver,x,y,(c) + 1)

void GSolver_free (GSolver *solver) {
  int i;
  if (solver->worker_thread) {
    solver->quit = 1;
    SDL_WaitThread (solver->worker_thread, NULL);
  }
  for (i = 0; i < solver->num_solutions; i++)
    SDL_free (solver->solutions[i]);
  SDL_free (solver->solutions);
  SDL_free (solver->current_option);
  SDL_free (solver->state);
  SDL_free (solver->initial_candidates);
  SDL_free (solver->tile_flags);
  for (i = 0; i < solver->num_cores; i++)
    GSprite_free ((GSprite *)solver->cores[i]);
  SDL_free (solver->cores);
  SDL_free (solver);
}

int GSolver_set_candidate (GSolver *solver, int x, int y, int c) {
  int id = GSOLVER_CANDIDATE (solver, x, y, c);
  int old_id = GSOLVER_CURRENT_OPTION (solver, x, y);

  // If this tile is not fixed, set it to this core id
  if ((GSOLVER_TILE_FLAG (solver, x, y) & GTILE_FLAG_FIXED) == 0) {
    GSpriteCore *core = solver->cores[id];
    int oppx, oppy;
    if (old_id > -1) {
      // Remove previous opposite, if there was one
      GSpriteCore *old_core = solver->cores[old_id];
      int old_oppx, old_oppy;
      GSpriteCore_get_opposite (old_core, x, y, &old_oppx, &old_oppy);
      if (GSOLVER_CURRENT_OPTION (solver, old_oppx, old_oppy) == old_id) {
        GSOLVER_STATE (solver, old_oppx, old_oppy) = 0; // This tile is now unlocked
      } // FIXME: else? assert?
    }
    GSpriteCore_get_opposite (core, x, y, &oppx, &oppy);
    if (GSOLVER_STATE (solver, oppx, oppy) == -1) {
      // This candidate core cannot be used: the opposite tile is locked
      return 0;
    }
    GSOLVER_CURRENT_OPTION (solver, x, y) = id;
    GSOLVER_STATE (solver, x, y) = c;
    GSOLVER_CURRENT_OPTION (solver, oppx, oppy) = id;
    GSOLVER_STATE (solver, oppx, oppy) = -1; // The opposite tile is locked until we detach this tile from this code
  }

  // Find next movable tile (= unlocked)
  do {
    x++;
    if (x >= solver->map_size_x) {
      x = 0;
      y++;
    }
  } while (y < solver->map_size_y && GSOLVER_STATE (solver, x, y) == -1);
  if (y < solver->map_size_y) {
    // If we found one, keep iterating
    int i = 0;
    while ((i < GSOLVER_CANDIDATE_LEN (solver, x, y)) && (GSolver_set_candidate (solver, x, y, i) == 0)) i++;
    return i < GSOLVER_CANDIDATE_LEN (solver, x, y);
  }
  return 1;
}

static void GSolver_output_current_solution (GSolver *solver) {
  int x, y;
  for (y = 0; y < solver->map_size_y; y++) {
    char buff[128];
    for (x = 0; x < solver->map_size_x; x++) {
      buff[x] = '0' + GSOLVER_CURRENT_OPTION (solver, x, y);
    }
    buff[x] = '\0';
    SDL_Log ("Solver:Option %3ld:%s", solver->total_options, buff);
  }
}

static void GSolver_output_state (GSolver *solver) {
  int x, y;
  for (y = 0; y < solver->map_size_y; y++) {
    char buff[128];
    for (x = 0; x < solver->map_size_x; x++) {
      buff[x] = '0' + GSOLVER_STATE (solver, x, y);
    }
    buff[x] = '\0';
    SDL_Log ("Solver:State:%s", buff);
  }
}

static void GSolver_output_initial_candidates (GSolver *solver) {
  int x, y, i;
  for (y = 0; y < solver->map_size_y; y++) {
    char buff[128];
    SDL_memset (buff, '.', sizeof (buff));
    for (x = 0; x < solver->map_size_x; x++) {
      for (i = 0; i < GSOLVER_CANDIDATE_LEN (solver, x, y); i++) {
        buff[x * solver->num_cores + i] = '0' + GSOLVER_CANDIDATE (solver, x, y, i);
      }
    }
    buff[x * solver->num_cores] = '\0';
    SDL_Log ("Solver:Initial:%s", buff);
  }
}

static int GSolver_next_configuration (GSolver *solver) {
  int repeat = 0;
  int curr_x = solver->map_size_x - 1;
  int curr_y = solver->map_size_y - 1;

  GSolver_output_state (solver);
  do {
    // Search backwards for the first unlocked tile with untried candidates (if there is one)
    while ((GSOLVER_STATE (solver, curr_x, curr_y) == -1) ||
      (GSOLVER_STATE (solver, curr_x, curr_y) == GSOLVER_CANDIDATE_LEN (solver, curr_x, curr_y) - 1)) {
      curr_x--;
      if (curr_x < 0) {
        curr_x = solver->map_size_x - 1;
        curr_y--;
        if (curr_y < 0) return 0;
      }
    }
    GSOLVER_STATE (solver, curr_x, curr_y)++;
    // Set the tile to its next candidate, if possible. Otherwise, iterate.
  } while (GSolver_set_candidate (solver, curr_x, curr_y, GSOLVER_STATE (solver, curr_x, curr_y)) == 0);

  return 1;
}

static void GSolver_flood_fill (GSolver *solver, int sx, int sy, int id) {
  if (GSOLVER_CURRENT_OPTION (solver, sx, sy) != id) return;
  if ((GSOLVER_TILE_FLAG (solver, sx, sy) & GTILE_FLAG_VISITED) > 0) return;
  GSOLVER_TILE_FLAG (solver, sx, sy) |= GTILE_FLAG_VISITED;

  if (sx > 0) GSolver_flood_fill (solver, sx - 1, sy, id);
  if (sx < solver->map_size_x - 1) GSolver_flood_fill (solver, sx + 1, sy, id);
  if (sy > 0) GSolver_flood_fill (solver, sx, sy - 1, id);
  if (sy < solver->map_size_y - 1) GSolver_flood_fill (solver, sx, sy + 1, id);
}

static void GSolver_check_current_configuration (GSolver *solver) {
  int i, x, y;
  char *sol;
  solver->explored_options++;

  for (i = 0; i < solver->num_cores; i++) {
    int sx, sy;

    // Clear flag
    for (y = 0; y < solver->map_size_y; y++) {
      for (x = 0; x < solver->map_size_x; x++) {
        GSOLVER_TILE_FLAG (solver, x, y) &= ~GTILE_FLAG_VISITED;
      }
    }

    // Fill all tiles connected to the core
    GSpriteCore_get_corner (solver->cores[i], &sx, &sy);
    GSolver_flood_fill (solver, sx, sy, i);

    // Detect unconnected tiles
    for (y = 0; y < solver->map_size_y; y++) {
      for (x = 0; x < solver->map_size_x; x++) {
        if ((GSOLVER_CURRENT_OPTION (solver, x, y) == i) &&
            ((GSOLVER_TILE_FLAG (solver, x, y) & GTILE_FLAG_VISITED) == 0))
          return;
      }
    }
  }

  // This is a solution: copy it
  sol = SDL_malloc (solver->map_size_x * solver->map_size_y);
  solver->num_solutions++;
  solver->solutions = SDL_realloc (solver->solutions, sizeof (char *) * solver->num_solutions);
  solver->solutions[solver->num_solutions - 1] = sol;
  for (y = 0; y < solver->map_size_y; y++) {
    for (x = 0; x < solver->map_size_x; x++) {
      sol[y * solver->map_size_x + x] = GSOLVER_CURRENT_OPTION (solver, x, y);
    }
  }
}

static void GSolver_reset_state (GSolver *solver) {
  int x, y;
  for (y = 0; y < solver->map_size_y; y++) {
    for (x = 0; x < solver->map_size_x; x++) {
      GSOLVER_STATE (solver, x, y) = 0;
      if ((GSOLVER_TILE_FLAG (solver, x, y) & GTILE_FLAG_FIXED) == 0) {
        GSOLVER_CURRENT_OPTION (solver, x, y) = -1;
      }
    }
  }
}

static int GSolver_worker_thread (GSolver *solver) {

  Uint32 timestamp = SDL_GetTicks ();

  // Phase 1: Count number of options
  GSolver_set_candidate (solver, 0, 0, 0);
  solver->total_options = 0;
  SDL_Log ("Solver:Counting options");
  while (!solver->quit) {
    GSolver_output_current_solution (solver);
    solver->total_options++;
    if (!GSolver_next_configuration (solver))
      break;
  }
  SDL_Log ("Solver:Total options: %ld", solver->total_options);

  // Phase 2: Check every option
  GSolver_reset_state (solver);
  GSolver_set_candidate (solver, 0, 0, 0);
  solver->explored_options = 0;
  SDL_Log ("Solver:Checking solutions");
  while (!solver->quit) {
    GSolver_check_current_configuration (solver);
    if (!GSolver_next_configuration (solver))
      break;
  }
  SDL_Log ("Solver:Solutions found: %d", solver->num_solutions);
  timestamp = SDL_GetTicks () - timestamp;
  SDL_Log ("Solver:Time spent: %dms", timestamp);
  if (timestamp != 0)
    SDL_Log ("Solver:Speed: %f options/s", 1000 * solver->total_options / (double)timestamp);
  else
    SDL_Log ("Solver:Speed: inaccurate");

  solver->quit = 1;
  return 0;
}

static void GSolver_push_candidate (GSolver *solver, int x, int y, int id) {
  int len = GSOLVER_CANDIDATE_LEN (solver, x, y);
  GSOLVER_CANDIDATE (solver, x, y, len) = id;
  len++;
  GSOLVER_CANDIDATE_LEN (solver, x, y) = len;
}

GSolver *GSolver_new (GSpriteBoard *board) {
  int i, x, y;
  GSolver *solver = SDL_malloc (sizeof (GSolver));
  SDL_memset (solver, 0, sizeof (GSolver));

  // The solver must be independent of everything else (to simplify threading)
  // so we start by copying all the input data we need.
  solver->num_cores = GSpriteBoard_get_num_cores (board);
  solver->cores = SDL_malloc (solver->num_cores * sizeof (GSpriteCore *));
  for (i = 0; i < solver->num_cores; i++)
    solver->cores[i] = GSpriteCore_clone (GSpriteBoard_get_core (board, i));

  solver->map_size_x = GSpriteBoard_get_map_size_x (board);
  solver->map_size_y = GSpriteBoard_get_map_size_y (board);
  solver->tile_flags = SDL_malloc (solver->map_size_x * solver->map_size_y);
  solver->initial_candidates = SDL_malloc (solver->map_size_x * solver->map_size_y * (solver->num_cores + 1));
  solver->state = SDL_malloc (solver->map_size_x * solver->map_size_y);
  solver->current_option = SDL_malloc (solver->map_size_x * solver->map_size_y);
  for (y = 0; y < solver->map_size_y; y++) {
    for (x = 0; x < solver->map_size_x; x++) {
      GSpriteTile *tile = GSpriteBoard_get_tile (board, x, y);
      GSOLVER_TILE_FLAG (solver, x, y) = GSpriteTile_get_flags (tile);
      GSOLVER_STATE (solver, x, y) = 0;
      GSOLVER_CANDIDATE_LEN (solver, x, y) = 0;
      if (GSOLVER_TILE_FLAG (solver, x, y) & GTILE_FLAG_FIXED) {
        GSolver_push_candidate (solver, x, y, GSpriteTile_get_id (tile));
        GSOLVER_CURRENT_OPTION (solver, x, y) = GSpriteTile_get_id (tile);
      } else {
        for (i = 0; i < solver->num_cores; i++) {
          int oppx, oppy;
          GSpriteCore_get_opposite (solver->cores[i], x, y, &oppx, &oppy);
          if (oppx < 0 || oppx >= solver->map_size_x || oppy < 0 || oppy >= solver->map_size_y) continue;
          if (GSpriteTile_get_flags (GSpriteBoard_get_tile (board, oppx, oppy)) & GTILE_FLAG_FIXED) continue;
          GSolver_push_candidate (solver, x, y, i);
        }
        if (GSOLVER_CANDIDATE_LEN (solver, x, y) == 0) goto no_solution;
        GSOLVER_CURRENT_OPTION (solver, x, y) = -1;
      }
    }
  }

  GSolver_output_initial_candidates (solver);

  solver->num_solutions = 0;
  solver->solutions = NULL;
  solver->total_options = 0;
  solver->explored_options = 0;

  solver->quit = 0;
  solver->worker_thread = SDL_CreateThread ((SDL_ThreadFunction)GSolver_worker_thread, "Solver", solver);

  return solver;

no_solution:
  GSolver_free (solver);
  return NULL;
}

int GSolver_get_num_solutions (GSolver *solver) {
  return solver->num_solutions;
}

char *GSolver_get_solution (GSolver *solver, int ndx) {
  if (ndx < 0 || ndx >= solver->num_solutions) return NULL;
  return solver->solutions[ndx];
}

float GSolver_get_progress (GSolver *solver) {
  if (solver->quit) return 1.f;
  if (solver->total_options == 0)
    return 0.f;
  return (float)(solver->explored_options / solver->total_options);
}