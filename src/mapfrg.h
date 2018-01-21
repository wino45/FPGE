/*
 * mapfrg.h
 *
 *  Created on: 2010-08-03
 *      Author: wino
 */

#ifndef MAPFRG_H_
#define MAPFRG_H_

#include <allegro.h>

#define MAPFRG_DIALOG_X_BMP (TILE_WIDTH*9+TILE_WIDTH/3)
#define MAPFRG_DIALOG_Y_BMP (TILE_HEIGHT*9+TILE_HEIGHT/2)

#define MapFrgBMPIdx 1
#define MapFrgNextIdx MapFrgBMPIdx+1
#define MapFrgPrevIdx MapFrgNextIdx+1
#define MapFrgCancelIdx MapFrgPrevIdx+1
#define MapFrgSelectIdx MapFrgCancelIdx+1

#define MapFrgRMountainsIdx MapFrgSelectIdx+1
#define MapFrgRIslesIdx MapFrgRMountainsIdx+1
#define MapFrgRFortsIdx MapFrgRIslesIdx+1
#define MapFrgRForestsIdx MapFrgRFortsIdx+1
#define MapFrgRRiversIdx MapFrgRForestsIdx+1
#define MapFrgRLakesIdx MapFrgRRiversIdx+1
#define MapFrgROtherIdx MapFrgRLakesIdx+1
#define MapFrgRRoadIdx MapFrgROtherIdx+1
#define MapFrgRAllIdx MapFrgRRoadIdx+1

#define MapFrgMoveIdx MapFrgRAllIdx+1

extern BITMAP *main_bmp;

#define MAPFRG_DLG_SIZE 18
extern DIALOG mapfrg_dlg[MAPFRG_DLG_SIZE];

struct map_fragment {
		int map_frg_big_table_type;
		int map_frg_big_table_offset;
		int dx;
		int dy;
		short map_frg_big_table[FRG_Y_SIZE][FRG_X_SIZE];
};


//extern short map_frg_big_table[MAX_MAP_FRG][FRG_Y_SIZE][FRG_X_SIZE];
//extern int map_frg_big_table_offset[MAX_MAP_FRG];
//extern int map_frg_big_table_type[MAX_MAP_FRG];
extern struct map_fragment map_frg_big_table_new[MAX_MAP_FRG];
extern int map_frg_big_table_size;
extern short map_frg[FRG_Y_SIZE][FRG_X_SIZE];
extern int map_frg_off;
extern int map_frg_dx;
extern int map_frg_dy;

void sort_map_fragments();
void draw_map_fragment(BITMAP *map_to_draw, int clear_only);
int d_box_proc_my(int msg, DIALOG *d, int c);
int d_button_proc_move(int msg, DIALOG *d, int c);
int d_button_proc_select(int msg, DIALOG *d, int c);
int d_radio_proc_my(int msg, DIALOG *d, int c);
int d_button_proc_move_dlg(int msg, DIALOG *d, int c);
#endif /* MAPFRG_H_ */
