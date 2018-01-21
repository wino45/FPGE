#include <allegro.h>

#include "fpge.h"
#include "maingui.h"
#include "makemap.h"
#include "drawmap.h"
#include "pgf.h"
#include "terr.h"
#include "unitutil.h"
#include "filename.h"

#define MM_FG_COLOR GUI_FG_COLOR
#define MM_BG_COLOR GUI_EDIT_COLOR
#define MM_SCREEN_COLOR GUI_BG_COLOR

#define MM_X  10
#define MM_Y  10
#define MM_LH  15

#define MM_W  160
#define MM_H  MM_LH*8

#define MM_LINE1  MM_Y+10

#define mmMxEditIdx 2
#define mmMyEditIdx 4
#define mmTileEditIdx 6

char mmMXStr[8]="32";
char mmMYStr[8]="32";
char mmTileStr[8]="130";
int mmLastTileID=-5;

int d_mm_start_proc(int msg, DIALOG *d, int c)
{
	int idx;
	BITMAP *tile_to_draw;

   if (msg==MSG_START)
   {
     makemap_dlg[mmMxEditIdx].dp=mmMXStr; 
     makemap_dlg[mmMyEditIdx].dp=mmMYStr;
	 strncpy(mmTileStr,tdTNStr,8);
     makemap_dlg[mmTileEditIdx].dp=mmTileStr;
     mmLastTileID=-5;
	 }

   if (msg==MSG_DRAW) mmLastTileID=-5;

   if (msg==MSG_IDLE ){
	   idx =atoi(mmTileStr);

	   if (idx!=mmLastTileID){
		   //printf("msg=%d idx=%d t=%d\n",msg,idx,tdLastTileID);
		   //print tile
		   if (idx>-1 && idx<total_tiles){
				switch(showWeather){
					case 0:tile_to_draw = til_bmp[idx];break;
					case 1:tile_to_draw = til_bmp_mud[idx];break;
					case 2:tile_to_draw = til_bmp_snow[idx];break;
					default:tile_to_draw = 0;break;
				}

			masked_blit(tile_to_draw,screen,
					0,0,
					makemap_dlg[0].x+10,
					makemap_dlg[0].y+MM_LINE1,
					TILE_FULL_WIDTH,TILE_HEIGHT);
			   mmLastTileID=idx;
		   }
	   }
   }


   return d_textbox_proc(msg,d,c);
}

int d_makemap_proc(int msg, DIALOG *d, int c){
	int i,j,newmapx,newmapy,tile;
	
	//if (msg==MSG_IDLE) rest(100);
	
	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
	{
		if (mapx>0 && mapy>0)
			if (alert("Do you really want to make new map ?", "This action will erase all changes already made", NULL,
					"&Yes", "&No", 'y', 'n')==2) return D_CLOSE;

		newmapx=MAX(MIN(atoi(mmMXStr),MAX_MAP_X),1);
		newmapy=MAX(MIN(atoi(mmMYStr),MAX_MAP_Y),1);
		
		if (newmapx<0 || newmapy < 0) return D_CLOSE;

		tile=atoi(mmTileStr);
		for (i=0;i<newmapx;i++)
			for (j=0;j<newmapy;j++){
					memset(&map[i][j],0, sizeof(map[i][j]));
					map[i][j].tile=tile;
					map[i][j].guidx=-1;
					map[i][j].auidx=-1;					
				}
		mapx=newmapx;
		mapy=newmapy;
		map_x0=0;
		map_y0=0;
		//clear all units
		clear_all_units();

		get_victory_hexes();
		//clean scenario name
		setScenarioNameRaw("");
		setScenarioNumber(0);

		main_dlg[dmHSlideIdx].d1=get_h_slide_max();
		main_dlg[dmHSlideIdx].d2=0;
		vslide_max=get_v_slide_max();
		main_dlg[dmVSlideIdx].d1=vslide_max;
		main_dlg[dmVSlideIdx].d2=vslide_max;
		//in case of PGF fill victory conditions
		if (pgf_mode){
			block7_lines=0;
			strncpy(block7[block7_lines],"AXIS VICTORY\t1\t0",MAX_LINE_SIZE);
			block7_lines++;
			strncpy(block7[block7_lines],"ALLIED VICTORY\t-1\t-1",MAX_LINE_SIZE);
			block7_lines++;

			if (scn_buffer[TURNS]==0){
				scn_buffer[TURNS]=1;

				for(i=0;i<scn_buffer[TURNS];i++){
					sprintf(block4[i],"%d\t%d\t%d\n",i+1,0,0);
				}
				block4_lines=scn_buffer[TURNS];
			}
		}
		//finally draw map
		draw_map(map_bmp,map_x0,map_y0,tiles_high,tiles_wide);
		return D_CLOSE;
	}
	return d_button_proc(msg,d,c);
}

DIALOG makemap_dlg[MAKEMAP_DLG_SIZE] = {
// 0
	{ d_mm_start_proc, MM_X - 70, MM_Y, MM_W + 70, MM_H, MM_FG_COLOR,MM_SCREEN_COLOR, 0, 0, 0, 0, 0, 0, 0 },
	{ d_text_proc, MM_X + MM_W / 2 - 50 - 10, MM_LINE1 + MM_LH * 0, 50,	MM_Y, MM_FG_COLOR, MM_SCREEN_COLOR, 0, 0, 0, 0,	(void *) "Map X", 0, 0 },
	{ d_edit_proc, MM_X + MM_W / 2 + 10,MM_LINE1 + MM_LH * 0, 50, MM_LH, MM_FG_COLOR, MM_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_text_proc, MM_X + MM_W / 2 - 50 - 10,MM_LINE1 + MM_LH * 1, 50, MM_LH, MM_FG_COLOR, MM_SCREEN_COLOR,	0, 0, 0, 0, (void *) "Map Y", 0, 0 },
	{ d_edit_proc, MM_X	+ MM_W / 2 + 10, MM_LINE1 + MM_LH * 1, 50, MM_LH, MM_FG_COLOR,MM_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_text_proc, MM_X + MM_W / 2 - 50 - 10, MM_LINE1 + MM_LH * 2, 50,	MM_LH, MM_FG_COLOR, MM_SCREEN_COLOR, 0, 0, 0, 0,(void *) "Tile", 0, 0 },
	{ d_edit_proc, MM_X + MM_W / 2 + 10, MM_LINE1 + MM_LH * 2, 50, MM_LH, MM_FG_COLOR, MM_BG_COLOR, 0, 0,4, 0, 0, 0, 0 },
	{ d_makemap_proc, MM_X-70+MM_W+70  - 96 - 10, MM_Y+ MM_H - MM_LH * 2, 96, MM_LH, MM_FG_COLOR, MM_SCREEN_COLOR,'m', 0, 0, 0, (void *) "&Make", 0, 0 },
	{ d_button_proc, MM_X+10-70 , MM_Y + MM_H - MM_LH * 2, 96, MM_LH,MM_FG_COLOR, MM_SCREEN_COLOR, 'c', D_EXIT, 0, 0,(void *) "&Cancel", 0, 0 },
	{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
	//Last
	{NULL},
};
