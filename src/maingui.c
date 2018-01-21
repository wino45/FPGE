/*
 * maingui.c
 *
 *  Created on: 2010-05-06
 *      Author: wino
 */
#include <stdio.h>
#include "allegro.h"

#include "maingui.h"
#include "fpge.h"
#include "menu.h"
#include "callback.h"
#include "unitlist.h"
#include "unit.h"
#include "save.h"
#include "load.h"
#include "pgf.h"
#include "help.h"
#include "makemap.h"
#include "scen.h"
#include "searep.h"
#include "mapfrg.h"
#include "map_resize.h"
#include "ag.h"
#include "drawmap.h"
#include "unitsum.h"
#include "pg.h"
#include "filename.h"
#include "cli.h"

DIALOG country_units_bmp_dlg[COUNTRY_UNITS_BMP_DLG_SIZE] =
{
   /* 0 */   { d_textbox_proc, 0,   0,   224, 160+16-80+8+16*4, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   0,                      NULL, NULL },

   /* 1 */   { d_text_proc,    16,  8,   192, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Save countries units bmps", NULL, NULL },
   /* 2 */   { d_radio_proc,   16,  80+16-80+8,  128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,  0,D_SELECTED,  0,   0,   (void *)"All countries",             NULL, NULL },
   /* 3 */   { d_radio_proc,   16,  96+16-80+8,  128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Single country",        NULL, NULL },

   /* 4 */   { d_text_proc,    16,  120+16-80+8,   192, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Details", NULL, NULL },

   /* 5  */   { d_text_proc,    16,  120+16-80+8+16*1+8,   128, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Country", NULL, NULL },
   /* 6  */   { d_edit_proc,    16+128,  120+16-80+8+16*1+8,   40, 16,  GUI_FG_COLOR,GUI_EDIT_COLOR,   0,    0,      4,   1,  0  },

   /* 7  */   { d_text_proc,    16,  120+16-80+8+16*2+8,   128, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Month", NULL, NULL },
   /* 8  */   { d_edit_proc,    16+128,  120+16-80+8+16*2+8,   40, 16,  GUI_FG_COLOR,GUI_EDIT_COLOR,   0,    0,      4,   1,  0  },

   /* 9  */   { d_text_proc,    16,  120+16-80+8+16*3+8,   128, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Year", NULL, NULL },
   /* 10 */   { d_edit_proc,    16+128,  120+16-80+8+16*3+8,   40, 16,  GUI_FG_COLOR,GUI_EDIT_COLOR,   0,    0,      4,   1,  0  },

   /* 11 */   { d_button_proc,  16,  112+16-80+8+16*5+8, 88,  16,  GUI_FG_COLOR,   GUI_BG_COLOR, 'c',D_EXIT,     0,   0,   (void *)"&Cancel",              NULL, NULL },
   /* 12 */   { d_button_proc,  120, 112+16-80+8+16*5+8, 88,  16,  GUI_FG_COLOR,   GUI_BG_COLOR, 's',D_EXIT,     0,   0,   (void *)"&Save bmps",             NULL, NULL },
		      { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
		      { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG units_bmp_dlg[UNITS_BMP_DLG_SIZE] =
{
   /* 0 */   { d_textbox_proc, 0,   0,   224, 160+16-80+8+16*8, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   0,                      NULL, NULL },

   /* 1 */   { d_text_proc,    16,  8,   192, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Save units bmps", NULL, NULL },
   /* 2 */   { d_radio_proc,   16,  80+16-80+8,  128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,  0,D_SELECTED,  0,   0,   (void *)"All units on sheet",             NULL, NULL },
   /* 3 */   { d_radio_proc,   16,  96+16-80+8,  128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Single unit",        NULL, NULL },
   /* 4 */   { d_radio_proc,   16,  112+16-80+8, 128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Range of units",           NULL, NULL },

   /* 5 */   { d_text_proc,    16,  120+16-80+8+16,   192, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Add information", NULL, NULL },
   /* 6 */   { d_radio_proc,   16,  120+16-80+8+16*2,  128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,  0,D_SELECTED,  1,   0,   (void *)"None",             NULL, NULL },
   /* 7 */   { d_radio_proc,   16,  120+16-80+8+16*3,  128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      1,   0,   (void *)"Flags",        NULL, NULL },
   /* 8 */   { d_radio_proc,   16,  120+16-80+8+16*4,  128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      1,   0,   (void *)"Descriptions",        NULL, NULL },
   /* 9 */   { d_radio_proc,   16,  120+16-80+8+16*5,  128+32, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      1,   0,   (void *)"Flags+Descriptions",        NULL, NULL },

   /* 10 */   { d_text_proc,    16,  120+16-80+8+16*6+8,   128, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Icon ID (start)", NULL, NULL },
   /* 11 */   { d_edit_proc,    16+128,  120+16-80+8+16*6+8,   40, 16,  GUI_FG_COLOR,GUI_EDIT_COLOR,   0,    0,      4,   1,  0  },

   /* 12 */   { d_text_proc,    16,  120+16-80+8+16*7+8,   128, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Icon ID (end)", NULL, NULL },
   /* 13 */   { d_edit_proc,    16+128,  120+16-80+8+16*7+8,   40, 16,  GUI_FG_COLOR,GUI_EDIT_COLOR,   0,    0,      4,   1,  0  },

   /* 14 */   { d_button_proc,  16,  112+16-80+8+16*9+8, 88,  16,  GUI_FG_COLOR,   GUI_BG_COLOR, 'c',D_EXIT,     0,   0,   (void *)"&Cancel",              NULL, NULL },
   /* 15 */   { d_button_proc,  120, 112+16-80+8+16*9+8, 88,  16,  GUI_FG_COLOR,   GUI_BG_COLOR, 's',D_EXIT,     0,   0,   (void *)"&Save bmps",             NULL, NULL },
		     { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
		     { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};


DIALOG load_layer_dlg[LOAD_LAYER_DLG_SIZE] = {
/* 0 */{ d_textbox_proc, 0,   0,   224, 56+16+16+16+16, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   0,                      NULL, NULL },
/* 1 */{ d_check_proc,   16,  24,  168, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      1,   0,   (void *)"name layer",   NULL, NULL },
/* 2 */{ d_check_proc,   16,  40,  168, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      1,   0,   (void *)"road layer",     NULL, NULL },
/* 3 */{ d_check_proc,   16,  56,  168, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      1,   0,   (void *)"tiles layer",       NULL, NULL },
/* 4 */{ d_check_proc,   16,  56+16,  168, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      1,   0,   (void *)"terrain type",  NULL, NULL },
/* 5 */{ d_text_proc,    16,  8,   192, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)"Load map layer", NULL, NULL },
/* 6 */{ d_button_proc,  16,  56+16+16+8, 88,  16,  GUI_FG_COLOR,   GUI_BG_COLOR, 'c',D_EXIT,     0,   0,   (void *)"&Cancel",              NULL, NULL },
/* 7 */{ d_button_proc,  120, 56+16+16+8, 88,  16,  GUI_FG_COLOR,   GUI_BG_COLOR, 'l',D_EXIT,     0,   0,   (void *)"&Load",             NULL, NULL },
	   { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL }
	};

DIALOG load_dlg[7+4]= {
/* 0*/  { d_textbox_proc, 0, 0, 198 + 16 + 60 - 18 + 16, 30 + 18 + 12 + 18 * 5, GUI_FG_COLOR, GUI_BG_COLOR, 0, 0, 0, 0, 0, 0, 0 },
/* 1 */	{ d_text_proc ,	16 ,	12 ,	198 ,	12 ,	GUI_FG_COLOR,GUI_BG_COLOR,	0 ,	0 ,	0 ,	0 ,	 "Scenario number to load" },
/* 2 */	{ d_button_proc ,	22 ,	30+18*5 ,	96 ,	18 ,GUI_FG_COLOR,GUI_BG_COLOR,	'c' ,	D_EXIT ,	0 ,	0 ,	 "&Cancel" },
/* 3 */	{ d_button_proc ,	178+16-36 ,	30+18*5 ,	96 ,	18 ,	GUI_FG_COLOR,GUI_BG_COLOR,	'l' ,	D_EXIT ,	0 ,	0 ,	 "&Load" },
/* 4 */	{ d_edit_proc ,	214 ,	12 ,	24+16 ,	12 ,	GUI_FG_COLOR,GUI_EDIT_COLOR,	0 ,	0 ,	4 ,	1 ,	/*EDIT*/0},
/* 5 */ { d_radio_proc,16,30+18*0,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,0,D_SELECTED,95,0,"Same as loaded"},
/* 6 */ { d_radio_proc,16,30+18*1,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,'p',D_DISABLED,95,0,"&PGDOS"},
/* 7 */ { d_radio_proc,16,30+18*2,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,'f',D_DISABLED,95,0,"PG&F"},
/* 8 */ { d_radio_proc,16,30+18*3,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,'a',D_DISABLED,95,0,"&AG/PGWIN"},
        { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL }
 };

DIALOG saveas_dlg[7+5+1]= {
/* 0*/  { d_textbox_proc, 0, -16, 198 + 16 + 60 + 2 * 18, 30 + 18 + 12 + 18 * 6 + 16, GUI_FG_COLOR, GUI_BG_COLOR, 0, 0, 0, 0, 0, 0, 0 },
/* 1 */	{ d_text_proc,16,12,200,12,GUI_FG_COLOR,GUI_BG_COLOR,0,0,0,0,"Equipment file used for saving" },
/* 2 */	{ d_button_proc,22,30+18*6,96,18,GUI_FG_COLOR,GUI_BG_COLOR,'c',D_EXIT,0,0,"&Cancel" },
/* 3 */	{ d_button_proc,178+3*18-36,30+18*6,96,18,GUI_FG_COLOR,GUI_BG_COLOR,'s',D_EXIT,0,0,"&Save" },
/* 4 */	{ d_edit_proc,16,12+18,200,12,GUI_FG_COLOR,GUI_EDIT_COLOR,0,0,2,1,/*EDIT*/0},
/* 5 */ { d_radio_proc,16,30+18*1,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,0,D_SELECTED,95,0,"Same as loaded"},
/* 6 */ { d_radio_proc,16,30+18*2,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,'p',0,95,0,"&PGDOS"},
/* 7 */ { d_radio_proc,16,30+18*3,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,'f',0,95,0,"PG&F"},
/* 8 */ { d_radio_proc,16,30+18*4,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,'a',0,95,0,"&AG/PGWIN"},
/* 9 */	{ d_button_proc,200+2*18,12+18,60,18,GUI_FG_COLOR,GUI_BG_COLOR,'e',D_EXIT,0,0,"&Equip" },
/* 10 */{ d_text_proc,16+10*8,-8,200,12,GUI_FG_COLOR,GUI_BG_COLOR,0,0,0,0,"Save As Dialog" },
		{ d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL }
};

DIALOG configuration_dlg[7+3]= {
/* 0*/  { d_textbox_proc, 0, 0, 198 + 16 + 60 + 5 * 18, 30 + 18 + 12 + 18 * 5, GUI_FG_COLOR, GUI_BG_COLOR, 0, 0, 0, 0, 0, 0, 0 },
/* 1 */	{ d_text_proc ,	16 ,	12 ,	200 ,	12 ,	GUI_FG_COLOR,GUI_BG_COLOR,	0 ,	0 ,	0 ,	0 ,	 "Configuration of FPGE" },
/* 2 */	{ d_button_proc ,	22 ,	30+18*5 ,	60 ,	18 ,	GUI_FG_COLOR,GUI_BG_COLOR,	'c' ,	D_EXIT ,	0 ,	0 ,	 "&Cancel" },
/* 3 */	{ d_button_proc ,	178+6*18 ,	30+18*5 ,	60 ,	18 ,	GUI_FG_COLOR,GUI_BG_COLOR,	'd' ,	D_EXIT ,	0 ,	0 ,	 "&Done" },
/* 4 */ { d_check_proc,16,30+18*0,60+70,12,GUI_FG_COLOR,GUI_BG_COLOR,0,0,95,0,"TBD"},
/* 5 */ { d_check_proc,16,30+18*1,60+70,12,GUI_FG_COLOR,GUI_BG_COLOR,0,0,95,0,"32bit graphics (requires FPGE restart)"},
/* 6 */ { d_check_proc,16,30+18*2,60+70,12,GUI_FG_COLOR,GUI_BG_COLOR,0,0,95,0,"Confirm load"},
/* 7 */ { d_check_proc,16,30+18*3,60+70,12,GUI_FG_COLOR,GUI_BG_COLOR,0,0,95,0,"Confirm exit"},
		{ d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL }
 };


/* -----  this defines the objects that are part of dmain */


DIALOG main_dlg[MAIN_DLG_SIZE]=
{
{d_clear_proc, 0, 0, 0, 0, 0, SCREEN_COLOR, 0, 0, 0, 0, 0, 0, 0 },//0 DIALOG dmClear=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 5 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'i', 0, 1, 0, (void *) "UN&IT", 0, 0 }, //1 DIALOG dmUnitBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 6 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 0, 0, 34, 0, (void *) "ULIST", 0, 0 },
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 12 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 't', 0, 2, 0, (void *) "&TERR", 0, 0 },// 2 DIALOG dmTerrBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 14 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'v', 0, 3, 0, (void *) "&VICT", 0, 0 },// 3 DIALOG dmVictBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 13 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'f', 0, 4, 0, (void *) "&FLAGS", 0, 0 },// 4 DIALOG dmOwnerBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 3 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 0, 0, 5, 0, (void *) "LOAD", 0, 0 },// 5 DIALOG dmLoadBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 2 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 0, 0, 6, 0, (void *) "SAVE", 0, 0 },// 6 DIALOG dmSaveBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 1 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 0, D_EXIT, 7, 0, (void *) "EXIT", 0, 0 },// 7 DIALOG dmExitBtn=
{ d_mapbmp_proc, 0, LINE_2_Y, MAP_W, MAP_H, 0, MAP_COLOR, 0, 0, 0, 0, 0, 0, 0 },// 8 DIALOG dmMapBmp=
{ d_arrow_proc, HSLIDE_X - 2 - HSLIDE_H, HSLIDE_Y, HSLIDE_H, HSLIDE_H, 0, 0, 0, 0, (int) 'L', 0, 0, 0, 0 },// 9 DIALOG dmLeftArrowBmp=
{ d_arrow_proc, HSLIDE_X + 3 + HSLIDE_W, LINE_1_Y, HSLIDE_H, HSLIDE_H, 0, 0, 0, 0, (int) 'R', 0, 0, 0, 0 },// 10 DIALOG dmRightArrowBmp=
{ d_arrow_proc, VSLIDE_X, VSLIDE_Y - VSLIDE_W - 2, VSLIDE_W, VSLIDE_W, 0, 0, 0, 0, (int) 'U', 0, 0, 0, 0 },// 11 DIALOG dmUpArrowBmp=
{ d_arrow_proc, VSLIDE_X, VSLIDE_Y + 2 + VSLIDE_H, VSLIDE_W, VSLIDE_W, 0, 0, 0, 0, (int) 'D', 0, 0, 0, 0 },// 12 DIALOG dmDownArrowBmp=
{ d_hslide_proc, HSLIDE_X, LINE_1_Y, HSLIDE_W, HSLIDE_H, SLIDE_FG_COLOR, SLIDE_BG_COLOR, 0, 0, 100, 0, 0, 0, 0 },// 13 DIALOG dmHSlide=
{ d_vslide_proc, VSLIDE_X, VSLIDE_Y, VSLIDE_W, VSLIDE_H, SLIDE_FG_COLOR, SLIDE_BG_COLOR, 0, 0, 100, 0, 0, 0, 0 },// 14 DIALOG dmVSlide=
{ d_GndAir_proc, HSLIDE_X + (HSLIDE_W - 200) / 2,                     LINE_1_Y - 13, 3 * 8 + 12, 10, FG_COLOR, SCREEN_COLOR, 'g', D_SELECTED, 100, 0, (void *) "&Gnd", 0, 0 },// 15 DIALOG dmGndRB=
{ d_GndAir_proc, HSLIDE_X + (HSLIDE_W - 200) / 2 + 50,                LINE_1_Y - 13, 3 * 8 + 12, 10, FG_COLOR, SCREEN_COLOR, 'a', 0, 100, 0, (void *) "&Air", 0, 0 },// 16 DIALOG dmAirRB=
{ d_GndAir_proc, HSLIDE_X + (HSLIDE_W - 200) / 2 + 50 + 50,           LINE_1_Y - 13, 5 * 8 + 12, 10, FG_COLOR, SCREEN_COLOR, 0, 0, 100, 0, (void *) "Trans", 0, 0 },// dmSeaRBIdx
{ d_GndAir_proc, HSLIDE_X + (HSLIDE_W - 200) / 2 + 50 + 50 + 50 + 16, LINE_1_Y - 13, 4 * 8 + 12, 10, FG_COLOR, SCREEN_COLOR, 0, 0, 100, 0, (void *) "None", 0, 0 },// dmNoneRBIdx
{ d_mapinfo_proc, 1, 2, 180, 50, FG_COLOR, SCREEN_COLOR, 0, 0, 0, 0, MapInfoTxt, 0, 0 },// 17 DIALOG dmMapInfoTxt=
{ d_mapstatus_proc, SCREEN_X - 200 - 10, 2, 200, 48, FG_COLOR, SCREEN_COLOR, 0, 0, 0, 0, MapStatusTxt, 0, 0 },// 18 DIALOG dmMapStatusTxt=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 7 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'p', 0, 8, 0, (void *) "&PLACE", 0, 0 },// 19 DIALOG dmPlaceBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 8 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'x', 0, 9, 0, (void *) "E&XP", 0, 0 },// 20 DIALOG dmExpBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 9 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 's', 0, 10, 0, (void *) "&STR", 0, 0 },// 21 DIALOG dmStrBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 10 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'e', 0, 11, 0, (void *) "&ENT", 0, 0 },// 22 DIALOG dmEntBtn=
{ d_coreRB_proc, HSLIDE_X + (HSLIDE_W - 200) / 2      , 2, 12 + 4 * 8, 10, FG_COLOR, SCREEN_COLOR, 'r', D_SELECTED, 99, 0, (void *) "Co&re", 0, 0 },// 23 DIALOG dmCoreRB=
{ d_coreRB_proc, HSLIDE_X + (HSLIDE_W - 200) / 2 + 50 , 2, 12 + 3 * 8, 10, FG_COLOR, SCREEN_COLOR, 'u', 0, 99, 0, (void *) "A&ux", 0, 0 },// 24 DIALOG dmAuxRB=
{ d_coreRB_proc, HSLIDE_X + (HSLIDE_W - 200) / 2 + 100, 2, 12 + 6 * 8, 10, FG_COLOR, SCREEN_COLOR, 'l', 0, 99, 0, (void *) "A&llied", 0, 0 },// 25 DIALOG dmAlliedRB=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 4 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'c', 0, 26, 0, (void *) "S&CEN", 0, 0 },// 26 DIALOG dmScenBtn=
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 11 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, ' ', 0, 27, 0, (void *) "TILES", 0, 0 },// 27 dmTileBtn
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 15 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'd', 0, 28, 0, (void *) "&DPLY", 0, 0 },//28 dmDeployBtn button
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 16 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'n', 0, 29, 0, (void *) "&NEUT", 0, 0 },//29 dmNeutralBtn button
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 20 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 'h', 0, 30, 0, (void *) "&HELP", 0, 0 }, //30 dmHelpBtn button
{ d_btn_proc, MAIN_BUTTON_X, EXIT_Y - 19 * BUTTON_SIZE, BUTTON_W, BUTTON_H, FG_COLOR, BG_COLOR, 0, 0, 31, 0, (void *) "REPL", 0, 0 },
{d_btn_proc,   MAIN_BUTTON_X,EXIT_Y-18*BUTTON_SIZE,BUTTON_W,BUTTON_H,   FG_COLOR,BG_COLOR,   0,    0,   32,0,   (void *)"RIMP",0,0},
{d_btn_proc,   MAIN_BUTTON_X,EXIT_Y-17*BUTTON_SIZE,BUTTON_W,BUTTON_H,   FG_COLOR,BG_COLOR,   0,    0,   33,0,   (void *)"REXP",0,0},
{d_filterstatus_proc,	SCREEN_X - 8*9  ,2+48-1,8*9,16+8,	FG_COLOR,SCREEN_COLOR,  0,  0,  0,0,  FilterStatusTxt,0,0},

{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_1,0,(void *)ctrl_1_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_2,0,(void *)ctrl_2_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_3,0,(void *)ctrl_3_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_4,0,(void *)ctrl_4_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_5,0,(void *)ctrl_5_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_6,0,(void *)ctrl_6_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_7,0,(void *)ctrl_7_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_8,0,(void *)ctrl_8_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_9,0,(void *)ctrl_9_keycallback,0,0},
//----------
//---------
//-------
{d_keyboard_proc,0,0,0,0,0,0,'v'-'a'+1,0,0,0,(void *)ctrl_v_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'a'-'a'+1,0,0,0,(void *)ctrl_a_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_N,0,(void *)ctrl_alt_n_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'t'-'a'+1,0,0,0,(void *)ctrl_t_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_R,0,(void *)ctrl_alt_r_keycallback,0,0},
//{d_keyboard_proc,0,0,0,0,0,0,'r'-'a'+1,0,KEY_R,0,(void *)ctrl_r_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'d'-'a'+1,0,0,0,(void *)ctrl_d_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F,0,(void *)ctrl_alt_f_keycallback,0,0},
//{d_keyboard_proc,0,0,0,0,0,0,'s'-'a'+1,0,0,0,(void *)ctrl_s_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_S,0,(void *)ctrl_alt_s_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'x'-'a'+1,0,0,0,(void *)ctrl_x_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'e'-'a'+1,0,0,0,(void *)ctrl_e_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'b'-'a'+1,0,0,0,(void *)ctrl_b_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'g'-'a'+1,0,0,0,(void *)ctrl_g_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_M,0,(void *)ctrl_alt_m_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'q'-'a'+1,0,0,0,(void *)ctrl_q_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'y'-'a'+1,0,0,0,(void *)ctrl_y_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'p'-'a'+1,0,0,0,(void *)ctrl_p_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'l'-'a'+1,0,0,0,(void *)ctrl_l_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'w'-'a'+1,0,0,0,(void *)ctrl_w_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'h'-'a'+1,0,0,0,(void *)ctrl_h_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'i'-'a'+1,0,0,0,(void *)ctrl_i_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'z'-'a'+1,0,0,0,(void *)ctrl_z_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_U,0,(void *)ctrl_alt_u_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'c'-'a'+1,0,0,0,(void *)ctrl_c_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'o'-'a'+1,0,0,0,(void *)ctrl_o_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'k'-'a'+1,0,0,0,(void *)ctrl_k_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'j'-'a'+1,0,0,0,(void *)ctrl_j_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'1',0,0,0,(void *)key_1_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'2',0,0,0,(void *)key_2_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'3',0,0,0,(void *)key_3_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'4',0,0,0,(void *)key_4_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'5',0,0,0,(void *)key_5_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'6',0,0,0,(void *)key_6_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'7',0,0,0,(void *)key_7_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'8',0,0,0,(void *)key_8_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,'9',0,0,0,(void *)key_9_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F1,0,(void *)f1_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F2,0,(void *)f2_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F3,0,(void *)f3_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F4,0,(void *)f4_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F5,0,(void *)f5_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F6,0,(void *)f6_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F7,0,(void *)f7_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F8,0,(void *)f8_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F9,0,(void *)f9_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F10,0,(void *)f10_keycallback,0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F11,0,(void *)f11_keycallback,0,0},
{d_menu_proc,        0,    0,    0,    0,   0,  0,    0,      0,       0,   0,   fpge_menu,     NULL,   NULL  },
{d_coreRB_proc,HSLIDE_X+(HSLIDE_W-200)/2+100+12+7*8,2,12+10*8,10,FG_COLOR,SCREEN_COLOR,'l',0,99,0,(void *)"Allied aux",0,0},
{d_keyboard_proc,0,0,0,0,0,0,0,0,KEY_F12,0,(void *)f12_keycallback,0,0},
{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
//DIALOG dmNull=
{NULL}
}; //end of dm definition
/* -------------------------------------------- */

int about_box(){
	alert(AboutTxt1, AboutTxt2, AboutTxt3,"&Ok", NULL, 'o', 0);
	return D_REDRAW;
}

int help_dialog(){
	centre_dialog(help_dlg);
    do_dialog(help_dlg,-1);
	broadcast_dialog_message(MSG_DRAW,0);
	return D_O_K;
}

int change_weather(){
	showWeather=(int)active_menu->dp;
	return draw_new_weather();
}

int change_unique_names(){
	drawNames=(int)active_menu->dp;
	return draw_new_unique_names();
}

int change_display_names(){
	showDecimal=(int)active_menu->dp;
	return draw_new_display_names();
}

int change_spottings(){
	show_ranges=(int)active_menu->dp;
	return draw_new_spottings();
}


int new_map_dialog(){

	if (tile_mode == 1) {
		tile_cleanup();
	}

	centre_dialog(makemap_dlg);
	do_dialog(makemap_dlg,-1);
	return D_REDRAW;
}

int load_dialog(){
	 //ask user
	if ((getScenarioNumber()==0)||(alert("Do you really want to load map ?", "This action will erase all unsaved changes already made.", NULL,
      "&Yes", "&No", 'y', 'n')==1))
	 {
		 if (load_scenario_dialog()){
		 //read_map(scn_number);
		 //load_scn(scn_number,0);

		main_dlg[dmHSlideIdx].d1=get_h_slide_max();
		main_dlg[dmHSlideIdx].d2=0;
		vslide_max=get_v_slide_max();
		main_dlg[dmVSlideIdx].d1=vslide_max;
		main_dlg[dmVSlideIdx].d2=vslide_max;

		 rectfill(map_bmp,0,0,MAP_W,MAP_H,MAP_COLOR);

		 draw_map(map_bmp,map_x0,map_y0,tiles_high,tiles_wide);
		 //dm[dmLoadBtnIdx].flags^=D_SELECTED;
		 if (pgf_mode)
			 sprintf(MapStatusTxt,"PGF scenario %d loaded\nPick an Operation",getScenarioNumber());
		 else
			 sprintf(MapStatusTxt,"Scenario %d loaded\nPick an Operation",getScenarioNumber());
		   }
	 }
	 main_dlg[dmLoadBtnIdx].flags&=~D_SELECTED;
    broadcast_dialog_message(MSG_DRAW,0);
	return D_O_K;
}

int save_pg_scenario(int save_type){
	  char backup_temp[80];

	if (backup_not_saved) {
		sprintf(backup_temp, "copy game0%02d.scn game0%02d.bak", getScenarioNumber(),
				getScenarioNumber());
		system(backup_temp);
		sprintf(backup_temp, "copy map%02d.set map_set%02d.bak", getScenarioNumber(),
				getScenarioNumber());
		system(backup_temp);
		sprintf(backup_temp, "copy map%02d.stm map_stm%02d.bak", getScenarioNumber(),
				getScenarioNumber());
		system(backup_temp);
		backup_not_saved = 0;
	}
	save_scenario(getScenarioNumber(),save_type);
	save_stm(getScenarioNumber(),0);
	save_set(getScenarioNumber(),0);
	//d_mapstatus_proc(MSG_DRAW,&(dm[dmMapStatusIdx]),0);
	//sprintf(MapStatusTxt, "Scenario %d saved\nPick an Operation", scn_number);
	//d_mapstatus_proc(MSG_DRAW, &(dm[dmMapStatusIdx]), 0);
	return D_O_K;
}

int save_dialog(){
	//not really a dialog
	int error;

	if (tile_mode == 1) {
		tile_cleanup();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}

	error=save_scenario_procedure();

	if (error)
		sprintf(MapStatusTxt, "Error during save !!\nScenario %d not saved\nPick an Operation", getScenarioNumber());
	else
		sprintf(MapStatusTxt, "Scenario %d saved\nPick an Operation", getScenarioNumber());

	d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
	return error;
}


int save_scenario_procedure() {

	if (pgf_mode){
		return save_pgf_pgscn(getScenarioNumber(), DO_NOT_SHOW_LOGS, NORMAL_SAVE,0,0,0);
	}else{
		if (ag_mode){
			return  save_ag(NORMAL_SAVE);
		}else{
			//save with bakcup
			return save_pg_scenario(NORMAL_SAVE);
		}
	}
}

int compare_by_name_and_country(const void *ii, const void *jj)
{
    struct conversion_equipment *i=(struct conversion_equipment *)ii;
    struct conversion_equipment *j=(struct conversion_equipment *)jj;

    if (strcmp(i->name,j->name)!=0)
    	return strcmp(i->name,j->name);
    return i->country-j->country;
}

int compare_by_old_id_and_country(const void *ii, const void *jj)
{
    struct conversion_equipment *i=(struct conversion_equipment *)ii;
    struct conversion_equipment *j=(struct conversion_equipment *)jj;

    if (i->old_id-j->old_id!=0)
    	return i->old_id-j->old_id;
    return i->country-j->country;
}

int compare_by_old_id(const void *ii, const void *jj)
{
    struct conversion_equipment *i=(struct conversion_equipment *)ii;
    struct conversion_equipment *j=(struct conversion_equipment *)jj;

    return i->old_id-j->old_id;
}

int save_as_dialog(){
	int i,loop=1,error;

	saveas_dlg[4].dp=&EquipmentFileToSave;
	centre_dialog(saveas_dlg);

	while (loop){
    	i=do_dialog(saveas_dlg,-1);
    	if (i==2 || i==3 || i==-1) loop=0;
    	if (i==9) {
    		file_select_ex("Choose equipment file",EquipmentFileToSave,"pgeqp;eqp",MAX_PATH,OLD_FILESEL_WIDTH,OLD_FILESEL_HEIGHT);
    	}
    }

    // we try to save
    if (i == 3) {

		if (tile_mode == 1) {
			tile_cleanup();
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		}

		if (saveas_dlg[5].flags & D_SELECTED) {
			save_dialog(); //normal save without conversion
		}
		//save as PG
		if (saveas_dlg[6].flags & D_SELECTED){
			//printf("PG\n");
			if (pgf_mode){
				//we need to convert PGF -> PG
				//load PG equipment for conversion
				error=load_equip(LOAD_CONVERSION_TABLE_ONLY,EquipmentFileToSave);
				if (error) return error;
				qsort(conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
				//TODO: tests, save and convert
				save_pg_scenario(SAVE_WITH_UNIT_IDS_CONVERSION);
			}else{
				//no conversion between PG and AG
				save_dialog(); //normal save without conversion
				//save_pg_scenario(NORMAL_SAVE);
			}
		}

		//save as pgf
		if (saveas_dlg[7].flags & D_SELECTED){
			//printf("PGF\n");
			if (pgf_mode){
				save_pgf_pgscn(getScenarioNumber(), DO_NOT_SHOW_LOGS, NORMAL_SAVE,0,0,0);
			}else{
				//we need to convert PG -> PGF
				//load PG equipment for conversion
				//save and convert
				//load PGF equipment for conversion
				error=load_pgf_equipment(LOAD_CONVERSION_TABLE_ONLY,EquipmentFileToSave);
				if (error) return error;
				qsort(conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
				save_pgf_pgscn(getScenarioNumber(), DO_NOT_SHOW_LOGS, SAVE_WITH_UNIT_IDS_CONVERSION,0,0,0);
			}
		}

		//save as ag
		if (saveas_dlg[8].flags & D_SELECTED){
			//printf("AG\n");
			if (pgf_mode){
				//we need to convert PGF -> PG
				//load PG equipment for conversion
				error=load_equip(LOAD_CONVERSION_TABLE_ONLY,EquipmentFileToSave);
				if (error) return error;
				qsort(conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
				//TODO: tests, save and convert
				save_pg_scenario(SAVE_WITH_UNIT_IDS_CONVERSION);
			}else{
				//no conversion between PG and AG
				save_dialog(); //normal save without conversion
				//save_pg_scenario(NORMAL_SAVE);
			}
		}
	}
    broadcast_dialog_message(MSG_DRAW,0);
    return D_O_K;
}

int configuration_dialog(){

	centre_dialog(configuration_dlg);
	do_dialog(configuration_dlg,-1);
	broadcast_dialog_message(MSG_DRAW,0);
    return D_O_K;
}


int do_scenario_dialog() {
    sprintf(MapStatusTxt,"Scenario");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

	//count the units and store in scn_buffer first
	scn_buffer[CORE_UNITS] = count_axis_core() & 255;
	scn_buffer[AUX_UNITS] = count_axis_aux() & 255;
	scn_buffer[ALLIED_UNITS] = count_allied_core() & 255;
	scn_buffer[AUX_ALLIED_UNITS] = count_allied_aux() & 255;
	//update the victory hex array
	get_victory_hexes();
	centre_dialog(senario_dlg);
	do_dialog(senario_dlg, 3); //focus on scenario number
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	pick_msg();
	//strncpy(MapStatusTxt, "Pick operation",256);
	broadcast_dialog_message(MSG_DRAW, 0);
	return D_O_K; //never gets selected
}

int units_place_mode(){
	if (tile_mode == 1) {
		tile_cleanup();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
    edit_op=edit_place;

    unum_selected=atoi(uselected);
    tnum_selected=atoi(tselected);
    cnum_selected=atoi(cselected);
    anum_selected=atoi(aselected);

    do_place_status();
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
    return D_O_K;
}

int unit_dialog() {
    sprintf(MapStatusTxt,"Units");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

	centre_dialog(unit_dlg);
	do_dialog(unit_dlg, -1);
	pick_msg();
	//strncpy(MapStatusTxt, "Pick operation",256);
	broadcast_dialog_message(MSG_DRAW, 0);
	return D_O_K; //don't let the button get selected
}

int unit_list_dialog() {
    sprintf(MapStatusTxt,"Units list");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

	centre_dialog(unitslist_dlg);
	do_dialog(unitslist_dlg, -1);
	pick_msg();
	//strncpy(MapStatusTxt, "Pick operation",256);
	//draw map
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	broadcast_dialog_message(MSG_DRAW, 0);
	return D_O_K; //don't let the button get selected
}

int search_replace_dialog(){

    sprintf(MapStatusTxt,"Replace");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
	centre_dialog(searchreplace_dlg);
    do_dialog(searchreplace_dlg,-1);
    broadcast_dialog_message(MSG_DRAW,0);
    pick_msg();
    //sprintf(MapStatusTxt,"Pick operation");
    //d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
	return D_O_K;
}

int map_resize_dialog(){

    sprintf(MapStatusTxt,"Map resize");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

    centre_dialog(resize_dlg);
    if (do_dialog(resize_dlg,-1)==MapResizeResizeIdx){
    	map_x0=0;
    	map_y0=0;
    	position_gui_elements(w_old,h_old);
    	vslide_max=get_v_slide_max();
    	main_dlg[dmHSlideIdx].d1=get_h_slide_max();
    	main_dlg[dmHSlideIdx].d2=map_x0;
    	main_dlg[dmVSlideIdx].d1=vslide_max;
    	main_dlg[dmVSlideIdx].d2=vslide_max-map_y0;

    	d_hslide_proc(MSG_DRAW,&(main_dlg[dmHSlideIdx]),0);
    	d_vslide_proc(MSG_DRAW,&(main_dlg[dmVSlideIdx]),0);

    	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);

    }
    pick_msg();
    //sprintf(MapStatusTxt,"Pick operation");
    broadcast_dialog_message(MSG_DRAW,0);
    //d_mapstatus_proc(MSG_DRAW,&(dm[dmMapStatusIdx]),0);
	return D_O_K;
}



int do_rectangle_import() {
	if (tile_mode == 1) {
		tile_cleanup();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	edit_op=edit_rimp;
	sprintf(MapStatusTxt,"Importing Map Fragment\n");
	strncat(MapStatusTxt,"Left Click to set insert point",256);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
	return D_O_K;
}

int do_rectangle_export() {
	if (tile_mode == 1) {
		tile_cleanup();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	edit_op=edit_rexp;
	rexp_left_mouse_click=0;
    sprintf(MapStatusTxt,"Exporting Map Fragment\n");
    strncat(MapStatusTxt,"Left Click to set top-left point\nRight Click to set bottom-right and save",256);
    main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
    //d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
    return D_O_K;
}

int strength_mode() {
	edit_op = edit_str;
	showCounter=0;
	strncpy(MapStatusTxt, "Left Click  + strength\nRight Click - strength",256);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	//draw map
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
	return D_O_K;
}

int expirience_mode(){
    edit_op=edit_exp;
    showCounter=1;
    strncpy(MapStatusTxt,"Left Click  + experience\nRight Click - experience",256);
    main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
    //d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
	//draw map
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
	return D_O_K;
}

int entrench_mode() {
	edit_op = edit_ent;
	showCounter=2;
	strncpy(MapStatusTxt, "Left Click  + entrench\nRight Click - entrench",256);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	//draw map
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	return D_O_K;
}

int flag_display_toggle() {
	   if (showCounter==3) showCounter=-1;
	   else
			if (showCounter==-1) showCounter=3;
			else showCounter=3;

	//draw map


	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
	return draw_new_flag();
}

int transport_mode() {
	edit_op = edit_transport;
	//showCounter=2;
	strncpy(MapStatusTxt, "Left Click -sea tr.\nRight Click - air tr.\nLeft+Shift-del aux. tr\nRight+Shift-del org. tr",256);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	//draw map
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
	return D_O_K;
}

int select_fragment_mode() {
	if (tile_mode == 1) {
		tile_cleanup();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	edit_op = edit_frg_select;
	//showCounter=2;
	strncpy(MapStatusTxt, "Left Click -(un)select\nRight Click - save\nShift Left Click - clear all",256);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	//draw map
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
	return D_O_K;
}

int place_fragment_mode() {
	int dialog_result,dialog_x,dialog_y;
	int end_of_dialog=0;

	if (tile_mode == 1) {
		tile_cleanup();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	//showCounter=2;
	while (!end_of_dialog){
		sprintf(MapStatusTxt,"Map fragments");
		d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

		dialog_x=-32;
		if (place_frg_dialog%2) dialog_x+=SCREEN_W-mapfrg_dlg[0].w;
		dialog_y=-48;
		if (place_frg_dialog/2>0) dialog_y+=SCREEN_H-mapfrg_dlg[0].h;
		//printf("%d -> dx=%d dy=%d\n",place_frg_dialog,dialog_x,dialog_y);

		position_dialog(mapfrg_dlg,dialog_x,dialog_y);
		//printf("(%d,%d)\n",d_map_frg[0].x,d_map_frg[0].y);

		dialog_result=do_dialog(mapfrg_dlg, -1);
		if (dialog_result!=-1 && dialog_result!=MapFrgCancelIdx){
			if (dialog_result==MapFrgSelectIdx){
				edit_op = edit_frg_place;
				end_of_dialog=1;
				strncpy(MapStatusTxt, "Left Click - try\nRight Click - place\nAlt Left Click - choose\nCenter Click - choose",256);
			}
		}else{
			end_of_dialog=1;
			edit_op = edit_none;
			pick_msg();
			//strncpy(MapStatusTxt, "Pick operation",256);
		}
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
		//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
		//draw map
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//broadcast_dialog_message(MSG_DRAW,0);
	}
	return D_O_K;
}


int owner_mode() {

	edit_op = edit_own;
	sprintf(MapStatusTxt, "Editing Hex Owners\n");
	strncat(MapStatusTxt,
			"Left Click for Axis\nRight Click for Allied\nSHIFT+Click to remove\nALT+Click unit flag",256);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW, &(main_dlg[dmMapBmpIdx]), 0);

	return D_O_K;
}

int victory_mode() {
	//setup the victory button ...
	edit_op = edit_vic;
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	sprintf(MapStatusTxt, "Editing Victory Hexes \n Now %d of %d Max\n",
			count_vic_hexes(), MAX_VICTORY_HEXES);
	strncat(MapStatusTxt, "Left Click to add\nRight Click to remove",256);
	//broadcast_dialog_message(MSG_DRAW,0);
	//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW, &(main_dlg[dmMapBmpIdx]), 0);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	return D_O_K;
}

int deploy_mode() {
	//setup the deploy button ...
	//dm[dmDeployBtnIdx].flags&=~D_SELECTED;
	edit_op = edit_deploy;
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	sprintf(MapStatusTxt, "Editing Deployment Hexes \n Now %d of %d Max\n",
			count_deploy_hexes(), MAX_DEPLOY_HEXES);
	strncat(MapStatusTxt, "Left Click to add\nRight Click to remove",256);
	// broadcast_dialog_message(MSG_DRAW,0);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW, &(main_dlg[dmMapBmpIdx]), 0);
	return D_O_K;
}

int neutral_mode() {
	//dm[dmNeutralBtnIdx].flags&=~D_SELECTED;
	//setup the neutral button ...
	edit_op = edit_neutral;
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	sprintf(MapStatusTxt, "Editing Neutral Hexes \n");
	strncat(MapStatusTxt, "Left Click to add\nRight Click to remove",256);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW, &(main_dlg[dmMapBmpIdx]), 0);
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	//broadcast_dialog_message(MSG_DRAW,0);
	return D_O_K;
}

int unitsum_dialog() {
    sprintf(MapStatusTxt,"Units stats");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

	centre_dialog(unitsum_dlg);
	do_dialog(unitsum_dlg, -1);
	pick_msg();
	//strncpy(MapStatusTxt, "Pick operation",256);
	//draw map
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	broadcast_dialog_message(MSG_DRAW, 0);
	return D_O_K; //don't let the button get selected
}

void check_and_save_mapnames() {
	if (mapnames_changed) {
		if (alert("Map names were changed !", "Do you want to save 'mapnames.str' ?", NULL, "&Yes", "&No", 'y', 'n') == 1) {
			save_names();
		}
	}
}

void set_stm_error_dialog(int error_code, char *path){
	char buf[200];

	switch(error_code){
	case 1: //file not found
			snprintf(buf,200,"File not found : %s",get_filename(path));
			break;
	case 2: //file too short
			snprintf(buf,200,"Map does not match current maps size : %s",get_filename(path));
			break;
	case 3: //file map does not match current maps size
		snprintf(buf,200,"Map does not match current maps size : %s",get_filename(path));
		break;
	default: //Unknown error
		snprintf(buf,200,"Unknown error while loading : %s",get_filename(path));
		break;
	}
	alert("ERROR", NULL,buf, "&Continue", NULL, 'c', 0);
}

int load_map_layer_dialog(){
	char path[MAX_PATH];
	char path2[MAX_PATH];
	char message[200];
	int err;

    sprintf(MapStatusTxt,"Load map layers");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

	strncpy(path, ".\\", MAX_PATH);
	canonicalize_filename(path, path, MAX_PATH);
	if (file_select_ex("Select SET file", path, "set", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {
		canonicalize_filename(path, path, MAX_PATH);
		if (exists(path)){
			centre_dialog(load_layer_dlg);
			if (do_dialog(load_layer_dlg,-1)==7){

				if (load_layer_dlg[1].flags & D_SELECTED){
					err=load_tiles_layer(path);
					if (err) set_stm_error_dialog(err,path);
				}
				if (load_layer_dlg[2].flags & D_SELECTED){
					err=load_roads_layer(path);
					if (err) set_stm_error_dialog(err,path);
				}
				if (load_layer_dlg[3].flags & D_SELECTED){
					replace_extension(path2, path, "stm", sizeof(path2));
					err=load_terrain_type_layer(path2); //STM !!!
					if (err) set_stm_error_dialog(err,path2);
				}
				if (load_layer_dlg[4].flags & D_SELECTED){
					err=load_names_layer(path);
					if (err) set_stm_error_dialog(err,path);
				}
				draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			}
		}else{
			snprintf(message,200,"File not found : %s",get_filename(path));
			alert(message, NULL, NULL, "&Continue", NULL, 'c', 0);
		}
	}
	pick_msg();
    //sprintf(MapStatusTxt,"Pick operation");
    broadcast_dialog_message(MSG_DRAW,0);
    //d_mapstatus_proc(MSG_DRAW,&(dm[dmMapStatusIdx]),0);
	return D_O_K;
}

int export_units_bmp_dialog(){
	int i;
	char start_str[2 * 8] = "1";
	char end_str[2 * 8] = "0";
	int start_bmp_idx=0;
	int end_bmp_idx=0;
	int comment_bmps=0;

    sprintf(MapStatusTxt,"Export units bmp.");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

	units_bmp_dlg[11].dp = &start_str;
	units_bmp_dlg[13].dp = &end_str;

	centre_dialog(units_bmp_dlg);
	if (do_dialog(units_bmp_dlg,-1)==15 ){

		if (units_bmp_dlg[2].flags & D_SELECTED) {
			//All units
			start_bmp_idx = 0;
			end_bmp_idx = 0;
		}
		if (units_bmp_dlg[3].flags & D_SELECTED) {
			//Single unit
			start_bmp_idx = atoi(start_str);
			end_bmp_idx = 0;
		}
		if (units_bmp_dlg[4].flags & D_SELECTED) {
			//range units
			start_bmp_idx = atoi(start_str);
			end_bmp_idx = atoi(end_str);
		}
		for(i=0;i<4;i++)
			if(units_bmp_dlg[6+i].flags & D_SELECTED) comment_bmps=i;

		handle_units_bmp_saving(1, 0, start_bmp_idx, end_bmp_idx, 0, 0, 0, comment_bmps);
	}
	pick_msg();
   //sprintf(MapStatusTxt,"Pick operation");
    broadcast_dialog_message(MSG_DRAW,0);

	return D_O_K;
}

int export_country_units_bmp_dialog(){
	char month_str[2 * 8] = "12";
	char year_str[2 * 8] = "44";
	char country_str[2 * 8] = "8";
	int m,y,c;

    sprintf(MapStatusTxt,"Country units bmp.");
    d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

    country_units_bmp_dlg[6].dp = &country_str;
    country_units_bmp_dlg[8].dp = &month_str;
    country_units_bmp_dlg[10].dp = &year_str;

	centre_dialog(country_units_bmp_dlg);
	if (do_dialog(country_units_bmp_dlg,-1)==12 ){

		if (country_units_bmp_dlg[2].flags & D_SELECTED) {
					//All countries
					c=0;
		}
		if (country_units_bmp_dlg[3].flags & D_SELECTED) {
					//One country
					c=atoi(country_str);
		}
		m=atoi(month_str);
		y=atoi(year_str);
		handle_units_bmp_saving(0, 1, 0, 0, c, m, y, 0);
	}
    //sprintf(MapStatusTxt,"Pick operation");
    pick_msg();
    broadcast_dialog_message(MSG_DRAW,0);
	return D_O_K;
}

