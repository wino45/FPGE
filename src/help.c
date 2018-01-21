#include <allegro.h>

#include "fpge.h"
#include "help.h"

#define HD_BG_COLOR GUI_BG_COLOR
#define HD_FG_COLOR GUI_FG_COLOR
#define HD_SCREEN_COLOR GUI_BG_COLOR

#define HD_X  1
#define HD_Y  1
#define HD_LINES   38
#define HD_COLUMNS 63

#define HD_LH  16

#define HD_DX  (HD_COLUMNS * 8)
#define HD_DY  (HD_LINES*HD_LH-HD_LH/2)
#define HD_LINE1  HD_Y+10
/*
int d_button_proc_with_idle(int msg, DIALOG *d, int c){
	if (msg==MSG_IDLE) rest(100);
	return d_button_proc(msg,d,c);
}
*/
DIALOG help_dlg[HELP_DLG_SIZE]={
// 0
{
  d_textbox_proc,
     HD_X,HD_Y, 
	 HD_DX, //width
	 HD_DY, //height
     HD_FG_COLOR,HD_SCREEN_COLOR,
     0,
     0,
     0,0,
	 "Display options:\n\n"
     "ctrl-n show not common names\n\n"
	 "ctrl-a show id's of all names\n\n"
	 "ctrl-t show terrain type\n\n"
	 "ctrl-r show road connections\n\n"
	 "* Note that using any of above will hide units/flags, unless:\n\n"
	 "ctrl-z override units or flags hiding\n\n"
	 "ctrl-c override special hex hiding\n\n"
	 "ctrl-d cycle between displaying hex->dec->name\n\n"
	 "ctrl-f filter output to current name or terrain type\n\n"
	 "ctrl-s toggle show strength of units\n\n"
	 "ctrl-x toggle show experience of units\n\n"
	 "ctrl-e toggle show entrenchment of units\n\n"
	 "ctrl-l display transport in Gnd and Trans modes\n\n"
	 "ctrl-w cycle weather: normal, mud, snow\n\n"
	 "ctrl-h show/hide hex grid on map\n\n"
	 "ctrl-u switch between campaign and scenario unit display modes\n\n"
	 "ctrl-v switch between edit and game unit display modes\n\n"
	 "ctrl-o show units ID in the scenario, two display modes.\n\n"
	 "ctrl-k show scan ranges\n\n"
     "ctrl-i clears all visual display flags to defaults\n\n"
	 "Actions:\n\n"
	 "ctrl-b save entire map as BMP\n\n"
	 "ctrl-g change graphic mode\n\n"
	 "ctrl-m generate terrain - coast, roads, rivers\n\n"
	 "ctrl-q make roads connections, terrain type and basic names\n\n"
	 "ctrl-y make map from BMP file. Use 4bpp files only.\n\n"
	 "ctrl-p make new map\n\n"
     "ctrl-j reread names file : mapnames.str\n\n"
     "alt-r  highlight hexes with road connection problems.\n\n"
     "alt-n  highlight hexes with city naming problems.\n\n"
     "alt-f  get map fragment from library\n\n"
     "alt-u  undo tile editing\n\n"
 //    "F11 - refresh palette - try on Vista & Windows 7 64-bit.\n\n"
     "1..9 - recall tile from memory slot\n\n"
	 "alt-1..alt-9 - store tile below cursor to memory slot\n\n"
	 "F1..F12 - filter terrain type in the tiles matrix\n\n"

     //"\n\n"
	 ,0,0
},
// 1
{ d_button_proc, HD_X + HD_DX / 2 - 96 / 2, HD_Y + HD_DY - HD_LH- 6, 96, 15, HD_FG_COLOR, HD_BG_COLOR, 'd', D_EXIT, 0,0, "&Done", 0, 0 },
//Last
{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
{NULL},
};