/* SCEN.C is the scenario dialog page */
#include <stdio.h>
#include <string.h>
#include <allegro.h>

#include "fpge.h"
#include "scen.h"
#include "load.h"
#include "pgf.h"
#include "tables.h"
#include "vc.h"
#include "class.h"
#include "unit.h"
#include "filename.h"
#include "fpge_colors.h"

// use this flag so that the cancel button doesn't save
int scen_save_data; //set this true on entry false by cancel button

// strings to hold data for the scenario dialog
unsigned char Neutral[6];
unsigned char AxisFlagsOld[6];
unsigned char AlliedFlagsOld[6];

unsigned char drawOnIdle;

char ScnStr[8];
char ScnNameStr[128];
char CtyStr[6*3][8];

char ShortDescStr[SCENARIO_STR_SIZE];
char LongDesStr[SCENARIO_DES_STR_SIZE];

char CurCoreStr[SCENARIO_STR_SIZE];
char CurAuxStr[SCENARIO_STR_SIZE];
char CurAlldStr[SCENARIO_STR_SIZE];
char CurAlldAuxStr[SCENARIO_STR_SIZE];

char MaxCoreStr[SCENARIO_STR_SIZE];
char MaxAlldStr[SCENARIO_STR_SIZE];
char MaxAlldAuxStr[SCENARIO_STR_SIZE];

char MaxAuxStr[SCENARIO_STR_SIZE];//why using MaxAuxStr does not show MaxAux in dialog ?????
char MaxAuxxStr[SCENARIO_STR_SIZE];//this one works ok, but MaxAuxStr must be defined

char MonthStr[SCENARIO_STR_SIZE];
char DayStr[SCENARIO_STR_SIZE];
char YearStr[SCENARIO_STR_SIZE];

char TurnsStr[SCENARIO_STR_SIZE];
char TpdStr[SCENARIO_STR_SIZE];
char DptStr[SCENARIO_STR_SIZE];

char AxisPrestigeAmtStr[SCENARIO_STR_SIZE];
char AlldPrestigeAmtStr[SCENARIO_STR_SIZE];
char AxisPrestigeTurnsStr[SCENARIO_STR_SIZE];
char AlldPrestigeTurnsStr[SCENARIO_STR_SIZE];

char AxisPrestigeStr[SCENARIO_STR_SIZE];
char AlliedPrestigeStr[SCENARIO_STR_SIZE];

char AxisAirTransNumStr[SCENARIO_STR_SIZE];
char AlldAirTransNumStr[SCENARIO_STR_SIZE];
char AxisAirTransTypeStr[SCENARIO_STR_SIZE];
char AlldAirTransTypeStr[SCENARIO_STR_SIZE];

char AxisSeaTransNumStr[SCENARIO_STR_SIZE];
char AlldSeaTransNumStr[SCENARIO_STR_SIZE];
char AxisSeaTransTypeStr[SCENARIO_STR_SIZE];
char AlldSeaTransTypeStr[SCENARIO_STR_SIZE];

char VictoryStr[1024+1];

