/* SEAREP.C handles input for Search and Replace */

#include <stdio.h>
#include <allegro.h>
#include "fpge.h"
#include "searep.h"
#include "drawmap.h"
#include "undo.h"

char srTTStr[8]="0";
char srRDStr[8]="0";
char srTNStr[8]="0";
char srGLNStr[8]="0";
char srSDStr[8]="0";
char srrTTStr[8]="0";
char srrRDStr[8]="0";
char srrTNStr[8]="0";
char srrGLNStr[8]="0";
char srrSDStr[8]="0";

int srTTStrOn=1;
int srRDStrOn=1;
int srTNStrOn=1;
int srGLNStrOn=1;
int srSDStrOn=1;

/*
#define SR_BG_COLOR 33
#define SR_FG_COLOR 15
#define SR_SCREEN_COLOR 88
*/
#define SR_BG_COLOR GUI_EDIT_COLOR
#define SR_FG_COLOR GUI_FG_COLOR
#define SR_SCREEN_COLOR GUI_BG_COLOR


#define SR_X  1
#define SR_Y  1
#define SR_LH  15
#define SR_LINE1  SR_Y+10+SR_LH

#define srTypeEditIdx 3
#define srRoadEditIdx 5
#define srTileEditIdx 7
#define srNameEditIdx 9
#define srSideEditIdx 11
#define srTypeEditIdxCheck srSideEditIdx+1
#define srRoadEditIdxCheck srTypeEditIdxCheck+1
#define srTileEditIdxCheck srRoadEditIdxCheck+1
#define srNameEditIdxCheck srTileEditIdxCheck+1
#define srSideEditIdxCheck srNameEditIdxCheck+1
#define srrTypeEditIdx srSideEditIdxCheck+1
#define srrRoadEditIdx srrTypeEditIdx+1
#define srrTileEditIdx srrRoadEditIdx+1
#define srrNameEditIdx srrTileEditIdx+1
#define srrSideEditIdx srrNameEditIdx+1
#define srRoadButton     srrSideEditIdx+1

int d_sr_start_proc(int msg, DIALOG *d, int c)
{
	//int mask;
	if (msg == MSG_START) {
		if (tile_mode == 1) {
			tile_cleanup();
		}

		searchreplace_dlg[srTypeEditIdx].dp = srTTStr;
		searchreplace_dlg[srRoadEditIdx].dp = srRDStr;
		searchreplace_dlg[srTileEditIdx].dp = srTNStr;
		searchreplace_dlg[srNameEditIdx].dp = srGLNStr;
		searchreplace_dlg[srSideEditIdx].dp = srSDStr;
		searchreplace_dlg[srrTypeEditIdx].dp = srrTTStr;
		searchreplace_dlg[srrRoadEditIdx].dp = srrRDStr;
		searchreplace_dlg[srrTileEditIdx].dp = srrTNStr;
		searchreplace_dlg[srrNameEditIdx].dp = srrGLNStr;
		searchreplace_dlg[srrSideEditIdx].dp = srrSDStr;

		if (srTTStrOn)
			searchreplace_dlg[srTypeEditIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srTypeEditIdxCheck].flags &= ~D_SELECTED;
		if (srRDStrOn)
			searchreplace_dlg[srRoadEditIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srRoadEditIdxCheck].flags &= ~D_SELECTED;
		if (srTNStrOn)
			searchreplace_dlg[srTileEditIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srTileEditIdxCheck].flags &= ~D_SELECTED;
		if (srGLNStrOn)
			searchreplace_dlg[srNameEditIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srNameEditIdxCheck].flags &= ~D_SELECTED;
		if (srSDStrOn)
			searchreplace_dlg[srSideEditIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srSideEditIdxCheck].flags &= ~D_SELECTED;

	}
	return d_textbox_proc(msg, d, c);
}

