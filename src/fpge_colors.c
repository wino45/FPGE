/*
 * fpge_colors.c
 *
 *  Created on: 2010-07-10
 *      Author: wino
 */

#include <allegro.h>

#include "fpge_colors.h"
#include "tables.h"
#include "maingui.h"
#include "fpge.h"
#include "findname.h"
#include "help.h"
#include "searep.h"
#include "scen.h"
#include "terr.h"
#include "unit.h"
#include "unitlist.h"
#include "makemap.h"
#include "callback.h"
#include "mapfrg.h"
#include "map_resize.h"
#include "randomize.h"
#include "savebmp.h"
#include "rotate.h"
#include "pacgen.h"
#include "vc.h"
#include "class.h"
#include "unitsum.h"
#include "pzc.h"
#include "pg2.h"
#include "lgeneral.h"

int fpge_colors_bits=32; //new default 32bits
int fpge_colors_bits_shift=0;
int fpge_mask_color=MASK_COLOR_8;

int find_pal_element(int color_to_find){
	int i;

	for (i=0;i<256;i++)
	if (fpge_colors_bits_shift) {
	if ( pgpal[i].r== getr(color_to_find) &&
			pgpal[i].g== getg(color_to_find) &&
			pgpal[i].b== getb(color_to_find) ) return i;
	}else{
	if ( pgpal[i].r== ((color_to_find>>16)&0xff) &&
			pgpal[i].g== ((color_to_find>>8)&0xff) &&
			pgpal[i].b== ((color_to_find)&0xff) ) return i;

	}
	return -1;
}

int find_pal_element92(int color_to_find){
	int i;

	i=92;
	if (fpge_colors_bits_shift) {
	if ( pgpal[i].r== getr(color_to_find) &&
			pgpal[i].g== getg(color_to_find) &&
			pgpal[i].b== getb(color_to_find) ) return i;
	}else{
	if ( pgpal[i].r== ((color_to_find>>16)&0xff) &&
			pgpal[i].g== ((color_to_find>>8)&0xff) &&
			pgpal[i].b== ((color_to_find)&0xff) ) return i;

	}
	return -1;
}


int make_color_fpge(int r, int g, int b){
	if (fpge_colors_bits_shift){
		if (fpge_colors_bits_shift==1)
		// for saving BMP only
			return makecol24(r,g,b)/*+(0xff << _rgb_a_shift_32)*/;
		else
			return makecol24(r,g,b)+0xff000000;
		}else
			//for interactive mode
			//return makecol(pgpal[color_to_convert].r,pgpal[color_to_convert].g,pgpal[color_to_convert].b);
			//return makeacol32(r,g,b,0xff);
			return (r<<16)+(g<<8)+b;
}

int colors_to24bits(int color_to_convert){
	return colors_to24bits_ex(color_to_convert,1);
}

int colors_to24bits_ex(int color_to_convert, int mask_color_present){

	if (fpge_colors_bits>8){
		if ((color_to_convert==0 && mask_color_present)||(fpge_colors_bits_shift==2 && color_to_convert==255) ) return fpge_mask_color;

		if (color_to_convert==GUI_FG_COLOR) color_to_convert= fpge_gui_fg_color;
		if (color_to_convert==GUI_BG_COLOR) color_to_convert= fpge_gui_bg_color;
		if (color_to_convert==GUI_EDIT_COLOR) color_to_convert= fpge_gui_edit_color;

		//I do not know why this must be done like this !!!
		if (color_to_convert<=GUI_EDIT_COLOR){
		if (fpge_colors_bits_shift){
			if (fpge_colors_bits_shift==1)
			// for saving BMP only
				return makecol24(pgpal[color_to_convert].r,pgpal[color_to_convert].g,pgpal[color_to_convert].b);
			else
				return makecol24(pgpal[color_to_convert].r,pgpal[color_to_convert].g,pgpal[color_to_convert].b)+0xff000000;
			}
			else
				//for interactive mode
				//return makeacol32(pgpal[color_to_convert].r,pgpal[color_to_convert].g,pgpal[color_to_convert].b,0xff);
				return (pgpal[color_to_convert].r<<16)+(pgpal[color_to_convert].g<<8)+pgpal[color_to_convert].b;
		}
		else
			//error
			return 0;
	}else{
		//8bit mode
		if (color_to_convert==GUI_FG_COLOR) return fpge_gui_fg_color;
		if (color_to_convert==GUI_BG_COLOR) return fpge_gui_bg_color;
		if (color_to_convert==GUI_EDIT_COLOR) return fpge_gui_edit_color;
	}
	return color_to_convert;
}

