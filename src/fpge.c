/* FPGE.C the main section of fpge. Here and in .h we define data structure */
/* ,global variables and the main dialog dmain                              */

#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "fpge.h"
#include "maingui.h"
#include "cli.h"
#include "config.h"
#include "load.h"
#include "save.h"
#include "tables.h"
#include "minimap.h"
#include "unitlist.h"
#include "unitutil.h"
#include "pgf.h"
#include "menu.h"
#include "fpge_colors.h"
#include "filename.h"
#include "terr.h"
#include "makemap.h"
#include "mapfrg.h"
#include "ag.h"
#include "pacgen.h"
#include "drawmap.h"
#include "loadpng.h"
#include "app6.h"
#include "undo.h"
#include "pg.h"
#include "move.h"
#include "bintables.h"
#include "maputils.h"
#include "tilemixer.h"

char MapInfoTxt[256];
char AboutTxt1[256] = "Fred's Panzer General Editor";
char AboutTxt2[256] = "Version 0.7.4";
char AboutTxt3[256] = "Modified by Wino";
char MapStatusTxt[256];
char FilterStatusTxt[256];
char EquipmentFileToSave[MAX_PATH * 4] = ".\\"; // *4 for in case of UTF
char PacGenFile[MAX_PATH * 4] = ".\\"; // *4 for in case of UTF

BITMAP *map_bmp;
BITMAP *left_arrow_bmp;
BITMAP *right_arrow_bmp;
BITMAP *up_arrow_bmp;
BITMAP *down_arrow_bmp;

BITMAP *vic_hex_bmp;
BITMAP *neutral_hex_bmp;
BITMAP *deploy_hex_bmp;
BITMAP *black_hex_bmp;
BITMAP *problem_hex_bmp;

BITMAP *road_hex_bmp;
BITMAP *river_hex_bmp;
BITMAP *road_and_river_hex_bmp;

BITMAP *unit_bmp[MAX_UICONS];
BITMAP *munit_bmp[MAX_MUICONS];
BITMAP *stack_bmp[MAX_SICONS];
BITMAP *save_map_bmp;

/* ----- the initialization of dm ---------------------------------- */
//#define dm_OBJECTS 29  //number of objects in dm Dialog
//#define FUNCTION_BUTTONS 11//buttons number 1-this are radio type


/* ------------- Here ends the main dialog ----------------------------- */

/* -------------- Globals --------------------------------------- */
struct shp_header header;
struct MAP map[MAX_MAP_X][MAX_MAP_Y]; //main map storage
unsigned char equip[MAX_UNITS][EQUIP_REC_SIZE];
unsigned char equip_name_utf8[MAX_UNITS][20 * 2];

unsigned char icon_name_aux1[MAX_UNITS][MAX_UNIT_AUX_NAME_SIZE * 2];
unsigned char icon_name_aux2[MAX_UNITS][MAX_UNIT_AUX_NAME_SIZE * 2];

char equip_country[MAX_UNITS];
WORD equip_flags[MAX_UNITS];
int8_t equip_last_month[MAX_UNITS];

struct conversion_equipment conversion_equip[MAX_UNITS];

BYTE scn_buffer[MAX_SCN_SIZE];
int scn_size; //bytes we actually read
BYTE s4_buffer[16]; //hold prestige and transport info
//s4 data starts at scn_buffer[0x75]*4+0x77
enum EDIT_FUNCTION edit_op;

struct UNIT all_units[MAX_AXIS_UNITS + MAX_ALLIED_UNITS + MAX_AXIS_AUX_UNITS];

long icon_addr[MAX_ICONS];
//tiles
BITMAP *til_bmp[MAX_TILES];
BITMAP *dark_til_bmp[MAX_TILES];

BITMAP *til_bmp_mud[MAX_TILES];
BITMAP *dark_til_bmp_mud[MAX_TILES];

BITMAP *til_bmp_snow[MAX_TILES];
BITMAP *dark_til_bmp_snow[MAX_TILES];

//mini tiles
BITMAP *mtil_bmp[MAX_TILES];
BITMAP *dark_mtil_bmp[MAX_TILES];
BITMAP *mtil_bmp_mud[MAX_TILES];
BITMAP *dark_mtil_bmp_mud[MAX_TILES];
BITMAP *mtil_bmp_snow[MAX_TILES];
BITMAP *dark_mtil_bmp_snow[MAX_TILES];

BITMAP *flag_bmp[MAX_FLAGS];
BITMAP *victory_flag_bmp[MAX_FLAGS];
BITMAP *mini_victory_hex_bmp[MAX_MINI_VIC_HEX];
BITMAP *strength_bmp[MAX_STRENGTH];

// show unit types: [g],[a],[t],hide all [u]nits
int drawNames = 0;// [0],[1],[2],[3] - do not draw names, nonunique,nonunique but not river,river only
int drawAllNames = 0;// yes,no -all [n]ames
int drawTerrain = 0;//yes/no -terrain t[y]pe
int drawRoads = 0;//yes/no -[r]oads
int showDecimal = 0;//0,1,2-[d]ecimal,[h]ex,show [f]ull name
int showFilter = 0;//
int filterTerrain = -1;
int filterName = -1;
int showCounter = 0; // STR -1,0,1,2,3 - [e],[s]trength,[x],no [c]ounters at all
int drawGndTransport = 0;//yes/no - [l]
int showWeather = 0;// 0,1,2 - w-n[o]rmal, [m]ud, [w]inter,
int showHex = 1;//yes/no -h show hex gr[i]d
int showHexMatrix = 1;//yes/no -h show hex grid in matrix
int showHexColor = 0;//GRAY, WHITE, BLACK, RED, GREEN, BLUE
int showHexMatrixColor = 0;//almost same colors as showHexColor
int showMatrixMode = 0; //only for filtered mode, 0 -normal,1-columns,2-grid
int sortMatrixMode = 0; //0-sorted by terrain type, 1 - sorted by tile id
int scenarioUnitsMode = 0; //
int displayAllUnits = 0;
int displayUnitsOrder = 0;
int show_ranges = 2;
int show_problems = 0; //0-do not show,1-river
int show_debug_problems = 0; //0-do not show
int colorizeNames = 0;
int mapShiftMode = 0;
int drawRoadsDebug=0;
int showDecimalDebug=0;
int drawNamesDebug=0;
int drawAllNamesDebug=0;
int colorizeNamesDebug=0;
int drawTerrainDebug=0;

int fpge_gui_fg_color;
int fpge_gui_bg_color;
int fpge_gui_edit_color;

int graphical_overide = 0;
int graphical_overide_hex = 0;
int debug_tile_matrix = 1;

int tiles_wide = (SCREEN_X - 80) / TILE_WIDTH;
int tiles_high = (SCREEN_Y - TILE_HEIGHT / 2 - 54) / TILE_HEIGHT;

int map_h = (TILES_HIGH * 2 + 1) * TILE_HEIGHT / 2 + 1;
int map_w = (TILES_WIDE * 3 / 2 + 1) * 30 - 15;

int matrix_x = 1;
int matrix_y = 1;
int frg_x = -1;//where to place map fragment
int frg_y = -1;
int frg_x0 = -1;//last map fragment place
int frg_y0 = -1;
int frg_dx0 = 0;//selected map fragment dx,dy
int frg_dy0 = 0;

int hslide_w = SCREEN_X - 440;
int hslide_x = HSLIDE_X;
int vslide_x = SCREEN_X - 80;
int vslide_h = SCREEN_Y - 280;
int top_spacer = 16;
int bottom_spacer = 0;

//int scn_number;
char scn_file_name[128];
int stm_number;
int map_number;
int pgf_map_number;

int vslide_max; //max position is defined by size of map...in load
int pix_pos, max_pix_x, max_pix_y;

int total_tiles;//total number of loaded titles
int total_mtiles = 0;
int total_uicons;//number of units icons
int total_muicons;
int total_sicons;
int total_units; //in on the map
int total_equip; //units on panzequip
int conversion_total_equip; //units on panzequip
int total_flags;
int total_countries;
int total_mflags;
int total_strength_counters; //NOT yet used except loading

struct VICTORY_HEXES victory_hexes[MAX_VICTORY_HEXES];
struct DEPLOY_HEXES deploy_hexes[MAX_DEPLOY_HEXES];

//units in the scenario
int total_axis_core;
int total_axis_aux;
int total_allied_core;
int total_allied_aux;

//gr
int card_old, w_old, h_old;

int total_deploy; //deployment hexes

int mapx, mapy; //the mapsize
int map_x0, map_y0; //upper left of map
int show_unit_layer_type = 0; //display ground=0 or air=1 or transport=2 or none=3
int map_mouse_x, map_mouse_y; //coordinates of map where mouse is
int last_map_mouse_x = -1, last_map_mouse_y = -1; //coordinates of map where mouse is
int tile_mode = 0; //0-no tile grid, 1-tile grid = must do cleanup
int filter_tiles = 0, filter_start, filter_stop, filter_last = -1;
int rexp_left_mouse_click = 0, rexp_x = -1, rexp_y = -1;
int backup_not_saved = 1;
int swap_indicators = 0;

short temp[MAX_TILES]; //use this for storing map bmp when viewing tiles
int x0temp, y0temp; //used during tile selection
int tempmapx, tempmapy;
int tempvslide_max;
int tempHSlide_d1, tempHSlide_d2;
int tempVSlide_d1, tempVSlide_d2;
int total_names=14; //first 14 names are predefined for PacGen mode

int unit_type_selected; //picked in the ud dialog
int unum_selected = 0; //the current unit for select and placement
int tnum_selected = 0; //the current transport
int cnum_selected = 0; //the current country
int anum_selected = 0;
int ustr_selected = 10;
int uent_selected = 0;
int uexp_selected = 0;

char uselected[16] = "0"; //an ascii number for the current unit to be placed
char tselected[16] = "0"; //an ascii number for its transport
char cselected[16] = "0"; //an ascii number for the country
char aselected[16] = "0"; //an ascii aux transport number

struct UNIT selected_unit;

char gln[MAX_NAMES][MAX_NAME_SIZE] = {
		"Clear","Coast","Ocean","Port","River",
		"Mountain","Airfield","Swamp","City","Rough",
		"Forest","Desert","Fortification","Bocage"
};
unsigned char gln_utf8[MAX_NAMES][MAX_NAME_UTF_SIZE]= {
		"Clear","Coast","Ocean","Port","River",
		"Mountain","Airfield","Swamp","City","Rough",
		"Forest","Desert","Fortification","Bocage"
};

unsigned short tempmap[MAX_MAP_X][MAX_MAP_Y];
//unsigned short tempmap2[MAX_MAP_X][MAX_MAP_Y];
unsigned char rc[MAX_MAP_X][MAX_MAP_Y];
unsigned char tiles_temp[MAX_TILES];

unsigned char scn_active[MAX_SCENARIOS];
unsigned char scn_short_description[MAX_SCENARIOS][SHORT_SCN_SIZE];
unsigned char scn_long_description[MAX_SCENARIOS][LONG_SCN_SIZE];

int pg_mode = 0;
int pgf_mode = 0;
int ag_mode = 0;
int pacgen_mode = 0;
int nupl_present = 0;
int bin_tables_present = 0;
int str0_bmp_loaded= 0;
int mapnames_changed = 0;

int place_frg_dialog = 3; //where to place map fragment dialog on screen 0-top.left, 3- bottom,right
/* ---------------------- Functions ------------------------------------ */

int isAllied(enum UTYPES uflag) {
	return (uflag == allied_core || uflag == allied_aux) ? 1 : 0;
}

int isNotAllied(enum UTYPES uflag) {
	return isAllied(uflag) ? 0 : 1;
}

int isAux(enum UTYPES uflag) {
	return (uflag == axis_aux || uflag == allied_aux) ? 1 : 0;
}

int isNotAux(enum UTYPES uflag) {
	return isAux(uflag) ? 0 : 1;
}

int d_coreRB_proc(int msg, DIALOG *d, int c) {
	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) {
		if (&(main_dlg[dmCoreRBIdx]) == d)
			ubelongs = axis_core;
		if (&(main_dlg[dmAuxRBIdx]) == d)
			ubelongs = axis_aux;
		if (&(main_dlg[dmAlliedRBIdx]) == d)
			ubelongs = allied_core;
		if (&(main_dlg[dmAlliedAuxRBIdx]) == d)
			ubelongs = allied_aux;
	}
	return d_radio_proc(msg, d, c);
}

int d_mapinfo_proc(int msg, DIALOG *d, int c) {
	return d_textbox_proc(msg, d, c);
}
int d_mapstatus_proc(int msg, DIALOG *d, int c) {
	return d_textbox_proc(msg, d, c);
}
int d_filterstatus_proc(int msg, DIALOG *d, int c) {
	return d_textbox_proc(msg, d, c);
}

void strength_click() {
	int x, y, gidx, aidx, changed = 0;
	int str_limit =15;

	x = map_mouse_x;
	y = map_mouse_y;
	gidx = map[x][y].guidx;
	aidx = map[x][y].auidx;

	if (pgf_mode) str_limit=20;

	//left click with ground units showing and unit there
	if ((mouse_b & 1) && show_unit_layer_type != 1 && gidx >= 0)
		if (all_units[gidx].str < str_limit) {
			++all_units[gidx].str;
			changed = 1;
		}
	//left click for air unit
	if ((mouse_b & 1) && show_unit_layer_type == 1 && aidx > +0)
		if (all_units[aidx].str < str_limit) {
			++all_units[aidx].str;
			changed = 1;
		}
	//rigt click on ground
	if ((mouse_b & 2) && show_unit_layer_type != 1 && gidx >= 0)
		if (all_units[gidx].str > 1) {
			--all_units[gidx].str;
			changed = 1;
		}
	//right click on air
	if ((mouse_b & 2) && show_unit_layer_type == 1 && aidx >= 0) //there's an air unit
		if (all_units[aidx].str > 1) {
			--all_units[aidx].str;
			changed = 1;
		}

	if ((changed) && (showCounter == 0)) {
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//broadcast_dialog_message(MSG_DRAW, 0);
	}
}

