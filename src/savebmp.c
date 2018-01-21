#include <allegro.h>
#include <stdio.h>
#include "callback.h"
#include "fpge.h"
#include "savebmp.h"
#include "minimap.h"
#include "cli.h"

#define SB_COL1 160
#define SB_COL2 248-16
#define SB_COL3 360-40+8
#define SB_COL4 456-64+8
#define SB_COL5 456+8+8

#define SB_ROW1 96
#define SB_ROW2 112
#define SB_ROW3 136-8
#define SB_ROW4 160-8*2
#define SB_ROW5 184-8*3


DIALOG savebmp_dlg[] =
{
   /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                      (dp2) (dp3) */
   { d_textbox_sb_proc,  144, 64,  400+16, 200, GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   NULL,                     NULL, NULL },
   { d_button_proc, 456+16-32, 240, 96,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   's',    D_EXIT,      0,   0,   "&Save",                   NULL, NULL },
   { d_button_proc, 160, 240, 96,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   'c',    D_EXIT,      0,   0,   "&Cancel",                 NULL, NULL },

/*3*/
   { d_text_proc,   SB_COL1, SB_ROW1,  64,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   "BMP size",               NULL, NULL },
   { d_radio_proc,  SB_COL1, SB_ROW2, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      1,   0,   "Big",                    NULL, NULL },
   { d_radio_proc,  SB_COL1, SB_ROW3, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      1,   0,   "Medium",                 NULL, NULL },
   { d_radio_proc,  SB_COL1, SB_ROW4, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      1,   0,   "Small",                  NULL, NULL },

/*7*/
   { d_text_proc,   SB_COL3, SB_ROW1,  64,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   "Tiles",                  NULL, NULL },
   { d_radio_proc,  SB_COL3, SB_ROW2, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      3,   0,   "Normal",                 NULL, NULL },
   { d_radio_proc,  SB_COL3, SB_ROW3, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      3,   0,   "Mud",                    NULL, NULL },
   { d_radio_proc,  SB_COL3, SB_ROW4, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      3,   0,   "Winter",                 NULL, NULL },

/*11*/
   { d_text_proc,   SB_COL4, SB_ROW1,  72,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   "Spotting",               NULL, NULL },
   { d_radio_proc,  SB_COL4, SB_ROW2, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      4,   0,   "None",                   NULL, NULL },
   { d_radio_proc,  SB_COL4, SB_ROW3, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      4,   0,   "Axis",                   NULL, NULL },
   { d_radio_proc,  SB_COL4, SB_ROW4, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      4,   0,   "Allied",                 NULL, NULL },

 /*15*/
   { d_text_proc,   SB_COL2, SB_ROW1,  72,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   "Units",                  NULL, NULL },
   { d_radio_proc,  SB_COL2, SB_ROW2, 96-8,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      2,   0,   "Ground",                 NULL, NULL },
   { d_radio_proc,  SB_COL2, SB_ROW3, 96-8,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      2,   0,   "Air",                    NULL, NULL },
   { d_radio_proc,  SB_COL2, SB_ROW4, 96-8,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      2,   0,   "Transport",              NULL, NULL },
   { d_radio_proc,  SB_COL2, SB_ROW5, 96-8,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      2,   0,   "None",                   NULL, NULL },

/*20*/
	{ d_text_proc,   SB_COL5, SB_ROW1,  72,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   "Sides",               NULL, NULL },
	{ d_radio_proc,  SB_COL5, SB_ROW2, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      5,   0,   "Both",                   NULL, NULL },
	{ d_radio_proc,  SB_COL5, SB_ROW3, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      5,   0,   "Axis",                   NULL, NULL },
	{ d_radio_proc,  SB_COL5, SB_ROW4, 96-32,  16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      5,   0,   "Allied",                 NULL, NULL },


   { d_text_proc,   160, 72,  208, 16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   "Map BMP export options", NULL, NULL },
   { d_text_proc,   152, 216, 384, 16,  GUI_FG_COLOR,GUI_BG_COLOR,   0,    0,      0,   0,   "Warning: Some options combination will not work.", NULL, NULL },
   {d_yield_proc,   144, 64,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                     NULL, NULL },
   { NULL,          144, 64,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                     NULL, NULL }
};

