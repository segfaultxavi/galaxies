#ifndef __GPREFS__
#define __GPREFS__

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GPrefs GPrefs;

#define GPREFS_MAX_LEVELS 50

struct _GPrefs {
  int audio;
  int music;
  int num_levels;
  int level_status[GPREFS_MAX_LEVELS];
  char *level_desc[GPREFS_MAX_LEVELS];
  char *editor_desc;
};

void GPrefs_save (GPrefs *prefs);
void GPrefs_load (GPrefs *prefs);
void GPrefs_free (GPrefs *prefs);

#ifdef __cplusplus
}
#endif

#endif