#include <SDL.h>
#include "GPrefs.h"

#define GPREFS_FILE_NAME "prefs.txt"
#define GPREFS_FILE_VERSION 1

void GPrefs_save (GPrefs *prefs) {
  SDL_RWops *rwops;
  char buff[512];
  char *pfilepath = SDL_GetPrefPath ("qojat", "galaxies");
  int l;

  SDL_snprintf (buff, sizeof(buff), "%s%s", pfilepath, GPREFS_FILE_NAME);
  SDL_Log ("Saving preferences to %s", buff);

  rwops = SDL_RWFromFile (buff, "w");

  SDL_snprintf (buff, sizeof (buff), "preferences %d\n", GPREFS_FILE_VERSION);
  SDL_RWwrite (rwops, buff, SDL_strlen (buff), 1);
  SDL_snprintf (buff, sizeof (buff), "num_levels %d\n", prefs->num_levels);
  SDL_RWwrite (rwops, buff, SDL_strlen (buff), 1);
  for (l = 0; l < prefs->num_levels; l++) {
    SDL_snprintf (buff, sizeof (buff), "level%03d.status %d\n", l, prefs->level_status[l]);
    SDL_RWwrite (rwops, buff, SDL_strlen (buff), 1);
    if (prefs->level_desc[l]) {
      SDL_snprintf (buff, sizeof (buff), "level%03d.desc %s\n", l, prefs->level_desc[l]);
      SDL_RWwrite (rwops, buff, SDL_strlen (buff), 1);
    }
  }

  SDL_RWclose (rwops);

  SDL_free (pfilepath);
}

void GPrefs_load (GPrefs *prefs) {
  SDL_RWops *rwops;
  char *buff = NULL, *ptr;
  Sint64 size;
  char *pfilepath = SDL_GetPrefPath ("qojat", "galaxies");
  int l, i;

  SDL_memset (prefs, 0, sizeof (*prefs));

  buff = SDL_malloc (512);
  SDL_snprintf (buff, 512, "%s%s", pfilepath, GPREFS_FILE_NAME);
  SDL_Log ("Loading preferences from %s", buff);

  rwops = SDL_RWFromFile (buff, "r");
  if (!rwops) {
    SDL_Log ("File does not exist and will be created");
    goto exit;
  }
  SDL_free (buff);

  size = SDL_RWsize (rwops);
  buff = SDL_malloc (size + 1);
  SDL_memset (buff, 0, size + 1);
  SDL_RWread (rwops, buff, size, 1);
  SDL_RWclose (rwops);

  ptr = buff;
  if (SDL_sscanf (ptr, "preferences %d\n", &i) != 1) {
    SDL_Log ("Corrupted file in line: %s", ptr);
    goto exit;
  }
  if (i != GPREFS_FILE_VERSION) {
    SDL_Log ("File version %d is too old, expected %d", i, GPREFS_FILE_VERSION);
    goto exit;
  }
  while (ptr < buff + size && ptr[0] != '\n') ptr++;
  ptr++;
  while (ptr < buff + size) {
    if (SDL_sscanf (ptr, " num_levels %d\n", &i) == 1) {
      prefs->num_levels = i;
    } else if (SDL_sscanf (ptr, " level%d.status %d\n", &l, &i) == 2) {
      prefs->level_status[l] = i;
    } else if (SDL_sscanf (ptr, " level%d.desc ", &l) == 1) {
      char *ptr_cr;
      ptr += 14;
      ptr_cr = ptr;
      while (ptr_cr < buff + size && ptr_cr[0] != '\n') ptr_cr++;
      prefs->level_desc[l] = SDL_malloc (ptr_cr - ptr + 1);
      SDL_strlcpy (prefs->level_desc[l], ptr, ptr_cr - ptr + 1);
    } else {
      SDL_Log ("Could not understand line: %s\n", ptr);
      goto exit;
    }
    while (ptr < buff + size && ptr[0] != '\n') ptr++;
    ptr++;
  }

exit:
  SDL_free (pfilepath);
  SDL_free (buff);
}

void GPrefs_free (GPrefs *prefs) {
  int l;

  for (l = 0; l < prefs->num_levels; l++) {
    if (prefs->level_desc[l])
      SDL_free (prefs->level_desc[l]);
  }
}
