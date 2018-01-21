/*
 * menu.c
 *
 *  Created on: 16-07-2010
 *      Author: wino
 */
#include "allegro.h"
#include "maingui.h"
#include "menu.h"
#include "pgf.h"
#include "randomize.h"
#include "savebmp.h"
#include "pacgen.h"
#include "rotate.h"
#include "unitutil.h"
#include "terr.h"
#include "undo.h"
#include "drawmap.h"
#include "pzc.h"
#include "lgeneral.h"
#include "pg2.h"
#include "pg.h"
#include "hexedit.h"

#define DISPLAY_MENU_SHIFT 7
#define DISPLAY_FLAG_MENU_ITEM 22-DISPLAY_MENU_SHIFT
#define DISPLAY_HEX_MENU_ITEM 18-DISPLAY_MENU_SHIFT
#define DISPLAY_ALL_UNITS_MENU_ITEM 17-DISPLAY_MENU_SHIFT
#define DISPLAY_SCENARIO_UNITS_ITEM 16-DISPLAY_MENU_SHIFT
#define DISPLAY_UNIT_ORDER_MENU_ITEM 12-DISPLAY_MENU_SHIFT+1
#define DISPLAY_GND_TRANSPORT_MENU_ITEM 14-DISPLAY_MENU_SHIFT
#define DISPLAY_ROAD_MENU_ITEM 7-DISPLAY_MENU_SHIFT
#define DISPLAY_TERRAIN_MENU_ITEM 8-DISPLAY_MENU_SHIFT
#define DISPLAY_ALL_NAMES_ITEM 10-DISPLAY_MENU_SHIFT-1

MENU export_bmp_menu[] =
{
/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
   { "Export map BMP",     (void *)save_bmp_dialog,       NULL,       0,  NULL  },
   { "Export units BMP",     (void *)export_units_bmp_dialog,       NULL,       0,  NULL  },
   { "Export country units",     (void *)export_country_units_bmp_dialog,       NULL,       0,  NULL  },
   { NULL,                  NULL,       NULL,       0,  NULL  }
};

MENU pacgen_menu[] =
{
{ "Import Map or Scn",            (void *)import_pacgen_scenario,       NULL,       0,  NULL  },
{ NULL,                  NULL,       NULL,       0,  NULL  }
};

MENU pzc_menu[] =
{
   { "Export PZSCN",    (void *)save_pzscn,       NULL,       0,  NULL  },
   { "Export PZEQP",    (void *)save_pzeqp,       NULL,       0,  NULL  },
   { "Export PzC units",    (void *)save_pzc_units,       NULL,       0,  NULL  },
   { "Export flags",    (void *)save_pzc_flags,       NULL,       0,  NULL  },
   { "Export terrain",    (void *)save_pzc_tiles,       NULL,       0,  NULL  },
   { NULL,                  NULL,       NULL,       0,  NULL  }

};

MENU lgeneral_menu[] =
{
   { "Export MAP",    (void *)save_lgeneral_map,       NULL,       0,  NULL  },
   { "Export SCN && MAP",    (void *)save_lgeneral_scenario,       NULL,       0,  NULL  },
   { "Export UDB",    (void *)save_lgeneral_equ,       NULL,       0,  NULL  },
   { "Export units BMP",    (void *)save_lgeneral_units_bmp,       NULL,       0,  NULL  },
   { "Export nations",    (void *)save_lgeneral_nations,       NULL,       0,  NULL  },
   { NULL,                  NULL,       NULL,       0,  NULL  }
};

MENU pg_menu[] =
{
   { "Export EQU",    (void *)save_equip_dialog,       NULL,       0,  NULL  },
   { NULL,                  NULL,       NULL,       0,  NULL  }
};

MENU pg2_menu[] =
{
	{ "Import EQU",    (void *)import_pg2_equip_dialog,       NULL,       0,  NULL  },
	{ "Import MAP/SCN",    (void *)import_pg2_map_scn_gui,       NULL,       0,  NULL  },
	{ "",     NULL,       NULL,       0,  NULL  },
	{ "Convert campaign",    (void *)convert_pg2_campaign_gui,       NULL,       0,  NULL  },
	{ NULL,   NULL,       NULL,       0,  NULL  }
};


MENU pgf_menu[] =
{
   { "Export PGEQP",    (void *)save_pgf_equipment,       NULL,       0,  NULL  },
   { "",     NULL,       NULL,       0,  NULL  },
   { "Export Units",        (void *)save_pgf_units_menu_bmp,       NULL,        0,  NULL  },
   { "Export Tiles",        (void *)save_pgf_dry_tiles_bmp,       NULL,        0,  NULL  },
   { "Export Stack Icons",  (void *)save_pgf_stack_bmp,       NULL,        0,  NULL  },
   { "Export Flags",        (void *)save_pgf_flags_bmp,       NULL,        0,  NULL  },
   { "Export Strength",        (void *)save_pgf_strength_bmp,       NULL,        0,  NULL  },
   { "",     NULL,       NULL,       0,  NULL  },
   { "Convert BRF to PGBRF",        (void *)convert_brf_to_pgbrf,       NULL,        0,  NULL  },
   { NULL,                  NULL,       NULL,       0,  NULL  }
};

