/*
 * map_resize.c
 *
 *  Created on: 2010-08-04
 *      Author: wino
 */
#include <stdio.h>

#include <allegro.h>
#include "fpge.h"
#include "map_resize.h"
#include "unitutil.h"

char current_x[64];
char current_y[64];
char edit_x[64];
char edit_y[64];
char edit_off_x[64];
char edit_off_y[64];
char info[128];

int le=MAX_MAP_X*4,lw=MAX_MAP_X*4,ls=MAX_MAP_X*4,ln=MAX_MAP_X*4; //very big values

#define RESIZE_BUTTON_W 96

DIALOG resize_dlg[RESIZE_DLG_SIZE] =
{
   /* (proc)        (x)  (y)  (w)  (h)           (fg)            (bg) (key) (flags) (d1) (d2) (dp)                        (dp2) (dp3) */
   { d_box_proc_my2,96,  80,  240, 152, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   NULL,                       NULL, NULL },
   { d_text_proc,   104, 88,  128, 8,   GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "Resize map",               NULL, NULL },
   { d_text_proc,   104, 136, 80,  8,   GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "New map X",                NULL, NULL },
   { d_text_proc,   104, 152, 80,  8,   GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "New map Y",                NULL, NULL },
   { d_edit_proc,   200, 136, 40,  8,   GUI_FG_COLOR,   GUI_EDIT_COLOR,   0,    0,      4,   0,   "X",                        NULL, NULL },
   { d_edit_proc,   200, 152, 40,  8,   GUI_FG_COLOR,   GUI_EDIT_COLOR,   0,    0,      4,   0,   "Y",                        NULL, NULL },
   { d_text_proc,   104, 104, 128, 8,   GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "Map X : 200",              NULL, NULL },
   { d_text_proc,   104, 120, 128, 8,   GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "Map Y : 200",              NULL, NULL },
   { d_edit_proc,   248, 136, 40,  8,   GUI_FG_COLOR,   GUI_EDIT_COLOR,   0,    0,      4,   0,   "X",                        NULL, NULL },
   { d_edit_proc,   248, 152, 40,  8,   GUI_FG_COLOR,   GUI_EDIT_COLOR,   0,    0,      4,   0,   "Y",                        NULL, NULL },
   { d_text_proc,   248, 120, 48,  8,   GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "offset",                   NULL, NULL },
   { d_button_proc_center, 256-32, 88,  RESIZE_BUTTON_W,  16,  GUI_FG_COLOR,   GUI_BG_COLOR,   'e',    0,      0,   0,   "C&enter",                   NULL, NULL },
   { d_button_proc_resize, 256-32, 200, RESIZE_BUTTON_W,  16,  GUI_FG_COLOR,   GUI_BG_COLOR,   'r',    0,      0,   0,   "&Resize",                   NULL, NULL },
   { d_button_proc, 112, 200, RESIZE_BUTTON_W,  16,  GUI_FG_COLOR,   GUI_BG_COLOR,   'c',    D_EXIT,      0,   0,   "&Cancel",                   NULL, NULL },
   { d_textbox_proc,   104, 168, 224-8, 24,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "E: +2, W: -2, N: 0, S: 1", NULL, NULL },
   { d_yield_proc,  0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                       NULL, NULL },
   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                       NULL, NULL }
};

void update_info(){
	int my_x,my_y,my_ox,my_oy;
	int e,w,s,n;

	my_x=atoi(edit_x);
	my_y=atoi(edit_y);
	my_ox=atoi(edit_off_x);
	my_oy=atoi(edit_off_y);

    //check
	if (abs(my_x)>MAX_MAP_X) my_x=0;
	if (abs(my_y)>MAX_MAP_Y) my_y=0;
	if (abs(my_ox)>MAX_MAP_X) my_ox=0;
	if (abs(my_oy)>MAX_MAP_Y) my_oy=0;

	e = my_ox;
	n = my_oy;
	w = my_x-(mapx+my_ox);
	s = my_y-(mapy+my_oy);

	// 64 - (61 + 3) = 0
	// 64 - 61 = 3

	sprintf(info,"E : %+3d W : %+3d\nN : %+3d S : %+3d",e,w,n,s);
	if (e!=le || w!=lw || n!=ln || s!=ls){
		d_textbox_proc(MSG_DRAW,&(resize_dlg[MapResizeInfoIdx]),0);
		le=e; lw=w; ln=n; ls=s;
	}
}

int d_box_proc_my2(int msg, DIALOG *d, int c){
	//int i;

	if (msg==MSG_START) {

		if (tile_mode == 1) {
			tile_cleanup();
		}

		resize_dlg[MapResizeXIdx].dp=edit_x;
		resize_dlg[MapResizeYIdx].dp=edit_y;
		resize_dlg[MapResizeOffXIdx].dp=edit_off_x;
		resize_dlg[MapResizeOffYIdx].dp=edit_off_y;

		resize_dlg[MapResizeCurrentXIdx].dp=current_x;
		resize_dlg[MapResizeCurrentYIdx].dp=current_y;

		resize_dlg[MapResizeInfoIdx].dp=info;

		sprintf(current_x,"Map X : %d",mapx);
		sprintf(current_y,"Map Y : %d",mapy);
		sprintf(edit_x,"%d",mapx);
		sprintf(edit_y,"%d",mapy);
		sprintf(edit_off_x,"%d",0);
		sprintf(edit_off_y,"%d",0);

		update_info();
	}
	if (msg==MSG_IDLE){
		update_info();
	}

	return d_box_proc(msg,d,c);
}

