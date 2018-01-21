#include <allegro.h>

#include "fpge.h"
#include "help.h"

#define HD_BG_COLOR GUI_BG_COLOR
#define HD_FG_COLOR GUI_FG_COLOR
#define HD_SCREEN_COLOR GUI_BG_COLOR

#define HD_X  1
#define HD_Y  1
#define HD_LINES   38
#define HD_LINES2   26
#define HD_COLUMNS 63

#define HD_LH  16

#define HD_DX  (HD_COLUMNS * 8)
#define HD_DY  (HD_LINES*HD_LH-HD_LH/2-1)
#define HD_DY2  (HD_LINES2*HD_LH-HD_LH/2)
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
     "alt-h  change color of hex grid on map\n\n"
	 "ctrl-u switch between campaign and scenario unit display modes\n\n"
	 "ctrl-v switch between edit and game unit display modes\n\n"
	 "ctrl-o show units ID in the scenario, two display modes.\n\n"
	 "ctrl-k show spotting ranges\n\n"
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
	 //"F1..F12 - filter terrain type in the tiles matrix\n\n"

     //"\n\n"
	 ,0,0
},
// 1
{ d_button_proc, HD_X + HD_DX / 2 - 96 / 2, HD_Y + HD_DY - HD_LH - 5, 96, 15, HD_FG_COLOR, HD_BG_COLOR, 'd', D_EXIT, 0,0, "&Done", 0, 0 },
//Last
{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
{NULL},
};

DIALOG help_dlg2[HELP_DLG_SIZE]={
// 0
{
  d_textbox_proc,
     HD_X,HD_Y,
	 HD_DX, //width
	 HD_DY2, //height
     HD_FG_COLOR,HD_SCREEN_COLOR,
     0,
     0,
     0,0,
     "F1..F12  - press to filter tiles by category, then filter by default terrain type\n\n"
	 "F1  - coast\n\n"
	 "F2  - city/port\n\n"
	 "F3  - mountains\n\n"
	 "F4  - clear/rough/airport\n\n"
	 "F5  - desert\n\n"
	 "F6  - road\n\n"
	 "F7  - river\n\n"
	 "F8  - swamp\n\n"
	 "F9  - forest\n\n"
	 "F10 - fort\n\n"
	 "F11 - bocage\n\n"
	 "F12 - escarpment\n\n"
	 "ctrl-h show/hide hex grid on tile matrix\n\n"
     "alt-h  change color of hex grid on tile matrix\n\n"
     "ctrl-l change tile matrix layout\n\n"
     "ctrl-s change tile matrix sorting\n\n"
     "ctrl-a - tile ID and ocean(O), group filter and passive road(R), main group filter information\n\n"
     "ctrl-d - use to change display mode for:\n\n"
     "ctrl-r - road, river information, tile generation mask\n\n"
     "ctrl-t - tile default terrain type\n\n"
     "ctrl-n - tile default name\n\n"
     //"\n\n"
	 ,0,0
},
// 1
{ d_button_proc, HD_X + HD_DX / 2 - 96 / 2, HD_Y + HD_DY2 - HD_LH- 6, 96, 15, HD_FG_COLOR, HD_BG_COLOR, 'd', D_EXIT, 0,0, "&Done", 0, 0 },
//Last
{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
{NULL},
};