void experience_click() {
	int x, y, gidx, aidx, changed = 0;

	x = map_mouse_x;
	y = map_mouse_y;
	gidx = map[x][y].guidx;
	aidx = map[x][y].auidx;

	//left click with ground units showing and unit there
	if ((mouse_b & 1) && show_unit_layer_type != 1 && gidx >= 0)
		if (all_units[gidx].exp < 5) {
			++all_units[gidx].exp;
			changed = 1;
		}
	//left click fro air unit
	if ((mouse_b & 1) && show_unit_layer_type == 1 && aidx > +0)
		if (all_units[aidx].exp < 5) {
			++all_units[aidx].exp;
			changed = 1;
		}
	//rigt click on ground
	if ((mouse_b & 2) && show_unit_layer_type != 1 && gidx >= 0)
		if (all_units[gidx].exp > 0) {
			--all_units[gidx].exp;
			changed = 1;
		}
	//right click on  air
	if ((mouse_b & 2) && show_unit_layer_type == 1 && aidx >= 0) //there's an air unit
		if (all_units[aidx].exp > 0) {
			--all_units[aidx].exp;
			changed = 1;
		}

	if ((changed) && (showCounter == 1)) {
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//broadcast_dialog_message(MSG_DRAW, 0);
	}

}

void entrench_click() {
	int x, y, gidx, changed = 0;

	x = map_mouse_x;
	y = map_mouse_y;
	gidx = map[x][y].guidx;

	//left click with ground units showing and unit there
	if ((mouse_b & 1) && show_unit_layer_type != 1 && gidx >= 0)
		if (all_units[gidx].entrench < 9) {
			++all_units[gidx].entrench;
			changed = 1;
		}
	//rigt click on ground
	if ((mouse_b & 2) && show_unit_layer_type != 1 && gidx >= 0)
		if (all_units[gidx].entrench > 0) {
			--all_units[gidx].entrench;
			changed = 1;
		}

	if ((changed) && (showCounter == 2)) {
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//broadcast_dialog_message(MSG_DRAW, 0);
	}
}

void victory_click() {
	int x, y;
	//int gidx;

	x = map_mouse_x;
	y = map_mouse_y;

	//left click
	if (mouse_b & 1){
		push_undo_one_tile(x,y);
		map[x][y].vic = 1;
	}
	//right click
	if (mouse_b & 2){
		push_undo_one_tile(x,y);
		map[x][y].vic = 0;
	}
	sprintf(MapStatusTxt, "Editing Victory Hexes \n Now %d of %d Max\n", count_vic_hexes(), MAX_VICTORY_HEXES);
	strncat(MapStatusTxt, "Left Click to add\nRight Click to remove",256);
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//broadcast_dialog_message(MSG_DRAW, 0);

}

