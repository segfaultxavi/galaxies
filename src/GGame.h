#ifndef __GGAME_H__
#define __GGAME_H__

typedef struct _GGame GGame;

extern int ggame_width;
extern int ggame_height;

GGame *GGame_new ();
void GGame_free (GGame *game);
void GGame_run (GGame *game);

#endif