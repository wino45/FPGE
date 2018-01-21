/* TERR.C handles input for changing terrain */

#include <stdio.h>
#include <allegro.h>
#include "fpge.h"
#include "findname.h"
#include "maingui.h"
#include "terr.h"
#include "tables.h"
#include "fpge_colors.h"

char tdTTStr[8]="0";
char tdRDStr[8]="0";
char tdTNStr[8]="0";
char tdGLNStr[8]="0";
char tdSDStr[8]="0";
//char tdTRStr[8]="1";
char tdTTStrMatch[8]="0";
char tdRDStrMatch[8]="0";
char tdTNStrMatch[8]="0";
char tdGLNStrMatch[8]="0";
char tdSDStrMatch[8]="0";

char tdRadiusStr[8]="0";
char tdProbStr[8]="100";

char td_name_str[MAX_NAME_UTF_SIZE]="";
char td_tt_name[TERRAIN_TYPE_SIZE]="";

int tdTTStrOn=1;
int tdRDStrOn=1;
int tdTNStrOn=1;
int tdGLNStrOn=1;
int tdSDStrOn=1;
int tdTTStrMatchOn=0;
int tdRDStrMatchOn=0;
int tdTNStrMatchOn=0;
int tdGLNStrMatchOn=0;
int tdSDStrMatchOn=0;

int tdMatchMatchOn=0;
int tdLastTileID=-5;
int tdLastNameID=-5;
int tdLastTTID=-5;
int tdLastRoad=-5;

int GUI_fill_default_tt = 0; //add default tt and name when placing tile
int GUI_use_brush = 0; //use brush
int GUI_only_on_clear = 0; //put tiles only on clear terrain

#define TD_X  1
#define TD_Y  1
#define TD_LH  15
#define TD_LINE1  TD_Y+10+TD_LH
#define TD_X0 10
#define TD_X1 TD_X0+45
#define TD_X2 TD_X1+60
#define TD_X3 TD_X2+20
#define TD_X4 TD_X3+60
//#define TD_BOX_X 10+4+3*8
#define TD_BOX_X 10
#define TD_BTN 96

#define tdTypeEditIdx 3
#define tdRoadEditIdx 5
#define tdTileEditIdx 7
#define tdNameEditIdx 9
#define tdSideEditIdx 11

#define tdTypeEditIdxCheck tdSideEditIdx+1
#define tdRoadEditIdxCheck tdTypeEditIdxCheck+1
#define tdTileEditIdxCheck tdRoadEditIdxCheck+1
#define tdNameEditIdxCheck tdTileEditIdxCheck+1
#define tdSideEditIdxCheck tdNameEditIdxCheck+1

#define tdTypeMatchIdx tdSideEditIdxCheck+1
#define tdRoadMatchIdx tdTypeMatchIdx+1
#define tdTileMatchIdx tdRoadMatchIdx+1
#define tdNameMatchIdx tdTileMatchIdx+1
#define tdSideMatchIdx tdNameMatchIdx+1

#define tdTypeMatchIdxCheck tdSideMatchIdx+1
#define tdRoadMatchIdxCheck tdTypeMatchIdxCheck+1
#define tdTileMatchIdxCheck tdRoadMatchIdxCheck+1
#define tdNameMatchIdxCheck tdTileMatchIdxCheck+1
#define tdSideMatchIdxCheck tdNameMatchIdxCheck+1
#define tdMatchMatchIdxCheck tdSideMatchIdxCheck+1

#define tdRoadNCheck     tdMatchMatchIdxCheck+1
#define tdRoadNWCheck    tdRoadNCheck+1
#define tdRoadNECheck    tdRoadNWCheck+1
#define tdRoadSWCheck    tdRoadNECheck+1
#define tdRoadSECheck    tdRoadSWCheck+1
#define tdRoadSCheck     tdRoadSECheck+1
#define tdRoadButton     tdRoadSCheck+1
#define tdNameButton     tdRoadButton+1
#define tdNameLabel      tdNameButton+3
#define tdTTLabel        tdNameLabel+1
#define tdFillDefaults   tdTTLabel+3
#define tdUseBrushIdx    tdFillDefaults+1
#define tdRadiusStrIdx   tdUseBrushIdx+2
#define tdProbStrIdx   	 tdRadiusStrIdx+2
#define tdOnlyClearIdx   tdProbStrIdx+1

