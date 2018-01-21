/*
 * terr.h
 *
 *  Created on: 2010-07-10
 *      Author: wino
 */

#ifndef TERR_H_
#define TERR_H_

/*
#define TD_BG_COLOR 33
#define TD_FG_COLOR 15
#define TD_SCREEN_COLOR 88
*/
#define TD_EDIT_COLOR GUI_EDIT_COLOR
#define TD_BG_COLOR GUI_BG_COLOR
#define TD_FG_COLOR GUI_FG_COLOR
#define TD_SCREEN_COLOR GUI_BG_COLOR

#define TERRAIN_DLG_SIZE 45

extern DIALOG terrain_dlg[TERRAIN_DLG_SIZE];

extern char tdTTStr[8];
extern char tdRDStr[8];
extern char tdTNStr[8];
extern char tdGLNStr[8];
extern char tdSDStr[8];
extern char tdTRStr[8];

extern int tdTTStrOn;
extern int tdRDStrOn;
extern int tdTNStrOn;
extern int tdGLNStrOn;
extern int tdSDStrOn;

extern char tdTTStrMatch[8];
extern char tdRDStrMatch[8];
extern char tdTNStrMatch[8];
extern char tdGLNStrMatch[8];
extern char tdSDStrMatch[8];

extern int tdTTStrMatchOn;
extern int tdRDStrMatchOn;
extern int tdTNStrMatchOn;
extern int tdGLNStrMatchOn;
extern int tdSDStrMatchOn;

extern int tdMatchMatchOn;

extern int GUI_fill_default_tt;

int do_findname_dlg();

#endif /* TERR_H_ */
