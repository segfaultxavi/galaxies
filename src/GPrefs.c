#include <SDL.h>
#include "GPrefs.h"

#define GPREFS_FILE_NAME "prefs.txt"

void GPrefs_save (GPrefs *prefs) {
  SDL_RWops *rwops;
  char *pfilepath = SDL_GetPrefPath ("qojat", "galaxies");
  int pfilelen = (int)SDL_strlen (pfilepath) + (int)SDL_strlen (GPREFS_FILE_NAME) + 1;
  char *pfile = malloc (pfilelen);

  SDL_snprintf (pfile, pfilelen, "%s%s", pfilepath, GPREFS_FILE_NAME);

  rwops = SDL_RWFromFile (pfile, "w");
  SDL_RWwrite (rwops, pfile, pfilelen, 1);
  SDL_RWclose (rwops);
  SDL_Log ("Preferences saved to %s", pfile);

  free (pfile);
  SDL_free (pfilepath);
}

void GPrefs_load (GPrefs *load) {

}
