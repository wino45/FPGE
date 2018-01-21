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

int srTTMatchOn=1;
int srRDMatchOn=1;
int srTNMatchOn=1;
int srGLNMatchOn=1;
int srSDMatchOn=1;

int srTTSetOn=1;
int srRDSetOn=1;
int srTNSetOn=1;
int srGLNSetOn=1;
int srSDSetOn=1;

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

#define srTypeMatchIdxCheck srSideEditIdx+1
#define srRoadMatchIdxCheck srTypeMatchIdxCheck+1
#define srTileMatchIdxCheck srRoadMatchIdxCheck+1
#define srNameMatchIdxCheck srTileMatchIdxCheck+1
#define srSideMatchIdxCheck srNameMatchIdxCheck+1

#define srrTypeEditIdx srSideMatchIdxCheck+1
#define srrRoadEditIdx srrTypeEditIdx+1
#define srrTileEditIdx srrRoadEditIdx+1
#define srrNameEditIdx srrTileEditIdx+1
#define srrSideEditIdx srrNameEditIdx+1

#define srTypeSetIdxCheck srrSideEditIdx+1
#define srRoadSetIdxCheck srTypeSetIdxCheck+1
#define srTileSetIdxCheck srRoadSetIdxCheck+1
#define srNameSetIdxCheck srTileSetIdxCheck+1
#define srSideSetIdxCheck srNameSetIdxCheck+1

//#define srRoadButton     srrSideEditIdx+1

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

		if (srTTMatchOn)
			searchreplace_dlg[srTypeMatchIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srTypeMatchIdxCheck].flags &= ~D_SELECTED;

		if (srRDMatchOn)
			searchreplace_dlg[srRoadMatchIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srRoadMatchIdxCheck].flags &= ~D_SELECTED;

		if (srTNMatchOn)
			searchreplace_dlg[srTileMatchIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srTileMatchIdxCheck].flags &= ~D_SELECTED;

		if (srGLNMatchOn)
			searchreplace_dlg[srNameMatchIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srNameMatchIdxCheck].flags &= ~D_SELECTED;

		if (srSDMatchOn)
			searchreplace_dlg[srSideMatchIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srSideMatchIdxCheck].flags &= ~D_SELECTED;

		if (srTTSetOn)
			searchreplace_dlg[srTypeSetIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srTypeSetIdxCheck].flags &= ~D_SELECTED;

		if (srRDSetOn)
			searchreplace_dlg[srRoadSetIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srRoadSetIdxCheck].flags &= ~D_SELECTED;

		if (srTNSetOn)
			searchreplace_dlg[srTileSetIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srTileSetIdxCheck].flags &= ~D_SELECTED;

		if (srGLNSetOn)
			searchreplace_dlg[srNameSetIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srNameSetIdxCheck].flags &= ~D_SELECTED;

		if (srSDSetOn)
			searchreplace_dlg[srSideSetIdxCheck].flags |= D_SELECTED;
		else
			searchreplace_dlg[srSideSetIdxCheck].flags &= ~D_SELECTED;
	}
	return d_textbox_proc(msg, d, c);
}

