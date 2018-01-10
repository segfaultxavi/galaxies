#ifndef __GSOLVER__
#define __GSOLVER__

#include <SDL.h>
#include "GSpriteBoard.h"

typedef struct _GSolver GSolver;
typedef struct _GSolverSolution GSolverSolution;

GSolver *GSolver_new (GSpriteBoard *board);
void GSolver_free (GSolver *solver);
int GSolver_get_num_solutions (GSolver *solver);
GSolverSolution *GSolver_get_solution (GSolver *solver, int ndx);

#endif