#define SIZE_SB_RATIO 4
#define SPOTTING_SB_RATIO 12
#define WEATHER_SB_RATIO 8
#define UNITS_SB_RATIO 16
#define SIDE_SB_RATIO 21

int save_bmp_dialog() {
	int i,l_show_air, l_show_ranges, l_showWeather, show_units=0, error;

	centre_dialog(savebmp_dlg);
	if (do_dialog(savebmp_dlg, -1) == 1) {
		//save
		l_show_air=show_unit_layer_type;
		l_show_ranges=show_ranges;
		l_showWeather=showWeather;

		for(i=0;i<3;i++)
			if (savebmp_dlg[SPOTTING_SB_RATIO+i].flags&D_SELECTED) show_ranges=(i-1+3)%3;
		for(i=0;i<3;i++)
			if (savebmp_dlg[WEATHER_SB_RATIO+i].flags&D_SELECTED) showWeather=i;
		for(i=0;i<4;i++)
			if (savebmp_dlg[UNITS_SB_RATIO+i].flags&D_SELECTED) show_unit_layer_type=i;
		for(i=0;i<3;i++)
			if (savebmp_dlg[SIDE_SB_RATIO+i].flags&D_SELECTED) show_units=i;

		//printf("show_ranges=%d, showWeather=%d, show_air=%d, show_units=%d, graphical_overide_hex=%d\n",show_ranges, showWeather, show_air, show_units,graphical_overide_hex);

		if (savebmp_dlg[SIZE_SB_RATIO].flags&D_SELECTED)
			ctrl_b_keycallback();
		if (savebmp_dlg[SIZE_SB_RATIO+1].flags&D_SELECTED)
			make_mmap(show_unit_layer_type, graphical_overide_hex, show_ranges,show_units,1); //1 mid size
		if (savebmp_dlg[SIZE_SB_RATIO+2].flags&D_SELECTED){
			if (total_mtiles==0){
				 error=prepare_mini_tiles();
				 if (error) return D_REDRAW; //exit on error
			}
			make_mmap(show_unit_layer_type, graphical_overide_hex, show_ranges,show_units,0); //0 small size
		}
		//restore
		show_unit_layer_type=l_show_air;
		show_ranges=l_show_ranges;
		showWeather=l_showWeather;

	}
	return D_REDRAW;
}

int d_textbox_sb_proc(int msg,DIALOG *d,int c){
	int i;

	if (msg==MSG_START){
		//size
		for(i=0;i<3;i++)
			savebmp_dlg[SIZE_SB_RATIO+i].flags&=~D_SELECTED;
		savebmp_dlg[SIZE_SB_RATIO].flags|=D_SELECTED;	//allways big
		//size
		for(i=0;i<3;i++)
			savebmp_dlg[SIDE_SB_RATIO+i].flags&=~D_SELECTED;
		savebmp_dlg[SIDE_SB_RATIO].flags|=D_SELECTED;	//allways Both
		//units
		for(i=0;i<4;i++)
			savebmp_dlg[UNITS_SB_RATIO+i].flags&=~D_SELECTED;
		savebmp_dlg[UNITS_SB_RATIO+show_unit_layer_type].flags|=D_SELECTED;
		//spottings
		for(i=0;i<3;i++)
			savebmp_dlg[SPOTTING_SB_RATIO+i].flags&=~D_SELECTED;
		savebmp_dlg[SPOTTING_SB_RATIO+(show_ranges+1)%3].flags|=D_SELECTED;
		//weather
		for(i=0;i<3;i++)
			savebmp_dlg[WEATHER_SB_RATIO+i].flags&=~D_SELECTED;
		savebmp_dlg[WEATHER_SB_RATIO+showWeather].flags|=D_SELECTED;
	}

	return d_textbox_proc(msg,d,c);
}