MENU file_menu[] =
{
/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
   { "New",               (void *)new_map_dialog,       NULL,       0,  NULL  },
   { "",               NULL,       NULL,       0,  NULL  },
   { "Load",            (void *)load_dialog,       NULL,       0,  NULL  },
   { "Save",            (void *)save_dialog ,       NULL,       0,  NULL  },
   { "&Save As\tAlt-s", (void *)save_as_dialog ,       NULL,       0,  NULL  },
   { "",               NULL,       NULL,       0,  NULL  },
   { "Scenario",     (void *)do_scenario_dialog,       NULL,       0,  NULL  },
   { "",               NULL,       NULL,       0,  NULL  },
   { "PG",     NULL,       pg_menu,       0,  NULL  },
   { "PG2",     NULL,       pg2_menu,       0,  NULL  },
   { "PGF",     NULL,       pgf_menu,       0,  NULL  },
   { "PzC",     NULL,       pzc_menu,       0,  NULL  },
   { "PacGen",     NULL,       pacgen_menu,       0,  NULL  },
   { "LGeneral",     NULL,       lgeneral_menu,       0,  NULL  },
   { "",               NULL,       NULL,       0,  NULL  },
   { "Export picture",     NULL,       export_bmp_menu,       0,  NULL  },
   { "",               NULL,       NULL,       0,  NULL  },
   { "Exit",           (void *)onExit,       NULL,       0,  NULL },
   { NULL,             NULL,       NULL,       0,  NULL  }
	};


	MENU edit_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
		{ "&Undo\tAlt-u",        (void *)onUndo,       NULL,       0,  NULL  },
		{ "",     NULL,       NULL,       0,  NULL  },
		{ "Replace",        (void *)search_replace_dialog,       NULL,       0,  NULL  },
		{ "",     NULL,       NULL,       0,  NULL  },
		{ "Map Fragment Export",     (void *)do_rectangle_export,       NULL,       0,  NULL  },
		{ "Map Fragment Import",     (void *)do_rectangle_import,       NULL,       0,  NULL  },
		{ "",     NULL,       NULL,       0,  NULL  },
		{ "Map Resize",     (void *)map_resize_dialog,       NULL,       0,  NULL  },
		{ "Load Map Layer",     (void *)load_map_layer_dialog,       NULL,       0,  NULL  },
	  // { "",     NULL,       NULL,       0,  NULL  },
	  // { "Configuration",     (void *)configuration_dialog,       NULL,       0,  NULL  },
		{ NULL,                  NULL,       NULL,       0,  NULL  }
	};

	MENU display_name_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	   { "Decimal ID",     change_display_names,       NULL,       0,  (void *)0  },
	   { "Hex ID",     change_display_names,       NULL,       0,  (void *)1  },
	   { "Name",        change_display_names,       NULL,        0,  (void *)2   },
	   { NULL,                  NULL,       NULL,       0,  NULL  }
	};


	MENU mode_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	//   { "Terrain",        NULL,       NULL,       0,  NULL  },
	//   { "Tiles",     NULL,       NULL,       0,  NULL  },
	//   { "",     NULL,       NULL,       0,  NULL  },
	   { "Neutral",     (void *)neutral_mode,       NULL,       0,  NULL  },
	   { "Deploy",     (void *)deploy_mode,       NULL,       0,  NULL  },
	   { "Victory",     (void *)victory_mode,       NULL,       0,  NULL  },
	   { "Flags",    (void *)owner_mode,       NULL,       0,  NULL  },
	   { "",     NULL,       NULL,       0,  NULL  },
	   { "Experience",     (void *)expirience_mode,       NULL,       0,  NULL  },
	   { "Strength",     (void *)strength_mode,       NULL,       0,  NULL  },
	   { "Entrenchment",     (void *)entrench_mode,       NULL,       0,  NULL  },
	   { "",     NULL,       NULL,       0,  NULL  },
	   { "Select Fragment",     (void *)select_fragment_mode,       NULL,       0,  NULL  },
	   { "Place &Fragment\tAlt-f",     (void *)place_fragment_mode,       NULL,       0,  NULL  },
	   { "",     NULL,       NULL,       0,  NULL  },
	   { "Transport",     (void *)transport_mode,       NULL,       0,  NULL  },

	   { NULL,                  NULL,       NULL,       0,  NULL  }
	};
	MENU spottings_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	   { "None",     change_spottings,       NULL,       0,  (void *)2  },
	   { "Axis",     change_spottings,       NULL,       0,  (void *)0  },
	   { "Allied",        change_spottings,       NULL,       0,  (void *)1   },
	   { NULL,                  NULL,       NULL,       0,  NULL  }
	};
	MENU unit_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	   { "Unit",       (void *)unit_dialog ,       NULL,       0,  NULL  },
	   { "Place",     (void*)units_place_mode,       NULL,       0,  NULL  },
	   { "",     NULL,       NULL,       0,  NULL  },
	   { "Units List",     (void *)unit_list_dialog,       NULL,       0,  NULL  },
	   { "Units Stats",     (void *)unitsum_dialog,       NULL,       0,  NULL  },
	   { NULL,                  NULL,       NULL,       0,  NULL  }
	};


	MENU weather_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	   { "Normal",        change_weather,       NULL,       0,  (void *)0  },
	   { "Mud",     change_weather,       NULL,       0,  (void *)1  },
	   { "Snow",     change_weather,       NULL,       0,  (void *)2  },
	   { NULL,                  NULL,       NULL,       0,  NULL  }
	};

	MENU unique_names_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	   { "No names",        change_unique_names,       NULL,       0,  (void *)0  },
	   { "Cites && Rivers",     change_unique_names,       NULL,       0,  (void *)1  },
	   { "Cites",     change_unique_names,       NULL,       0,  (void *)2  },
	   { "River",     change_unique_names,       NULL,       0,  (void *)3  },
	   { NULL,                  NULL,       NULL,       0,  NULL  }
	};

	MENU view_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