void dialog_to24bits(DIALOG *dialog_to_convert, int size){
	int i,c;

	for (i=0; i < size; i++){
		c=dialog_to_convert[i].fg;
		//dialog_to_convert[i].fg = (pgpal[c].r<<16)+(pgpal[c].g<<8)+pgpal[c].b;
		//dialog_to_convert[i].fg = makecol(pgpal[c].r,pgpal[c].g,pgpal[c].b);
		dialog_to_convert[i].fg =colors_to24bits(c);
		c=dialog_to_convert[i].bg;
		//dialog_to_convert[i].bg = (pgpal[c].r<<16)+(pgpal[c].g<<8)+pgpal[c].b;
		//dialog_to_convert[i].bg = makecol(pgpal[c].r,pgpal[c].g,pgpal[c].b);
		dialog_to_convert[i].bg =colors_to24bits(c);
	}

}

void initialize_dm() {
	//White on Blue
	//gui_fg_color = colors_to24bits(FG_COLOR);
	//gui_bg_color = colors_to24bits(BG_COLOR);

	//white on gray
	//gui_fg_color = colors_to24bits(255);
	//gui_bg_color = colors_to24bits(10*16+10);


	gui_fg_color = colors_to24bits(fpge_gui_fg_color);
	gui_bg_color = colors_to24bits(fpge_gui_bg_color);
}

void set_color_mode() {
	int i;
	//return;
	//int dd[]= {1,2,3,4};
	//Sets the global pixel color depth.
	//Sets the pixel format to be used by subsequent calls to set_gfx_mode() and create_bitmap().
	//Valid depths are 8 (the default), 15, 16, 24, and 32 bits

	set_color_depth(fpge_colors_bits);
	if (fpge_colors_bits==24) fpge_mask_color=MASK_COLOR_24;
	if (fpge_colors_bits==32) fpge_mask_color=MASK_COLOR_32;

	if (fpge_colors_bits==8){
		for (i=0; i < 256; i++){
			pgpal[i].r /= 4;
			pgpal[i].g /= 4;
			pgpal[i].b /= 4;
		}
		//Tells Allegro how to convert images during loading time.
		//Specifies how to convert images between the various color depths when reading graphics from external bitmap files or datafiles.
		set_color_conversion(COLORCONV_REDUCE_TO_256);
	}else{
		set_color_conversion(COLORCONV_NONE);
	}
}