int d_button_proc_center(int msg, DIALOG *d, int c){
	int ox,oy;
	int my_x,my_y,my_ox,my_oy;

	if ( msg==MSG_CLICK || msg==MSG_KEY) {
		my_x=atoi(edit_x);
		my_y=atoi(edit_y);
		my_ox=atoi(edit_off_x);
		my_oy=atoi(edit_off_y);
	    //check
		if (abs(my_x)>MAX_MAP_X) my_x=0;
		if (abs(my_y)>MAX_MAP_Y) my_y=0;
		if (abs(my_ox)>MAX_MAP_X) my_ox=0;
		if (abs(my_oy)>MAX_MAP_Y) my_oy=0;

		ox = (my_x-mapx)/2;
		oy = (my_y-mapy)/2;
		sprintf(edit_off_x,"%d",ox);
		sprintf(edit_off_y,"%d",oy);

		d_edit_proc(MSG_DRAW,&(resize_dlg[MapResizeOffXIdx]),0);
		d_edit_proc(MSG_DRAW,&(resize_dlg[MapResizeOffYIdx]),0);

		update_info();

		return D_O_K; //do not allow to be pressed
	}

	return d_button_proc(msg,d,c);
}

int d_button_proc_resize(int msg, DIALOG *d, int c){
	int i,x,y,error=0,continue_anyway=1;
	int my_x,my_y,my_ox,my_oy;

	if ( msg==MSG_CLICK || msg==MSG_KEY) {
		my_x=atoi(edit_x);
		my_y=atoi(edit_y);
		my_ox=atoi(edit_off_x);
		my_oy=atoi(edit_off_y);
	    //check
		if (abs(my_x)>MAX_MAP_X) error=1;
		if (abs(my_y)>MAX_MAP_Y) error=2;
		if (abs(my_ox)>MAX_MAP_X) error=3;
		if (abs(my_oy)>MAX_MAP_Y) error=4;

		if (error==0){
			if(alert("Do you really want to resize map ?",
					 "Note that some units may be deleted",
					 "when map will be resized.",
				       "&Yes", "&No", 'y', 'n')==1){
				//check for my_ox%2!=0
				if (my_ox%2)
					if (alert("X offset is not even number !","This will break map.","Continue with map resize ?",
							"&Yes", "&No", 'y', 'n')==2) continue_anyway=0;

				if(continue_anyway) {

				//do resize
				//move X
				if (my_ox != 0) {
					if (my_ox > 0) {
						for (y = 0; y < mapy; y++)
							for (x = mapx - 1; x >= 0; x--)
								if (x + my_ox<MAX_MAP_X)
									map[x + my_ox][y] = map[x][y];
					} else {
						for (y = 0; y < mapy; y++)
							for (x = 0; x < mapx ; x++)
								if (x + my_ox>=0)
									map[x + my_ox][y] = map[x][y];
					}
					//fill with default the rest
					}
					for (y = 0; y < mapy; y++){
						//left
						for(x=0;x<my_ox;x++){
							memset(&map[x][y],0, sizeof(map[x][y]));
							map[x][y].tile=map[my_ox][y].tile;
							map[x][y].guidx=-1;
							map[x][y].auidx=-1;
						}
						//right
						for(x=mapx+my_ox;x<my_x;x++){
							memset(&map[x][y],0, sizeof(map[x][y]));
							map[x][y].tile=map[mapx+my_ox-1][y].tile;
							map[x][y].guidx=-1;
							map[x][y].auidx=-1;
						}
					}

				//move Y
				if (my_oy != 0) {
					if (my_oy > 0) {
						for (x = 0; x < my_x; x++)
							for (y = mapy - 1; y >= 0; y--)
								if (y + my_oy<MAX_MAP_Y)
									map[x][y+my_oy] = map[x][y];
					} else {
						for (x = 0; x < my_x; x++)
							for (y = 0; y < mapy; y++)
								if (y + my_oy>=0)
									map[x][y+my_oy] = map[x][y];
					}
				}
					//fill with default the rest
					for (x = 0; x < my_x; x++){
						//left
						for(y=0;y<my_oy;y++){
							memset(&map[x][y],0, sizeof(map[x][y]));
							map[x][y].tile=map[x][my_oy].tile;
							map[x][y].guidx=-1;
							map[x][y].auidx=-1;
						}
						//right
						for(y=mapy+my_oy;y<my_y;y++){
							memset(&map[x][y],0, sizeof(map[x][y]));
							map[x][y].tile=map[x][mapy+my_oy-1].tile;
							map[x][y].guidx=-1;
							map[x][y].auidx=-1;
						}
					}


				//units
				//move
				for(i=0;i<total_units;i++){
					all_units[i].x +=my_ox;
					all_units[i].y +=my_oy;
				}
				//delete out of map units
				i=0;
				while(i<total_units){
					if ( all_units[i].x<0 || all_units[i].x>=my_x || all_units[i].y<0 || all_units[i].y>=my_y){
						remove_unit_idx(i);
					}else{
						i++;
					}
				}
				//new sizes
				mapx=my_x;
				mapy=my_y;

				return D_CLOSE;
				}//continue_anyway
			}
		}else{
			//error
			alert("ERROR: Wrong parameter(s)", NULL, NULL,"&Continue", NULL, 'c', 0);
		}
	}

	return d_button_proc(msg,d,c);
}

