#ifndef __GGAME_H__
#define __GGAME_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GGame GGame;

GGame *GGame_new ();
void GGame_free (GGame *game);
void GGame_run (GGame *game);

#ifdef __cplusplus
}
#endif

#endif