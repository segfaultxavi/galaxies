#ifndef __GSOLVER__
#define __GSOLVER__

#include <SDL.h>
#include "GSpriteBoard.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GSolver GSolver;

GSolver *GSolver_new (GSpriteBoard *board);
void GSolver_free (GSolver *solver);

// From 0 to 1
float GSolver_get_progress (GSolver *solver);
int GSolver_get_num_solutions (GSolver *solver);
// Solution is a map_size_x * map_size_y array of core IDs
// Do not free
char *GSolver_get_solution (GSolver *solver, int ndx);

#ifdef __cplusplus
}
#endif

#endif