int d_sr_coreT_proc(int msg, DIALOG *d, int c)
{
   if ((msg==MSG_CLICK)||(msg==MSG_KEY))
   {
		int y= d_check_proc(msg,d,c);
		if (&(searchreplace_dlg[srTypeMatchIdxCheck])==d) srTTMatchOn = ((searchreplace_dlg[srTypeMatchIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srRoadMatchIdxCheck])==d) srRDMatchOn = ((searchreplace_dlg[srRoadMatchIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srTileMatchIdxCheck])==d) srTNMatchOn = ((searchreplace_dlg[srTileMatchIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srNameMatchIdxCheck])==d) srGLNMatchOn = ((searchreplace_dlg[srNameMatchIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srSideMatchIdxCheck])==d) srSDMatchOn = ((searchreplace_dlg[srSideMatchIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);

		if (&(searchreplace_dlg[srTypeSetIdxCheck])==d) srTTSetOn = ((searchreplace_dlg[srTypeSetIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srRoadSetIdxCheck])==d) srRDSetOn = ((searchreplace_dlg[srRoadSetIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srTileSetIdxCheck])==d) srTNSetOn = ((searchreplace_dlg[srTileSetIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srNameSetIdxCheck])==d) srGLNSetOn = ((searchreplace_dlg[srNameSetIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);
		if (&(searchreplace_dlg[srSideSetIdxCheck])==d) srSDSetOn = ((searchreplace_dlg[srSideSetIdxCheck].flags&D_SELECTED)==D_SELECTED?1:0);

		return y;
   }
   return d_check_proc(msg,d,c);
}

int is_match(int probe, char* pattern){
	if (pattern[0]=='!'){
		return probe != atoi(&pattern[1]);
	}else
		return probe == atoi(pattern);
}

int d_sr_replace_button_proc(int msg, DIALOG *d, int c)
{
	int x,y,found,global_found=0,match=0;;

	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
	{
	d_button_proc(msg,d,c);
	d->flags^=D_SELECTED;

	global_found=0;
	match=srTTMatchOn+srRDMatchOn+srTNMatchOn+srGLNMatchOn+srSDMatchOn;
    for (y=0; y<mapy; ++y)
		for (x=0; x<mapx; ++x){
			//first check
			found=0;
			if ((srTTMatchOn)&&(is_match(map[x][y].utr,srTTStr))) found++;
			if ((srRDMatchOn)&&(is_match(map[x][y].rc,srRDStr))) found++;
			if ((srTNMatchOn)&&(is_match(map[x][y].tile,srTNStr))) found++;
			if ((srGLNMatchOn)&&(is_match(map[x][y].gln,srGLNStr))) found++;
			if ((srSDMatchOn)&&(is_match(map[x][y].side,srSDStr))) found++;
			if (found==match && global_found==0){
				global_found=1;
				open_push_undo_sequenece();
			}
			if (found==match){
				push_undo_tile(x,y);
				if (srTTSetOn) map[x][y].utr=atoi(srrTTStr);
				if (srRDSetOn) map[x][y].rc=atoi(srrRDStr);
				if (srTNSetOn) map[x][y].tile=atoi(srrTNStr);
				if (srGLNSetOn) map[x][y].gln=atoi(srrGLNStr);
				if (srSDSetOn) map[x][y].side=atoi(srrSDStr);
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
	{ d_sr_start_proc, SR_X, SR_Y, 160 + 75 + 16+8*6, 150, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 0, 0, 0, 0, 0 },// 0
	{ d_sr_replace_button_proc, SR_X + 50 + 45 + 80 - 8 - 40 + 16 + 16 - (96 - 66) + 8*7, SR_Y + 120, 96, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 'r', 0, 0, 0, (void *) "&Replace", 0, 0 },// 1
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
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 0 * SR_LH, 10 + 8 * 5 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "match", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 1 * SR_LH, 10 + 8 * 5 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1,	0, (void *) "match", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 2 * SR_LH, 10 + 8 * 5 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "match", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 3 * SR_LH, 10 + 8 * 5 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "match", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60, SR_LINE1 + 4 * SR_LH, 10 + 8 * 5 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "match", 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 6, SR_LINE1 + 0 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 6, SR_LINE1 + 1 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 6, SR_LINE1 + 2 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 6, SR_LINE1 + 3 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_edit_proc, SR_X + 74 + 60 + 10 + 8 * 6, SR_LINE1 + 4 * SR_LH, 50, 15, SR_FG_COLOR, SR_BG_COLOR, 0, 0, 4, 0, 0, 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60+ 10 + 8 * 5+10 + 8 * 6 + 4, SR_LINE1 + 0 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "set", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60+ 10 + 8 * 5+10 + 8 * 6 + 4, SR_LINE1 + 1 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1,	0, (void *) "set", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60+ 10 + 8 * 5+10 + 8 * 6 + 4, SR_LINE1 + 2 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "set", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60+ 10 + 8 * 5+10 + 8 * 6 + 4, SR_LINE1 + 3 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "set", 0, 0 },
	{ d_sr_coreT_proc, SR_X + 74 + 60+ 10 + 8 * 5+10 + 8 * 6 + 4, SR_LINE1 + 4 * SR_LH, 10 + 8 * 3 + 4, 10, SR_FG_COLOR, SR_SCREEN_COLOR, 0, 0, 1, 0, (void *) "set", 0, 0 },
	{ d_button_proc, SR_X + 50 + 45 + 80 - 70 - 8 - 80 + 8 - 8, SR_Y + 120, 96, 15, SR_FG_COLOR, SR_SCREEN_COLOR, 'c', D_EXIT, 0, 0, (void *) "&Cancel", 0, 0 },
	{ d_text_proc ,	    SR_X+74,SR_LINE1-1*SR_LH,50,15,SR_FG_COLOR,SR_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Match" },
	{ d_text_proc ,	    SR_X+74+60+10+8*6,SR_LINE1-1*SR_LH,50,15,SR_FG_COLOR,SR_SCREEN_COLOR,	0 ,	0 ,	0 ,	0 ,	 (void *)"Set to" },
	{ d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
	//Last
	{NULL},
};