int d_start_proc(int msg, DIALOG *d, int c);
int d_coreT_proc(int msg, DIALOG *d, int c);
int d_roadB_proc(int msg, DIALOG *d, int c);
int d_nameB_proc(int msg, DIALOG *d, int c);
int d_default_tt_proc(int msg, DIALOG *d, int c);
int d_only_clear_proc(int msg, DIALOG *d, int c);
int d_use_brush_proc(int msg, DIALOG *d, int c);
int d_defaults_proc(int msg, DIALOG *d, int c);

DIALOG terrain_dlg[TERRAIN_DLG_SIZE]={
{ d_start_proc, TD_X - 70, TD_Y, 160 + 75 + 55, 150 + TD_LH * 9+8, TD_FG_COLOR, TD_SCREEN_COLOR, 0, 0, 0, 0, 0, 0, 0 }, // 0
{ d_button_proc, TD_X + 50 + 45 + 80 - 70, 4+TD_Y + 120 - 18 + 40 + (3+5) * TD_LH, TD_BTN, 15, TD_FG_COLOR, TD_BG_COLOR, 'd', D_EXIT, 0, 0, (void *) "&Done", 0, 0 },// 1
//edits and labels
{ d_text_proc, TD_X + TD_X0, TD_LINE1 + 0 * TD_LH, 50, 15, TD_FG_COLOR, TD_SCREEN_COLOR, 0, 0, 0, 0, (void *) "T&ype", 0, 0 },//2
{ d_edit_proc, TD_X + TD_X1, TD_LINE1 + 0 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 6, 0, 0, 0, 0 },//3 Terrain Type Edit
{ d_text_proc, TD_X + TD_X0, TD_LINE1 + 1 * TD_LH, 50, 15, TD_FG_COLOR, TD_SCREEN_COLOR, 0, 0, 0, 0, (void *) "R&oad", 0, 0 },//4
{ d_edit_proc, TD_X + TD_X1, TD_LINE1 + 1 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },//5 Road Type Edit
{ d_text_proc, TD_X + TD_X0, TD_LINE1 + 2 * TD_LH, 50, 15, TD_FG_COLOR, TD_SCREEN_COLOR, 0, 0, 0, 0, (void *) "&Tile#", 0, 0 },
{ d_edit_proc, TD_X + TD_X1, TD_LINE1 + 2 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },//7 Tile Number Edit
{ d_text_proc, TD_X + TD_X0, TD_LINE1 + 3 * TD_LH, 50, 15, TD_FG_COLOR, TD_SCREEN_COLOR, 0, 0, 0, 0, (void *) "N&ame", 0, 0 },//9 Name Edit
{ d_edit_proc, TD_X + TD_X1, TD_LINE1 + 3 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },
{ d_text_proc, TD_X + TD_X0, TD_LINE1 + 4 * TD_LH, 50, 15, TD_FG_COLOR, TD_SCREEN_COLOR, 0, 0, 0, 0, (void *) "&Side", 0, 0 },//10
{ d_edit_proc, TD_X + TD_X1, TD_LINE1 + 4 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },//11 Side Edit
//check boxes
{ d_coreT_proc, TD_X + TD_X2, TD_LINE1 + 0 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 'y', 0, 1, 0, (void *) "", 0, 0 },
{ d_coreT_proc, TD_X + TD_X2, TD_LINE1 + 1 * TD_LH, TD_BOX_X,10, TD_FG_COLOR, TD_BG_COLOR, 'o', 0, 1, 0, (void *) "", 0, 0 },
{ d_coreT_proc, TD_X + TD_X2, TD_LINE1 + 2 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 't', 0, 1, 0, (void *) "",0, 0 },
{ d_coreT_proc, TD_X + TD_X2, TD_LINE1 + 3 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 'a', 0, 1, 0, (void *) "", 0, 0 },
{ d_coreT_proc, TD_X + TD_X2, TD_LINE1 + 4 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 's', 0, 1, 0, (void *) "", 0, 0 },
// match edits
{ d_edit_proc, TD_X + TD_X3, TD_LINE1 + 0 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },
{ d_edit_proc, TD_X + TD_X3, TD_LINE1 + 1 * TD_LH, 50, 15, TD_FG_COLOR,	TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },
{ d_edit_proc, TD_X + TD_X3, TD_LINE1 + 2 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },
{ d_edit_proc, TD_X + TD_X3, TD_LINE1 + 3 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },
{ d_edit_proc, TD_X + TD_X3, TD_LINE1 + 4 * TD_LH, 50, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },
// second check boxes
{ d_coreT_proc, TD_X + TD_X4, TD_LINE1 + 0 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 1, 0, (void *) "", 0, 0 },
{ d_coreT_proc, TD_X + TD_X4, TD_LINE1 + 1 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 1, 0, (void *) "", 0, 0 },
{ d_coreT_proc, TD_X + TD_X4, TD_LINE1 + 2 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 1, 0, (void *) "", 0, 0 },
{ d_coreT_proc, TD_X + TD_X4, TD_LINE1 + 3 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 1, 0, (void *) "", 0, 0 },
{ d_coreT_proc, TD_X + TD_X4, TD_LINE1 + 4 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 1, 0, (void *) "", 0, 0 },
{ d_coreT_proc, TD_X + TD_X4, TD_LINE1 - 1 * TD_LH, TD_BOX_X, 10, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 1, 0, (void *) "", 0, 0 },
//roads check boxes
{ d_check_proc, TD_X + 25 + 30 - 16, TD_LINE1 + 8 * TD_LH, 12 + 1 * 8, 14, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 0, 0, (void *) "N", 0, 0 },
{ d_check_proc, TD_X + 25 - 16, TD_LINE1 + 9 * TD_LH, 12 + 2 * 8, 14, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 0, 0, (void *) "NW", 0, 0 },
{ d_check_proc, TD_X + 25 + 42 - 16, TD_LINE1 + 9 * TD_LH, 12 + 2 * 8, 14, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 0, 0, (void *) "NE", 0, 0 },
{ d_check_proc, TD_X + 25 - 16, TD_LINE1 + 10 * TD_LH, 12 + 2 * 8, 14, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 0, 0, (void *) "SW", 0, 0 },
{ d_check_proc, TD_X + 25 + 42 - 16, TD_LINE1 + 10 * TD_LH, 12 + 2 * 8, 14, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 0, 0, (void *) "SE", 0, 0 },
{ d_check_proc, TD_X + 25 + 30 - 16, TD_LINE1 + 11 * TD_LH, 12 + 1 * 8, 14, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 0, 0, (void *) "S", 0, 0 },
//buttons
{ d_roadB_proc, TD_X + 50 + 45 + 80 - 70, TD_Y + 120 - 18 + 20 + 4 * TD_LH, TD_BTN, 15, TD_FG_COLOR, TD_BG_COLOR, 'r', 0, 0, 0, (void *) "Make &road", 0, 0 }, 		//28
{ d_nameB_proc, TD_X + 50 + 45 + 80 - 70, TD_Y + 120 - 18 + 3 * TD_LH, TD_BTN, 15, TD_FG_COLOR, TD_BG_COLOR, 'n', 0, 0, 0, (void *) "&Name", 0, 0 },		//29
//labels
{ d_text_proc ,	TD_X+TD_X1,TD_LINE1-1*TD_LH,50,15,TD_FG_COLOR,TD_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Set to" },
{ d_text_proc ,	TD_X+TD_X3,TD_LINE1-1*TD_LH,50,15,TD_FG_COLOR,TD_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Match" },
{ d_text_proc ,	TD_X+TD_X0,TD_LINE1+6*TD_LH,MAX_NAME_SIZE*8,15,TD_FG_COLOR,TD_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"" },
{ d_text_proc ,	TD_X+TD_X0,TD_LINE1+7*TD_LH,TERRAIN_TYPE_SIZE*8,15,TD_FG_COLOR,TD_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"" },
{ d_text_proc ,	TD_X-70+8,TD_LINE1+6*TD_LH,MAX_NAME_SIZE*8,15,TD_FG_COLOR,TD_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Name:" },
{ d_text_proc ,	TD_X-70+8,TD_LINE1+7*TD_LH,MAX_NAME_SIZE*8,15,TD_FG_COLOR,TD_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Terrain:" },

{ d_default_tt_proc, TD_X-70+8, TD_LINE1 + 12 * TD_LH, 12 + 25 * 8, 16, TD_FG_COLOR, TD_BG_COLOR, 0, 0, 1, 0, (void *) "Add default name and type", 0, 0 },
{ d_use_brush_proc, TD_X-70+8, TD_LINE1+4+ 13 * TD_LH, 12 + 9 * 8, 16, TD_FG_COLOR, TD_BG_COLOR, 'u', 0, 1, 0, (void *) "&Use brush", 0, 0 },
{ d_text_proc ,	TD_X-70+24  +8*15,TD_LINE1 + 13 * TD_LH+8,50,15,TD_FG_COLOR,TD_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Radius" },
{ d_edit_proc,  TD_X+32+16  +8*15,TD_LINE1 + 13 * TD_LH+8,32, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },
{ d_text_proc ,	TD_X-70+24  +8*15,4+TD_LINE1 + 14 * TD_LH+8,50,15,TD_FG_COLOR,TD_SCREEN_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Probability" },
{ d_edit_proc,  TD_X+32+16  +8*15,4+TD_LINE1 + 14 * TD_LH+8,32, 15, TD_FG_COLOR, TD_EDIT_COLOR, 0, 0, 4, 0, 0, 0, 0 },
{ d_only_clear_proc, TD_X-70+8, 4+TD_LINE1 + 14 * TD_LH+4, 12 + 13 * 8, 16, TD_FG_COLOR, TD_BG_COLOR, 'c', 0, 1, 0, (void *) "Only on &clear", 0, 0 },
{ d_defaults_proc, TD_X-70+8, 4+TD_Y + 120 - 18 + 40 + (3+5) * TD_LH, TD_BTN, 15, TD_FG_COLOR, TD_BG_COLOR, 'e', 0, 0, 0, (void *) "D&efaults", 0, 0 },// 1

{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
//Last
{NULL},

};

void update_name_label() {
	int idx;

	idx = atoi(tdGLNStr);
	//printf("idx=%d\n",idx);
	if (idx > -1 && idx < total_names) {
		strncpy(td_name_str, gln_utf8[idx], MAX_NAME_UTF_SIZE);
		while(ustrlen(td_name_str)<MAX_NAME_SIZE){
			strncat(td_name_str," ",MAX_NAME_UTF_SIZE-strlen(td_name_str));
		}
		terrain_dlg[tdNameLabel].flags |= D_DIRTY;
	}
}

void update_terrain_label() {
	int idx;

	idx = atoi(tdTTStr);
	//printf("idx=%d\n",idx);
	if (idx > -1 && idx < MAX_TERRAIN_TYPE) {
		strncpy(td_tt_name, utr_names[idx], TERRAIN_TYPE_SIZE);
		while(ustrlen(td_tt_name)<TERRAIN_TYPE_SIZE){
			strncat(td_tt_name," ",TERRAIN_TYPE_SIZE-strlen(td_tt_name));
		}
		terrain_dlg[tdTTLabel].flags |= D_DIRTY;
	}
}

void draw_road_connections(BITMAP *map_to_draw, int xs,int ys, int rc){
	 int line_color = colors_to24bits(LINE_COLOR);
	if (rc & 0x01) {
		line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + TILE_FULL_WIDTH / 2, ys, line_color);
	}
	if (rc & 0x02) {
		line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 1.7, ys + TILE_HEIGHT / 4, line_color);
	}
	if (rc & 0x08) {
		line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 1.7, ys - 1 + (-(float) TILE_HEIGHT / 4.0 + TILE_HEIGHT), line_color);
	}

	if (rc & 0x10) {
		line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT, line_color);
	}
	if (rc & 0x20) {
		line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 0.3, ys - 1 + (-(float) TILE_HEIGHT / 4.0 + TILE_HEIGHT), line_color);
	}
	if (rc & 0x80) {
		line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 0.3, ys + TILE_HEIGHT / 4, line_color);
	}
}

void clear_all(){
	int mask_RC;

	 tdLastTileID=-5;
	 tdLastNameID=-5;
	 tdLastTTID=-5;
	 tdLastRoad=-5;

     terrain_dlg[tdTypeEditIdx].dp=tdTTStr; 
     terrain_dlg[tdRoadEditIdx].dp=tdRDStr;
     terrain_dlg[tdTileEditIdx].dp=tdTNStr;
     terrain_dlg[tdNameEditIdx].dp=tdGLNStr;
     terrain_dlg[tdSideEditIdx].dp=tdSDStr;

     terrain_dlg[tdTypeMatchIdx].dp=tdTTStrMatch; 
     terrain_dlg[tdRoadMatchIdx].dp=tdRDStrMatch;
     terrain_dlg[tdTileMatchIdx].dp=tdTNStrMatch;
     terrain_dlg[tdNameMatchIdx].dp=tdGLNStrMatch;
     terrain_dlg[tdSideMatchIdx].dp=tdSDStrMatch;
	 
     terrain_dlg[tdRadiusStrIdx].dp=tdRadiusStr;
     terrain_dlg[tdProbStrIdx].dp=tdProbStr;

     terrain_dlg[tdNameLabel].dp=td_name_str;
     terrain_dlg[tdTTLabel].dp=td_tt_name;

	if (tdTTStrOn) {
		terrain_dlg[tdTypeEditIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdTypeEditIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdRDStrOn) {
		terrain_dlg[tdRoadEditIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdRoadEditIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdTNStrOn) {
		terrain_dlg[tdTileEditIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdTileEditIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdGLNStrOn) {
		terrain_dlg[tdNameEditIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdNameEditIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdSDStrOn) {
		terrain_dlg[tdSideEditIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdSideEditIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdTTStrMatchOn) {
		terrain_dlg[tdTypeMatchIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdTypeMatchIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdRDStrMatchOn) {
		terrain_dlg[tdRoadMatchIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdRoadMatchIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdTNStrMatchOn) {
		terrain_dlg[tdTileMatchIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdTileMatchIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdGLNStrMatchOn) {
		terrain_dlg[tdNameMatchIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdNameMatchIdxCheck].flags &= ~D_SELECTED;
	}
	if (tdSDStrMatchOn) {
		terrain_dlg[tdSideMatchIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdSideMatchIdxCheck].flags &= ~D_SELECTED;
	}

	if (tdMatchMatchOn) {
		terrain_dlg[tdMatchMatchIdxCheck].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdMatchMatchIdxCheck].flags &= ~D_SELECTED;
	}

	if (tdRoadEditIdxCheck) {
		mask_RC = atoi(tdRDStr);
		if ((mask_RC & 0x01) == 0x01) {
			terrain_dlg[tdRoadNCheck].flags |= D_SELECTED;
		} else {
			terrain_dlg[tdRoadNCheck].flags &= ~D_SELECTED;
		}
		if ((mask_RC & 0x80) == 0x80) {
			terrain_dlg[tdRoadNWCheck].flags |= D_SELECTED;
		} else {
			terrain_dlg[tdRoadNWCheck].flags &= ~D_SELECTED;
		}
		if ((mask_RC & 0x02) == 0x02) {
			terrain_dlg[tdRoadNECheck].flags |= D_SELECTED;
		} else {
			terrain_dlg[tdRoadNECheck].flags &= ~D_SELECTED;
		}
		if ((mask_RC & 0x20) == 0x20) {
			terrain_dlg[tdRoadSWCheck].flags |= D_SELECTED;
		} else {
			terrain_dlg[tdRoadSWCheck].flags &= ~D_SELECTED;
		}
		if ((mask_RC & 0x08) == 0x08) {
			terrain_dlg[tdRoadSECheck].flags |= D_SELECTED;
		} else {
			terrain_dlg[tdRoadSECheck].flags &= ~D_SELECTED;
		}
		if ((mask_RC & 0x10) == 0x10) {
			terrain_dlg[tdRoadSCheck].flags |= D_SELECTED;
		} else {
			terrain_dlg[tdRoadSCheck].flags &= ~D_SELECTED;
		}
	}

	if (GUI_fill_default_tt) {
		terrain_dlg[tdFillDefaults].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdFillDefaults].flags &= ~D_SELECTED;
	}

	if (GUI_use_brush) {
		terrain_dlg[tdUseBrushIdx].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdUseBrushIdx].flags &= ~D_SELECTED;
	}

	if (GUI_only_on_clear) {
		terrain_dlg[tdOnlyClearIdx].flags |= D_SELECTED;
	} else {
		terrain_dlg[tdOnlyClearIdx].flags &= ~D_SELECTED;
	}

	 update_name_label();
	 update_terrain_label();
}


int d_start_proc(int msg, DIALOG *d, int c)
{
	int idx,idx1;
	BITMAP *tile_to_draw;

	if (msg == MSG_START) {
		clear_all();
	}

	if (msg == MSG_DRAW) {
		tdLastTileID = -5;
		tdLastNameID = -5;
		tdLastTTID = -5;
		tdLastRoad = -5;
	}

	if (msg == MSG_IDLE) {
		idx = atoi(tdTNStr);
		idx1 = atoi(tdRDStr);

		if (idx != tdLastTileID || idx1 != tdLastRoad) {
			//printf("msg=%d idx=%d t=%d\n",msg,idx,tdLastTileID);
			//print tile
			if (idx > -1 && idx < total_tiles) {
				switch (showWeather) {
				case 0:
					tile_to_draw = til_bmp[idx];
					break;
				case 1:
					tile_to_draw = til_bmp_mud[idx];
					break;
				case 2:
					tile_to_draw = til_bmp_snow[idx];
					break;

				default:
					tile_to_draw = 0;
					break;
				}

				masked_blit(tile_to_draw, screen, 0, 0, terrain_dlg[0].x + 10, terrain_dlg[0].y + TD_LINE1,
				TILE_FULL_WIDTH, TILE_HEIGHT);

				draw_road_connections(screen, terrain_dlg[0].x + 10, terrain_dlg[0].y + TD_LINE1, atoi(tdRDStr));
				tdLastTileID = idx;
				tdLastRoad = idx1;
			}
			if (GUI_fill_default_tt) {
				//fill default type
				if (idx < total_tiles && TTData_Max_Tiles[idx] > -1) {
					sprintf(tdTTStr, "%d", TTData_Max_Tiles[idx]);
					terrain_dlg[tdTypeEditIdx].flags |= D_DIRTY;
				}
				//fill default name
				if (idx < total_tiles && NData_Max_Tiles[idx] > -1) {
					sprintf(tdGLNStr, "%d", NData_Max_Tiles[idx]);
					terrain_dlg[tdNameEditIdx].flags |= D_DIRTY;
				}
			}
		}

	   //
		idx = atoi(tdGLNStr);
		if (idx != tdLastNameID) {
			update_name_label();
			tdLastNameID = idx;
		}

		idx = atoi(tdTTStr);
		if (idx != tdLastTTID) {
			update_terrain_label();
			tdLastTTID = idx;
		}
   }

   return d_textbox_proc(msg,d,c);
}

int d_coreT_proc(int msg, DIALOG *d, int c)
{
   int group=0, item, val, i;
   int groups[2][5] = {
		   {tdTypeEditIdxCheck,  tdRoadEditIdxCheck,  tdTileEditIdxCheck,  tdNameEditIdxCheck,  tdSideEditIdxCheck},
		   {tdTypeMatchIdxCheck, tdRoadMatchIdxCheck, tdTileMatchIdxCheck, tdNameMatchIdxCheck, tdSideMatchIdxCheck}};

   if ((msg==MSG_CLICK)||(msg==MSG_KEY))
   {
		int y= d_check_proc(msg,d,c);
		if (&(terrain_dlg[tdTypeEditIdxCheck]) == d) {
			tdTTStrOn = ((terrain_dlg[tdTypeEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 1;
			item = tdTypeEditIdxCheck;
		}
		if (&(terrain_dlg[tdRoadEditIdxCheck]) == d) {
			tdRDStrOn = ((terrain_dlg[tdRoadEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 1;
			item = tdRoadEditIdxCheck;
		}
		if (&(terrain_dlg[tdTileEditIdxCheck]) == d) {
			tdTNStrOn = ((terrain_dlg[tdTileEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 1;
			item = tdTileEditIdxCheck;
		}
		if (&(terrain_dlg[tdNameEditIdxCheck]) == d) {
			tdGLNStrOn = ((terrain_dlg[tdNameEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 1;
			item = tdNameEditIdxCheck;
		}
		if (&(terrain_dlg[tdSideEditIdxCheck]) == d) {
			tdSDStrOn = ((terrain_dlg[tdSideEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 1;
			item = tdSideEditIdxCheck;
		}

		if (&(terrain_dlg[tdTypeMatchIdxCheck]) == d) {
			tdTTStrMatchOn = ((terrain_dlg[tdTypeMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 2;
			item = tdTypeMatchIdxCheck;
		}
		if (&(terrain_dlg[tdRoadMatchIdxCheck]) == d) {
			tdRDStrMatchOn = ((terrain_dlg[tdRoadMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 2;
			item = tdRoadMatchIdxCheck;
		}
		if (&(terrain_dlg[tdTileMatchIdxCheck]) == d) {
			tdTNStrMatchOn = ((terrain_dlg[tdTileMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 2;
			item = tdTileMatchIdxCheck;
		}
		if (&(terrain_dlg[tdNameMatchIdxCheck]) == d) {
			tdGLNStrMatchOn = ((terrain_dlg[tdNameMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 2;
			item = tdNameMatchIdxCheck;
		}
		if (&(terrain_dlg[tdSideMatchIdxCheck]) == d) {
			tdSDStrMatchOn = ((terrain_dlg[tdSideMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			group = 2;
			item = tdSideMatchIdxCheck;
		}

		if ((key_shifts & KB_SHIFT_FLAG) && group){

			val = ((terrain_dlg[item].flags&D_SELECTED)==D_SELECTED?1:0);

			if (val) {
				//is set so clear
				for (i = 0; i < 5; i++) {
					terrain_dlg[groups[group - 1][i]].flags &= ~D_SELECTED;
				}
				//recover
				terrain_dlg[item].flags |= D_SELECTED;

			} else {
				//is unset so set
				for (i = 0; i < 5; i++) {
					terrain_dlg[groups[group - 1][i]].flags |= D_SELECTED;
				}
				//recover
				terrain_dlg[item].flags &= ~D_SELECTED;
			}
			for (i = 0; i < 5; i++) {
				terrain_dlg[groups[group - 1][i]].flags |= D_DIRTY;
			}
			if (group == 1) {
				tdTTStrOn = ((terrain_dlg[tdTypeEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
				tdRDStrOn = ((terrain_dlg[tdRoadEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
				tdTNStrOn = ((terrain_dlg[tdTileEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
				tdGLNStrOn = ((terrain_dlg[tdNameEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
				tdSDStrOn = ((terrain_dlg[tdSideEditIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			} else {
				tdTTStrMatchOn = ((terrain_dlg[tdTypeMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
				tdRDStrMatchOn = ((terrain_dlg[tdRoadMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
				tdTNStrMatchOn = ((terrain_dlg[tdTileMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
				tdGLNStrMatchOn = ((terrain_dlg[tdNameMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
				tdSDStrMatchOn = ((terrain_dlg[tdSideMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
			}
		}

		if (&(terrain_dlg[tdMatchMatchIdxCheck]) == d) {
			tdMatchMatchOn = ((terrain_dlg[tdMatchMatchIdxCheck].flags & D_SELECTED) == D_SELECTED ? 1 : 0);
		}

		return y;
   }
   return d_check_proc(msg,d,c);
}

int d_roadB_proc(int msg, DIALOG *d, int c)
{
    int road_RC=0;

	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
	{
		// toggle
		//terrain_dlg[tdRoadButton].flags^=D_SELECTED;
		road_RC = ((terrain_dlg[ tdRoadNCheck].flags & D_SELECTED) == D_SELECTED ? 0x01 : 0);
		road_RC += ((terrain_dlg[tdRoadNWCheck].flags & D_SELECTED) == D_SELECTED ? 0x80 : 0);
		road_RC += ((terrain_dlg[tdRoadNECheck].flags & D_SELECTED) == D_SELECTED ? 0x02 : 0);
		road_RC += ((terrain_dlg[tdRoadSWCheck].flags & D_SELECTED) == D_SELECTED ? 0x20 : 0);
		road_RC += ((terrain_dlg[tdRoadSECheck].flags & D_SELECTED) == D_SELECTED ? 0x08 : 0);
		road_RC += ((terrain_dlg[tdRoadSCheck].flags & D_SELECTED) == D_SELECTED ? 0x10 : 0);
		sprintf(tdRDStr, "%d", road_RC);
		terrain_dlg[tdRoadEditIdx].dp = tdRDStr;
		terrain_dlg[tdRoadEditIdx].flags |= D_DIRTY;
		//terrain_dlg[tdRoadEditIdx].flags |= D_DIRTY;
		//broadcast_dialog_message(MSG_DRAW,0);
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}

int do_findname_dlg(){
	centre_dialog(findname_dlg);
	do_dialog(findname_dlg,-1);
	return D_REDRAW;
}

int d_nameB_proc(int msg, DIALOG *d, int c)
{
	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
	{
		do_findname_dlg();
		tdLastTileID=-5;
	    terrain_dlg[tdNameButton].flags&=~D_SELECTED;
		object_message(&main_dlg[dmMapBmpIdx], MSG_DRAW, 0);
		broadcast_dialog_message(MSG_DRAW,0);
		// do not proceess
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}

int d_default_tt_proc(int msg, DIALOG *d, int c)
{
	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
	{
		d_check_proc(msg, d, c);
		if ((terrain_dlg[tdFillDefaults].flags & D_SELECTED) == D_SELECTED) {
			GUI_fill_default_tt = 1;
		} else {
			GUI_fill_default_tt = 0;
		}
		return D_O_K;
	}
	return d_check_proc(msg,d,c);
}

int d_use_brush_proc(int msg, DIALOG *d, int c)
{
	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
	{
		d_check_proc(msg,d,c);
		if ((terrain_dlg[tdUseBrushIdx].flags & D_SELECTED) == D_SELECTED) {
			GUI_use_brush = 1;
		} else {
			GUI_use_brush = 0;
		}
		return D_O_K;
	}
	return d_check_proc(msg,d,c);
}

int d_only_clear_proc(int msg, DIALOG *d, int c)
{
	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
	{
		d_check_proc(msg,d,c);
		if ((terrain_dlg[tdOnlyClearIdx].flags & D_SELECTED) == D_SELECTED) {
			GUI_only_on_clear = 1;
		} else {
			GUI_only_on_clear = 0;
		}
		return D_O_K;
	}
	return d_check_proc(msg,d,c);
}

int d_defaults_proc(int msg, DIALOG *d, int c)
{
	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) {
		GUI_fill_default_tt=0;
		GUI_only_on_clear=0;
		GUI_use_brush=0;

		strncpy(tdTTStr,"0",8);
		strncpy(tdRDStr,"0",8);
		strncpy(tdTNStr,"0",8);
		strncpy(tdGLNStr,"0",8);;
		strncpy(tdSDStr,"0",8);
		strncpy(tdTTStrMatch,"0",8);
		strncpy(tdRDStrMatch,"0",8);
		strncpy(tdTNStrMatch,"0",8);
		strncpy(tdGLNStrMatch,"0",8);
		strncpy(tdSDStrMatch,"0",8);

		tdTTStrOn=1;
		tdRDStrOn=1;
		tdTNStrOn=1;
		tdGLNStrOn=1;
		tdSDStrOn=1;
		tdTTStrMatchOn=0;
		tdRDStrMatchOn=0;
		tdTNStrMatchOn=0;
		tdGLNStrMatchOn=0;
		tdSDStrMatchOn=0;

		tdMatchMatchOn=0;

		strncpy(tdRadiusStr,"0",8);
		strncpy(tdProbStr,"100",8);
		d_start_proc(MSG_DRAW,d,0);
		clear_all();
		broadcast_dialog_message(MSG_DRAW,0);
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}