void frg_select_click() {
	int x, y;
	int x0=0, y0=0;
	//int x1,y1;
	int found, tile_to_save, fragment_type;
	char line[1024];
	char tmp_line[256];
	char path[MAX_PATH] = ".\\";
	FILE *outf;

	x = map_mouse_x;
	y = map_mouse_y;

	//left click
	if (mouse_b & 1) {
		if (key_shifts & KB_SHIFT_FLAG) {
			//clear all
			for (y = 0; y < mapy ; ++y)
				for (x = 0; x < mapx ; ++x)
					map[x][y].shade &= ~SELECTED_FRG_MASK;
		} else {
			//toggle
			if (map[x][y].shade & SELECTED_FRG_MASK)
				map[x][y].shade &= ~SELECTED_FRG_MASK;
			else
				map[x][y].shade |= SELECTED_FRG_MASK;
		}
	}

	//right click -save
	if (mouse_b & 2) {
		//first find is there is anything selected
		found = 0;
		for (x = 0; x < mapx; ++x)
			for (y = 0; y < mapy; ++y)
				if (map[x][y].shade & SELECTED_FRG_MASK) {
					found = 1;
					x0 = x;
					//y0=y;
					x = mapx;
					break;
				}
		for (y = 0; y < mapy; ++y)
			for (x = 0; x < mapx; ++x)
				if (map[x][y].shade & SELECTED_FRG_MASK) {
					found = 1;
					y0 = y;
					y = mapy;
					break;
				}

		/*for (y = mapy-1; y >= 0 ; --y)
		 for (x = mapx-1; x >= 0 ; --x)
		 if (map[x][y].shade & SELECTED_FRG_MASK) {
		 found = 1;
		 x1=x;
		 y1=y;
		 break;
		 }*/
		if (found) {
			//find top, left corner
			//x0=min(x0,x1);
			//y0=min(y0,y1);
			//printf("%d %d\n",x,y);
			strncat(path,fpge_mapfrgt,MAX_PATH);

			canonicalize_filename(path,path,MAX_PATH);
			if (file_select_ex("Save map fragment to file", path, "fgt", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {
				outf = fopen(path, "wb");
				if (outf) {

					//header
					fwrite(&UCS2_header, sizeof(UCS2_header), 1, outf);
					if (pgf_mode  || pacgen_mode) {
						sprintf(line, "# FPGE text map fragment (%d,%d) from '%s' scenario", x0, y0, block1_Name);
					}else{
						sprintf(line, "# FPGE text map fragment (%d,%d) from '%s' scenario", x0, y0, scn_short_description[getScenarioNumber() - 1]);
					}
					fake_UTF_write_string_with_eol(outf, line);
					sprintf(line, "# DX, DY, Offset, Type");
					fake_UTF_write_string_with_eol(outf, line);
					sprintf(line, "# Type: 0-mountains, 1-isles, 2-forts, 3-forest, 4-rivers, 5-lake, 6-other, 7-road, 8-all");
					fake_UTF_write_string_with_eol(outf, line);
					fake_UTF_write_string_with_eol(outf, "");
					fragment_type = 6;
					sprintf(line, "%d\t%d\t%d\t%d", FRG_X_SIZE, FRG_Y_SIZE, x0 % 2, fragment_type);
					fake_UTF_write_string_with_eol(outf, line);
					//save
					for (y = y0; y < y0 + FRG_Y_SIZE; ++y) {
						strncpy(line, "",1024);
						for (x = x0; x < x0 + FRG_X_SIZE; ++x) {
							if (x < mapx && y < mapy && (map[x][y].shade & SELECTED_FRG_MASK))
								tile_to_save = map[x][y].tile;
							else
								tile_to_save = -1;
							//printf("%d %d %d\n",x,y,tile_to_save);
							sprintf(tmp_line, "%d%c", tile_to_save, (x - x0 < FRG_X_SIZE - 1) ? '\t' : ' ');
							strncat(line, tmp_line,1024);
						}
						fake_UTF_write_string_with_eol(outf, line);
					}
					fake_UTF_write_string_with_eol(outf, "");
					fclose(outf);
				}//outf ok

				//else{
				//	return ERROR_TEXT_MAP_FRAGMENT_BASE+ERROR_FPGE_FILE_CANNOT_BE_WRITTEN;
				//}

			}//file_select_ex
		}
	}

	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//broadcast_dialog_message(MSG_DRAW, 0);
}

void transport_click() {

	int x0, y0, idx;

	x0 = map_mouse_x;
	y0 = map_mouse_y;

	if (key_shifts & KB_SHIFT_FLAG) {
		idx = -1;
		if (map[x0][y0].guidx > -1)
			idx = map[x0][y0].guidx;
		if (map[x0][y0].auidx > -1 && show_unit_layer_type==1)
			idx = map[x0][y0].auidx;

		if ((mouse_b & 1) && idx > -1 && all_units[idx].auxtnum > 0)
			all_units[idx].auxtnum = 0;
		if ((mouse_b & 2) && idx > -1 && all_units[idx].orgtnum > 0)
			all_units[idx].orgtnum = 0;
	} else {

		//left click
		if (mouse_b & 1) {
			idx = -1;
			if (map[x0][y0].guidx > -1)
				idx = map[x0][y0].guidx;
			if (map[x0][y0].auidx > -1)
				idx = map[x0][y0].auidx;
			if (idx > -1) {
				if (all_units[idx].uflag == allied_core || all_units[idx].uflag == allied_aux)
					all_units[idx].auxtnum = s4_buffer[ALLIED_SEA_TYPE] + 256 * s4_buffer[ALLIED_SEA_TYPE + 1];
				else
					all_units[idx].auxtnum = s4_buffer[AXIS_SEA_TYPE] + 256 * s4_buffer[AXIS_SEA_TYPE + 1];
			}
		}
		//right click
		if (mouse_b & 2) {
			idx = -1;
			if (map[x0][y0].guidx > -1)
				idx = map[x0][y0].guidx;
			if (map[x0][y0].auidx > -1)
				idx = map[x0][y0].auidx;
			//printf("r idx=%d\n",idx);
			//printf("AXIS_AIR_TYPE=%d\n",s4_buffer[AXIS_AIR_TYPE]+256*s4_buffer[AXIS_AIR_TYPE+1]);
			if (idx > -1) {
				if (all_units[idx].uflag == allied_core || all_units[idx].uflag == allied_aux)
					all_units[idx].auxtnum = s4_buffer[ALLIED_AIR_TYPE] + 256 * s4_buffer[ALLIED_AIR_TYPE + 1];
				else
					all_units[idx].auxtnum = s4_buffer[AXIS_AIR_TYPE] + 256 * s4_buffer[AXIS_AIR_TYPE + 1];
			}
		}
	}
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//broadcast_dialog_message(MSG_DRAW, 0);
}

void frg_place_click() {
	int x, y, xx, yy;
	int x0, y0, mode;

	x0 = map_mouse_x;
	y0 = map_mouse_y;

	if ((key_shifts & KB_ALT_FLAG) || (mouse_b & 4)) {
		place_fragment_mode();
	}

	//left click
	if (mouse_b & 1) {

		if (frg_x0 == x0 && frg_y0 == y0) {
			if (x0 == 0)
				frg_dx0 = (frg_dx0 + 2 * map_frg_dx - 2) % map_frg_dx - map_frg_dx + 1;
			if (y0 == 0)
				frg_dy0 = (frg_dy0 + 2 * map_frg_dy - 2) % map_frg_dy - map_frg_dy + 1;
		} else {
			frg_dx0 = 0;
			frg_dy0 = 0;
		}
		frg_x0 = x0;
		frg_y0 = y0;

		//set global variables
		frg_x = x0 + frg_dx0;
		frg_y = y0 + frg_dy0;
		//we must handle x<0 properly in % so we add 100
		if ((frg_x + 100) % 2 != map_frg_off)
			mode = 1;
		else
			mode = 0;

		//clear
		for (x = 0; x < mapx; x++)
			for (y = 0; y < mapy; y++)
				map[x][y].shade &= ~PLACED_FRG_MASK;
		//mark
		for (x = 0; x < FRG_X_SIZE; ++x)
			for (y = 0; y < FRG_Y_SIZE; ++y)
				if (map_frg[y][x] > -1 && x + frg_x > -1 && y + frg_y + mode * ((x + frg_x + 1) % 2) > -1 && x + frg_x < mapx && y + frg_y + mode * ((x + frg_x + 1) % 2) < mapy)
					map[x + frg_x][y + frg_y + mode * ((x + frg_x + 1) % 2)].shade |= PLACED_FRG_MASK;

	}
	//right click
	if ((mouse_b & 2) && x0 != -1) {
		//clear
		for (x = 0; x < mapx; x++)
			for (y = 0; y < mapy; y++)
				map[x][y].shade &= ~PLACED_FRG_MASK;
		//we must handle x<0 properly in % so we add 100
		if ((frg_x + 100) % 2 != map_frg_off)
			mode = 1;
		else
			mode = 0;
		//place
		open_push_undo_sequenece();
		for (x = 0; x < FRG_X_SIZE; ++x)
			for (y = 0; y < FRG_Y_SIZE; ++y)
				if (map_frg[y][x] > -1 && x + frg_x > -1 && y + frg_y + mode * ((x + frg_x + 1) % 2) > -1 && x + frg_x < mapx && y + frg_y + mode * ((x + frg_x + 1) % 2) < mapy){
					xx=x + frg_x;
					yy=y + frg_y + mode * ((x + frg_x + 1) % 2);
					push_undo_tile(xx,yy);
					map[xx][yy].tile = map_frg[y][x];
				}

		close_push_undo_sequenece();

	}
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//broadcast_dialog_message(MSG_DRAW, 0);
}

void neutral_click() {
	int x, y;
	//int gidx;

	x = map_mouse_x;
	y = map_mouse_y;

	//left click
	if (mouse_b & 1){
		push_undo_one_tile(x,y);
		map[x][y].side = 3;
	}
	//right click
	if (mouse_b & 2){
		push_undo_one_tile(x,y);
		map[x][y].side = 0;
	}
	if (show_ranges != 2)
		compute_shade(show_ranges);
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//broadcast_dialog_message(MSG_DRAW, 0);

}

void deploy_click() {
	int x, y;
	//int gidx;

	x = map_mouse_x;
	y = map_mouse_y;

	//left click
	if (mouse_b & 1){
		push_undo_one_tile(x,y);
		map[x][y].deploy = 1;
	}
	//right click
	if (mouse_b & 2){
		push_undo_one_tile(x,y);
		map[x][y].deploy = 0;
	}
	sprintf(MapStatusTxt, "Editing Deployment Hexes \n Now %d of %d Max\n", count_deploy_hexes(), MAX_DEPLOY_HEXES);
	strncat(MapStatusTxt, "Left Click to add\nRight Click to remove",256);
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//broadcast_dialog_message(MSG_DRAW, 0);
}

void owner_click() {
	int x, y, done = 0, idx = -1;
	unsigned char own;

	x = map_mouse_x;
	y = map_mouse_y;

	//if (keyboard_needs_poll() ) poll_keyboard();
	if (key_shifts & KB_SHIFT_FLAG) {
		own = 0;
	} else {
		own = (mouse_b & 1 ? scn_buffer[0] : scn_buffer[1]);
		if (map[x][y].side == 3)
			own = Neutral[0];
	}
	if (key_shifts & KB_ALT_FLAG) {
		//change unit flag
		if (map[x][y].guidx > -1 && show_unit_layer_type == 0)
			idx = map[x][y].guidx;
		if (map[x][y].auidx > -1 && show_unit_layer_type == 1)
			idx = map[x][y].auidx;

		if (idx > -1) {
			all_units[idx].country = own;
			done = 1;
		}
	}

	if (!done) {
		//left click
		if (mouse_b & 1) {
			push_undo_one_tile(x,y);
			map[x][y].own = own;
			if (map[x][y].side != 3)
				map[x][y].side = 0;
		}
		//right click
		if (mouse_b & 2) {
			push_undo_one_tile(x,y);
			map[x][y].own = own;
			if (map[x][y].side != 3)
				map[x][y].side = 1;
		}
	}
	// update scan ranges
	if (show_ranges != 2)
		compute_shade(show_ranges);
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
}

void do_place_status() {
	 char line1[64], line2[64], tmp[64], tmp2[64];

	strncpy(MapStatusTxt, "Left Click to place\nRight Click: copy&remove",256);
	if (unum_selected > 0)
		strncpy(tmp2, equip_name_utf8[unum_selected],64);
	else
		strncpy(tmp2, "-",64);
	if (tnum_selected > 0)
		strncpy(tmp, equip_name_utf8[tnum_selected],64);
	else
		strncpy(tmp, "-",64);
	sprintf(line1, "\n%s/%s", tmp2, tmp);
	strncat(MapStatusTxt, line1,64);
	if (anum_selected > 0)
		strncpy(tmp, equip_name_utf8[anum_selected],64);
	else
		strncpy(tmp, "-",64);
	sprintf(line1, "\nAux. T. %s", tmp);
	strncat(MapStatusTxt, line1,64);
	sprintf(line2, "\n(str=%d exp=%d ent=%d)", ustr_selected, uexp_selected, uent_selected);

	strncat(MapStatusTxt, line2,64);
}

void place_unit_click() {
	int idx = -1, x, y, where_add_new;
	enum {
		none, air, ground
	} utype;

	x = map_mouse_x;
	y = map_mouse_y; //save some typing
	// process a right click on a ground or air unit
	if ((map[x][y].guidx >= 0 || map[x][y].auidx >= 0) && (mouse_b & 2)) {
		//set the current place unit to this unit's data
		if (show_unit_layer_type == 1)
			idx = map[x][y].auidx;
		else
			idx = map[x][y].guidx;
		unum_selected = all_units[idx].unum;
		tnum_selected = all_units[idx].orgtnum;
		anum_selected = all_units[idx].auxtnum;
		cnum_selected = all_units[idx].country;

		ustr_selected = all_units[idx].str;
		uexp_selected = all_units[idx].exp;
		uent_selected = all_units[idx].entrench;

		//get rid of the unit
		if (show_unit_layer_type != 1)
			remove_gunit(x, y);
		else
			remove_aunit(x, y);

		do_place_status();
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;

		//switch units mode
		main_dlg[dmGndRBIdx].flags &= ~D_SELECTED;
		main_dlg[dmAirRBIdx].flags &= ~D_SELECTED;
		main_dlg[dmSeaRBIdx].flags &= ~D_SELECTED;
		main_dlg[dmNoneRBIdx].flags &= ~D_SELECTED;

		if (show_unit_layer_type == 1){
			main_dlg[dmAirRBIdx].flags |= D_SELECTED;
		}
		else{
			main_dlg[dmGndRBIdx].flags |= D_SELECTED;
			show_unit_layer_type=0;
		}

		main_dlg[dmGndRBIdx].flags |= D_DIRTY;
		main_dlg[dmAirRBIdx].flags |= D_DIRTY;
		main_dlg[dmSeaRBIdx].flags |= D_DIRTY;
		main_dlg[dmNoneRBIdx].flags |= D_DIRTY;

		//switch side mode
		main_dlg[dmCoreRBIdx].flags &= ~D_SELECTED;
		main_dlg[dmAuxRBIdx].flags &= ~D_SELECTED;
		main_dlg[dmAlliedRBIdx].flags &= ~D_SELECTED;
		main_dlg[dmAlliedAuxRBIdx].flags &= ~D_SELECTED;

		switch (all_units[idx].uflag) {
		case axis_core:
			main_dlg[dmCoreRBIdx].flags |= D_SELECTED;
			ubelongs = axis_core;
			break;
		case axis_aux:
			main_dlg[dmAuxRBIdx].flags |= D_SELECTED;
			ubelongs = axis_aux;
			break;
		case allied_core:
			main_dlg[dmAlliedRBIdx].flags |= D_SELECTED;
			ubelongs = allied_core;
			break;
		case allied_aux:
			main_dlg[dmAlliedAuxRBIdx].flags |= D_SELECTED;
			ubelongs = allied_aux;
			break;
		}

		main_dlg[dmCoreRBIdx].flags |= D_DIRTY;
		main_dlg[dmAuxRBIdx].flags |= D_DIRTY;
		main_dlg[dmAlliedRBIdx].flags |= D_DIRTY;
		main_dlg[dmAlliedAuxRBIdx].flags |= D_DIRTY;


		//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	}
	//-----end right click on unit --------------
	//-------- process a left click ---------------
	if (unum_selected <= 0)
		utype = none;
	else if (equip[unum_selected][GAF] == 1)
		utype = air;
	else
		utype = ground;
	//place a ground unit
	if (/*map[x][y].guidx < 0 &&*/show_unit_layer_type != 1 && utype == ground && (mouse_b & 1)) {
		switch (ubelongs) {
		case axis_core:
			where_add_new = total_axis_core;
			total_axis_core++;
			break;
		case axis_aux:
			where_add_new = total_axis_core + total_axis_aux;
			total_axis_aux++;
			break;
		case allied_core:
			where_add_new = total_axis_core + total_axis_aux + total_allied_core;
			total_allied_core++;
			break;
		case allied_aux:
			where_add_new = total_axis_core + total_axis_aux + total_allied_core + total_allied_aux;
			total_allied_aux++;
			break;
		default:
			where_add_new = total_units;
			break;
		}

		add_unit(where_add_new);
		all_units[where_add_new].unum = unum_selected;
		all_units[where_add_new].orgtnum = tnum_selected;
		all_units[where_add_new].country = cnum_selected;
		all_units[where_add_new].auxtnum = anum_selected;
		//set proposed x,y
		all_units[where_add_new].x = x;
		all_units[where_add_new].y = y;
		//find new x,y
		find_xy(where_add_new, show_unit_layer_type, &x, &y);
		//set new x,y
		all_units[where_add_new].x = x;
		all_units[where_add_new].y = y;

		all_units[where_add_new].str = ustr_selected;
		all_units[where_add_new].entrench = uent_selected;
		all_units[where_add_new].exp = uexp_selected;
		all_units[where_add_new].uflag = ubelongs;
		map[x][y].guidx = where_add_new;
		++total_units;
	}
	//place an air unit
	if (/*map[x][y].auidx < 0 && */show_unit_layer_type == 1 && utype == air && (mouse_b & 1)) {
		switch (ubelongs) {
		case axis_core:
			where_add_new = total_axis_core;
			total_axis_core++;
			break;
		case axis_aux:
			where_add_new = total_axis_core + total_axis_aux;
			total_axis_aux++;
			break;
		case allied_core:
			where_add_new = total_axis_core + total_axis_aux + total_allied_core;
			total_allied_core++;
			break;
		case allied_aux:
			where_add_new = total_axis_core + total_axis_aux + total_allied_core + total_allied_aux;
			total_allied_aux++;
			break;
		default:
			where_add_new = total_units;
			break;
		}

		add_unit(where_add_new);
		all_units[where_add_new].unum = unum_selected;
		all_units[where_add_new].orgtnum = tnum_selected;
		all_units[where_add_new].country = cnum_selected;
		all_units[where_add_new].auxtnum = anum_selected;
		//set proposed x,y
		all_units[where_add_new].x = x;
		all_units[where_add_new].y = y;
		//find new x,y
		find_xy(where_add_new, show_unit_layer_type, &x, &y);
		//set new x,y
		all_units[where_add_new].x = x;
		all_units[where_add_new].y = y;
		all_units[where_add_new].str = ustr_selected;
		all_units[where_add_new].entrench = uent_selected;
		all_units[where_add_new].exp = uexp_selected;
		all_units[where_add_new].uflag = ubelongs;
		map[x][y].auidx = where_add_new;
		++total_units;
	}

	if (show_ranges != 2)
		compute_shade(show_ranges);

	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//broadcast_dialog_message(MSG_DRAW, 0);
}

void pick_msg() {
	sprintf(MapStatusTxt, "Pick an Operation.");
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
}

void tile_make_matrix() {
	int i, totalTiles = total_tiles+3; //3 magic tiles

	matrix_x = STD_MATRIX_MAX_X;
	matrix_y = (int) ( totalTiles / matrix_x) + ((totalTiles % matrix_x) > 0);

	edit_op = edit_tile;

	//clear mouse, so description will be shown
	map_mouse_x = -1;
	map_mouse_y = -1;

	//save all tiles
	for (i = 0; i < matrix_x * matrix_y; ++i)
		temp[i] = map[i % (matrix_x)][i / (matrix_x)].tile;

	draw_tiles_matrix();
	tile_mode = 1; // we must cleanup afterwards
	//save values
	x0temp = map_x0;
	y0temp = map_y0;
	tempmapx = mapx;
	tempmapy = mapy;

	tempvslide_max = vslide_max;
	tempHSlide_d1 = main_dlg[dmHSlideIdx].d1;
	tempHSlide_d2 = main_dlg[dmHSlideIdx].d2;
	tempVSlide_d1 = main_dlg[dmVSlideIdx].d1;
	tempVSlide_d2 = main_dlg[dmVSlideIdx].d2;
}

void tile_cleanup() {
	int i;

	//matrix_y = (int)(total_tiles / STD_MATRIX_MAX_X) + ((total_tiles%STD_MATRIX_MAX_X)>0);
	//matrix_x = STD_MATRIX_MAX_X;

	//clear mouse, so description will be shown
	map_mouse_x = -1;
	map_mouse_y = -1;

	tile_mode = 0;
	for (i = 0; i < matrix_x * matrix_y; ++i)
		map[i % (matrix_x)][i / (matrix_x)].tile = temp[i];
	//cleanup
	map_x0 = x0temp;
	map_y0 = y0temp;
	mapx = tempmapx;
	mapy = tempmapy;
	//printf("%d\n",map_x0);
	vslide_max = tempvslide_max;
	main_dlg[dmHSlideIdx].d1 = tempHSlide_d1;
	main_dlg[dmHSlideIdx].d2 = tempHSlide_d2;
	main_dlg[dmVSlideIdx].d1 = tempVSlide_d1;
	main_dlg[dmVSlideIdx].d2 = tempVSlide_d2;
	//position_gui_elements(w_old,h_old);
	main_dlg[dmVSlideIdx].flags |= D_DIRTY;
	main_dlg[dmHSlideIdx].flags |= D_DIRTY;
	//d_hslide_proc(MSG_DRAW,&(main_dlg[dmHSlideIdx]),0);
	//d_vslide_proc(MSG_DRAW,&(main_dlg[dmVSlideIdx]),0);

	main_dlg[dmTileBtnIdx].flags &= ~D_SELECTED;
	main_dlg[dmTileBtnIdx].flags |= D_DIRTY;
	edit_op = edit_none;
}

void tile_click() {
	int idx;
	//matrix_y = (int)(total_tiles / STD_MATRIX_MAX_X) + ((total_tiles%STD_MATRIX_MAX_X)>0);

	//a click transfers the tile bmp and cancels the op
	//edit_op=none;
	main_dlg[dmTileBtnIdx].flags &= ~D_SELECTED;
	d_btn_proc(MSG_DRAW, &(main_dlg[dmTileBtnIdx]), 0);
	if (map_mouse_x < matrix_x && map_mouse_y < matrix_y ) {
		if (map[map_mouse_x][map_mouse_y].tile ==BLACK_TILE)
			return; //do nothing
		idx = map[map_mouse_x][map_mouse_y].tile;
		sprintf(tdTNStr, "%d", idx);

		if (GUI_fill_default_tt) {
			//fill default type
			if (idx < total_tiles && TTData_Max_Tiles[idx] > -1) {
				sprintf(tdTTStr, "%d", TTData_Max_Tiles[idx]);
			}
			//fill default name
			if (idx < total_tiles && NData_Max_Tiles[idx] > -1) {
				sprintf(tdGLNStr, "%d", NData_Max_Tiles[idx]);
			}
		}
	}

	tile_cleanup();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
	//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
	//sprintf(MapStatusTxt,"Pick an Operation",0);
	//d_mapstatus_proc(MSG_DRAW,&(dm[dmMapStatusIdx]),0);

	// if somebody picks tiles then go to ter mode
	edit_op = edit_ter;
	main_dlg[dmTerBtnIdx].flags = D_SELECTED;
	d_btn_proc(MSG_DRAW, &(main_dlg[dmTerBtnIdx]), 0);
	setup_terrain_info();
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
}

void copyToTerrain(int x, int y) {

	if (x<0 || x>=mapx || y<0 || y>=mapy) return; //out of map

	if (tdMatchMatchOn) {
		if (tdTTStrOn) {
			if (tdTTStrMatchOn) {
				if (map[x][y].utr == atoi(tdTTStrMatch))
					map[x][y].utr = atoi(tdTTStr);
			} else
				map[x][y].utr = atoi(tdTTStr);
		}
		if (tdRDStrOn) {
			if (tdRDStrMatchOn) {
				if (map[x][y].rc == atoi(tdRDStrMatch))
					map[x][y].rc = atoi(tdRDStr);
			} else
				map[x][y].rc = atoi(tdRDStr);
		}
		if (tdTNStrOn) {
			if (tdTNStrMatchOn) {
				if (map[x][y].tile == atoi(tdTNStrMatch))
					map[x][y].tile = atoi(tdTNStr);
			} else
				map[x][y].tile = atoi(tdTNStr);
		}
		if (tdGLNStrOn) {
			if (tdGLNStrMatchOn) {
				if (map[x][y].gln == atoi(tdGLNStrMatch))
					map[x][y].gln = atoi(tdGLNStr);
			} else
				map[x][y].gln = atoi(tdGLNStr);
		}
		if (tdSDStrOn) {
			if (tdSDStrMatchOn) {
				if (map[x][y].side == atoi(tdSDStrMatch)) {
					map[x][y].side = atoi(tdSDStr);
				}
			} else {
				map[x][y].side = atoi(tdSDStr);
			}
		}
	} else {
		if (tdTTStrOn)
			map[x][y].utr = atoi(tdTTStr);
		if (tdRDStrOn)
			map[x][y].rc = atoi(tdRDStr);
		if (tdTNStrOn)
			map[x][y].tile = atoi(tdTNStr);
		if (tdGLNStrOn)
			map[x][y].gln = atoi(tdGLNStr);
		if (tdSDStrOn)
			map[x][y].side = atoi(tdSDStr);
	}
}

void terrain_click() {
	int x, y, i, j, ssx, ssy, esx, esy;

	x = map_mouse_x;
	y = map_mouse_y;
	int prob=atoi(tdProbStr);

	if (mouse_b & 1) {
		if (key_shifts & KB_SHIFT_FLAG) {
			if (last_map_mouse_x != -1) {
				ssx = (x > last_map_mouse_x) ? last_map_mouse_x : x;
				esx = (x > last_map_mouse_x) ? x : last_map_mouse_x;
				ssy = (y > last_map_mouse_y) ? last_map_mouse_y : y;
				esy = (y > last_map_mouse_y) ? y : last_map_mouse_y;

				open_push_undo_sequenece();
				for (i = ssx; i <= esx; i++)
					for (j = ssy; j <= esy; j++){
						if (rand()%100 < prob ){
							if ( (TTData_Max_Tiles[map[x][y].tile] == 0 && GUI_only_on_clear) || !GUI_only_on_clear){
								push_undo_tile(i, j);
								copyToTerrain(i, j);
							}
						}
					}
				close_push_undo_sequenece();
			}
		} else {
			open_push_undo_sequenece();
			int xx0,yy0;

			if (rand()%100 < prob ){
				if ( (TTData_Max_Tiles[map[x][y].tile] == 0 && GUI_only_on_clear) || !GUI_only_on_clear){
					push_undo_tile(x, y);
					copyToTerrain(x, y);
				}
			}

			if (GUI_use_brush) {
				int r=atoi(tdRadiusStr);
				for (i = 0; i <= r; i++){
					for (j = -r + i / 2; j <= r - (i + 1) / 2; j++)
						{
							if (rand()%100 < prob ){
								xx0 = x + i;
								yy0 = y + j + x % 2 * (x + i + 1) % 2;
								if ( (TTData_Max_Tiles[map[xx0][yy0].tile] == 0 && GUI_only_on_clear) || !GUI_only_on_clear){
									push_undo_tile(xx0, yy0);
									copyToTerrain(xx0, yy0);
								}
							}
							if (rand()%100 < prob ){
								xx0 = x - i;
								yy0 = y + j + x % 2 * (x + i + 1) % 2;
								if ((TTData_Max_Tiles[map[xx0][yy0].tile] == 0 && GUI_only_on_clear) || !GUI_only_on_clear){
									push_undo_tile(xx0, yy0);
									copyToTerrain(xx0, yy0);
								}
							}
						}
					}
				}
			close_push_undo_sequenece();
		}

		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//d_mapbmp_proc(MSG_DRAW, &(main_dlg[dmMapBmpIdx]), 0);
		last_map_mouse_x = map_mouse_x;
		last_map_mouse_y = map_mouse_y;
	}
	//right click
	if (mouse_b & 2) {
		if (tdTTStrOn)
			sprintf(tdTTStr, "%d", map[x][y].utr);
		if (tdRDStrOn)
			sprintf(tdRDStr, "%d", map[x][y].rc);
		if (tdTNStrOn)
			sprintf(tdTNStr, "%d", map[x][y].tile);
		if (tdGLNStrOn)
			sprintf(tdGLNStr, "%d", map[x][y].gln);
		if (tdSDStrOn)
			sprintf(tdSDStr, "%d", map[x][y].side);
		//setup_terrain_info();
		setup_terrain_info();
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
		//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
	}
}

void rimp_click() {
	int magic = 0, filemagic;
	int x, y, i, j, xx, yy;
	//int ssx,ssy,esx,esy;
	int dx, dy, off = 0, off_now, mode = 0;
	char path[MAX_PATH] = ".\\";
	short tile;
	FILE *inf;
	x = map_mouse_x;
	y = map_mouse_y;

	magic += 'F';
	magic += (int) 'P' << 8;
	magic += (int) 'G' << 16;
	magic += (int) 'E' << 24;

	if (mouse_b & 1) {
		strncat(path,fpge_mapfrg_fgm,MAX_PATH);
		canonicalize_filename(path,path,MAX_PATH);
		if (file_select_ex("Load map fragment from file (tiles only)", path, "fgm", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {
			inf = fopen(path, "rb");
			if (inf) {
				fread(&filemagic, sizeof(filemagic), 1, inf);
				if (filemagic == magic) {
					fread(&dx, sizeof(dx), 1, inf);
					fread(&dy, sizeof(dy), 1, inf);
					fread(&off, sizeof(off), 1, inf);
					off_now = x % 2;
					if (off_now != off)
						mode = 1;
					// tile numbers
					open_push_undo_sequenece();

					for (i = y; i <= y + dy; ++i)
						for (j = x; j <= x + dx; ++j) {
							//read always
							fread(&tile, 2, 1, inf);
							//check for map boundary
							if ((j < mapx) && (i + mode * ((j + 1) % 2) < mapy)){
								xx=j;
								yy=i + mode * ((j + 1) % 2);
								push_undo_tile(xx,yy);
								map[xx][yy].tile = tile;
							}
						}
					close_push_undo_sequenece();
					draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
				} else {
					alert("ERROR: Wrong file format:", path, NULL, "&Continue", NULL, 'c', 0);
				}
				fclose(inf);
			} else {
				alert("ERROR: Cannot load map fragment file:", path, NULL, "&Continue", NULL, 'c', 0);
			}
		}
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//broadcast_dialog_message(MSG_DRAW, 0);
	}
}

void rexp_click() {
	int magic = 0;
	int x, y, i, j, ssx, ssy, esx, esy, dx, dy, off = 0;
	char path[MAX_PATH] = ".\\";
	FILE *outf;
	x = map_mouse_x;
	y = map_mouse_y;

	magic += 'F';
	magic += (int) 'P' << 8;
	magic += (int) 'G' << 16;
	magic += (int) 'E' << 24;

	if (mouse_b & 1) {
		rexp_left_mouse_click = 1;
		rexp_x = x;
		rexp_y = y;
	}
	//right click
	if (mouse_b & 2) {
		if (rexp_left_mouse_click) {
			if (x > rexp_x) {
				ssx = rexp_x;
				esx = x;
			} else {
				esx = rexp_x;
				ssx = x;
			}
			if (y > rexp_y) {
				ssy = rexp_y;
				esy = y;
			} else {
				esy = rexp_y;
				ssy = y;
			}
			dx = esx - ssx;
			dy = esy - ssy;
			strncat(path,fpge_mapfrg_fgm,MAX_PATH);
			canonicalize_filename(path,path,MAX_PATH);
			if (file_select_ex("Save map fragment to file", path, "fgm", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {
				outf = fopen(path, "wb");
				if (outf) {
					fwrite(&magic, sizeof(magic), 1, outf);
					fwrite(&dx, sizeof(dx), 1, outf);
					fwrite(&dy, sizeof(dy), 1, outf);
					off = ssx % 2;
					fwrite(&off, sizeof(off), 1, outf);
					// tile numbers
					for (i = ssy; i <= esy; ++i)
						for (j = ssx; j <= esx; ++j) {
							fputc(map[j][i].tile & 255, outf);
							fputc(map[j][i].tile / 256, outf);
						}
					fclose(outf);

				} else {
					alert("ERROR: Cannot save map fragment file:", path, NULL, "&Continue", NULL, 'c', 0);
				}
			}
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
			//broadcast_dialog_message(MSG_DRAW, 0);
		}
	}
}
/*
 void print_binary8(char *to, int from){
 int i;

 strncpy(to,"",16);
 for(i=0;i<8;i++){
 strncat(to,(from&(1<<(7-i)))?"1":"0",16);
 }
 }
 */

int filter_roads(int x, int y) {
	int i, t, neighbour_tile_idx;
	int rc = map[x][y].rc, central_tile_idx;

	//find road_tiles index
	central_tile_idx = get_road_tile_index(map[x][y].tile);
	if (central_tile_idx == -1)
		return 0; //trying to check non road tile, exit

	for (i = 0; i < 6; i++) {
		//printf("nr %d-(%d:%d)\n",i,x+dx_tab[i],y+dy_tab[i][x%2]);

		if (rc & dir_bit_mask_RC[i]) {
			t = map[x + dx_tab_N_CW[i]][y + dy_tab_N_CW[i][x % 2]].tile;

			neighbour_tile_idx = get_road_tile_index(t);

			if (neighbour_tile_idx == -1) {
				continue;
			} //tile is not road tile, do nothing
			  //check
			  // A O   O A
			  // O X   X O
			if ((      road_connection_info[central_tile_idx].corners_NNW_CW[i] == 1
					&& road_connection_info[neighbour_tile_idx].corners_NNW_CW[(i + 2) % 6] == 0
					&& road_connection_info[central_tile_idx].corners_NNW_CW[(i - 1 + 6) % 6] == 0)

					|| (       road_connection_info[central_tile_idx].corners_NNW_CW[(i - 1 + 6) % 6] == 1
							&& road_connection_info[neighbour_tile_idx].corners_NNW_CW[(i - 3 + 6) % 6] == 0
							&& road_connection_info[central_tile_idx].corners_NNW_CW[i] == 0))
					// this is wrong connection, clear it
					{
				rc &= ~dir_bit_mask_RC[i];
			}
		}
	}
	return rc;
}

int find_one_way_road(int x, int y) {
	int rc = map[x][y].rc;

	if (map[x][y].rc > 0) {
		if ((map[x][y].rc & 0x80) && map[x - 1][y - 1 + x % 2].rc > 0 && !(map[x - 1][y - 1 + x % 2].rc & 0x08))
			rc &= ~0x80;
		if ((map[x][y].rc & 0x01) && map[x][y - 1].rc > 0 && !(map[x][y - 1].rc & 0x10))
			rc &= ~0x01;
		if ((map[x][y].rc & 0x02) && map[x + 1][y - 1 + x % 2].rc > 0 && !(map[x + 1][y - 1 + x % 2].rc & 0x20))
			rc &= ~0x02;
		if ((map[x][y].rc & 0x08) && map[x + 1][y + x % 2].rc > 0 && !(map[x + 1][y + x % 2].rc & 0x80))
			rc &= ~0x08;
		if ((map[x][y].rc & 0x10) && map[x][y + 1].rc > 0 && !(map[x][y + 1].rc & 0x01))
			rc &= ~0x10;
		if ((map[x][y].rc & 0x20) && map[x - 1][y + x % 2].rc > 0 && !(map[x - 1][y + x % 2].rc & 0x02))
			rc &= ~0x20;
	}
	return rc;
}

int find_road_mask(int x, int y) {
	int road_connections = 0, mask;

	if (is_tile_road_tile(map[x][y].tile)) {
		//the tile is a road tile, check connections
		mask = 0;

		if (is_tile_passive_or_road_tile(map[x - 1][y - 1 + x % 2].tile)) {
			mask += 0x80;
		}

		if (is_tile_passive_or_road_tile(map[x][y - 1].tile)) {
			mask += 0x01;
		}

		if (is_tile_passive_or_road_tile(map[x + 1][y - 1 + x % 2].tile)) {
			mask += 0x02;
		}

		if (is_tile_passive_or_road_tile(map[x + 1][y + x % 2].tile)) {
			mask += 0x08;
		}

		if (is_tile_passive_or_road_tile(map[x][y + 1].tile)) {
			mask += 0x10;
		}

		if (is_tile_passive_or_road_tile(map[x - 1][y + x % 2].tile)) {
			mask += 0x20;
		}

		road_connections = mask & road_connection_info[get_road_tile_index(map[x][y].tile)].bits_RC;
		//road_connections = mask & roads_tiles_mask_RC[get_road_tile_index(map[x][y].tile)];
	}
	return road_connections;
}

void none_click() {
	int x, y;
	//int mask;

	x = map_mouse_x;
	y = map_mouse_y;

	// road mode only
	if (drawRoads) {
		if (mouse_b & 1) {
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
			//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
			last_map_mouse_x = map_mouse_x;
			last_map_mouse_y = map_mouse_y;
		}
		//right click - auto fix if possible
		if (mouse_b & 2) {
			push_undo_one_tile(x,y);
			map[x][y].rc = find_road_mask(x, y);
			map[x][y].rc = find_one_way_road(x, y);
			map[x][y].rc = filter_roads(x, y);
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
			//object_message(&main_dlg[dmMapBmpIdx], MSG_DRAW, 0);
		}
	}
}

int d_mapbmp_proc(int msg, DIALOG *d, int c) {
	int column, row, X, Y, idx, v=D_O_K, tt, tn, t;
	char line1[64], line2[64], line3[64], line4[64];

	if (msg == MSG_DRAW) {
		//print_str("MSG_DRAW");
		scare_mouse();
		v = d_bitmap_proc(msg, d, c);

		//BITMAP * b = (BITMAP *)d->dp;
		//if (msg==MSG_DRAW)
		      //blit(b, gui_get_screen(), 0, 0, d->x, d->y, d->w, d->h);
			//masked_blit(b, gui_get_screen(), 0, 0, d->x, d->y, d->w, d->h);
				//stretch_blit(b, gui_get_screen(), 0, 0, d->w, d->h, d->x, d->y, d->w, d->h);
		unscare_mouse();
		return v;
	}
	//process mouse clicks according to the current edit op
	if (msg == MSG_CLICK) {
		X = mouse_x;
		Y = mouse_y - LINE_2_Y;
		column = (X - 8) / TILE_WIDTH;
		if (column < 0)
			column = 0;
		if ((column + map_x0 % 2) % 2)
			row = (Y - TILE_HEIGHT / 2) / TILE_HEIGHT;
		else
			row = Y / TILE_HEIGHT;
		column += map_x0;
		row += map_y0;

		if (edit_op == edit_none)
			none_click();
		if (edit_op == edit_ent)
			entrench_click();
		if (edit_op == edit_str)
			strength_click();
		if (edit_op == edit_exp)
			experience_click();
		if (edit_op == edit_place)
			place_unit_click();
		if (edit_op == edit_vic)
			victory_click();
		if (edit_op == edit_deploy)
			deploy_click();
		if (edit_op == edit_own)
			owner_click();
		if (edit_op == edit_ter)
			terrain_click();
		if (edit_op == edit_tile)
			tile_click();
		if (edit_op == edit_neutral)
			neutral_click();
		if (edit_op == edit_rimp)
			rimp_click();
		if (edit_op == edit_rexp)
			rexp_click();
		if (edit_op == edit_frg_select)
			frg_select_click();
		if (edit_op == edit_frg_place)
			frg_place_click();
		if (edit_op == edit_transport)
			transport_click();
		if (edit_op == move_mode)
			move_click();
		map_mouse_x = -1; //force a redraw on idle of info

	} //end click msg

	if (msg == MSG_LOSTMOUSE) {
		MapInfoTxt[0] = 0;
		//d_mapinfo_proc(MSG_DRAW,&(main_dlg[dmMapInfoTxtIdx]),0);
		main_dlg[dmMapInfoTxtIdx].flags |= D_DIRTY;
		return d_bitmap_proc(msg, d, c);
	}

	if (msg == MSG_IDLE && mouse_x < main_dlg[dmMapBmpIdx].w && mouse_y > main_dlg[dmMapBmpIdx].y) {
		X = mouse_x;
		Y = mouse_y - LINE_2_Y - top_spacer;
		column = (X - 8) / TILE_WIDTH;
		if (column < 0)
			column = 0;
		if ((column + map_x0 % 2) % 2)
			row = (Y - TILE_HEIGHT / 2) / TILE_HEIGHT;
		else
			row = Y / TILE_HEIGHT;
		column += map_x0;
		row += map_y0;
		if (column != map_mouse_x || row != map_mouse_y) {
			map_mouse_x = column;
			map_mouse_y = row;
			if (map_mouse_x < mapx && map_mouse_y < mapy) {
				if (edit_op == edit_tile) {
					t = map[map_mouse_x][map_mouse_y].tile;
					if (t < MAX_TILES) {
						tt = TTData_Max_Tiles[t];
						tn = NData_Max_Tiles[t];
					} else {
						tt = -1;
						tn = -1;
					}
					sprintf(line1, "%2d,%2d\n", map_mouse_x, map_mouse_y);
					if (t < MAX_TILES)
						sprintf(line2, "Tile   : %d\n", t);
					else
						sprintf(line2, "\n");
					if (tn > -1)
						sprintf(line3, "Name   : %s (%d)\n", gln_utf8[tn], tn);
					else
						sprintf(line3, "\n");
					if (tt > -1)
						sprintf(line4, "Terrain: %s (%d)\n", utr_names[tt], tt);
					else
						sprintf(line4, "\n");
					//strncpy(line4,"",64);

				} else {
					sprintf(line1, "%2d,%2d %s\n", map_mouse_x, map_mouse_y, gln_utf8[map[map_mouse_x][map_mouse_y].gln]);

					sprintf(line2, "(%2d,%2Xh,%3d,%4d,%d)\n",
							//sprintf(line2, "(%2d,%2Xh,%2Xh,%4d,%d)\n",
							map[map_mouse_x][map_mouse_y].utr, map[map_mouse_x][map_mouse_y].rc, map[map_mouse_x][map_mouse_y].tile, map[map_mouse_x][map_mouse_y].gln,
							map[map_mouse_x][map_mouse_y].side);
					//now we take care of the unit info
					line3[0] = 0;
					line4[0] = 0;
					if (show_unit_layer_type == 1 && map[map_mouse_x][map_mouse_y].auidx >= 0) //air unit visible and present
					{
						idx = map[map_mouse_x][map_mouse_y].auidx;
						if (all_units[idx].uflag == axis_core || all_units[idx].uflag == allied_core)
							//sprintf(line3,"+%s\n", equip[all_units[idx].unum]);
							sprintf(line3, "+%s\n", equip_name_utf8[all_units[idx].unum]);
						else
							//sprintf(line3,"%s\n", equip[all_units[idx].unum]);
							sprintf(line3, "%s\n", equip_name_utf8[all_units[idx].unum]);

						sprintf(line4, "(str %d,exp %d,ent %d)", all_units[idx].str, all_units[idx].exp, all_units[idx].entrench);

					}
					if (show_unit_layer_type != 1 && map[map_mouse_x][map_mouse_y].guidx >= 0) //ground unit visible and present
					{
						idx = map[map_mouse_x][map_mouse_y].guidx;
						if (all_units[idx].uflag == axis_core || all_units[idx].uflag == allied_core)
							//sprintf(line3,"+%s", equip[all_units[idx].unum]);
							sprintf(line3, "+%s", equip_name_utf8[all_units[idx].unum]);
						else
							//sprintf(line3,"%s", equip[all_units[idx].unum]);
							sprintf(line3, "%s", equip_name_utf8[all_units[idx].unum]);
						if (all_units[idx].orgtnum > 0)
							//sprintf(line4,"/%s\n",equip[all_units[idx].orgtnum]);
							sprintf(line4, "/%s\n", equip_name_utf8[all_units[idx].orgtnum]);
						else
							sprintf(line4, "\n");
						strncat(line3, line4,64);
						sprintf(line4, "(str %d,exp %d,ent %d)", all_units[idx].str, all_units[idx].exp, all_units[idx].entrench);
					}
				}
				MapInfoTxt[0] = 0;
				strncat(MapInfoTxt, line1,64);
				strncat(MapInfoTxt, line2,64);
				strncat(MapInfoTxt, line3,64);
				strncat(MapInfoTxt, line4,64);
			} else {
				MapInfoTxt[0] = 0;
			}
			main_dlg[dmMapInfoTxtIdx].flags |= D_DIRTY;
			//d_mapinfo_proc(MSG_DRAW,&(main_dlg[dmMapInfoTxtIdx]),0);
		}
	}
	//if (msg==MSG_IDLE) rest(100);

	return d_bitmap_proc(msg, d, c);
}

int d_GndAir_proc(int msg, DIALOG *d, int c) {
	//int i;
	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) //calls for each button
	{
		if (&(main_dlg[dmGndRBIdx]) == d)
			show_unit_layer_type = 0;
		if (&(main_dlg[dmAirRBIdx]) == d)
			show_unit_layer_type = 1;
		if (&(main_dlg[dmSeaRBIdx]) == d)
			show_unit_layer_type = 2;
		if (&(main_dlg[dmNoneRBIdx]) == d)
			show_unit_layer_type = 3;

		//printf("show_unit_layer_type=%d\n",show_unit_layer_type);

		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//d_bitmap_proc(MSG_DRAW,&main_dlg[dmMapBmpIdx],c);
		//if (msg==MSG_KEY) return D_USED_CHAR|D_REDRAW;
	}
	return d_radio_proc(msg, d, c);
}

int d_hslide_proc(int msg, DIALOG *d, int c) {
	int x0t, y0t, exit_used_char = 0;
	BYTE scancode;

	if ((msg != MSG_CLICK) && (msg != MSG_CHAR))
		return d_slider_proc(msg, d, c);

	//printf("H slide click or char\n");
	//process the click on the horizontal slider
	x0t = map_x0;
	y0t = map_y0;
	d_slider_proc(msg, d, c);
	map_x0 = d->d2;

	scancode = (c & 0xff00) >> 8;
	if (msg == MSG_CHAR) {
		if (scancode == KEY_UP) {
			if (map_y0 > 0)
				--map_y0;
			exit_used_char = 1;
		}
		if (scancode == KEY_DOWN) {
			if (get_v_slide_max() > map_y0)
				map_y0++;
			exit_used_char = 1;
		}
		//update slider
		if (exit_used_char) {
			main_dlg[dmVSlideIdx].d2 = vslide_max - map_y0;
			main_dlg[dmVSlideIdx].flags |= D_DIRTY;
			//d_slider_proc(MSG_DRAW, &main_dlg[dmVSlideIdx], 0);
		}

	}
	if ((x0t != map_x0) || (y0t != map_y0)) {
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//d_bitmap_proc(MSG_DRAW,&main_dlg[dmMapBmpIdx],c);
	}
	if ((scancode == KEY_RIGHT) || (scancode == KEY_LEFT))
		exit_used_char = 1;
	return exit_used_char ? D_USED_CHAR : D_O_K;

}

int d_vslide_proc(int msg, DIALOG *d, int c) {
	int x0t, y0t, exit_used_char = 0;
	BYTE scancode;

	if ((msg != MSG_CLICK) && (msg != MSG_CHAR))
		return d_slider_proc(msg, d, c);
	//process the click on the veritical slider
	x0t = map_x0;
	y0t = map_y0;
	d_slider_proc(msg, d, c);
	map_y0 = d->d1 - d->d2; //high d2 is at top !!!

	scancode = (c & 0xff00) >> 8;
	if (msg == MSG_CHAR) {
		if (scancode == KEY_LEFT) {
			if (map_x0 > 0)
				map_x0--;
			if (map_x0 > 0)
				map_x0--;
			exit_used_char = 1;
		}
		if (scancode == KEY_RIGHT) {
			if (get_h_slide_max() > map_x0)
				map_x0++;
			if (get_h_slide_max() > map_x0)
				map_x0++;
			exit_used_char = 1;
		}
		//update slider
		if (exit_used_char) {
			main_dlg[dmHSlideIdx].d2 = map_x0;
			//d_slider_proc(MSG_DRAW, &main_dlg[dmHSlideIdx], 0);
			main_dlg[dmHSlideIdx].flags |= D_DIRTY;
		}

	}
	if ((x0t != map_x0) || (y0t != map_y0)) {
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		//d_bitmap_proc(MSG_DRAW,&main_dlg[dmMapBmpIdx],c);
	}
	if ((scancode == KEY_UP) || (scancode == KEY_DOWN))
		exit_used_char = 1;
	return exit_used_char ? D_USED_CHAR : D_O_K;
}

int count_axis_core() {
	int i, count = 0;

	for (i = 0; i < total_units; ++i)
		if (all_units[i].uflag == axis_core)
			++count;
	return count;
}

void get_victory_hexes() {
	int i, count, x, y;
	//empty the victory hex array
	for (i = 0; i < MAX_VICTORY_HEXES; ++i) {
		victory_hexes[i].x = -1;
		victory_hexes[i].y = -1;
		victory_hexes[i].own = 0;
	}
	//get the victory hexes from the map
	//and put them in the array
	count = 0;
	for (x = 0; x < mapx; ++x)
		for (y = 0; y < mapy; ++y) {
			if (map[x][y].vic && count < MAX_VICTORY_HEXES) {
				victory_hexes[count].x = x;
				victory_hexes[count].y = y;
				victory_hexes[count].own = map[x][y].own;
				++count;
			}
		}
}

int count_axis_aux() {
	int i, count = 0;

	for (i = 0; i < total_units; ++i)
		if (all_units[i].uflag == axis_aux)
			++count;
	return count;
}

int count_allied_core() {
	int i, count = 0;

	for (i = 0; i < total_units; ++i)
		if (all_units[i].uflag == allied_core)
			++count;
	return count;
}

int count_allied_aux() {
	int i, count = 0;

	for (i = 0; i < total_units; ++i)
		if (all_units[i].uflag == allied_aux)
			++count;
	return count;
}

void setup_show_filters_info() {
	char stat[40] = "";

	switch (drawNames) {
	case 0:
		strncat(stat, ".", 40);
		break;
	case 1:
		strncat(stat, "N", 40);
		break;
	case 2:
		strncat(stat, "n", 40);
		break;
	case 3:
		strncat(stat, "r", 40);
		break;
	}
	if (drawAllNames)
		strncat(stat, "A", 40);
	else
		strncat(stat, ".", 40);
	if (drawTerrain)
		strncat(stat, "T", 40);
	else
		strncat(stat, ".", 40);
	if (showFilter)
		strncat(stat, "F", 40);
	else
		strncat(stat, ".", 40);
	switch (showDecimal) {
	case 0:
		strncat(stat, "d", 40);
		break;
	case 1:
		strncat(stat, "h", 40);
		break;
	case 2:
		strncat(stat, "n", 40);
		break;
	default:
		strncat(stat, ".", 40);
		break;
	}
	if (drawRoads)
		strncat(stat, "R", 40);
	else
		strncat(stat, ".", 40);
	switch (showCounter) {
	case 0:
		strncat(stat, "S", 40);
		break;
	case 1:
		strncat(stat, "X", 40);
		break;
	case 2:
		strncat(stat, "E", 40);
		break;
	default:
		strncat(stat, ".", 40);
		break;
	}
	if (drawGndTransport)
		strncat(stat, "t", 40);
	else
		strncat(stat, ".", 40);
	//strncat(stat,"\n", 40);

	switch (showWeather) {
	case 0:
		strncat(stat, ".", 40);
		break;
	case 1:
		strncat(stat, "m", 40);
		break;
	case 2:
		strncat(stat, "w", 40);
		break;
	default:
		strncat(stat, ".", 40);
		break;
	}
	if (showHex)
		strncat(stat, "H", 40);
	else
		strncat(stat, ".", 40);
	switch (graphical_overide) {
	case 0:
		strncat(stat, "..", 40);
		break;
	case 1:
		strncat(stat, "z.", 40);
		break;
	case 2:
		strncat(stat, ".#", 40);
		break;
	case 3:
		strncat(stat, "z#", 40);
		break;
	default:
		strncat(stat, "z#", 40);
		break;
	}
	switch (graphical_overide_hex) {
	case 0:
		strncat(stat, "0", 40);
		break;
	case 1:
		strncat(stat, "1", 40);
		break;
	case 2:
		strncat(stat, "2", 40);
		break;
	case 3:
		strncat(stat, "3", 40);
		break;
	case 4:
		strncat(stat, "4", 40);
		break;
	case 5:
		strncat(stat, "5", 40);
		break;
	case 6:
		strncat(stat, "6", 40);
		break;
	case 7:
		strncat(stat, "7", 40);
		break;
	default:
		strncat(stat, "7", 40);
		break;
	}
	if (scenarioUnitsMode)
		strncat(stat, "s", 40);
	else
		strncat(stat, ".", 40);
	if (displayAllUnits)
		strncat(stat, "+", 40);
	else
		strncat(stat, ".", 40);

	switch (displayUnitsOrder) {
	case 0:
		strncat(stat, ".", 40);
		break;
	case 1:
		strncat(stat, "o", 40);
		break;
	case 2:
		strncat(stat, "O", 40);
		break;
	default:
		strncat(stat, ".", 40);
		break;
	}

	strncpy(FilterStatusTxt, stat,256);
	main_dlg[dmFilterStatusTxtIdx].flags |= D_DIRTY;
	//d_filterstatus_proc(MSG_DRAW,&(main_dlg[dmFilterStatusTxtIdx]),0);
}

void setup_terrain_info() {
	strncpy(MapStatusTxt, "Terrain Edit: ",256);
	strcat(MapStatusTxt, "Type=");
	strncat(MapStatusTxt, tdTTStr,8);
	strcat(MapStatusTxt, "\nRoad=");
	strncat(MapStatusTxt, tdRDStr,8);
	strcat(MapStatusTxt, "  Tile#=");
	strncat(MapStatusTxt, tdTNStr,8);
	strcat(MapStatusTxt, "\nName=");
	strncat(MapStatusTxt, tdGLNStr,8);
	strcat(MapStatusTxt, "  Side=");
	strncat(MapStatusTxt, tdSDStr,8);
	strcat(MapStatusTxt, "\nLeft-change Right-pick");
}

//load scenario dialog
int load_scenario_dialog() {
	int temp_scen_number, temp_map_number, error;
	char scenario_number_str[2 * 8] = "";
	char error_str[256] = "";

	load_dlg[4].dp = &scenario_number_str;
	sprintf(scenario_number_str, "%04d", getScenarioNumber());
	centre_dialog(load_dlg);
	if (do_dialog(load_dlg, 4) == 3) {
		map_x0 = 0;
		map_y0 = 0;

		if (tile_mode == 1) {
			tile_cleanup();
		}

		//save old values
		temp_scen_number = getScenarioNumber();
		temp_map_number = map_number;

		setScenarioNumber(atoi(scenario_number_str));
		//map_number=scn_number; //not needed, but true for original PG

		if (getScenarioNumber() > 0) {
			//clear the map
			error = load_scenario(getScenarioNumber(), DO_NOT_SHOW_LOGS);
			if (error) {
				strncpy(error_str, load_error_str,256);
				alert(error_str, NULL, NULL, "&Continue", NULL, 'c', 0);
			}
		} else {
			sprintf(error_str, "Wrong number : %d", getScenarioNumber());
			alert(error_str, NULL, NULL, "&Continue", NULL, 'c', 0);

			setScenarioNumber(temp_scen_number);
			map_number = temp_map_number;
			return 0;
		}
		//on error reload last scenario
		//success fully loaded
		if (error) {
			setScenarioNumber(temp_scen_number);
			map_number = temp_map_number;

			load_scenario(getScenarioNumber(), DO_NOT_SHOW_LOGS);
			/*
			 read_map(map_number);
			 load_scn(scn_number,DO_NOT_SHOW_LOGS,LOAD_FILE);
			 */
			return 2;
		}
		return 1;
	}
	return 0;
}

int d_btn_proc(int msg, DIALOG *d, int c) {
	int msg_ret;
	//static int idle_flag;

	//enum EDIT_FUNCTION last_edit_op;

	//we will intercept the click message. whenever a button
	//is pressed, we will unselect all the buttons and
	//do cleanup as needed. Then we will do the action for
	//the button pressed

	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) {
		//last_edit_op = edit_op;
		if (tile_mode == 1) {
			if (&(main_dlg[dmHelpBtnIdx]) == d) {
				return help_dialog(); //never gets selected
			} else {
				tile_cleanup();
				//restore selected since tile_cleanup removes selected
				main_dlg[dmTileBtnIdx].flags |= D_SELECTED;

				draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			}
		} else {
			edit_op = edit_none;
			//idle_flag = 0;
		}

		//
		// EXIT from buttons procedure = XXX&D_SELECTED
		//

		// deploy button unpressed = this is exit from deploy
		if (main_dlg[dmDeployBtnIdx].flags & D_SELECTED) {
			//cleanup the deploy button ...
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
			//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
			//reset the button
			main_dlg[dmDeployBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmDeployBtnIdx]), 0);
		}
		// neutral button
		if (main_dlg[dmNeutralBtnIdx].flags & D_SELECTED) {
			//alert("Neutral Out", NULL, NULL,"&Yes", "&No", 'y', 'n');
			//cleanup the neutral button ...
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
			//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
			//reset the button
			main_dlg[dmNeutralBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmNeutralBtnIdx]), 0);
		}
		//Victory Button
		if (main_dlg[dmVictBtnIdx].flags & D_SELECTED) {
			//cleanup the victory button ...
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
			//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
			//reset the button
			main_dlg[dmVictBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmVictBtnIdx]), 0);
		}
		//Owner
		if (main_dlg[dmOwnBtnIdx].flags & D_SELECTED) {
			//cleanup the Own button ...

			//reset the button
			main_dlg[dmOwnBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmOwnBtnIdx]), 0);
		}
		//see below
		//Entrench
		if (main_dlg[dmEntBtnIdx].flags & D_SELECTED) {
			//cleanup the Entrench button ...

			//reset the button
			main_dlg[dmEntBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmEntBtnIdx]), 0);
		}
		// Strength
		if (main_dlg[dmStrBtnIdx].flags & D_SELECTED) {
			//cleanup the Strength button ...

			//reset the button
			main_dlg[dmStrBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmStrBtnIdx]), 0);
		}
		// Experience
		if (main_dlg[dmExpBtnIdx].flags & D_SELECTED) {
			//cleanup the Experience button ...

			//reset the button
			main_dlg[dmExpBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmExpBtnIdx]), 0);
		}
		// Place unit
		if (main_dlg[dmPlaceBtnIdx].flags & D_SELECTED) {
			//cleanup the Place button ...

			//reset the button
			main_dlg[dmPlaceBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmPlaceBtnIdx]), 0);
		}
		//Unit edit
		if (main_dlg[dmUnitBtnIdx].flags & D_SELECTED) {
			//cleanup the Unit button ...

			//reset the button
			main_dlg[dmUnitBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmUnitBtnIdx]), 0);
		}
		//Scen edit
		if (main_dlg[dmScenBtnIdx].flags & D_SELECTED) {
			//cleanup the Scen button ...

			//reset the button
			main_dlg[dmScenBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmScenBtnIdx]), 0);
		}
		//Load
		if (main_dlg[dmLoadBtnIdx].flags & D_SELECTED) {
			//cleanup the Load button ...

			//reset the button
			main_dlg[dmLoadBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmLoadBtnIdx]), 0);
		}
		//Save
		if (main_dlg[dmSaveBtnIdx].flags & D_SELECTED) {
			//cleanup the Save button ...

			//reset the button
			main_dlg[dmSaveBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmSaveBtnIdx]), 0);
		}
		//Help
		if (main_dlg[dmHelpBtnIdx].flags & D_SELECTED) {
			//cleanup the Help button ...

			//reset the button
			main_dlg[dmHelpBtnIdx].flags &= ~D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmHelpBtnIdx]), 0);
		}
		if (main_dlg[dmSearchRBtnIdx].flags & D_SELECTED) {
			//cleanup the Search and Replace button ...

			//reset the button
			main_dlg[dmSearchRBtnIdx].flags &= ~D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmSearchRBtnIdx]), 0);
		}
		if (main_dlg[dmRectImportBtnIdx].flags & D_SELECTED) {
			//cleanup the Rectangle Import button ...

			//reset the button
			main_dlg[dmRectImportBtnIdx].flags &= ~D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmRectImportBtnIdx]), 0);
		}
		if (main_dlg[dmRectExportBtnIdx].flags & D_SELECTED) {
			//cleanup the Rectangle Export button ...
			rexp_left_mouse_click = 0;
			//reset the button
			main_dlg[dmRectExportBtnIdx].flags &= ~D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmRectExportBtnIdx]), 0);
		}
		//Exit
		if (main_dlg[dmExitBtnIdx].flags & D_SELECTED) {
			//cleanup the Exit button ...

			//reset the button
			main_dlg[dmExitBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmExitBtnIdx]), 0);
		}
		//------------ end of unselect -------------------------
		//-------- now do the setup for each button ------------
		// neutral button
		if (d == &(main_dlg[dmNeutralBtnIdx])) {
			neutral_mode();
		}

		// deploy button
		if (d == &(main_dlg[dmDeployBtnIdx])) {
			deploy_mode();
		}

		// victory button
		if (d == &(main_dlg[dmVictBtnIdx])) {
			victory_mode();
		}
		// owner button
		if (d == &(main_dlg[dmOwnBtnIdx])) {
			owner_mode();
		}

		//Terrain
		if (main_dlg[dmTerBtnIdx].flags & D_SELECTED) {
			//reset the button
			main_dlg[dmTerBtnIdx].flags &= ~D_SELECTED;
			msg_ret = d_button_proc(MSG_DRAW, &(main_dlg[dmTerBtnIdx]), 0);
			//cleanup the Terrain button ...
			/*if (last_edit_op==ter) {
			 //special case
			 d=&dm[dmTileBtnIdx];
			 d_button_proc(MSG_DRAW,&(dm[dmTerBtnIdx]),0);
			 }
			 else*/
			//exit on click only and when user wants ter mode

			if (msg == MSG_CLICK && d == &(main_dlg[dmTerBtnIdx])) {
				pick_msg();
				return msg_ret;
			}
		}
		// Terrain
		if (d == &(main_dlg[dmTerBtnIdx])) {
			centre_dialog(terrain_dlg);
			if (do_dialog(terrain_dlg, -1) != -1) {
				edit_op = edit_ter;
				setup_terrain_info();
				main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
				//d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
				// force button draw...default doesn't seem to select
				if (msg == MSG_CLICK)
					main_dlg[dmTerBtnIdx].flags ^= D_SELECTED;
			} else {
				if (msg != MSG_CLICK)
					main_dlg[dmTerBtnIdx].flags ^= D_SELECTED;
				pick_msg();
			}
			//d_mapbmp_proc(MSG_DRAW, &(main_dlg[dmMapBmpIdx]), 0);
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
		}

		//Tile matrix
		if (main_dlg[dmTileBtnIdx].flags & D_SELECTED) {
			//cleanup the Tiles button ...
			//restore things

			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			//pick_msg();
			//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
			// sprintf(MapStatusTxt,"Pick an Operation",0);
			// d_mapstatus_proc(MSG_DRAW,&(dm[dmMapStatusIdx]),0);
			//reset the button
			main_dlg[dmTileBtnIdx].flags ^= D_SELECTED;
			pick_msg();
			return d_button_proc(MSG_DRAW, &(main_dlg[dmTileBtnIdx]), 0);
		}
		if (d == &(main_dlg[dmTileBtnIdx])) {
			//we will temporarily display the complete
			//set of tile in the upper left corner...
			//a click will restore the map and transfer
			//the bmp # to
			//if (mapx>15+4&&mapy>15) //too small ?
			//{
			tile_make_matrix();
			//set new values
			map_x0 = 0;
			map_y0 = 0;
			mapx = matrix_x;
			mapy = matrix_y;
			position_gui_elements(w_old, h_old);
			//mapy=STD_MATRIX_MAX_Y+1;
			//dm[dmVSlideIdx].flags |= D_DISABLED | D_HIDDEN;
			//dm[dmHSlideIdx].flags |= D_DISABLED | D_HIDDEN;
			/*
			 vslide_max=get_v_slide_max();
			 dm[dmHSlideIdx].d1=get_h_slide_max();
			 dm[dmHSlideIdx].d2=map_x0;
			 dm[dmVSlideIdx].d1=vslide_max;
			 dm[dmVSlideIdx].d2=vslide_max-map_y0;
			 d_hslide_proc(MSG_DRAW,&(dm[dmHSlideIdx]),0);
			 d_vslide_proc(MSG_DRAW,&(dm[dmVSlideIdx]),0);
			 */
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
			//d_mapbmp_proc(MSG_DRAW,&(main_dlg[dmMapBmpIdx]),0);
			//Click the one you want.\n
			sprintf(MapStatusTxt, "F1 coast     F2 city\nF3 mountains F4 clear\nF5 desert    F6 road\nF7 river     F8 swamp\nF9 forest    F10 fort");
			main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
			//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

			//}
		}
		//entrench
		if (&(main_dlg[dmEntBtnIdx]) == d) //entrench
		{
			entrench_mode();
		}
		//strength
		if (&(main_dlg[dmStrBtnIdx]) == d) //strength
		{
			strength_mode();
		}
		//experience
		if (&(main_dlg[dmExpBtnIdx]) == d) //experience
		{
			expirience_mode();
		}
		//place unit
		if (&(main_dlg[dmPlaceBtnIdx]) == d) //place unit
		{
			units_place_mode();
		}
		//unit dialog
		if (&(main_dlg[dmUnitBtnIdx]) == d) //unit dialog
		{
			return unit_dialog();
		}
		//unit list dialog
		if (&(main_dlg[dmUlistBtnIdx]) == d) //unit dialog
		{
			return unit_list_dialog();
		}
		//scenario dialog
		if (&(main_dlg[dmScenBtnIdx]) == d) //scenario dialog
		{
			return do_scenario_dialog();
		}
		//Load
		if (&(main_dlg[dmLoadBtnIdx]) == d) {
			return load_dialog();
		}
		//Save
		if (&(main_dlg[dmSaveBtnIdx]) == d) {
			return save_dialog();
		}
		// Help
		if (&(main_dlg[dmHelpBtnIdx]) == d) {
			return help_dialog(); //never gets selected
		}
		if (&(main_dlg[dmSearchRBtnIdx]) == d) {
			search_replace_dialog();
		}
		if (&(main_dlg[dmRectImportBtnIdx]) == d) {
			do_rectangle_import();
		}
		if (&(main_dlg[dmRectExportBtnIdx]) == d) {
			do_rectangle_export();
		}
	}//end of process MSG_CLICK and MSG_KEY
	//if (msg==MSG_KEY) return D_USED_CHAR|d_button_proc(msg,d,c);
	msg_ret = d_button_proc(msg, d, c);
	// d_mapstatus_proc(MSG_DRAW,&(dm[dmMapBmpIdx]),0);
	return msg_ret;
}