DIALOG senario_dlg[SCENARIO_DLG_SIZE]={
		{d_clr_proc,0,0,640,480,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     0,0,0  }, // 0 clear
		{d_cancelbtn_proc,COL7-8,SCEN_BUTTON_LINE,S_BUTTON_W,15,SD_FG_COLOR,SD_BG_COLOR,     'c',     D_EXIT,     0,0,     (void *)"&Cancel",0,0  }, // 1 Cancel
		{d_button_proc,COL9-16,SCEN_BUTTON_LINE,S_BUTTON_W,15,SD_FG_COLOR,SD_BG_COLOR,     'd',     D_EXIT,     0,0,     (void *)"&Done",0,0  }, // 2 done
		{d_edit_proc,COL2,ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     7,0,     0,0,0  }, // 3 ScenNumEdit
		{d_edit_proc,COL3+40,ROW1,24,RHEIGHT,  EDIT_FG_COLOR,EDIT_BG_COLOR,       0,       0,       2,0,       0,0,0   }, // 4 the month field
		{d_edit_proc,COL3+72,ROW1,24,RHEIGHT,  EDIT_FG_COLOR,EDIT_BG_COLOR,       0,       0,       2,0,       0,0,0   }, // 5 the day field
		{d_edit_proc,COL3+104,ROW1,24,RHEIGHT,  EDIT_FG_COLOR,EDIT_BG_COLOR,       0,       0,       2,0,       0,0,0   }, // 6 the year field
		{d_ctyedit_proc,COL1,4*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 7 axis country 1
		{d_ctyedit_proc,COL2,4*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 8 allied country 1
		{d_ctyedit_proc,COL1,5*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 9 axis country 2
		{d_ctyedit_proc,COL2,5*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 10 allied country 2
		{d_ctyedit_proc,COL1,6*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,0,     0,     3,0,     0,0,0  }, // 11 axis country 3
		{d_ctyedit_proc,COL2,6*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 12 allied country 3
		{d_ctyedit_proc,COL1,7*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 13 axis country 4
		{d_ctyedit_proc,COL2,7*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 14 allied country 4
		{d_ctyedit_proc,COL1,8*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 15 axis country 5
		{d_ctyedit_proc,COL2,8*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 16 allied country 5
		{d_ctyedit_proc,COL1,9*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 17 axis country 6
		{d_ctyedit_proc,COL2,9*RHEIGHT+ROW1,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 18 allied country 6
		{d_list_proc,COL3,ROW1+10*RHEIGHT,2*CWIDTH-5+16+8,14*RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)add_countries,0,0  }, // 19 country list box
		{d_radio_proc,COL1,11*RHEIGHT+ROW1,CWIDTH,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     100,0,     (void *)"-->",0,0  }, // 20 OrtRtRB
		{d_radio_proc,COL2,11*RHEIGHT+ROW1,CWIDTH,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     100,0,     (void *)"<--",0,0  }, // 21 OrtLfRB
		{d_radio_proc,COL1,14*RHEIGHT+ROW1,CWIDTH,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     200,0,     (void *)"Yes",0,0}, // 22 sdFrtYRB
		{d_radio_proc,COL2,14*RHEIGHT+ROW1,CWIDTH,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     200,0,     (void *)"No",0,0}, // 23 sdFrtNRB
		{d_text_proc,COL1,16*RHEIGHT+ROW1+5,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Scenario Local",0,0 }, // 24 Scenario Locale
		{d_radio_proc,COL1,17*RHEIGHT+ROW1,CWIDTH*1.7,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     400,0,     (void *)"North Africa",0,0}, // 25 North Africa
		{d_radio_proc,COL1,18*RHEIGHT+ROW1,CWIDTH*1.7,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     400,0,     (void *)"South Europe",0,0}, // 26 S. Europe
		{d_radio_proc,COL1,19*RHEIGHT+ROW1,CWIDTH*1.7,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     400,0,     (void *)"North Europe",0,0}, // 27 N. Europe
		{d_radio_proc,COL1,20*RHEIGHT+ROW1,CWIDTH*1.7,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     400,0,     (void *)"East Europe",0,0}, // 28 E. Europe
		{d_text_proc,COL1,23*RHEIGHT+ROW1,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Axis Stance",0,0 }, // 29
		{d_radio_proc,COL1,24*RHEIGHT+ROW1,CWIDTH,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     300,0,     (void *)"Offense",0,0}, // 30 sdAxisOff
		{d_radio_proc,COL2+20,24*RHEIGHT+ROW1,CWIDTH,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     300,0,     (void *)"Defense",0,0}, // 31 sdAxisDef
		{d_text_proc,COL1,25*RHEIGHT+ROW1+5,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Allied Stance",0,0 }, // 32
		{d_radio_proc,COL1,26*RHEIGHT+ROW1,CWIDTH,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     310,0,     (void *)"Offense",0,0}, // 33 sdAlldOff
		{d_radio_proc,COL2+20,26*RHEIGHT+ROW1,CWIDTH,RHEIGHT,EDIT_FG_COLOR,SD_BG_COLOR,     0,     0,     310,0,     (void *)"Defense",0,0}, // 34 sdAlldDef
		{d_text_proc,COL6,ROW2,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Number of Units",0,0 }, // 35
		{d_text_proc,COL6-10,ROW3,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Core",0,0 }, //36
		{d_text_proc,COL7-10,ROW3,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Aux",0,0 }, //37
		{d_text_proc,COL8-10,ROW3,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Allied",0,0 }, //38
		{d_text_proc,COL5,ROW4,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Maximum",0,0 }, //39
		{d_text_proc,COL5,ROW5,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Current",0,0 }, //40
		{d_text_proc,COL6,ROW5,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     0,0,0 }, //41 current core sdCurCore
		{d_text_proc,COL7,ROW5,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     0,0,0 }, //42 current core sdCurAux
		{d_text_proc,COL8,ROW5,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     0,0,0 }, //43 current core sdCurAlld
		{d_edit_proc,COL6,ROW4,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, // 44 sdMaxCore
		{d_edit_proc,COL7,ROW4,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, // 45 sdMaxAux
		{d_edit_proc,COL8,ROW4,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, // 46 sdMaxAlld
		{d_text_proc,COL3,ROW1,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Date",0,0 }, // 47
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		{d_text_proc,(COL1+CWIDTH/3),ROW3_OLD,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Countries",0,0  }, // 48 text
		{d_text_proc,COL1,ROW3_OLD+RHEIGHT,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Axis",0,0  }, // 49 text
		{d_text_proc,COL2,ROW3_OLD+RHEIGHT,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Allied",0,0  }, // 50 text
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		{d_text_proc,COL3+40,ROW2_OLD,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"(mm/dd/yr)",0,0 }, // 51 date format string
		{d_text_proc,COL3+60,ROW1,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"/",0,0 }, // 52 a slash date separator
		{d_text_proc,COL3+92,ROW1,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"/",0,0 }, //53 another slash separator
		{d_text_proc,COL6,ROW5+2*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Turns",0,0 }, //54 turns label
		{d_text_proc,COL6,ROW5+3*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"T/Day",0,0 }, //55 T/day label
		{d_text_proc,COL6,ROW5+4*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Day/T",0,0 }, //56 Day/T label
		{d_edit_proc,COL7,ROW5+2*RHEIGHT,36,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //57 the turns edit
		{d_edit_proc,COL7,ROW5+3*RHEIGHT,36,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //58 the turns/day edit
		{d_edit_proc,COL7,ROW5+4*RHEIGHT,36,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //59 the day/t edit
		{d_text_proc,COL6-CWIDTH/2,ROW5+6*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"AI Prestige Settings",0,0 }, // 60 Prestige Label
		{d_text_proc,COL6,ROW5+7*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Axis",0,0 }, // 61
		{d_text_proc,COL7,ROW5+7*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Allied",0,0 }, // 62
		{d_text_proc,COL5+16,ROW5+8*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Amount",0,0 }, // 63
		{d_text_proc,COL5+16,ROW5+9*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Turns",0,0 }, // 64
		{d_edit_proc,COL6,ROW5+8*RHEIGHT,48,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     5,0,     0,0,0 }, //65 Axis AI amt
		{d_edit_proc,COL7,ROW5+8*RHEIGHT,48,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     5,0,     0,0,0 }, //66 Alld AI amt
		{d_edit_proc,COL6,ROW5+9*RHEIGHT,36,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //67 Axis AI turns
		{d_edit_proc,COL7,ROW5+9*RHEIGHT,36,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //68 Alld AI turns
		{d_text_proc,COL1,ROW1,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Scen #",0,0  }, // 69 text
		{d_text_proc,COL5+CWIDTH/2,ROW5+11*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Starting Prestige",0,0 }, // 70 Prestige label
		{d_text_proc,COL5+CWIDTH/2,ROW5+12*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Axis",0,0 }, //71 Axis Prestige label
		{d_text_proc,COL5+CWIDTH/2,ROW5+13*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Allied",0,0 }, //72 Allied Prestige label
		{d_edit_proc,COL6+CWIDTH/2,ROW5+12*RHEIGHT,48,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     5,0,     0,0,0 }, //73 sdAxisPrestige
		{d_edit_proc,COL6+CWIDTH/2,ROW5+13*RHEIGHT,48,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     5,0,     0,0,0 }, //74 sdAlliedPrestige
		{d_text_proc,COL5+CWIDTH/2,ROW5+15*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Auxillary Transport",0,0 }, //75 aux transport label
		{d_text_proc,COL6,ROW5+16*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Axis",0,0}, // 76 Axis transport Label
		{d_text_proc,COL7,ROW5+16*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Allied",0,0}, // 77 Allied transport
		{d_text_proc,COL5,ROW5+17*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Air #",0,0}, // 78 Air Number label
		{d_text_proc,COL5,ROW5+18*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"A/Type",0,0}, // 79 Air type
		{d_text_proc,COL5,ROW5+19*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Sea #",0,0}, // 80 Sea Number label
		{d_text_proc,COL5,ROW5+20*RHEIGHT,36,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"S/Type",0,0}, // 81 Sea type
		{d_edit_proc,COL6,ROW5+17*RHEIGHT,WIDTH5,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //82 AxisAirTransNum
		{d_edit_proc,COL7,ROW5+17*RHEIGHT,WIDTH5,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //83 AlliedAirTransNum
		{d_edit_proc,COL6,ROW5+18*RHEIGHT,WIDTH5,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     4,0,     0,0,0 }, //84 AxisAirTransType
		{d_edit_proc,COL7,ROW5+18*RHEIGHT,WIDTH5,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     4,0,     0,0,0 }, //85 AlliedAirTransType
		{d_edit_proc,COL6,ROW5+19*RHEIGHT,WIDTH5,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //86 AxisSeaTransNum
		{d_edit_proc,COL7,ROW5+19*RHEIGHT,WIDTH5,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, //87 AlldSeaTransNum
		{d_edit_proc,COL6,ROW5+20*RHEIGHT,WIDTH5,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     4,0,     0,0,0 }, //88 AxisSeaTransType
		{d_edit_proc,COL7,ROW5+20*RHEIGHT,WIDTH5,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     4,0,     0,0,0 }, //89 AlldSeaTransType
		{d_textbox_proc,COL8,100+3*RHEIGHT,110+32,192,SD_FG_COLOR,SD_BG_COLOR,   0,   0,   0,0,   0,0,0}, //90 Victory..
		{d_text_proc,COL3,ROW3_OLD+RHEIGHT,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Neutral",0,0  }, // 91 text
		{d_ctyedit_proc,COL3,4*RHEIGHT+ROW1,CEDITWIDTH,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 92 Neutral country 1
		{d_ctyedit_proc,COL3,5*RHEIGHT+ROW1,CEDITWIDTH,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 93 Neutral country 2
		{d_ctyedit_proc,COL3,6*RHEIGHT+ROW1,CEDITWIDTH,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 94 Neutral country 3
		{d_ctyedit_proc,COL3,7*RHEIGHT+ROW1,CEDITWIDTH,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 95 Neutral country 4
		{d_ctyedit_proc,COL3,8*RHEIGHT+ROW1,CEDITWIDTH,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 96 Neutral country 5
		{d_ctyedit_proc,COL3,9*RHEIGHT+ROW1,CEDITWIDTH,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0  }, // 97 Neutral country 6
		{d_text_proc,COL1,10*RHEIGHT+ROW1,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Axis Icons Point",0,0  }, // 98 icon orientation
		{d_text_proc,COL1,13*RHEIGHT+ROW1,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Weather Front",0,0 }, // 99
		{d_text_proc,COL6,ROW1,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,0,0,0,0,0,0,0  }, // 100 short desc
		{d_textbox_proc,COL5,ROW2_OLD,640-(COL5)-5,50,SD_FG_COLOR,SD_BG_COLOR,0,0,0,0,0,0,0 }, // 101 long desc
		{d_rotate_btn_proc,COL1+8,SCEN_BUTTON_LINE,S_BUTTON_W,15,SD_FG_COLOR,SD_BG_COLOR,'x',0,0,0,(void*)"RA&xis",0,0}, // 102 Rotare axis
		{d_rotate_btn_proc,COL3,SCEN_BUTTON_LINE,S_BUTTON_W,15,SD_FG_COLOR,SD_BG_COLOR,'l',0,0,0,(void*)"RA&llied",0,0}, // 103 Rotate allied
		{d_genpfc_btn_proc,COL8,ROW5+15*RHEIGHT,110+32,15,SD_FG_COLOR,SD_BG_COLOR,0,0,0,0,(void*)"Gen. PfC",0,0}, // 104 Gen Prestige for Capturing
		{d_genaip_btn_proc,COL8,ROW5+15*RHEIGHT+24,110+32,15,SD_FG_COLOR,SD_BG_COLOR,0,0,0,0,(void*)"Gen. AI Prestige",0,0}, // 105

		{d_text_proc,COL9,ROW5,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     0,0,0 }, //106 current core sdCurAlldAux
		{d_edit_proc,COL9,ROW4,WIDTH4,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     3,0,     0,0,0 }, // 107 sdMaxAlldAux
		{d_text_proc,COL9-10,ROW3,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"Allied Aux",0,0 }, //108

		{d_vc_btn_proc,COL8,ROW5+15*RHEIGHT+24*2,110+32,15,SD_FG_COLOR,SD_BG_COLOR,0,0,0,0,(void*)"Axis Vic. Cond.",0,0}, // 105
		{d_vc_btn_proc,COL8,ROW5+15*RHEIGHT+24*3,110+32,15,SD_FG_COLOR,SD_BG_COLOR,0,0,0,0,(void*)"Allied Vic. Cond.",0,0}, // 106

		{d_class_btn_proc,COL5,SCEN_BUTTON_LINE,S_BUTTON_W,15,SD_FG_COLOR,SD_BG_COLOR,     'a',     0,     0,0,     (void *)"Cl&ass",0,0  }, // 107 class button

		{d_edit_proc,COL2-16,ROW1+16,WIDTH12,RHEIGHT,EDIT_FG_COLOR,EDIT_BG_COLOR,     0,     0,     7,0,     0,0,0  }, // 108 ScenNameEdit
		{d_text_proc,COL1,ROW1+16,CWIDTH,RHEIGHT,SD_FG_COLOR,SD_BG_COLOR,     0,     0,     0,0,     (void *)"File",0,0 }, //109

		{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
		// last
		  {NULL}
		};

//a click on the country edit should enter the current country from the lb
int d_ctyedit_proc(int msg,DIALOG *d,int c)
{
   int i;
   char buffer[64];

   //when a country edit is clicked on it gets the current country
   //from the country list box
   if (msg==MSG_CLICK )
   {
     for (i=0; i<10; ++i)
     {
        if (&(senario_dlg[sdCtyNumEdit1Idx+i])==d)
           {
        	sprintf(buffer,"%d",get_country_from_current_list(senario_dlg[sdCtyListIdx].d1));
        	if (strcmp(buffer,CtyStr[i])) drawOnIdle=1;
        	sprintf(CtyStr[i],"%d",get_country_from_current_list(senario_dlg[sdCtyListIdx].d1));
           }

     }
	 //two 6th countries
	for (i=0; i<2; ++i)
     {
        if (&(senario_dlg[sdCtyNumEdit6Idx+i])==d){
        	sprintf(buffer,"%d",get_country_from_current_list(senario_dlg[sdCtyListIdx].d1));
        	if (strcmp(buffer,CtyStr[i+10])) drawOnIdle=1;
        	sprintf(CtyStr[i+10],"%d",get_country_from_current_list(senario_dlg[sdCtyListIdx].d1));
        }
     }

	for (i=0; i<6; ++i)
     {
        if (&(senario_dlg[sdCtyNumNeutralEdit1Idx+i])==d)
           sprintf(CtyStr[i+12],"%d",get_country_from_current_list(senario_dlg[sdCtyListIdx].d1));
     }
   }

   if (msg==MSG_LOSTFOCUS) drawOnIdle=1;
   return d_edit_proc(msg,d,c);
}
// the cancel button sets the scen_save_data flag to 0 so data is
// not saved on exit
int d_cancelbtn_proc(int msg,DIALOG *d,int c)
{
    //if (msg==MSG_IDLE) rest(100);
    if ((msg==MSG_CLICK)|(msg==MSG_KEY))
      scen_save_data=0;
    return d_button_proc(msg,d,c);
}

int d_rotate_btn_proc(int msg,DIALOG *d,int c)
{
	int i,f,t,o=0;


	if (&senario_dlg[sdRotAlliedBtnIdx]==d) o=1;

    //if (msg==MSG_IDLE) rest(100);
    if ((msg==MSG_CLICK)|(msg==MSG_KEY)){
    	f=-1;
    	//search for non 0 value
    	for(i=5;i>-1;i--){
    		if (atoi(CtyStr[i*2+o])>0) {f=i; break;}
    	}
    	//printf("f=%d\n",f);
    	if (f>0){ //at least 2 must be present
    		t=atoi(CtyStr[o]);
    		for(i=0;i<f;i++)
    			strncpy(CtyStr[i*2+o],CtyStr[(i+1)*2+o],8);
    		sprintf(CtyStr[f*2+o],"%d",t);
    	}

    	for(i=0;i<10;i++)
    		d_ctyedit_proc(MSG_DRAW,&(senario_dlg[sdCtyNumEdit1Idx+i]),0);
    	for(i=0;i<2;i++)
    		d_ctyedit_proc(MSG_DRAW,&(senario_dlg[sdCtyNumEdit6Idx+i]),0);
    	drawOnIdle=1;
    	return D_O_K;
    }

    return d_button_proc(msg,d,c);
}

void draw_flags(){
	int i;

	 for (i=0; i<10; ++i)
		    {
		       if (atoi(CtyStr[i])>0 && atoi(CtyStr[i])<=total_countries){
		    	   int xx=senario_dlg[sdCtyNumEdit1Idx+i].x-10+WIDTH4;
		    	   int yy=senario_dlg[sdCtyNumEdit1Idx+i].y-22-12-5;
		    	   rectfill(screen,xx+20,yy+36,xx+20+20,yy+36+14,colors_to24bits(SD_BG_COLOR));
		   		//flag
		    	   masked_blit(flag_bmp[atoi(CtyStr[i])-1],screen,0,0,xx,yy,60,50);
		       }
		    }
			for (i=0; i<2; ++i)
		    {
			       if (atoi(CtyStr[10+i])>0 && atoi(CtyStr[10+i])<=total_countries){
			    	   int xx=senario_dlg[sdCtyNumEdit6Idx+i].x-10+WIDTH4;
			    	   int yy=senario_dlg[sdCtyNumEdit6Idx+i].y-22-12-5;
			    	   rectfill(screen,xx+20,yy+36,xx+20+20,yy+36+14,colors_to24bits(SD_BG_COLOR));
			    	   //flag
			    	   masked_blit(flag_bmp[atoi(CtyStr[10+i])-1],screen,0,0,xx,yy,60,50);
			       }

		     //  sprintf(CtyStr[10+i],"%d",scn_buffer[10+i]&255);
		     //  sd[sdCtyNumEdit6Idx+i].dp=&(CtyStr[i+10]);
		    }
}

//use this to initialize variables and save them on exit
int d_clr_proc(int msg,DIALOG *d,int c)
{
  int i,val;
  char str[1024+1];

  //initialize everything that needs it
  if (msg==MSG_START)
  {
		make_countries_table(FILTER_COUNTRIES_ALL);

	    drawOnIdle=1;

		senario_dlg[sdShortDescIdx].dp = ShortDescStr;
		senario_dlg[sdLongDescIdx].dp = LongDesStr;

		if (pgf_mode || pacgen_mode) {
			strncpy(ShortDescStr,block1_Name, SCENARIO_STR_SIZE);
			strncpy(LongDesStr,block1_Description, SCENARIO_DES_STR_SIZE);
		} else {
			if (getScenarioNumber()-1>=0 && getScenarioNumber()-1<MAX_SCENARIOS){
				convert_from_cp1250_to_utf8(ShortDescStr,scn_short_description[getScenarioNumber()-1], PG_SCENARIO_STR_SIZE);
				convert_from_cp1250_to_utf8(LongDesStr,scn_long_description[getScenarioNumber()-1], PG_SCENARIO_DES_STR_SIZE);
			}else{
				strncpy(ShortDescStr,"",SCENARIO_STR_SIZE);
				strncpy(LongDesStr,"",SCENARIO_DES_STR_SIZE);
			}
		}

		scen_save_data=1; //default is to save

    sprintf(ScnStr,"%d",getScenarioNumber());
    senario_dlg[sdScnNumEditIdx].dp=ScnStr;
    //strncpy(ScnNameStr,scn_file_name,128);

		getScenarioNameRaw(ScnNameStr);
		senario_dlg[sdScenNameEdit].dp=ScnNameStr;
		//countries
		for (i = 0; i < 10; ++i) {
				sprintf(CtyStr[i], "%d", scn_buffer[i] & 255);
				senario_dlg[sdCtyNumEdit1Idx + i].dp = &(CtyStr[i]);
				if (i % 2)
					AlliedFlagsOld[i / 2] = scn_buffer[i];
				else
					AxisFlagsOld[i / 2] = scn_buffer[i];
			}
			for (i = 0; i < 2; ++i) {
				sprintf(CtyStr[10 + i], "%d", scn_buffer[10 + i] & 255);
				senario_dlg[sdCtyNumEdit6Idx + i].dp = &(CtyStr[i + 10]);
				if (i % 2)
					AlliedFlagsOld[(i + 10) / 2] = scn_buffer[i + 10];
				else
					AxisFlagsOld[(i + 10) / 2] = scn_buffer[i + 10];
			}
	
	//neutral
    for (i=0; i<6; ++i)
    {
       sprintf(CtyStr[i+12],"%d",Neutral[i]);
       senario_dlg[sdCtyNumNeutralEdit1Idx+i].dp=&(CtyStr[i+12]);

    }	
	
    //orientation
    if (scn_buffer[ORIENTATION] == 1) {
			senario_dlg[sdOrtRtRBIdx].flags = D_SELECTED;
			senario_dlg[sdOrtLfRBIdx].flags &= ~D_SELECTED;
		} else {
			senario_dlg[sdOrtRtRBIdx].flags &= ~D_SELECTED;
			senario_dlg[sdOrtLfRBIdx].flags = D_SELECTED;
		}

    //storm front
    if (scn_buffer[STORM_FRONT] == 0) {
			senario_dlg[sdFrtNRBIdx].flags &= ~D_SELECTED;
			senario_dlg[sdFrtYRBIdx].flags = D_SELECTED;
		} else {
			senario_dlg[sdFrtNRBIdx].flags = D_SELECTED;
			senario_dlg[sdFrtYRBIdx].flags &= ~D_SELECTED;
		}
   // offense defense
		if (scn_buffer[AXIS_STANCE] == 0) {
			senario_dlg[sdAxisOffRBIdx].flags &= ~D_SELECTED;
			senario_dlg[sdAxisDefRBIdx].flags = D_SELECTED;
		} else {
			senario_dlg[sdAxisOffRBIdx].flags = D_SELECTED;
			senario_dlg[sdAxisDefRBIdx].flags &= ~D_SELECTED;
		}

		if (scn_buffer[ALLIED_STANCE] == 0) {
			senario_dlg[sdAlldOffRBIdx].flags &= ~D_SELECTED;
			senario_dlg[sdAlldDefRBIdx].flags = D_SELECTED;
		} else {
			senario_dlg[sdAlldOffRBIdx].flags = D_SELECTED;
			senario_dlg[sdAlldDefRBIdx].flags &= ~D_SELECTED;
		}
   //location
	//clear all
	for (i = 0; i < 4; ++i)
		senario_dlg[sdNALocRBIdx + i].flags &= ~D_SELECTED;
	//set
	senario_dlg[sdNALocRBIdx + scn_buffer[SCEN_LOCALE]].flags = D_SELECTED;

   sprintf(CurCoreStr,"%d",scn_buffer[CORE_UNITS]&255);
   sprintf(CurAuxStr,"%d",scn_buffer[AUX_UNITS]&255);
   sprintf(CurAlldStr,"%d",scn_buffer[ALLIED_UNITS]&255);
   sprintf(CurAlldAuxStr,"%d",scn_buffer[AUX_ALLIED_UNITS]&255);

   senario_dlg[sdCurCoreIdx].dp=CurCoreStr;
   senario_dlg[sdCurAuxIdx].dp=CurAuxStr;
   senario_dlg[sdCurAlldIdx].dp=CurAlldStr;
   senario_dlg[sdCurAlldAuxIdx].dp=CurAlldAuxStr;

   //setup the Max for units
   sprintf(MaxCoreStr,"%d",scn_buffer[CORE_UNITS_LIMIT]&255);
   sprintf(MaxAuxxStr,"%d",scn_buffer[AUX_UNITS_LIMIT]&255);
   sprintf(MaxAlldStr,"%d",scn_buffer[ALLIED_UNITS_LIMIT]&255);
   sprintf(MaxAlldAuxStr,"%d",scn_buffer[ALLIED_AUX_UNITS_LIMIT]&255);

   senario_dlg[sdMaxCoreIdx].dp=MaxCoreStr;
   senario_dlg[sdMaxAuxIdx].dp=MaxAuxxStr;
   senario_dlg[sdMaxAlldIdx].dp=MaxAlldStr;
   senario_dlg[sdMaxAlldAuxIdx].dp=MaxAlldAuxStr;
   //date
   senario_dlg[sdMonthIdx].dp=MonthStr;
   senario_dlg[sdDayIdx].dp=DayStr;
   senario_dlg[sdYearIdx].dp=YearStr;
   sprintf(MonthStr,"%02d",scn_buffer[MONTH]&255);
   sprintf(DayStr,"%02d",scn_buffer[DAY]&255);
   sprintf(YearStr,"%02d",scn_buffer[YEAR]&255);
 
   //turns
   senario_dlg[sdTurnsIdx].dp=TurnsStr;
   senario_dlg[sdTurnsPerDayIdx].dp=TpdStr;
   senario_dlg[sdDaysPerTurnIdx].dp=DptStr;
   sprintf(TurnsStr,"%d",scn_buffer[TURNS]&255);
   sprintf(TpdStr,"%d",scn_buffer[TURNS_PER_DAY]&255);
   sprintf(DptStr,"%d",scn_buffer[DAYS_PER_TURN]&255);
 
   // AI prestige
   senario_dlg[sdAxisPrestigeAmtIdx].dp=AxisPrestigeAmtStr;
   senario_dlg[sdAlldPrestigeAmtIdx].dp=AlldPrestigeAmtStr;
   senario_dlg[sdAxisPrestigeTurnsIdx].dp=AxisPrestigeTurnsStr;
   senario_dlg[sdAlldPrestigeTurnsIdx].dp=AlldPrestigeTurnsStr;
   sprintf(AxisPrestigeAmtStr,"%d",scn_buffer[AI_AXIS_BUCKET]+ 256*scn_buffer[AI_AXIS_BUCKET+1]);
   sprintf(AlldPrestigeAmtStr,"%d",scn_buffer[AI_ALLIED_BUCKET]+ 256*scn_buffer[AI_ALLIED_BUCKET+1]);
   sprintf(AxisPrestigeTurnsStr,"%d",scn_buffer[AI_AXIS_INTERVAL]&0xFF);
   sprintf(AlldPrestigeTurnsStr,"%d",scn_buffer[AI_ALLIED_INTERVAL]&0xFF);
 
   //starting prestige
   sprintf(AxisPrestigeStr,"%d", s4_buffer[AXIS_PRESTIGE]+256*s4_buffer[AXIS_PRESTIGE+1]);
   sprintf(AlliedPrestigeStr,"%d", s4_buffer[ALLIED_PRESTIGE]+256*s4_buffer[ALLIED_PRESTIGE+1]);
   senario_dlg[sdAxisPrestigeIdx].dp=AxisPrestigeStr;
   senario_dlg[sdAlliedPrestigeIdx].dp=AlliedPrestigeStr;
 
   //air aux transport
   sprintf(AxisAirTransNumStr,"%d",s4_buffer[AXIS_AIR_NUMBER]);
   sprintf(AlldAirTransNumStr,"%d",s4_buffer[ALLIED_AIR_NUMBER]);
   sprintf(AxisAirTransTypeStr,"%d",s4_buffer[AXIS_AIR_TYPE]+ 256*s4_buffer[AXIS_AIR_TYPE+1]);
   sprintf(AlldAirTransTypeStr,"%d",s4_buffer[ALLIED_AIR_TYPE]+ 256*s4_buffer[ALLIED_AIR_TYPE+1]);
   senario_dlg[sdAxisAirTransNumIdx].dp=AxisAirTransNumStr;
   senario_dlg[sdAxisAirTransTypeIdx].dp=AxisAirTransTypeStr;
   senario_dlg[sdAlldAirTransNumIdx].dp=AlldAirTransNumStr;
   senario_dlg[sdAlldAirTransTypeIdx].dp=AlldAirTransTypeStr;
 
   //sea aux transport
   sprintf(AxisSeaTransNumStr,"%d",s4_buffer[AXIS_SEA_NUMBER]);
   sprintf(AlldSeaTransNumStr,"%d",s4_buffer[ALLIED_SEA_NUMBER]);
   sprintf(AxisSeaTransTypeStr,"%d",s4_buffer[AXIS_SEA_TYPE]+ 256*s4_buffer[AXIS_SEA_TYPE+1]);
   sprintf(AlldSeaTransTypeStr,"%d",s4_buffer[ALLIED_SEA_TYPE]+ 256*s4_buffer[ALLIED_SEA_TYPE+1]);
   senario_dlg[sdAxisSeaTransNumIdx].dp=AxisSeaTransNumStr;
   senario_dlg[sdAxisSeaTransTypeIdx].dp=AxisSeaTransTypeStr;
   senario_dlg[sdAlldSeaTransNumIdx].dp=AlldSeaTransNumStr;
   senario_dlg[sdAlldSeaTransTypeIdx].dp=AlldSeaTransTypeStr;

   //the victory hex info box
   senario_dlg[sdVicTxtBoxIdx].dp=VictoryStr;
   sprintf(VictoryStr,"Victory Hexes\n  x, y, own\n");
   for (i=0; i<MAX_VICTORY_HEXES; ++i)
   {
      sprintf(str," %2d,%2d,%3d (%s)\n",victory_hexes[i].x,
                                    victory_hexes[i].y,
                                    victory_hexes[i].own,
                                    country_active[victory_hexes[i].own]?country_names_short[victory_hexes[i].own]:"  ");
     strncat(VictoryStr,str,1024-strlen(VictoryStr));
   }

   //hide buttons
   if (pgf_mode){
	   senario_dlg[sdGenAIPBtnIdx].flags &= ~D_HIDDEN;
	   senario_dlg[sdGenPfCBtnIdx].flags &= ~D_HIDDEN;
	   senario_dlg[sdCurAlldAuxIdx].flags &= ~D_HIDDEN;
	   senario_dlg[sdMaxAlldAuxIdx].flags &= ~D_HIDDEN;
	   senario_dlg[sdMaxAlldAuxLabelIdx].flags &= ~D_HIDDEN;
	   senario_dlg[sdAxisVCButtonIdx].flags &= ~D_HIDDEN;
	   senario_dlg[sdAlliedVCButtonIdx].flags &= ~D_HIDDEN;
	   senario_dlg[sdClassButtonIdx].flags &= ~D_HIDDEN;
   }else{
	   senario_dlg[sdGenAIPBtnIdx].flags |=D_HIDDEN;
	   senario_dlg[sdGenPfCBtnIdx].flags |=D_HIDDEN;
	   if (pacgen_mode){
		   senario_dlg[sdCurAlldAuxIdx].flags &= ~D_HIDDEN;
		   senario_dlg[sdMaxAlldAuxIdx].flags &= ~D_HIDDEN;
		   senario_dlg[sdMaxAlldAuxLabelIdx].flags &= ~D_HIDDEN;
	   }else{
		   senario_dlg[sdCurAlldAuxIdx].flags |=D_HIDDEN;
		   senario_dlg[sdMaxAlldAuxIdx].flags |=D_HIDDEN;
		   senario_dlg[sdMaxAlldAuxLabelIdx].flags |=D_HIDDEN;
	   }
	   senario_dlg[sdAxisVCButtonIdx].flags |=D_HIDDEN;
	   senario_dlg[sdAlliedVCButtonIdx].flags |=D_HIDDEN;
	   senario_dlg[sdClassButtonIdx].flags |=D_HIDDEN;
   }

   draw_flags();
  //-------- end initialize
  }

  if (msg==MSG_IDLE && drawOnIdle){
	  drawOnIdle=0;
	  draw_flags();
 }

  //make changes where they are needed
  if (msg==MSG_END && scen_save_data)
  {
    //scenario number
    int new_scen_number=atoi(ScnStr);
    if (new_scen_number!= getScenarioNumber()){
    	setScenarioNumber(new_scen_number);
    	sprintf(block1_SET_file,"map%02d.set",getScenarioNumber());
    }
    //countries
    for (i=0; i<12; ++i)
      scn_buffer[i]=Min(atoi(CtyStr[i]),BYTE_MAX);
	for (i=0; i<6; ++i)
      Neutral[i]=Min(atoi(CtyStr[i+12]),BYTE_MAX);
	  
    //orientation
    if (senario_dlg[sdOrtRtRBIdx].flags&D_SELECTED)
    {
      scn_buffer[ORIENTATION]=1;
      scn_buffer[ORIENTATION+1]=0;
    }
    else
    {
      scn_buffer[ORIENTATION]=255;
      scn_buffer[ORIENTATION+1]=255;
    }
    //storm front
    if (senario_dlg[sdFrtYRBIdx].flags&D_SELECTED)
      scn_buffer[STORM_FRONT]=0;
    else
      scn_buffer[STORM_FRONT]=1;

    // axis allied stance
    if (senario_dlg[sdAxisDefRBIdx].flags&D_SELECTED)
      scn_buffer[AXIS_STANCE]=0;
    else
      scn_buffer[AXIS_STANCE]=1;
    if (senario_dlg[sdAlldDefRBIdx].flags&D_SELECTED)
       scn_buffer[ALLIED_STANCE]=0;
    else
      scn_buffer[ALLIED_STANCE]=1;

    //locale
    for (i=0; i<4; ++i)
     if (senario_dlg[sdNALocRBIdx+i].flags&D_SELECTED) scn_buffer[SCEN_LOCALE]=i;

    //max strength
    val=(Min(atoi(MaxCoreStr),BYTE_MAX)&255);
    if(val>-1) scn_buffer[CORE_UNITS_LIMIT]  =(unsigned char)val;

    val=(Min(atoi(MaxAuxxStr),BYTE_MAX)&255);
    if(val>-1) scn_buffer[AUX_UNITS_LIMIT]   =(unsigned char)val;

    val=(Min(atoi(MaxAlldStr),BYTE_MAX)&255);
    if(val>-1) scn_buffer[ALLIED_UNITS_LIMIT]=(unsigned char)val;

    val=(Min(atoi(MaxAlldAuxStr),BYTE_MAX)&255);
    if(val>-1) scn_buffer[ALLIED_AUX_UNITS_LIMIT]=(unsigned char)val;

    //date
    val=(Min(atoi(MonthStr),BYTE_MAX)&255);
    if(val>-1) scn_buffer[MONTH]=(unsigned char)val;
    val=(Min(atoi(DayStr)  ,BYTE_MAX)&255);
    if(val>-1) scn_buffer[DAY]=  (unsigned char)val;
    val=(Min(atoi(YearStr) ,BYTE_MAX)&255);
    if(val>-1) scn_buffer[YEAR]= (unsigned char)val;

    //turns
    val=Min(atoi(TurnsStr),BYTE_MAX);
    if(val>-1) scn_buffer[TURNS]        =(unsigned char)val;
    val=Min(atoi(TpdStr),BYTE_MAX);
    if(val>-1) scn_buffer[TURNS_PER_DAY]=(unsigned char)val;
    val=Min(atoi(DptStr),BYTE_MAX);
    if(val>-1) scn_buffer[DAYS_PER_TURN]=(unsigned char)val;

    //AI prestige
    val=Min(atoi(AxisPrestigeAmtStr),WORD_MAX);
    if(val>-1) {
		scn_buffer[AI_AXIS_BUCKET]    =val&255;
		scn_buffer[AI_AXIS_BUCKET+1]  =val/256;
	}
    val=Min(atoi(AlldPrestigeAmtStr),WORD_MAX);
    if(val>-1) {
		scn_buffer[AI_ALLIED_BUCKET]  =val&255;
		scn_buffer[AI_ALLIED_BUCKET+1]=val/256;
    }

    val=Min(atoi(AxisPrestigeTurnsStr),BYTE_MAX);
    if(val>-1) scn_buffer[AI_AXIS_INTERVAL]  =val;
    val=Min(atoi(AlldPrestigeTurnsStr),BYTE_MAX);
    if(val>-1) scn_buffer[AI_ALLIED_INTERVAL]=val;

    //starting prestige
    val=Min(atoi(AxisPrestigeStr),WORD_MAX);
    if(val>-1) {
		s4_buffer[AXIS_PRESTIGE]=val&255;
		s4_buffer[AXIS_PRESTIGE+1]=val/256;
    }
    val=Min(atoi(AlliedPrestigeStr),WORD_MAX);
    if(val>-1) {
		s4_buffer[ALLIED_PRESTIGE]=val&255;
		s4_buffer[ALLIED_PRESTIGE+1]=val/256;
    }
    // aux air transports
    val=Min(atoi(AxisAirTransNumStr),BYTE_MAX);
    if(val>-1) s4_buffer[AXIS_AIR_NUMBER]=val;
    val=Min(atoi(AlldAirTransNumStr),BYTE_MAX);
    if(val>-1) s4_buffer[ALLIED_AIR_NUMBER]=val;

    val=Min(atoi(AxisAirTransTypeStr),WORD_MAX);
    if(val>-1) {
		s4_buffer[AXIS_AIR_TYPE]=val&255;
		s4_buffer[AXIS_AIR_TYPE+1]=val/256;
    }

    val=Min(atoi(AlldAirTransTypeStr),WORD_MAX);
    if(val>-1) {
		s4_buffer[ALLIED_AIR_TYPE]=val&255;
		s4_buffer[ALLIED_AIR_TYPE+1]=val/256;
    }
    // aux sea transports
    val=Min(atoi(AxisSeaTransNumStr),BYTE_MAX);
    if(val>-1) s4_buffer[AXIS_SEA_NUMBER]=val;
    val=Min(atoi(AlldSeaTransNumStr),BYTE_MAX);
    if(val>-1) s4_buffer[ALLIED_SEA_NUMBER]=val;

    val=Min(atoi(AxisSeaTransTypeStr),WORD_MAX);
    if(val>-1) {
		s4_buffer[AXIS_SEA_TYPE]=val&255;
		s4_buffer[AXIS_SEA_TYPE+1]=val/256;
    }
    val=Min(atoi(AlldSeaTransTypeStr),WORD_MAX);
    if(val>-1) {
		s4_buffer[ALLIED_SEA_TYPE]=val&255;
		s4_buffer[ALLIED_SEA_TYPE+1]=val/256;
    }
  }

  return d_textbox_proc(msg,d,c);
}

//generate prestige for capturing hexes
int d_genpfc_btn_proc(int msg,DIALOG *d,int c)
{
	int x,y,i;

    if ((msg==MSG_CLICK)|(msg==MSG_KEY)){
    	if (alert("Do you really want to generate prestige for capturing ?", "This action will erase all prestige for capturing loaded", NULL,
    	       "&Yes", "&No", 'y', 'n')==2) return D_O_K;

    	block5_lines=0;

		for (y = 0; y < mapy; ++y)
			for (x = 0; x < mapx; ++x)
				if (map[x][y].own != 0 && map[x][y].side != 3) {
					i = 40;
					if (map[x][y].vic == 1)
						i += 40;
					sprintf(block5[block5_lines], "(%d:%d)\t%d", x, y, i);
					block5_lines++;
				}
    	/*
    	for (y = 0; y < mapy ; ++y)
    		for (x = 0; x < mapx ; ++x)
    			for (i = tiles_filter[CITY_FILTER_INDEX][0]; i <= tiles_filter[CITY_FILTER_INDEX][1]; i++)
    				if (map[x][y].tile == tiles_display[i]){
    					val=40;
    					for (j = 0; j < MAX_VICTORY_HEXES; j++)
    						if (victory_hexes[j].x > -1 &&
    							victory_hexes[j].y > -1 &&
    							victory_hexes[j].own > 0 &&
    							victory_hexes[j].x==x &&
    							victory_hexes[j].y==y
    							) {
									val=80;
									break;
								}

    					sprintf(block5[block5_lines],"%d\t%d\t%d\n",x,y,val);
    					block5_lines++;
    					//printf("%d,%d,%d\n",x,y,val);
    				}
*/
    	return D_O_K;
    }

    return d_button_proc(msg,d,c);
}
//generate AI prestige per turn
int d_genaip_btn_proc(int msg,DIALOG *d,int c)
{
    if ((msg==MSG_CLICK)|(msg==MSG_KEY)){

    	if (alert("Do you really want to generate AI prestige ?", "This action will erase all AI prestige loaded", NULL,
    	       "&Yes", "&No", 'y', 'n')==2) return D_O_K;

    	int i;
    	int val, axis_p=0, allied_p=0, axis_t=1, allied_t=1;
    	int turns_now =0;

    	val=Min(atoi(TurnsStr),BYTE_MAX);
    	if(val>-1) turns_now=val;

    	val=Min(atoi(AxisPrestigeAmtStr),WORD_MAX);
        if(val>-1) axis_p=val;
        val=Min(atoi(AlldPrestigeAmtStr),WORD_MAX);
        if(val>-1) allied_p=val;

        val=Min(atoi(AxisPrestigeTurnsStr),BYTE_MAX);
        if(val>0) axis_t=val;
        val=Min(atoi(AlldPrestigeTurnsStr),BYTE_MAX);
        if(val>0) allied_t=val;

        for(i=0;i<turns_now;i++){
        	//printf("%d,%d,%d\n",i+1,i%axis_t?0:axis_p,i%allied_t?0:allied_p);
        	sprintf(block4[i],"%d\t%d\t%d\n",i+1,(i%axis_t)?0:axis_p,(i%allied_t)?0:allied_p);
        }
        block4_lines=turns_now;

    	return D_O_K;
    }

    return d_button_proc(msg,d,c);
}

int d_vc_btn_proc(int msg,DIALOG *d,int c)
{
    if ((msg==MSG_CLICK)|(msg==MSG_KEY)){

    	if(&(senario_dlg[sdAxisVCButtonIdx])==d) strncpy(ParseVC,block7[0],1024);
    	if(&(senario_dlg[sdAlliedVCButtonIdx])==d) strncpy(ParseVC,block7[1],1024);
    	//strncpy(ParseVC,"AXIS VICTORY\t0\t1\t",1024);
    	//strncpy(ParseVC,"AXIS VICTORY\t0\t3\t(7,9)(7,14)(9,12)",1024);
    	//strncpy(ParseVC,"AXIS VICTORY\t2\t0\t(1,2)(3,4)(5,6)",1024);
    	//strncpy(ParseVC,"AXIS VICTORY",1024);
    	//ALLIED VICTORY	-1	-1

    	centre_dialog(vc_dlg);
    	if (do_dialog(vc_dlg,-1)==vcOKButton){
    		//printf("%s\n",ParseVC);

        	if(&(senario_dlg[sdAxisVCButtonIdx])==d) strncpy(block7[0],ParseVC,MAX_LINE_SIZE);
        	if(&(senario_dlg[sdAlliedVCButtonIdx])==d) strncpy(block7[1],ParseVC,MAX_LINE_SIZE);

    	}
    	drawOnIdle=1;
    	return D_O_K|D_REDRAW;
    }

    return d_button_proc(msg,d,c);
}

int d_class_btn_proc(int msg,DIALOG *d,int c)
{
    if ((msg==MSG_CLICK)|(msg==MSG_KEY)){

    	centre_dialog(class_dlg);
    	do_dialog(class_dlg,-1);
    	drawOnIdle=1;
    	return D_O_K|D_REDRAW;
    }

    return d_button_proc(msg,d,c);
}

