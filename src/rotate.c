/*
 * rotate.c
 *
 *  Created on: 2011-01-11
 *      Author: wino
 */
#include <stdio.h>
#include "fpge.h"
#include "tables.h"
#include "callback.h"
#include "fpge_colors.h"

#define ROT_ROT90  1
#define ROT_ROT180 2
#define ROT_ROT270 3
#define ROT_ROTATE 5
#define ROT_FILTER 6

DIALOG rotate_dlg[] =
{
   /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)      (dp2) (dp3) */
   { d_textbox_proc,  32,  48,  240, 136,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   NULL,     NULL, NULL },
   { d_radio_proc,  48,  64,  104, 16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    D_SELECTED,      1,   0,   "90 CW",  NULL, NULL },
   { d_radio_proc,  48,  88,  104, 16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      1,   0,   "180 CW", NULL, NULL },
   { d_radio_proc,  48,  112, 104, 16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      1,   0,   "270 CW", NULL, NULL },
   { d_button_proc, 48,  152, 96,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   'c',    D_EXIT,      0,   0,   "&Cancel", NULL, NULL },
   { d_button_proc, 160, 152, 96,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   'r',    D_EXIT,      0,   0,   "&Rotate", NULL, NULL },
   { d_check_proc,  160, 64,  96,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   'f',    0,      1,   0,   "&Filter", NULL, NULL },
   {d_yield_proc,   0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,     NULL, NULL },
   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,     NULL, NULL }
};

int rotate_map_dialog() {
	int x,y,tmp,dest_x,dest_y;

	centre_dialog(rotate_dlg);
	if (do_dialog(rotate_dlg, -1) == ROT_ROTATE) {
		if (tile_mode == 1) {
			tile_cleanup();
		}
		//rotate
		struct MAP *tempmap = (struct MAP *)malloc(sizeof(struct MAP)*mapx*mapy);

		for(y=0;y<mapy;y++)
			for(x=0;x<mapx;x++){
				if(rotate_dlg[ROT_FILTER].flags&D_SELECTED){

					if (TTData_Max_Tiles[map[x][y].tile] > -1)
						map[x][y].utr = TTData_Max_Tiles[map[x][y].tile];

					if (map[x][y].utr==9 || map[x][y].utr==10|| map[x][y].utr==11){ //river
						map[x][y].tile=river_check; //standard river tile
						map[x][y].rc=0;
					}
					if(map[x][y].rc!=0){ //if road, then change to standard road
						map[x][y].tile=road_check;
						map[x][y].rc=0;
					}
					if (map[x][y].utr==4 || map[x][y].utr==5|| map[x][y].utr==6){ //coast
						map[x][y].tile=coast_check; //standard ocean tile
					}
					//more coast
					if (map[x][y].utr==1){
						map[x][y].tile=tiles_for_bmp[6][0]; //clear
					}
				}
				memcpy(tempmap+x+mapx*y,&map[x][y],sizeof(struct MAP));
			}

		for (y = 0; y < mapy; y++)
			for (x = 0; x < mapx; x++) {
				dest_x=-1; //just in case of GUI problem
				dest_y=-1; //

				if(rotate_dlg[ROT_ROT90].flags&D_SELECTED){
					dest_x=mapy - (y + 1);
					dest_y=x;
				}
				if(rotate_dlg[ROT_ROT180].flags&D_SELECTED){
					dest_x=mapx - (x + 1);
					dest_y=mapy - (y + 1);
				}
				if(rotate_dlg[ROT_ROT270].flags&D_SELECTED){
					dest_x=y;
					dest_y=mapx - (x + 1);
				}

				if (dest_x != -1)
					memcpy(&map[dest_x][dest_y], tempmap + x + mapx * y,
							sizeof(struct MAP));
			}

		//swap mapx,mapy only when rotating 90 or 270
		if ((rotate_dlg[ROT_ROT90].flags&D_SELECTED)||(rotate_dlg[ROT_ROT270].flags&D_SELECTED)){
			tmp=mapx;
			mapx=mapy;
			mapy=tmp;
		}
		position_gui_elements(w_old,h_old);

		free((void *)tempmap);
	}
	return D_REDRAW;
}