int d_arrow_proc(int msg, DIALOG *d, int c) {
	int tx, ty;

	// if (msg!=MSG_CLICK) return d_bitmap_proc(msg,d,c);
	if (msg == MSG_CLICK) {
		tx = map_x0;
		ty = map_y0;

		switch (d->d1) {
		case 'U':
			if (map_y0 > 0)
				--map_y0;
			break;
		case 'D':
			if (get_v_slide_max() > map_y0)
				map_y0++;
			break;
		case 'L':
			if (map_x0 > 0)
				map_x0--;
			if (map_x0 > 0)
				map_x0--;
			break;
		case 'R':
			if (get_h_slide_max() > map_x0)
				map_x0++;
			if (get_h_slide_max() > map_x0)
				map_x0++;
			break;
		}
		if (tx != map_x0 || ty != map_y0) //avoid redraw if nothing changed
		{
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			main_dlg[dmHSlideIdx].d2 = map_x0;
			main_dlg[dmVSlideIdx].d2 = vslide_max - map_y0;

			//fix for a very long drawing of dotted rectangle, do not redraw here
			main_dlg[dmHSlideIdx].flags |= D_DIRTY;
			main_dlg[dmVSlideIdx].flags |= D_DIRTY;
			main_dlg[dmMapBmpIdx].flags |= D_DIRTY;

			//printf("H slide draw start\n");
			//d_hslide_proc(MSG_DRAW, &main_dlg[dmHSlideIdx], c);//0
			//printf("V slide draw start\n");
			//d_vslide_proc(MSG_DRAW, &main_dlg[dmVSlideIdx], c);//0
			//printf("map draw start\n");
			//d_bitmap_proc(MSG_DRAW, &main_dlg[dmMapBmpIdx], c);

			//printf("end\n");

		}
	}
	return d_bitmap_proc(msg, d, c);
}

