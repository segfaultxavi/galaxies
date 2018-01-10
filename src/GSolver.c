#include <SDL.h>
#include "GSolver.h"

struct _GSolver {
  int num_solutions;
  GSolverSolution *solutions; // List
};

GSolver *GSolver_new (GSpriteBoard *board) {
  GSolver *solver = SDL_malloc (sizeof (GSolver));
  solver->num_solutions = 0;
  solver->solutions = NULL;

  return solver;
}

void GSolver_free (GSolver *solver) {
  // Cancel thread and wait for it
  SDL_free (solver);
}

int GSolver_get_num_solutions (GSolver *solver) {
  solver->num_solutions++;
  return solver->num_solutions;
}

GSolverSolution *GSolver_get_solution (GSolver *solver, int ndx) {
  return NULL;
}