/*0*/	  // 	{ "Ground Units",        NULL,       NULL,       0,  NULL  },
/*1*/	//	{ "Air Units",     NULL,       NULL,       0,  NULL  },
/*2*/	//	{ "",     NULL,       NULL,       0,  NULL  },
/*3*/	 //  	{ "Axis",     NULL,       NULL,       0,  NULL  },
/*4*/	  // 	{ "Core",     NULL,       NULL,       0,  NULL  },
/*5*/	 //  { "Allied",     NULL,       NULL,       0,  NULL  },
/*6*/	 //  { "",     NULL,       NULL,       0,  NULL  },
/*7*/	   { "Roads",     (void *)push_new_road,       NULL,       0,  NULL  },
/*8*/	   { "Terrain Type",     (void *)ctrl_t_keycallback,       NULL,       0,  NULL  },
/*10*/	   { "All Names",      (void *)ctrl_a_keycallback,       NULL,       0,  NULL  },
/*9*/	   { "Unique Names",     NULL,       unique_names_menu,       0,  NULL  },
/*11*/	   { "Display Names",     NULL,       display_name_menu,       0,  NULL  },
/*13*/	   { "",     NULL,       NULL,       0,  NULL  },
/*12*/	   { "Unit ID",     (void *)ctrl_o_keycallback,       NULL,       0,  NULL  },
/*14*/	   { "Transports",     (void *)ctrl_l_keycallback,       NULL,       0,  NULL  },
/*15*/	   { "Spotting",     NULL,       spottings_menu,       0,  NULL  },
/*16*/	   { "Scenario view",     (void *)change_scenarioUnitsMode,       NULL,       0,  NULL  },
/*17*/	   { "Game View",     (void *)ctrl_v_keycallback,       NULL,       0,  NULL  },
/*18*/	   { "Hex Grid",     (void *)ctrl_alt_h_keycallback,       NULL,       0,  NULL  },
	   { "",     NULL,       NULL,       0,  NULL  },
	   { "Weather",     NULL,       weather_menu,       0,  NULL  },
	   { "",     NULL,       NULL,       0,  NULL  },
	   { "Unit Flags",     (void *)flag_display_toggle,       NULL,       0,  NULL  },
	   { NULL,                  NULL,       NULL,       0,  NULL  }
	};


	MENU tools_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	   { "BMP to Map",       (void *)ctrl_y_keycallback,  NULL,       0,  NULL  },
	   { "Rotate Map",       (void *)rotate_map_dialog,   NULL,       0,  NULL  },
	   { "",                 NULL,                        NULL,       0,  NULL  },
	   { "Make Tiles",       (void *)generate_dialog,     NULL,       0,  NULL  },
	   { "Generate layers",  (void *)ctrl_q_keycallback,  NULL,       0,  NULL  },
	   { "Randomize",        (void *)randomize_dialog,    NULL,       0,  NULL  },
	   { "",                 NULL,                        NULL,       0,  NULL  },
	   { "Reread Names",     (void *)ctrl_j_keycallback,  NULL,       0,  NULL  },
	   { "Edit Names",       (void *)do_findname_dlg,     NULL,       0,  NULL  },
	   { "",                 NULL,                        NULL,       0,  NULL  },
	   { "Sort Units",       (void *)sort_units,          NULL,       0,  NULL  },
	   { "",                 NULL,                        NULL,       0,  NULL  },
	   { "Move table",       (void *)show_move_table_dlg, NULL,       0,  NULL  },
	   { "Weather table",    (void *)show_weather_table_dlg,NULL,     0,  NULL  },

	   { NULL,               NULL,                        NULL,       0,  NULL  }
	};

	MENU help_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	   { "Help",               (void *)help_dialog,       NULL,       0,  NULL  },
	   { "",               NULL,       NULL,       0,  NULL  },
	   { "About",              (void *)about_box,       NULL,       0,  NULL  },
	   { NULL,                  NULL,       NULL,       0,  NULL  }
	};

	MENU fpge_menu[] =
	{
	/* { "Text\tsc",      (callback),  (Submenu), (flags),  (dp)  )*/
	   { "File",                NULL,   file_menu,      0,  NULL  },
	   { "Edit",             NULL,   edit_menu,      0,  NULL  },
	   { "View",             NULL,   view_menu,      0,  NULL  },
	   { "Mode",             NULL,   mode_menu,      0,  NULL  },
	   { "Units",             NULL,   unit_menu,      0,  NULL  },
	   { "Tools",             NULL,   tools_menu,      0,  NULL  },
	   { "Help",               NULL,   help_menu,      0,  NULL  },
	   { NULL,                  NULL,        NULL,      0,  NULL  }
	};