int str_bmp_offset(int idx) {
	if (ag_mode) {
		//is allied_aux ok ?
		return ((all_units[idx].uflag == axis_aux) * (3 - scenarioUnitsMode) + (all_units[idx].uflag == axis_core) * 2 + (all_units[idx].uflag == allied_core) + (all_units[idx].uflag == allied_aux)
				* 0) * MAX_STRENGTH_IN_ROW;
	}
	if (pgf_mode || pacgen_mode) {
		return ((all_units[idx].uflag == axis_aux) * (1 - scenarioUnitsMode) + (all_units[idx].uflag == axis_core) * 0 + (all_units[idx].uflag == allied_core) * 2 + (all_units[idx].uflag
				== allied_aux) * (3 - scenarioUnitsMode)) * MAX_STRENGTH_IN_ROW;
	}
	return ((all_units[idx].uflag == axis_aux) * (1 - scenarioUnitsMode) + (all_units[idx].uflag == axis_core) * 1 + (all_units[idx].uflag == allied_core) * 2 +
	//will not work
			(all_units[idx].uflag == allied_aux) * (3 - scenarioUnitsMode)) * MAX_STRENGTH_IN_ROW;

}

int count_deploy_hexes() {
	int x, y, count = 0;

	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].deploy)
				++count;

	return count;
}

