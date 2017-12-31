#ifndef __GPREFS__
#define __GPREFS__

#include <SDL.h>

typedef struct _GPrefs GPrefs;

#define GPREFS_MAX_LEVELS 5

struct _GPrefs {
  int num_levels;
  int levels[GPREFS_MAX_LEVELS];
};

void GPrefs_save (GPrefs *prefs);
void GPrefs_load (GPrefs *load);

#endif