int d_sr_coreT_proc(int msg, DIALOG *d, int c)
{
   if ((msg==MSG_CLICK)||(msg==MSG_KEY))
   {
		int y= d_check_proc(msg,d,c);
		if (&(searchreplace_dlg[srTypeEditIdxCheck])==d) srTTStrOn = ((searchreplace_dlg[srTypeEditIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srRoadEditIdxCheck])==d) srRDStrOn = ((searchreplace_dlg[srRoadEditIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srTileEditIdxCheck])==d) srTNStrOn = ((searchreplace_dlg[srTileEditIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srNameEditIdxCheck])==d) srGLNStrOn = ((searchreplace_dlg[srNameEditIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srSideEditIdxCheck])==d) srSDStrOn = ((searchreplace_dlg[srSideEditIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);		
		return y;
   }
   return d_check_proc(msg,d,c);
}

int d_sr_replace_button_proc(int msg, DIALOG *d, int c)
{
	int x,y,found,global_found=0;

    
	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
	{
	d_button_proc(msg,d,c);
	d->flags^=D_SELECTED;

	global_found=0;
    for (y=0; y<mapy; ++y)
		for (x=0; x<mapx; ++x){
			//first check
			found=0;
			if ((srTTStrOn)&&(map[x][y].utr==atoi(srTTStr))) found=1;
			if ((srRDStrOn)&&(map[x][y].rc==atoi(srRDStr))) found=1;
			if ((srTNStrOn)&&(map[x][y].tile==atoi(srTNStr))) found=1;
			if ((srGLNStrOn)&&(map[x][y].gln==atoi(srGLNStr))) found=1;
			if ((srSDStrOn)&&(map[x][y].side==atoi(srSDStr))) found=1;
			if (found && global_found==0){
				global_found=1;
				open_push_undo_sequenece();
			}
			if (found){
				push_undo_tile(x,y);
				if ((srTTStrOn)&&(map[x][y].utr==atoi(srTTStr))) map[x][y].utr=atoi(srrTTStr);
				if ((srRDStrOn)&&(map[x][y].rc==atoi(srRDStr))) map[x][y].rc=atoi(srrRDStr);
				if ((srTNStrOn)&&(map[x][y].tile==atoi(srTNStr))) map[x][y].tile=atoi(srrTNStr);
				if ((srGLNStrOn)&&(map[x][y].gln==atoi(srGLNStr))) map[x][y].gln=atoi(srrGLNStr);
				if ((srSDStrOn)&&(map[x][y].side==atoi(srSDStr))) map[x][y].side=atoi(srrSDStr);
			}
		}
	   if (global_found)
		   close_push_undo_sequenece();
       draw_map(map_bmp,map_x0,map_y0,tiles_high,tiles_wide);
       return D_CLOSE;
    }
	return d_button_proc(msg,d,c);
}

DIALOG searchreplace_dlg[SEARCHREPLACE_DLG_SIZE] = {
	{ d_sr_start_proc, SR_X, SR_Y, 160 + 75 + 16, 150, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 0, 0, 0, 0, 0 },// 0
	{ d_sr_replace_button_proc, SR_X + 50 + 45 + 80 - 8 - 40 + 16 + 16 - (96 - 66) + 8, SR_Y + 120, 96, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 'r', 0, 0, 0, (void *) "&Replace", 0, 0 },// 1
	{ d_text_proc, SR_X + 10, SR_LINE1 + 0 * SR_LH, 50, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 0, 0, (void *) "Type", 0, 0 },//2
	{ d_edit_proc, SR_X + 74, SR_LINE1 + 0 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },//3 Terrain Type Edit
	{ d_text_proc, SR_X + 10, SR_LINE1 + 1 * SR_LH, 50, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 0, 0, (void *) "Road", 0, 0 },//4
	{ d_edit_proc, SR_X + 74, SR_LINE1 + 1 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },//5 Road Type Edit
	{ d_text_proc, SR_X + 10, SR_LINE1 + 2 * SR_LH, 50, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 0, 0, (void *) "Tile#", 0, 0 },//6
	{ d_edit_proc, SR_X + 74, SR_LINE1 + 2 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },//7 Tile Number Edit
	{ d_text_proc, SR_X + 10, SR_LINE1 + 3 * SR_LH, 50, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 0, 0, (void *) "Name", 0, 0 },//8
	{ d_edit_proc, SR_X + 74, SR_LINE1 + 3 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },//9 Name Edit
	{ d_text_proc, SR_X + 10, SR_LINE1 + 4 * SR_LH, 50, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 0, 0, (void *) "Side", 0, 0 },//10
	{ d_edit_proc, SR_X + 74, SR_LINE1 + 4 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 }, //11 Side Edit
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 0 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "use", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 1 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1,	0, (void *) "use", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 2 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "use", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 3 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "use", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 4 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "use", 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 4, SR_LINE1 + 0 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 4, SR_LINE1 + 1 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 4, SR_LINE1 + 2 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 4, SR_LINE1 + 3 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 4, SR_LINE1 + 4 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_button_proc, SR_X + 50 + 45 + 80 - 70 - 8 - 80 + 8 - 8, SR_Y + 120, 96, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 'c', D_EXIT, 0, 0, (void *) "&Cancel", 0, 0 },
	{ d_text_proc ,	    SR_X+74,SR_LINE1-1*SR_LH,50,15,SR_FG_COLOR,SR_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Find" },
	{ d_text_proc ,	    SR_X+74+60+10+8*4,SR_LINE1-1*SR_LH,50,15,SR_FG_COLOR,SR_SCREEN_COLOR,	0 ,	0 ,	0 ,	0 ,	 (void *)"Set to" },
	{ d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
	//Last
	{NULL},
};