void prepare_dialogs(){

	dialog_to24bits(main_dlg,sizeof(main_dlg)/sizeof(*main_dlg));
	dialog_to24bits(findname_dlg,sizeof(findname_dlg)/sizeof(*findname_dlg));
	dialog_to24bits(help_dlg,sizeof(help_dlg)/sizeof(*help_dlg));
	dialog_to24bits(help_dlg2,sizeof(help_dlg2)/sizeof(*help_dlg2));
	dialog_to24bits(searchreplace_dlg,sizeof(searchreplace_dlg)/sizeof(*searchreplace_dlg));
	dialog_to24bits(senario_dlg,sizeof(senario_dlg)/sizeof(*senario_dlg));
	dialog_to24bits(terrain_dlg,sizeof(terrain_dlg)/sizeof(*terrain_dlg));
	dialog_to24bits(unit_dlg,sizeof(unit_dlg)/sizeof(*unit_dlg));
	dialog_to24bits(unitslist_dlg,sizeof(unitslist_dlg)/sizeof(*unitslist_dlg));
	dialog_to24bits(makemap_dlg,sizeof(makemap_dlg)/sizeof(*makemap_dlg));
	dialog_to24bits(generate_dlg,sizeof(generate_dlg)/sizeof(*generate_dlg));
	dialog_to24bits(rtnfix_dlg,sizeof(rtnfix_dlg)/sizeof(*rtnfix_dlg));
	dialog_to24bits(load_dlg,sizeof(load_dlg)/sizeof(*load_dlg));
	dialog_to24bits(saveas_dlg,sizeof(saveas_dlg)/sizeof(*saveas_dlg));
	dialog_to24bits(configuration_dlg,sizeof(configuration_dlg)/sizeof(*configuration_dlg));
	dialog_to24bits(mapfrg_dlg,sizeof(mapfrg_dlg)/sizeof(*mapfrg_dlg));
	dialog_to24bits(resize_dlg,sizeof(resize_dlg)/sizeof(*resize_dlg));
	dialog_to24bits(randomize_dlg,sizeof(randomize_dlg)/sizeof(*randomize_dlg));
	dialog_to24bits(savebmp_dlg,sizeof(savebmp_dlg)/sizeof(*savebmp_dlg));
	dialog_to24bits(rotate_dlg,sizeof(rotate_dlg)/sizeof(*rotate_dlg));
	dialog_to24bits(pacgen_dlg,sizeof(pacgen_dlg)/sizeof(*pacgen_dlg));
	dialog_to24bits(vc_dlg,sizeof(vc_dlg)/sizeof(*vc_dlg));
	dialog_to24bits(class_dlg,sizeof(class_dlg)/sizeof(*class_dlg));
	dialog_to24bits(unitsum_dlg,sizeof(unitsum_dlg)/sizeof(*unitsum_dlg));
	dialog_to24bits(findpzcid_dlg,sizeof(findpzcid_dlg)/sizeof(*findpzcid_dlg));
	dialog_to24bits(findpg2id_dlg,sizeof(findpg2id_dlg)/sizeof(*findpg2id_dlg));
	dialog_to24bits(pg2_convert_dlg,sizeof(pg2_convert_dlg)/sizeof(*pg2_convert_dlg));
	dialog_to24bits(pg2_cam_dlg,sizeof(pg2_cam_dlg)/sizeof(*pg2_cam_dlg));
	dialog_to24bits(load_layer_dlg,sizeof(load_layer_dlg)/sizeof(*load_layer_dlg));
	dialog_to24bits(units_bmp_dlg,sizeof(units_bmp_dlg)/sizeof(*units_bmp_dlg));
	dialog_to24bits(pg2_convert_map_dlg,sizeof(pg2_convert_map_dlg)/sizeof(*pg2_convert_map_dlg));
	dialog_to24bits(country_units_bmp_dlg,sizeof(country_units_bmp_dlg)/sizeof(*country_units_bmp_dlg));
	dialog_to24bits(lgen_uicons_dlg,sizeof(lgen_uicons_dlg)/sizeof(*lgen_uicons_dlg));
	dialog_to24bits(lgen_equip_dlg,sizeof(lgen_equip_dlg)/sizeof(*lgen_equip_dlg));
}


void set_pal(){
	/* set the color pallete */

	//Sets the entire palette of 256 colors.
	set_pallete(pgpal);
	set_weather_palette(); //FPGE specific functions
}

void set_weather_palette(){
	int pal_entry = 92;
	int pal_entry_dark = 92+0x70;
	if (fpge_colors_bits == 8){
	switch(showWeather){
		//normal
		case 0:
			pgpal[pal_entry].r=195/4;
			pgpal[pal_entry].g=195/4;
			pgpal[pal_entry].b=119/4;
			pgpal[pal_entry_dark].r=147/4;
			pgpal[pal_entry_dark].g=147/4;
			pgpal[pal_entry_dark].b=91/4;
			break;
		//mud
		case 1:
			pgpal[pal_entry].r=211/4;
			pgpal[pal_entry].g=178/4;
			pgpal[pal_entry].b=105/4;
			pgpal[pal_entry_dark].r=162/4;
			pgpal[pal_entry_dark].g=134/4;
			pgpal[pal_entry_dark].b=85/4;
			break;
		//snow
		case 2:
			pgpal[pal_entry].r=231/4;
			pgpal[pal_entry].g=239/4;
			pgpal[pal_entry].b=239/4;
			pgpal[pal_entry_dark].r=174/4;
			pgpal[pal_entry_dark].g=178/4;
			pgpal[pal_entry_dark].b=178/4;
			break;
	}
	set_pallete(pgpal);
	}
}