int count_deploy_hexes_on_sea() {
	int x, y, count = 0;

	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].deploy && map[x][y].utr>=4 && map[x][y].utr<=7 )
				++count;

	return count;
}

int is_core_unit(int idx){
	return (all_units[idx].uflag == axis_core || all_units[idx].uflag == allied_core);
}

int is_aux_unit(int idx){
	return (all_units[idx].uflag == axis_aux || all_units[idx].uflag == allied_aux);
}

int count_deploy_hexes_occupied() {
	int x, y, count = 0;

	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].deploy && ( (map[x][y].guidx>=0 && is_aux_unit(map[x][y].guidx)) || (map[x][y].auidx>=0 && is_aux_unit(map[x][y].auidx))) )
				++count;

	return count;
}

int count_deploy_hexes_occupied_on_sea() {
	int x, y, count = 0;

	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].deploy && map[x][y].utr>=4 && map[x][y].utr<=7 && ( (map[x][y].guidx>=0 && is_aux_unit(map[x][y].guidx)) || (map[x][y].auidx>=0 && is_aux_unit(map[x][y].auidx))) )
				++count;

	return count;
}

int count_vic_hexes() {
	int x, y, count = 0;

	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].vic)
				++count;

	return count;
}

void my_log(char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

void exit_with_key(int error) {

	clear_keybuf();
	my_log("Press any key to exit.\n");
	readkey();
	allegro_exit();
	exit(error);
}

//void prepare_tiles_filters() {
	//int i, j;

	/*
	for (j = 0; j < MAX_TILES_IN_PG; j++)
		FilterTiles[j] = 0;

	for (i = 0; i < 12; i++)
		for (j = 0; j < MAX_TILES_IN_PG; j++)
			if (j >= tiles_filter[i][0] && j <= tiles_filter[i][1])
				FilterTiles[tiles_display[j]] += (1 << i);

	FilterTiles[151] |= (1 << 5);
	FilterTiles[152] |= (1 << 5);
	FilterTiles[153] |= (1 << 5);
	FilterTiles[154] |= (1 << 5);
	FilterTiles[155] |= (1 << 5);
	FilterTiles[156] |= (1 << 5);
	FilterTiles[161] |= (1 << 5);
	FilterTiles[163] |= (1 << 5);
	FilterTiles[164] |= (1 << 5);
	FilterTiles[166] |= (1 << 5);
	FilterTiles[171] |= (1 << 5);
	FilterTiles[172] |= (1 << 5);
	FilterTiles[173] |= (1 << 5);
	FilterTiles[174] |= (1 << 5);
	FilterTiles[175] |= (1 << 5);
	FilterTiles[176] |= (1 << 5);

	FilterTiles[162] |= (1 << 6);
	FilterTiles[165] |= (1 << 6);

	FilterTiles[128] &= ~(1 << 4);
	FilterTiles[129] &= ~(1 << 4);
	FilterTiles[130] &= ~(1 << 4);
	FilterTiles[139] &= ~(1 << 4);
	FilterTiles[148] &= ~(1 << 4);
	FilterTiles[149] &= ~(1 << 4);
	FilterTiles[150] &= ~(1 << 4);
*/
	/*
	 for(j=0;j<MAX_TILES_IN_PG;j++){
	 printf("0x%04x, ", FilterTiles[j]);
	 if (j%8==7) printf("// 0x%02x\n",j+1);
	 }
	 printf("\n");
*/
//}

int main(int argc, char *argv[]) {
	int error, i, really_exit = 1;
	char tmp_str[128];
	int max_scn, min_scn;
	unsigned short is_app6_bmp[MAX_UICONS];
	BITMAP *mousie;

	memset(is_app6_bmp, 0, sizeof(is_app6_bmp));
	memset(flag_bmp, 0, sizeof(flag_bmp));
	for(i=25;i<MAX_COUNTRY;i++)
		country_active[i]=0;

	strncpy(MapStatusTxt, AboutTxt1,256);
	strncat(MapStatusTxt, "\n",256);
	strncat(MapStatusTxt, AboutTxt2,256);
	strncat(MapStatusTxt, "\n",256);
	strncat(MapStatusTxt, AboutTxt3,256);
	strncat(MapStatusTxt, "\n",256);

	my_log("%s%s", AboutTxt1, "\n");
	my_log("%s%s", AboutTxt2, "\n");
	my_log("%s%s", AboutTxt3, "\n");
	my_log("Compiled with GCC %s\n", __VERSION__);
	my_log("Compiled with Allegro %s\n", ALLEGRO_VERSION_STR);
	//my_log("Compiled with Allegro %d.%d.%d\n",ALLEGRO_VERSION,ALLEGRO_SUB_VERSION,ALLEGRO_WIP_VERSION);
	//my_log("Compile time : %s %s%s",__DATE__,__TIME__,"\n");
	my_log("\n");

	/* you should always do this at the start of Allegro programs */
	my_log("Allegro library init...\n");
	allegro_init();

	loadpng_init();
	/* set up the keyboard handler */
	install_keyboard();

	if (argc > 1 && strcmp(argv[1], "-h") == 0) {
		CLIHelp(MapStatusTxt);
		exit_with_key(0);
	}
	fpge_colors_bits = 32; //new default for 0.6.0beta2
	if (argc > 1 && strcmp(argv[argc - 1], "-G") == 0) {
		fpge_colors_bits = 32;
		argc--;
	}
	if (argc > 1 && strcmp(argv[argc - 1], "-P") == 0) {
		fpge_colors_bits = 8;
		argc--;
	}
	if (argc > 1 && (argv[1][1] == 'B' || argv[1][1] == 'M' || argv[1][1] == 'D')) {
		fpge_colors_bits_shift = 1;
		for (i = 0; i < strlen(argv[1]); i++)
			if (argv[1][i] == '4')
				fpge_colors_bits_shift = 2; //PNG
		//my_log("fpge_colors_bits_shift=%d\n",fpge_colors_bits_shift);
	}
	if (argc > 1 && argv[1][1] == 'Q') {
		draw_app6 = 1;
		draw_app6_tiles = 1;
		for (i = 0; i < strlen(argv[1]); i++) {
			if (argv[1][i] == 'n')
				draw_app6_tiles = 0;
			if (argv[1][i] == 'c')
				draw_app6_tiles = 2;
			if (argv[1][i] == 'u')
				draw_app6_units = 0;
			if (argv[1][i] == 'g')
				draw_app6_color = 1;
		}

	}
	set_color_mode();

	make_hexes();
	my_log("Starting FPGE...\n");
	for (i = 4; i < TILES_HOT_KEY_NUMBER; i++) {
		EditorTile[i].set = 0;
	}
	//--------------------------------------------------
	//0str.bmp load
	my_log("Loading '%s'...\n",fpge_0str_bmp);
	error = load_0str_bmp();
	if (error == 0) {
		my_log("File '%s' loaded.\n",fpge_0str_bmp);
	} else {
		my_log("File '%s' not found...\n",fpge_0str_bmp);
	}
	//--------------------------------------------------
	//AG load
	my_log("Loading '%s'...\n",ag_tileart);
	error = load_tileart();
	if (error == 0) {
		my_log("'%s' file loaded.\nStarting Allied General mode.\n",ag_tileart);
		//all ok AG graphic loaded, skip other loading
		ag_mode = 1;
		//allegro_exit();
		//return 0;
	} else {
		my_log("Not found...\n");
	}
	if (!ag_mode) {
		error = 0;
		my_log("Loading '%s'...\n",pac_ext);
		error += load_ext_idx();
		if (error == 0) {
			pacgen_mode = 1;
			my_log("Loading '%s', '%s', '%s', '%s'\n",pac_pfpdata,pac_til,pac_shp,pac_pal);
			error += load_pfpdata_idx();
			if (error == 0) {
				my_log("Loading '%s', '%s'\n",pac_pacequip,pac_pacequip_txt);
				error += load_pacgen_equip();
			}
		}
		if (error == 0) {
			nupl_present = 1;
			my_log("All loaded.\nStarting PacGen mode.\n");
		} else {
			if (pacgen_mode){
				my_log("Loading of PacGen data failed. Aborting.\n");
				exit_with_key(error);
			}else{
				my_log("Not found...\n");
			}
		}
	}

	if (!ag_mode && !pacgen_mode) {
		//--------------------------------------------------
		my_log("Loading tiles...\n");
		//do not try to load when 8bit mode

		if (fpge_colors_bits > 8) {
			error = load_bmp_tacmap();
			if (error) {
				pgf_mode = 0;
				nupl_present = 0;
			} else {
				pgf_mode = 1;
				nupl_present = 1;
			}
			//if (error) {
			//	printf("ERROR ID : %d\n",error);
			//}
		} else
			error = 0;

		if (error != 0) {
			error = load_tiles(SHOW_LOGS);
			if (error) {
				printf("ERROR ID : %d\n", error);
				exit_with_key(error);
			}
			my_log("PG 'tacmap.shp' loaded.\n");
		} else {
			my_log("PGF 'tacmap_dry.bmp', 'tacmap_muddy.bmp' and 'tacmap_frozen.bmp' loaded.\n");
		}
		//--------------------------------------------------
		my_log("Loading unit icons...\n");

		if (fpge_colors_bits > 8)
			error = load_bmp_tacticons();
		else
			error = 0;

		if (error != 0) {
			error = load_uicons();
			if (error) {
				exit_with_key(error);
			}
			my_log("PG 'tacicons.shp' loaded.\n");
		} else {
			my_log("PGF 'tacicons.bmp' loaded.\n");
		}

		//--------------------------------------------------

		my_log("Loading stack unit icons...\n");
		if (fpge_colors_bits > 8)
			error = load_bmp_stackicn();
		else
			error = 0;

		if (error != 0) {
			error = load_sicons();

			if (error) {
				exit_with_key(error);
			}
			my_log("PG 'stackicn.shp' loaded.\n");
		} else {
			my_log("PGF 'stackicn.bmp' loaded.\n");
		}

		//--------------------------------------------------

		my_log("Loading flags...\n");
		if (fpge_colors_bits > 8)
			error = load_bmp_flags();
		else
			error = 0;

		if (error != 0) {
			error = load_flags();
			if (error) {
				exit_with_key(error);
			}
			my_log("PG 'flags.shp' loaded.\n");
		} else {
			my_log("PGF 'flags.bmp' loaded.\n");
		}
		//--------------------------------------------------

		my_log("Loading strength icons...\n");
		if (fpge_colors_bits > 8)
			error = load_bmp_strength();
		else
			error = 0;

		if (error != 0) {
			error = load_strength();
			if (error) {
				exit_with_key(error);
			}
			my_log("PG 'strength.shp' loaded.\n");

		} else {
			my_log("PGF 'strength.bmp' loaded.\n");
		}

	}//ag mode
	// end of graphics load, adjust flags
	total_countries = total_flags / 2;
	// all flags are in flag_bmp while should be splited in half
	for (i = 0; i < total_countries; i++) {
		victory_flag_bmp[i] = flag_bmp[i + total_countries];
		flag_bmp[i + total_countries] = NULL;
	}

	//-------------------------------------------------
	// why equipment is read twice ??
	// this is needed for NUPL !!!

	if (!pacgen_mode && !pgf_mode) {
		//PG and AG modes
		my_log("Loading equipment...\n");
		if (!ag_mode) pg_mode = 1;
		error = load_equip(LOAD_FILE, equip_file);
		if (error) {
			my_log("ERROR: %s equipment file '%s' not loaded!\n",(ag_mode?"AG":"PG"),equip_file);
			exit_with_key(error);
		}
		my_log("%s equipment file loaded.\n",(ag_mode?"AG":"PG"));
	}

	if (pgf_mode){
		my_log("Loading equipment...\n");
		error = load_pgf_equipment(LOAD_FILE, pgf_equip_file);
		if (error) {
			my_log("ERROR: PGF equipment file '%s' not loaded!\n",pgf_equip_file);
			exit_with_key(error);
		}else {
			my_log("PGF equipment file loaded.\n");
		}
	}

	//--------------------------------------------------
	// in PGF names are in scenario folder
	if (!pacgen_mode) {
		my_log("Loading names...\n");
		error = load_names(SHOW_LOGS);
		if (error) {
			exit_with_key(error);
		}
	}else{
			total_names=0;
		}
		//--------------------------------------------------
		//this is not needed for PGF

		if (!pgf_mode) {
			my_log("Loading scenario description...\n");
			if (ag_mode) {
				error = load_ag_scenario_desc();
				if (error) {
					//ignore file not found
					sprintf(tmp_str, "AG scenario description not loaded.\n");
					my_log(tmp_str);
					//exit_with_key(error);
				} else {
					sprintf(tmp_str, "AG scenario description loaded.\n");
					my_log(tmp_str);
				}
			} else {
				error = load_description(SHOW_LOGS);
				if (error) {
					//ignore file not found
					sprintf(tmp_str, "PG %s not loaded.\n", description_file);
					my_log(tmp_str);
					//exit_with_key(error);
				} else {
					sprintf(tmp_str, "PG %s loaded.\n", description_file);
					my_log(tmp_str);
				}
			}
		}

	//--------------------------------------------------

	init_tables();
	//this is my file
	my_log("Loading tiles description...\n");
	error = load_tiles_description();
	if (error == 0) {
		my_log("File 'tiles.txt' loaded.\n");
	} else {
		my_log("File 'tiles.txt' not found...\n");
	}
	my_log("Loading countries description...\n");
	error = load_countries_description();
	if (error == 0) {
		my_log("File 'countries.txt' loaded.\n");
	} else {
		my_log("File 'countries.txt' not found...\n");
	}

	memset(icon_name_aux1, 0, sizeof(icon_name_aux1));
	memset(icon_name_aux2, 0, sizeof(icon_name_aux2));

	my_log("Loading aux icons description...\n");
	error = load_icons_description();
	if (error == 0) {
		my_log("File 'icons.txt' loaded.\n");
	} else {
		my_log("File 'icons.txt' not found...\n");
	}

	my_log("Loading map fragments...\n");
	error = load_map_fragments();
	if (error == 0) {
		my_log("File 'mapfrg.txt' loaded.\n");
	} else {
		my_log("File 'mapfrg.txt' not found...\n");
	}
	sort_map_fragments();
	//--------------------------------------------------
	my_log("Loading PacGenTT2PGTile '%s' file.\n",pac_tt2tiles);
	error = load_pacgen_tt2tiles();
	if (error == 0) {
		my_log("File '%s' loaded.\n",pac_tt2tiles);
	} else {
		my_log("File '%s' not found...\n",pac_tt2tiles);
	}

	if (pacgen_mode) {
		my_log("Changing countries to PacGen.\n");
		initialize_pacgen_countries_table();

		my_log("Setting basic terrain names.\n");
		initialize_pacgen_std_names();

		my_log("Loading PacGen class to PG class '%s' file.\n",pac_cl2pg_cl);
		error = load_pacgen_cl2pg_cl();
		if (error == 0) {
			my_log("File '%s' loaded.\n",pac_cl2pg_cl);
		} else {
			my_log("File '%s' not found...\n",pac_cl2pg_cl);
		}
		my_log("Loading PacGen movement type to PG movement type  '%s' file.\n",pac_mt2pg_mt);
		error = load_pacgen_mt2pg_mt();
		if (error == 0) {
			my_log("File '%s' loaded.\n",pac_mt2pg_mt);
		} else {
			my_log("File '%s' not found...\n",pac_mt2pg_mt);
		}
	}

	//no nulp needed for PGF
	if (!pgf_mode && !pacgen_mode) {
		if (ag_mode) {
			my_log("Loading NUPL from 'p_slots.txt'...\n");
			error = load_p_slots_txt();
			if (error == 0) {
				nupl_present = 1;
				my_log("Loaded.\n");
			} else {
				my_log("Not found.\n");
				my_log("Loading NUPL from 'ag.exe' or 'pg.exe'...\n");
				error = load_ag_nupl();
				if (error == 0) {
					nupl_present = 1;
					my_log("Loaded.\n");
				} else {
					my_log("Not found.\n");
				}
			}
		} else {
			my_log("Loading NUPL from 'nulp.txt'...\n");
			error = load_nulp_txt();
			if (error == 0) {
				nupl_present = 1;
				my_log("Loaded.\n");
			} else {
				my_log("Not found.\n");
				my_log("Loading NUPL from 'unit.txt'...\n");
				error = load_unit_txt();

				if (error == 0) {
					nupl_present = 1;
					my_log("Loaded.\n");
				} else {
					my_log("Not found.\n");

					my_log("Loading NUPL from 'panzer.exe'...\n");
					error = load_nupl();
					if (error == 0) {
						nupl_present = 1;
						my_log("Loaded.\n");
					} else {
						my_log("Not found...Using default NUPL\n");
						nupl_present = 1;
						for (i = 0; i < PG_NUPL_SIZE; i++)
							nupl[i] = orginal_nupl[i];
						parse_nupl();
					}
				}
			}
		}
	}

	//load weather and movement table from binary when avaliable
	bin_tables_present = 0;
	if (ag_mode || pg_mode) {
		error = load_classic_bin_tables();
		if (!error) {
			my_log("Loaded weather and movement information from exe.\n");
			bin_tables_present = 1;
		}
	}
	if (pgf_mode) {
		error = load_pgf_bin_tables();
		if (!error) {
			my_log("Loaded weather and movement information from PGF exe.\n");
			bin_tables_present = 1;
		}
	}
	if (pacgen_mode) {
		error = load_pacgen_bin_tables();
		if (!error) {
			my_log("Loaded weather and movement information from PACGEN exe.\n");
			bin_tables_present = 1;
		}
	}
	if (!bin_tables_present) {
		my_log("Using default weather and movement information.\n");
	}
	//if (error) my_log("%d\n",error);
	/*
	 //how to make rought dark tiles ? - all color components * 3/4
	 for(i=0x10;i<0x50;i++)
	 printf("%d %d %d\n",
	 pgpal[i].r,
	 pgpal[i+0x70].r,
	 pgpal[i].r*3/4 );
	 */

	strncpy(tmp_str,"",128);
	if (pg_mode)
		strncpy(tmp_str, "PG mode",128);
	if (pgf_mode)
		strncpy(tmp_str, "PGF mode",128);
	if (ag_mode)
		strncpy(tmp_str, "AG mode",128);
	if (pacgen_mode)
		strncpy(tmp_str, "PacGen mode",128);

	strncat(MapStatusTxt,tmp_str,256);

	my_log("%s\n",tmp_str);

	//--------------------------------------
	// end of data load, start scenario load
	//--------------------------------------
	init_tables_post_load();

	if (argc > 1) {
		min_scn = 0;
		max_scn = 0;
		if (strcmp("ALL", argv[argc - 1]) == 0) {
			min_scn = 1;
			max_scn = 40;
		}
		if (strcmp("ALLX", argv[argc - 1]) == 0) {
			min_scn = 1;
			max_scn = 99;
		}
		if (strcmp("ALLXX", argv[argc - 1]) == 0) {
			min_scn = 1;
			max_scn = 199;
		}
		if (strcmp("ALLXXX", argv[argc - 1]) == 0) {
			min_scn = 1;
			max_scn = 299;
		}
		if (strcmp("ALLXXXX", argv[argc - 1]) == 0) {
			min_scn = 1;
			max_scn = 399;
		}
		if (strcmp("ALLXXXXX", argv[argc - 1]) == 0) {
			min_scn = 1;
			max_scn = 499;
		}

		if (min_scn != max_scn) //check if last parameter is ALL[XXXXX]
		{
			for (i = min_scn; i <= max_scn; i++) {
				sprintf(argv[argc - 1], "%02d", i);
				cli_parsing(argc, argv);
			}
			return 0;
		} else {
			if (strcmp("UALL", argv[argc - 1]) == 0) {
				sprintf(argv[argc - 1], "%d", -1);
				//sprintf(argv[argc    ], "%d", total_uicons-1);
				//argc++;
				cli_parsing(argc, argv);
//				for (i = 0; i < total_uicons; i++) {
//					sprintf(argv[argc - 1], "%02d", i);
//					cli_parsing(argc, argv);
//				}

				return 0;
			} else {
				error = cli_parsing(argc, argv);
				//my_log("%d\n",error);
				if (error > 1)
					exit_with_key(error);
				else if (error == 1)
					exit(0);
			}
		}
	} else {
		setScenarioNumber(0);
	}
	/* set graphics mode */
	if (load_config() > 0) {
		//use defaults
		card_old = GFX_AUTODETECT;
		w_old = SCREEN_X;
		h_old = SCREEN_Y;
	} else {
		my_log("Reading FPGE config file succeeded.\n");
	}

	mark_problems();

	fpge_gui_fg_color = FPGE_FG_COLOR;
	fpge_gui_bg_color = FPGE_SCREEN_COLOR;
	fpge_gui_edit_color = MAP_COLOR; //BG_COLOR;

	prepare_dialogs();

	install_timer();
	install_keyboard();
	/* try mouse */
	//--------These are required ...
	error = install_mouse();
	if (error < 0) {
		my_log("Error. Cannot init mouse. Exiting.");
		exit_with_key(8);
	}

	if (set_gfx_mode(card_old, w_old, h_old, 0, 0) < 0) {
		//use defaults
		card_old = GFX_AUTODETECT;
		w_old = SCREEN_X;
		h_old = SCREEN_Y;
		//try again default 1024x768
		if (set_gfx_mode(card_old, w_old, h_old, 0, 0) < 0)
			my_log("Error. Cannot set default graphic mode. Exiting.");
		exit_with_key(1);
	}

	set_pal();
	initialize_dm();

	//...--------to show the default mouse

	mousie = create_bitmap(11, 11);
	rectfill(mousie, 0, 0, 11, 11, colors_to24bits(0));
	line(mousie, 0, 5, 11, 5, colors_to24bits(15));
	line(mousie, 5, 0, 5, 11, colors_to24bits(15));
	circle(mousie, 5, 5, 5, colors_to24bits(22));
	set_mouse_sprite(mousie);
	set_mouse_sprite_focus(5, 5);
	set_mouse_speed(1, 1);

	show_mouse(screen);

	/*----------------Set up Bitmaps -------------------------- */

	if (draw_app6 && pgf_mode && draw_app6_units) {
		int bmp_idx;
		init_app_components();

		memset(bmp_to_country, -1, sizeof(bmp_to_country));
		my_log("Loading BMP2Country from bmp2ctry.txt...\n");
		error = load_bmp2country_description();
		if (error == 0) {
			my_log("Loaded.\n");
		} else {
			my_log("Not found.\n");
		}

		//printf("%d\n",bmp_to_country[MAX_UICONS-1]);
		for (i = 0; i < total_equip; i++) {
			bmp_idx = equip[i][BMP] + 256 * equip[i][BMP + 1];
			int c = equip_country[i];
			if (c == -1) {
				c = bmp_to_country[bmp_idx];
			}
			//printf("i=%d bmp_idx=%d c=%d\n",i, bmp_idx,c);

			if (bmp_idx > 0 && bmp_idx < MAX_UICONS && is_app6_bmp[bmp_idx] == 0 && unit_bmp[bmp_idx] != NULL) { //convert BMP
				if (c > 0)
					draw_app6_unit_symbol(unit_bmp[bmp_idx], 0, 0, i, country_side[c] == 0 ? 0 : 1
					//0
					);
				is_app6_bmp[bmp_idx] = 1;
			}
		}
	}
	map_bmp = create_bitmap(MAP_W, MAP_H);
	rectfill(map_bmp, 0, 0, MAP_W, MAP_H, colors_to24bits(MAP_COLOR));

	position_gui_elements(w_old, h_old);
	//draw_map(map_bmp,x0,y0,tiles_high,tiles_wide);
	//dm[dmMapBmpIdx].dp=map_bmp;
	//  blit(map_bmp,screen,0,0,0,LINE_2_Y,MAP_W,MAP_H);

	left_arrow_bmp = create_bitmap(HSLIDE_H, HSLIDE_H);
	rectfill(left_arrow_bmp, 0, 0, HSLIDE_H, HSLIDE_H, colors_to24bits(SLIDE_BG_COLOR));
	triangle(left_arrow_bmp, 0, HSLIDE_H / 2, HSLIDE_H, 0, HSLIDE_H, HSLIDE_H, colors_to24bits(SLIDE_FG_COLOR));
	main_dlg[dmLeftArrowIdx].dp = left_arrow_bmp;

	right_arrow_bmp = create_bitmap(HSLIDE_H, HSLIDE_H);
	rectfill(right_arrow_bmp, 0, 0, HSLIDE_H, HSLIDE_H, colors_to24bits(SLIDE_BG_COLOR));
	triangle(right_arrow_bmp, HSLIDE_H, HSLIDE_H / 2, 0, 0, 0, HSLIDE_H, colors_to24bits(SLIDE_FG_COLOR));
	main_dlg[dmRightArrowIdx].dp = right_arrow_bmp;

	up_arrow_bmp = create_bitmap(VSLIDE_W, VSLIDE_W);
	rectfill(up_arrow_bmp, 0, 0, VSLIDE_W, VSLIDE_W, colors_to24bits(SLIDE_BG_COLOR));
	triangle(up_arrow_bmp, VSLIDE_W / 2, 0, 0, VSLIDE_W, VSLIDE_W, VSLIDE_W, colors_to24bits(SLIDE_FG_COLOR));
	main_dlg[dmUpArrowIdx].dp = up_arrow_bmp;

	down_arrow_bmp = create_bitmap(VSLIDE_W, VSLIDE_W);
	rectfill(down_arrow_bmp, 0, 0, VSLIDE_W, VSLIDE_W, colors_to24bits(SLIDE_BG_COLOR));
	triangle(down_arrow_bmp, 0, 0, VSLIDE_W / 2, VSLIDE_W, VSLIDE_W, 0, colors_to24bits(SLIDE_FG_COLOR));
	main_dlg[dmDownArrowIdx].dp = down_arrow_bmp;

	main_bmp = create_bitmap(MAPFRG_DIALOG_X_BMP, MAPFRG_DIALOG_Y_BMP);

	setup_show_filters_info();
	select_weather_item();

	vslide_max = get_v_slide_max();
	main_dlg[dmHSlideIdx].d1 = get_h_slide_max();
	main_dlg[dmHSlideIdx].d2 = map_x0;
	main_dlg[dmVSlideIdx].d1 = vslide_max;
	main_dlg[dmVSlideIdx].d2 = vslide_max - map_y0;

	setup_menu_ticks();
	//move to 0,0
	//position_dialog(d_map_frg,-d_map_frg[0].x,-d_map_frg[0].y);

	//not longer needed but can be useful in future
	//prepare_tiles_filters();
	//test_cbuf();return 0;

	//test_undo();return 0;

	init_undo();

	//load_wtiles();

	while (really_exit) {
		do_dialog(main_dlg, -1);
		really_exit = (alert("Do you really want to exit FPGE ?", NULL, NULL, "&Yes", "&No", 'y', 'n') + 1) % 2;
	}
	check_and_save_mapnames();

	free_undo();
	allegro_exit();

	if (save_config())
		my_log("Error in writing FPGE config file.\n");
	else
		my_log("Writing FPGE config file succeeded.\n");
	return 0;
}
END_OF_MAIN()