void select_weather_item(){
	int i;

	for(i=0;i<3;i++) weather_menu[i].flags&=~D_SELECTED;
	weather_menu[showWeather].flags|=D_SELECTED;
}

void select_unique_name_item(){
	int i;

	for(i=0;i<4;i++) unique_names_menu[i].flags&=~D_SELECTED;
	unique_names_menu[drawNames].flags|=D_SELECTED;
}

void select_display_name_item(){
	int i;

	for(i=0;i<3;i++) display_name_menu[i].flags&=~D_SELECTED;
	display_name_menu[showDecimal].flags|=D_SELECTED;
}

void select_hex_item(){
	if (showHex) view_menu[DISPLAY_HEX_MENU_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_HEX_MENU_ITEM].flags&=~D_SELECTED;
}

void select_flag_item(){
	if (showCounter==3) view_menu[DISPLAY_FLAG_MENU_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_FLAG_MENU_ITEM].flags&=~D_SELECTED;
}

void select_all_units_item(){
	if (displayAllUnits) view_menu[DISPLAY_ALL_UNITS_MENU_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_ALL_UNITS_MENU_ITEM].flags&=~D_SELECTED;
}

void select_unit_order_item(){
	if (displayUnitsOrder) view_menu[DISPLAY_UNIT_ORDER_MENU_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_UNIT_ORDER_MENU_ITEM].flags&=~D_SELECTED;
}

void select_scenario_units_item(){
	if (scenarioUnitsMode) view_menu[DISPLAY_SCENARIO_UNITS_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_SCENARIO_UNITS_ITEM].flags&=~D_SELECTED;
}

void select_show_ranges_item(){
	int i;

	for(i=0;i<3;i++) spottings_menu[i].flags&=~D_SELECTED;
	spottings_menu[ (show_ranges+1)%3].flags|=D_SELECTED;
}

void select_gnd_transport_item(){
	if (drawGndTransport) view_menu[DISPLAY_GND_TRANSPORT_MENU_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_GND_TRANSPORT_MENU_ITEM].flags&=~D_SELECTED;
}

void select_road_item(){
	if (drawRoads) view_menu[DISPLAY_ROAD_MENU_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_ROAD_MENU_ITEM].flags&=~D_SELECTED;
}

void select_terrain_item(){
	if (drawTerrain) view_menu[DISPLAY_TERRAIN_MENU_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_TERRAIN_MENU_ITEM].flags&=~D_SELECTED;
}

void select_all_names_item(){
	if (drawAllNames) view_menu[DISPLAY_ALL_NAMES_ITEM].flags|=D_SELECTED;
	else view_menu[DISPLAY_ALL_NAMES_ITEM].flags&=~D_SELECTED;
}

int onExit(){
	return D_CLOSE;
}

void setup_menu_ticks(){
	draw_new_weather();
	draw_new_unique_names();
	draw_new_display_names();
	draw_new_hex();
	draw_new_flag();
	draw_new_gnd_transport();
	draw_new_unit_order();
	draw_new_all_units();
	draw_new_scenario_units();
	draw_new_spottings();
	draw_new_road();
	draw_new_terain();
	draw_new_all_names();
}

int onUndo(){
	if (tile_mode == 1) {
		tile_cleanup();
	}

	pull_undo();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	return D_REDRAW;
}
