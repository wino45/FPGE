/*
 * callback.c
 *
 *  Created on: 2010-03-09
 *      Author: wino
 */
#include <stdio.h>
#include "fpge.h"
#include "tables.h"
#include "minimap.h"
#include "load.h"
#include "pgf.h"
#include "maingui.h"
#include "menu.h"
#include "fpge_colors.h"
#include "save.h"
#include "filename.h"
#include "savebmp.h"
#include "drawmap.h"
#include "cli.h"
#include "pg.h"
#include "terr.h"
#include "move.h"
#include "unitlist.h"

DIALOG generate_dlg[9] = {
/* 0 */{ d_textbox_proc, 10, 0, 250, 138+24, GUI_FG_COLOR, GUI_BG_COLOR, 0, 0, 0, 0, 0, 0, 0 },
/* 1 */{ d_text_proc, 22, 12, 156, 18, GUI_FG_COLOR, GUI_BG_COLOR, 0, 0, 0, 0,"Choose tiles to generate" },
/* 2 */{ d_check_proc, 22, 30, 8 * 13, 15, GUI_FG_COLOR, GUI_BG_COLOR, 'o', 0, 1, 0, "C&oast tiles" },
/* 3 */{ d_check_proc, 22, 54, 8 * 13, 15, GUI_FG_COLOR, GUI_BG_COLOR, 'r', 0, 1, 0, "&Road tiles" },
/* 4 */{ d_check_proc, 22, 78, 8 * 13, 15, GUI_FG_COLOR, GUI_BG_COLOR, 'i', 0, 1, 0, "R&iver tiles" },
/* 5 */{ d_button_proc, 22, 102+24, 96, 18, GUI_FG_COLOR, GUI_BG_COLOR, 'c', D_EXIT, 0, 0, "&Cancel" },
/* 6 */{ d_button_proc, 148, 102+24, 96, 18, GUI_FG_COLOR, GUI_BG_COLOR, 'g', D_EXIT, 0, 0, "&Generate" },
/* 7 { d_check_proc, 22, 78+24, 8 * 15, 15, GUI_FG_COLOR, GUI_BG_COLOR, 'a', 0, 1, 0, "R &and R tiles" },*/
	   { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0 },
	   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL } };

DIALOG rtnfix_dlg[9] = {
/* 0 */{ d_textbox_proc, 10, 0, 250, 138, GUI_FG_COLOR, GUI_BG_COLOR, 0, 0, 0, 0, 0, 0, 0 },
/* 1 */{ d_text_proc, 22, 12, 156, 18, GUI_FG_COLOR, GUI_BG_COLOR, 0, 0, 0, 0,"Choose layers to generate" },
/* 2 */{ d_check_proc, 22, 30, 8 * 18, 15, GUI_FG_COLOR, GUI_BG_COLOR, 'r', 0, 1, 0, "&Roads layer" },
/* 3 */{ d_check_proc, 22, 54, 8 * 18, 15, GUI_FG_COLOR, GUI_BG_COLOR, 't', 0, 1, 0, "&Terrain type IDs" },
/* 4 */{ d_check_proc, 22, 78, 8 * 18, 15, GUI_FG_COLOR, GUI_BG_COLOR, 'n', 0, 1, 0, "Simple &names" },
/* 5 */{ d_button_proc, 22, 102, 96, 18, GUI_FG_COLOR, GUI_BG_COLOR, 'c',D_EXIT, 0, 0, "&Cancel" },
/* 6 */{ d_button_proc, 148, 102, 96, 18, GUI_FG_COLOR, GUI_BG_COLOR, 'g',D_EXIT, 0, 0, "&Generate" },
	   { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL } };

void copyFromTile(int i) {
	if (EditorTile[i - 1].set) {
		tdTTStrOn = EditorTile[i - 1].tdTTStrOn;
		tdRDStrOn = EditorTile[i - 1].tdRDStrOn;
		tdTNStrOn = EditorTile[i - 1].tdTNStrOn;
		tdGLNStrOn = EditorTile[i - 1].tdGLNStrOn;
		tdSDStrOn = EditorTile[i - 1].tdSDStrOn;
		strncpy(tdTTStr, EditorTile[i - 1].tdTTStr, 8);
		strncpy(tdRDStr, EditorTile[i - 1].tdRDStr, 8);
		strncpy(tdTNStr, EditorTile[i - 1].tdTNStr, 8);
		strncpy(tdGLNStr, EditorTile[i - 1].tdGLNStr, 8);
		strncpy(tdSDStr, EditorTile[i - 1].tdSDStr, 8);
	}
	if (edit_op == edit_ter) {
		setup_terrain_info();
		//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	}
}

int get_v_slide_max() {
	return (mapy - tiles_high > 0) ? mapy - tiles_high : 1; //should be 0 but there is crash when =0
}
int get_h_slide_max() {
	return (mapx - tiles_wide > 0) ? mapx - tiles_wide : 1;
}

void copyToTile(int i) {
	int column, row, X, Y;
	char temp[8];

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

	EditorTile[i - 1].tdTTStrOn = tdTTStrOn;
	EditorTile[i - 1].tdRDStrOn = tdRDStrOn;
	EditorTile[i - 1].tdTNStrOn = tdTNStrOn;
	EditorTile[i - 1].tdGLNStrOn = tdGLNStrOn;
	EditorTile[i - 1].tdSDStrOn = tdSDStrOn;
	sprintf(temp, "%d", map[column][row].utr);
	strncpy(EditorTile[i - 1].tdTTStr, temp, 8);
	sprintf(temp, "%d", map[column][row].rc);
	strncpy(EditorTile[i - 1].tdRDStr, temp, 8);
	sprintf(temp, "%d", map[column][row].tile);
	strncpy(EditorTile[i - 1].tdTNStr, temp, 8);
	sprintf(temp, "%d", map[column][row].gln);
	strncpy(EditorTile[i - 1].tdGLNStr, temp, 8);
	sprintf(temp, "%d", map[column][row].side);
	strncpy(EditorTile[i - 1].tdSDStr, temp, 8);
	EditorTile[i - 1].set = 1;
	copyFromTile(i);
}


void do_filter_tiles_matrix(int i) {
	if (filter_last == i){
		int mod=(filter_number_ingroup[i]==1)?0:2;
		filter_number_current_ingroup[i]= (filter_number_current_ingroup[i]+1)%(filter_number_ingroup[i]+mod);
		if (!filter_number_current_ingroup[i])
			filter_tiles = (filter_tiles + 1) % 2;
		else
			filter_tiles=1;
	}else {
		if (filter_number_current_ingroup[i]==0) filter_number_current_ingroup[i]=1;
		if (!filter_tiles){
			filter_tiles = 1;
		}
	}
	filter_last = i;
	//print_dec(filter_number_current_ingroup[i]);
	//print_dec(filter_last);

	//filter_start=tiles_filter[i][0];
	//filter_stop=tiles_filter[i][1];

	if (edit_op == edit_tile) {
		draw_tiles_matrix();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide); // change me !! is 0,0 ok ?
	}
}

int key_1_keycallback() {
	copyFromTile(1);
	return D_O_K;
}
int key_2_keycallback() {
	copyFromTile(2);
	return D_O_K;
}
int key_3_keycallback() {
	copyFromTile(3);
	return D_O_K;
}
int key_4_keycallback() {
	copyFromTile(4);
	return D_O_K;
}
int key_5_keycallback() {
	copyFromTile(5);
	return D_O_K;
}
int key_6_keycallback() {
	copyFromTile(6);
	return D_O_K;
}
int key_7_keycallback() {
	copyFromTile(7);
	return D_O_K;
}
int key_8_keycallback() {
	copyFromTile(8);
	return D_O_K;
}
int key_9_keycallback() {
	copyFromTile(9);
	return D_O_K;
}

int f1_keycallback() {
	do_filter_tiles_matrix(0);
	return D_REDRAW;
}

int f2_keycallback() {
	do_filter_tiles_matrix(1);
	return D_REDRAW;
}

int f3_keycallback() {
	do_filter_tiles_matrix(2);
	return D_REDRAW;
}

int f4_keycallback() {
	do_filter_tiles_matrix(3);
	return D_REDRAW;
}

int f5_keycallback() {
	do_filter_tiles_matrix(4);
	return D_REDRAW;
}

int f6_keycallback() {
	do_filter_tiles_matrix(5);
	return D_REDRAW;
}

int f7_keycallback() {
	do_filter_tiles_matrix(6);
	return D_REDRAW;
}

int f8_keycallback() {
	do_filter_tiles_matrix(7);
	return D_REDRAW;
}

int f9_keycallback() {
	do_filter_tiles_matrix(8);
	return D_REDRAW;
}

int f10_keycallback() {
	do_filter_tiles_matrix(9);
	return D_REDRAW;
}

int f11_keycallback() {
	/*
	 set_pallete(pgpal);
	 set_weather_palette();
	 */
	do_filter_tiles_matrix(10);
	return D_REDRAW;
}

int f12_keycallback() {
	do_filter_tiles_matrix(11);
	return D_REDRAW;
}

int ctrl_1_keycallback() {
	copyToTile(1);
	return D_O_K;
}
int ctrl_2_keycallback() {
	copyToTile(2);
	return D_O_K;
}
int ctrl_3_keycallback() {
	copyToTile(3);
	return D_O_K;
}
int ctrl_4_keycallback() {
	copyToTile(4);
	return D_O_K;
}
int ctrl_5_keycallback() {
	copyToTile(5);
	return D_O_K;
}
int ctrl_6_keycallback() {
	copyToTile(6);
	return D_O_K;
}
int ctrl_7_keycallback() {
	copyToTile(7);
	return D_O_K;
}
int ctrl_8_keycallback() {
	copyToTile(8);
	return D_O_K;
}
int ctrl_9_keycallback() {
	copyToTile(9);
	return D_O_K;
}

int draw_new_all_names() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_all_names_item();
	return D_REDRAW;
}

int ctrl_a_keycallback() {
	if (edit_op == edit_tile && debug_tile_matrix){
		if (drawAllNamesDebug)
			colorizeNamesDebug = (colorizeNamesDebug + 1) % 3;
		if (colorizeNamesDebug == 0)
			drawAllNamesDebug = (drawAllNamesDebug + 1) % 2;
	}else{
		if (drawAllNames)
			colorizeNames = (colorizeNames + 1) % 3;
		if (colorizeNames == 0)
			drawAllNames = (drawAllNames + 1) % 2;
	}
	return draw_new_all_names();
}

void mark_problems() {
	int x, y;

	if (show_problems && !show_debug_problems) //clear all problems
		for (y = 0; y < mapy; ++y)
			for (x = 0; x < mapx; ++x)
				map[x][y].shade &= ~PROBLEM_MASK;

	switch (show_problems) {
	case 1:
		drawRoads = 1;
		mark_road_problems();
		break;
	case 2:
		drawRoads = 1;
		mark_city_problems();
		break;
	case 3:
		checking_units(1);
		break;
	case 4:
		checking_flags(1);
		break;
	case 5:
		checking_ocean(1, 0);
		break;
	case 6:
		checking_river_names(1);
		break;
	}

}

void mark_city_problems() {
	int x, y, found;

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			found = 0;
			//for (i = tiles_filter[CITY_FILTER_INDEX][0];i <= tiles_filter[CITY_FILTER_INDEX][1]; i++)
				if ( is_tile_a_city_tile(map[x][y].tile) && is_tile_name_standard(map[x][y].gln))
					found = 1;
			if (found == 1)
				map[x][y].shade |= PROBLEM_MASK;
			//else
			//map[x][y].shade &=~PROBLEM_MASK;
		}
}

int draw_new_unique_names() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_unique_name_item();
	return D_REDRAW;
}

int ctrl_alt_n_keycallback() {
	if (key_shifts & KB_ALT_FLAG) {
		if (tile_mode == 1) {
			tile_cleanup();
		}
		if (show_problems)
			show_problems = 0;
		else
			show_problems = 2;

		mark_problems();

		setup_show_filters_info();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}

	if (key_shifts & KB_CTRL_FLAG) {
		if (edit_op == edit_tile && debug_tile_matrix) {
			drawNamesDebug = (drawNamesDebug + 1) % 2;
		}else{
			drawNames = (drawNames + 1) % 4;
		}
		draw_new_unique_names();
	}
	return D_REDRAW;
}

int draw_new_terain() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_terrain_item();
	return D_REDRAW;
}

int ctrl_t_keycallback() {
	if (edit_op == edit_tile && debug_tile_matrix){
		drawTerrainDebug = (drawTerrainDebug + 1) % 2;
	}else{
		drawTerrain = (drawTerrain + 1) % 2;
	}
	draw_new_terain();
	return D_REDRAW;
}

void mark_road_problems() {
	int x, y;

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			//save orginal RC
			rc[x][y] = map[x][y].rc;
			//check mask
			map[x][y].rc = find_road_mask(x, y);
		}

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			//filter mask
			tempmap[x][y] = find_one_way_road(x, y);

	// copy temp to rc and filter it
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			if (map[x][y].rc > 0) {
				map[x][y].rc = (unsigned char) tempmap[x][y];
				//filter it
				map[x][y].rc = filter_roads(x, y);
			}

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			if (map[x][y].rc != rc[x][y])
				map[x][y].shade |= PROBLEM_MASK;
			//else
			//	map[x][y].shade &=~PROBLEM_MASK;
			//restore RC
			map[x][y].rc = rc[x][y];
		}
}

int draw_new_road() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_road_item();
	return D_REDRAW;
}

int push_new_road() {
	if (edit_op == edit_tile && debug_tile_matrix){
		drawRoadsDebug = (drawRoadsDebug + 1) % 4;
	}else{
		drawRoads = (drawRoads + 1) % 2;
	}
	return draw_new_road();
}

int ctrl_alt_r_keycallback() {
	if (key_shifts & KB_ALT_FLAG) {
		//alert("ERROR: Wrong file format:", NULL, NULL, "&Continue", NULL, 'c',0);
		if (tile_mode == 1) {
			tile_cleanup();
		}

		show_problems = (show_problems + 1) % 2;

		mark_problems();

		setup_show_filters_info();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	if (key_shifts & KB_CTRL_FLAG) {
		push_new_road();
	}
	return D_REDRAW;
}

int draw_new_display_names() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_display_name_item();
	return D_REDRAW;
}

int ctrl_d_keycallback() {
	if (edit_op == edit_tile && debug_tile_matrix){
		showDecimalDebug = (showDecimalDebug + 1) % 3;
	}else{
		showDecimal = (showDecimal + 1) % 3;
	}
	return draw_new_display_names();

}

int draw_new_gnd_transport() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_gnd_transport_item();
	return D_REDRAW;
}

int ctrl_l_keycallback() {
	if (tile_mode==1){
		showMatrixMode=(showMatrixMode+1)%4;
		setup_show_filters_info();
		draw_tiles_matrix();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		return D_REDRAW;
	}
	drawGndTransport = (drawGndTransport + 1) % 2;
	return draw_new_gnd_transport();

}
int ctrl_alt_s_keycallback() {
	if (key_shifts & KB_ALT_FLAG) {
		save_as_dialog();
		//save_pgf_pgscn(scn_number,SHOW_LOGS, NORMAL_SAVE);
		sprintf(MapStatusTxt, "PGF scenario %02d saved\nPick an Operation.",
				getScenarioNumber());
		//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	}

	if (key_shifts & KB_CTRL_FLAG) {
		if (tile_mode==1){
			sortMatrixMode = (sortMatrixMode+1)%2;
			setup_show_filters_info();
			draw_tiles_matrix();
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			return D_REDRAW;
		}
		if (showCounter == 0)
			showCounter = -1;
		else //if (showCounter == -1)
			showCounter = 0;
		//else
			//showCounter = 0;

		setup_show_filters_info();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	return D_REDRAW;
}

int ctrl_x_keycallback() {
	if (showCounter == 1)
		showCounter = -1;
	else //if (showCounter == -1)
		showCounter = 1;
	//else
		//showCounter = 1;
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	return D_REDRAW;
}
int ctrl_e_keycallback() {
	if (showCounter == 2)
		showCounter = -1;
	else //if (showCounter == -1)
		showCounter = 2;
	//else
		//showCounter = 2;
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	return D_REDRAW;
}

int ctrl_alt_f_keycallback() {
	int column, row, X, Y;

	if (key_shifts & KB_ALT_FLAG) {
		place_fragment_mode();
	}
	if (key_shifts & KB_CTRL_FLAG) {
		showFilter = (showFilter + 1) % 2;
		setup_show_filters_info();
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

		filterTerrain = map[column][row].utr;
		filterName = map[column][row].gln;
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	return D_REDRAW;
}
int ctrl_b_keycallback() {
	char path[MAX_PATH];
	sprintf(path, ".\\map%02d.bmp", getScenarioNumber());
	canonicalize_filename(path, path, MAX_PATH);
	if (file_select_ex("Save BMP file", path, "bmp", MAX_PATH,
			OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {
		save_map_bmp = create_bitmap(mapx * TILE_WIDTH + 16, mapy * TILE_HEIGHT + TILE_HEIGHT / 2);
		if (!save_map_bmp) {
			alert("ERROR: Cannot save BMP file.", "Not enough memory.", NULL,
					"&Continue", NULL, 'c', 0);
		} else {
			sprintf(MapStatusTxt, "Saving BMP file\nPlease wait...");
			//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
			main_dlg[dmMapStatusIdx].flags |= D_DIRTY;

			draw_map(save_map_bmp, 0, 0, mapy, mapx);
			if (save_bmp(path, save_map_bmp, pgpal)) {
				alert("ERROR: Cannot save BMP file.", "File cannot be saved.",
						NULL, "&Continue", NULL, 'c', 0);
			}
			destroy_bitmap(save_map_bmp);
			sprintf(MapStatusTxt, "BMP file saved !\nPick an Operation.");
			main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
			//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
		}
	}
	return D_REDRAW;
}

int ctrl_y_keycallback() {
	char path[MAX_PATH] = ".\\";
	RGB pal1[256];
	int c, i, j, k = 0, newmapx, newmapy, newmapstep = 5;

	BITMAP *new_map_bitmap;

//	srand(rawclock());
	srand((unsigned int) time((time_t *) NULL));

	if (alert("Do you really want to make new map ?",
			"This action will erase all changes already made", NULL, "&Yes",
			"&No", 'y', 'n') == 2)
		return D_O_K;

	canonicalize_filename(path, path, MAX_PATH);
	if (file_select_ex("Load BMP file to convert into map", path, "bmp",
			MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {
		set_color_conversion(COLORCONV_NONE);
		new_map_bitmap = load_bmp(path, pal1);
		//printf("%d\n",bitmap_color_depth(new_map_bitmap));
		//for (i=0;i<16;i++) printf("%d,%d,%d,%d\n",i,pal1[i].r,pal1[i].g,pal1[i].b);

		if (new_map_bitmap) {
			if (tile_mode == 1) {
				tile_cleanup();
			}
			//do the map
			newmapx = new_map_bitmap->w / newmapstep;
			newmapy = new_map_bitmap->h / newmapstep;

			if (newmapx > MAX_MAP_X || newmapy>MAX_MAP_Y) {
				char buf[1024];
				snprintf(buf,1024,"%d px X %d px",MAX_MAP_X*newmapstep,MAX_MAP_Y*newmapstep);
				alert("ERROR: BMP size must be smaller then ", buf, NULL, "&Continue",	NULL, 'c', 0);
			}else{
				for (j = 0; j < newmapy; j++)
					for (i = 0; i < newmapx; i++) {
						c = getpixel(
								new_map_bitmap,
								i * newmapstep + newmapstep / 2,
								j * newmapstep + newmapstep / 2
										+ (i % 2) * newmapstep / 2);
						//if (j==0 && i<16) printf("%d\n",c);
						if (bitmap_color_depth(new_map_bitmap) == 8)
							for (k = 0; k < max_colors_for_bmp; k++) {
								if ((pal1[c].r == (colors_for_bmp[k][0] >> 2))
										&& (pal1[c].g == (colors_for_bmp[k][1] >> 2))
										&& (pal1[c].b == (colors_for_bmp[k][2] >> 2)))
									break;
							}
						if (bitmap_color_depth(new_map_bitmap) == 24)
							for (k = 0; k < max_colors_for_bmp; k++) {
								if ((getr24(c) == (colors_for_bmp[k][0]))
										&& (getg24(c) == (colors_for_bmp[k][1]))
										&& (getb24(c) == (colors_for_bmp[k][2])))
									break;
							}
						if (bitmap_color_depth(new_map_bitmap) == 32)
							for (k = 0; k < max_colors_for_bmp; k++) {
								if ((getr32(c) == (colors_for_bmp[k][0]))
										&& (getg32(c) == (colors_for_bmp[k][1]))
										&& (getb32(c) == (colors_for_bmp[k][2])))
									break;
							}
						memset(&map[i][j], 0, sizeof(map[i][j]));
						map[i][j].tile = tiles_for_bmp[(k == max_colors_for_bmp ? k - 1 : k)][rand() % 3];
						//the map point was build. Now delete all units, vic points, owner, etc
						map[i][j].guidx = -1;
						map[i][j].auidx = -1;
						//if (j==1 && i<16) printf("(%d,%d) k=%d c=%8x -> %d t_b=%d c_b=%d\n",i,j,k,c,map[i][j].tile,tiles_for_bmp[k][0],colors_for_bmp[k][0]);
					}
				mapx = newmapx;
				mapy = newmapy;
				map_x0 = 0;
				map_y0 = 0;
				//the map was build. Now delete all units, vic points, owner, etc

				//clear all units
				for (i = 0; i < total_units; i++)
					all_units[i].unum = -1;
				total_units = 0;

				main_dlg[dmHSlideIdx].d1 = get_h_slide_max();
				vslide_max = get_v_slide_max();

				main_dlg[dmVSlideIdx].d1 = vslide_max;
				main_dlg[dmVSlideIdx].d2 = vslide_max;
				//finally draw map
				draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			}
			destroy_bitmap(new_map_bitmap);
		} else {
			alert("ERROR: Cannot load this BMP file.", path, NULL, "&Continue",	NULL, 'c', 0);
		}
	}
	return D_REDRAW;
}

int ctrl_p_keycallback() {
	return new_map_dialog();
}

int draw_new_weather() {
	set_weather_palette();
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide); //needed for 24bit mode
	select_weather_item();
	return D_REDRAW;
}

int ctrl_w_keycallback() {
	showWeather = (showWeather + 1) % 3;
	return draw_new_weather();
}

int draw_new_hex() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_hex_item();
	return D_REDRAW;
}

int draw_new_flag() {
	select_flag_item();
	return D_REDRAW;
}
int ctrl_alt_h_keycallback() {
	int redraw=0;

	if (key_shifts & KB_ALT_FLAG) {
		if (tile_mode==1) {
			showHexMatrixColor = (showHexMatrixColor + 1) % 6;
			redraw=2;
		}else{
			showHexColor = (showHexColor + 1) % 6;
			redraw=1;
		}
	}
	if (key_shifts & KB_CTRL_FLAG) {
		if (tile_mode==1) {
			showHexMatrix = (showHexMatrix + 1) % 2;
			redraw=2;
		}else{
			showHex = (showHex + 1) % 2;
			redraw=1;
		}
	}
	if (redraw==1){
		return draw_new_hex();
	}
	if (redraw==2){
		setup_show_filters_info();
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		return D_REDRAW;
	}
	return D_O_K;
}

int draw_new_scenario_units() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_scenario_units_item();
	return D_REDRAW;
}

int change_scenarioUnitsMode(){
	scenarioUnitsMode = (scenarioUnitsMode + 1) % 2;
	return draw_new_scenario_units();
}

int ctrl_alt_u_keycallback() {
	if (key_shifts & KB_ALT_FLAG) {
		return onUndo();
	}

	if (key_shifts & KB_CTRL_FLAG) {
		return change_scenarioUnitsMode();
	}
	return D_REDRAW;
}

int draw_new_all_units() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_all_units_item();
	return D_REDRAW;
}

int ctrl_v_keycallback() {
	displayAllUnits = (displayAllUnits + 1) % 2;
	return draw_new_all_units();
}

int draw_new_unit_order() {
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_unit_order_item();
	return D_REDRAW;
}

int ctrl_o_keycallback() {
	displayUnitsOrder = (displayUnitsOrder + 1) % 3;
	return draw_new_unit_order();
}

int ctrl_z_keycallback() {
	graphical_overide = (graphical_overide + 1) % 4;
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	return D_REDRAW;
}

int ctrl_c_keycallback() {
	graphical_overide_hex = (graphical_overide_hex + 1) % 8;
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	return D_REDRAW;
}

int ctrl_i_keycallback() {

	drawNames = 0;
	drawAllNames = 0;
	drawTerrain = 0;
	drawRoads = 0;
	showDecimal = 0;
	showFilter = 0;
	filterTerrain = -1;
	filterName = -1;
	showCounter = 0;
	drawGndTransport = 0;
	showWeather = 0;
	showHex = 1;
	showHexMatrix = 1;
	showHexColor = 0;
	showHexMatrixColor = 0;
	mapShiftMode = 0;
	showMatrixMode=0;
	sortMatrixMode=0;
	graphical_overide = 0;
	scenarioUnitsMode = 0;
	displayAllUnits = 0;
	graphical_overide_hex = 0;
	displayUnitsOrder = 0;
	show_ranges = 2;
	show_problems = 0;
	//colorize_ocean=0;
	colorizeNames = 0;

	drawRoadsDebug=0;
	showDecimalDebug=0;
	drawNamesDebug=0;
	drawAllNamesDebug=0;
	colorizeNamesDebug=0;
	drawTerrainDebug=0;

	//clear all filters for tiles matrix
	memset(filter_number_current_ingroup,0,sizeof(filter_number_current_ingroup));
	if (tile_mode==1) draw_tiles_matrix();
	//setup_show_filters_info();
	//draw_map(map_bmp,map_x0,map_y0,tiles_high,tiles_wide);
	setup_menu_ticks();
	return D_REDRAW;
}

void position_gui_elements(int w, int h) {
	int main_button_x, exit_y;

	//find the new positions for elements
	main_button_x = w - 50 + 2;
	exit_y = h - bottom_spacer;
	//buttons
	main_dlg[dmHelpBtnIdx].x = main_button_x;
	//main_dlg[dmHelpBtnIdx].x=10;
	//main_dlg[dmHelpBtnIdx].w=w-10;
	main_dlg[dmHelpBtnIdx].y = exit_y - 20 * BUTTON_SIZE;
	main_dlg[dmSearchRBtnIdx].x = main_button_x;
	main_dlg[dmSearchRBtnIdx].y = exit_y - 19 * BUTTON_SIZE;
	main_dlg[dmRectImportBtnIdx].x = main_button_x;
	main_dlg[dmRectImportBtnIdx].y = exit_y - 18 * BUTTON_SIZE;
	main_dlg[dmRectExportBtnIdx].x = main_button_x;
	main_dlg[dmRectExportBtnIdx].y = exit_y - 17 * BUTTON_SIZE;
	main_dlg[dmNeutralBtnIdx].x = main_button_x;
	main_dlg[dmNeutralBtnIdx].y = exit_y - 16 * BUTTON_SIZE;
	main_dlg[dmDeployBtnIdx].x = main_button_x;
	main_dlg[dmDeployBtnIdx].y = exit_y - 15 * BUTTON_SIZE;
	main_dlg[dmVictBtnIdx].x = main_button_x;
	main_dlg[dmVictBtnIdx].y = exit_y - 14 * BUTTON_SIZE;
	main_dlg[dmOwnBtnIdx].x = main_button_x;
	main_dlg[dmOwnBtnIdx].y = exit_y - 13 * BUTTON_SIZE;
	main_dlg[dmTerBtnIdx].x = main_button_x;
	main_dlg[dmTerBtnIdx].y = exit_y - 12 * BUTTON_SIZE;
	main_dlg[dmTileBtnIdx].x = main_button_x;
	main_dlg[dmTileBtnIdx].y = exit_y - 11 * BUTTON_SIZE;
	main_dlg[dmEntBtnIdx].x = main_button_x;
	main_dlg[dmEntBtnIdx].y = exit_y - 10 * BUTTON_SIZE;
	main_dlg[dmStrBtnIdx].x = main_button_x;
	main_dlg[dmStrBtnIdx].y = exit_y - 9 * BUTTON_SIZE;
	main_dlg[dmExpBtnIdx].x = main_button_x;
	main_dlg[dmExpBtnIdx].y = exit_y - 8 * BUTTON_SIZE;
	main_dlg[dmPlaceBtnIdx].x = main_button_x;
	main_dlg[dmPlaceBtnIdx].y = exit_y - 7 * BUTTON_SIZE;
	main_dlg[dmUnitBtnIdx].x = main_button_x;
	main_dlg[dmUnitBtnIdx].y = exit_y - 6 * BUTTON_SIZE;
	main_dlg[dmUlistBtnIdx].x = main_button_x;
	main_dlg[dmUlistBtnIdx].y = exit_y - 5 * BUTTON_SIZE;
	main_dlg[dmScenBtnIdx].x = main_button_x;
	main_dlg[dmScenBtnIdx].y = exit_y - 4 * BUTTON_SIZE;
	main_dlg[dmLoadBtnIdx].x = main_button_x;
	main_dlg[dmLoadBtnIdx].y = exit_y - 3 * BUTTON_SIZE;
	main_dlg[dmSaveBtnIdx].x = main_button_x;
	main_dlg[dmSaveBtnIdx].y = exit_y - 2 * BUTTON_SIZE;
	main_dlg[dmExitBtnIdx].x = main_button_x;
	main_dlg[dmExitBtnIdx].y = exit_y - 1 * BUTTON_SIZE;
	//bmp
	destroy_bitmap(map_bmp);

	tiles_wide = (w - 80) / TILE_WIDTH;
	if (w == 800)
		tiles_wide -= 1;
	tiles_high = (h - TILE_HEIGHT / 2 - 54 - top_spacer - bottom_spacer)/ TILE_HEIGHT;

	map_h = (tiles_high * 2 + 1) * TILE_HEIGHT / 2 + 1;
	map_w = (tiles_wide * 3 / 2 + 1) * 30 - ((tiles_wide % 2) ? 0 : 15);

	map_bmp = create_bitmap(map_w, map_h);
	rectfill(map_bmp, 0, 0, map_w, map_h, MAP_COLOR);
	main_dlg[dmMapBmpIdx].y = LINE_2_Y + top_spacer;
	main_dlg[dmMapBmpIdx].dp = map_bmp;
	main_dlg[dmMapBmpIdx].w = map_w;
	main_dlg[dmMapBmpIdx].h = map_h;

	if (w <= 640)
		hslide_x = HSLIDE_X;
	else
		hslide_x = HSLIDE_X + 8 * 6;

	main_dlg[dmMapInfoTxtIdx].y = top_spacer;
	main_dlg[dmMapInfoTxtIdx].w = hslide_x - 6 - HSLIDE_H; //HSLIDE_H??
	//hslide + right,left arrow
	hslide_w = w - (440 - HSLIDE_X) - hslide_x;
	main_dlg[dmRightArrowIdx].x = hslide_x + 2 + hslide_w;
	main_dlg[dmRightArrowIdx].y = LINE_1_Y + top_spacer;
	main_dlg[dmLeftArrowIdx].x = hslide_x - 2 - HSLIDE_H;
	main_dlg[dmLeftArrowIdx].y = HSLIDE_Y + top_spacer;

	main_dlg[dmHSlideIdx].x = hslide_x;
	main_dlg[dmHSlideIdx].y = LINE_1_Y + top_spacer;
	main_dlg[dmHSlideIdx].w = hslide_w;
	main_dlg[dmHSlideIdx].d1 = get_h_slide_max();
	main_dlg[dmHSlideIdx].d2 = 0;
	//vslide + up/down arrows
	vslide_x = w - 80;
	vslide_h = h - 280;
	main_dlg[dmUpArrowIdx].x = vslide_x;
	main_dlg[dmUpArrowIdx].y = VSLIDE_Y - VSLIDE_W - 2 + top_spacer;
	main_dlg[dmDownArrowIdx].x = vslide_x;
	main_dlg[dmDownArrowIdx].y = VSLIDE_Y + top_spacer + 2 + vslide_h;

	main_dlg[dmVSlideIdx].x = vslide_x;
	main_dlg[dmVSlideIdx].y = VSLIDE_Y + top_spacer;
	main_dlg[dmVSlideIdx].h = vslide_h;
	vslide_max = get_v_slide_max();
	;
	main_dlg[dmVSlideIdx].d1 = vslide_max;
	main_dlg[dmVSlideIdx].d2 = vslide_max;
	map_x0 = 0;
	map_y0 = 0;
	//GndAir
	main_dlg[dmGndRBIdx].x = hslide_x + (hslide_w - 200) / 2;
	main_dlg[dmGndRBIdx].y = LINE_1_Y - 13 + top_spacer;
	main_dlg[dmAirRBIdx].x = hslide_x + (hslide_w - 200) / 2 + 50;
	main_dlg[dmAirRBIdx].y = LINE_1_Y - 13 + top_spacer;
	main_dlg[dmSeaRBIdx].x = hslide_x + (hslide_w - 200) / 2 + 2 * 50;
	main_dlg[dmSeaRBIdx].y = LINE_1_Y - 13 + top_spacer;
	main_dlg[dmNoneRBIdx].x = hslide_x + (hslide_w - 200) / 2 + 3 * 50 + 8;
	main_dlg[dmNoneRBIdx].y = LINE_1_Y - 13 + top_spacer;
	//mapinfo+ mapstatus
	main_dlg[dmMapStatusIdx].x = w - 200 - 10 + 7;
	main_dlg[dmMapStatusIdx].y = 2 + top_spacer;

	//dmFilterStatusTxtIdx
	main_dlg[dmFilterStatusTxtIdx].h = top_spacer;
	main_dlg[dmFilterStatusTxtIdx].w = 8 * 9 * 2;
	main_dlg[dmFilterStatusTxtIdx].x = w - main_dlg[dmFilterStatusTxtIdx].w - 3;
	main_dlg[dmFilterStatusTxtIdx].y = 2/*+48-1+top_spacer*/;

	//core+aux+allied+allied aux
	main_dlg[dmCoreRBIdx].x = hslide_x + (hslide_w - 200) / 2;
	main_dlg[dmCoreRBIdx].y = 2 + top_spacer;
	main_dlg[dmAuxRBIdx].x = hslide_x + (hslide_w - 200) / 2 + 50;
	main_dlg[dmAuxRBIdx].y = 2 + top_spacer;
	main_dlg[dmAlliedRBIdx].x = hslide_x + (hslide_w - 200) / 2 + 50 * 2 - 8;
	main_dlg[dmAlliedRBIdx].y = 2 + top_spacer;
	main_dlg[dmAlliedAuxRBIdx].x = hslide_x + (hslide_w - 200) / 2 + 50 * 3 + 12
			+ 8 - 8 - 4;
	main_dlg[dmAlliedAuxRBIdx].y = 2 + top_spacer;

	//finally draw map
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);

	//we need this due to bug in Allegro 4.4.0.0
	//when invoking from 2 level menu center-dialog will not work properly
	//centre_dialog(savebmp);
}

int ctrl_g_keycallback() {
	int card, w, h;
	if (gfx_mode_select(&card, &w, &h)) {
		//set_color_mode();
		if (set_gfx_mode(card, w, h, 0, 0)) {
			set_gfx_mode(card_old, w_old, h_old, 0, 0);
		} else {
			card_old = card;
			w_old = w;
			h_old = h;
			position_gui_elements(w_old, h_old);

		}
		/*
		 set_color_depth(8);
		 set_color_conversion(COLORCONV_REDUCE_TO_256);
		 set_pallete(pgpal);
		 */
		set_pal();

		initialize_dm();
		show_mouse(screen);
	}
	return D_REDRAW;
}

int check_terrain_mask(unsigned char tile, unsigned short mask) {
	int i;

	for (i = 0; i < total_tiles; i++)
		if ((FilterTiles_Max_Tiles[i] & mask) && tile == i) {
			return 1;
		}
	return 0;
}

int check_terrain(unsigned char tile, unsigned char filterType) {
	//int i;

	if (filterType == NO_FILTER_INDEX)
		return 0;

	return check_terrain_mask(tile,1 << filterType);

	/*
	for (i = 0; i < MAX_TILES_IN_PG; i++)
		if ((FilterTiles_Max_Tiles[i] & mask) && tile == i) {
			return 1;
		}
		*/
	/*
	for (i = tiles_filter[filterType][0]; i <= tiles_filter[filterType][1]; i++)
		if (tiles_display[i] == tile)
			return 1;
	*/
	return 0;
}

void set_ntile(int x, int y, int no, int value){
	tempmap[x + dx_tab_N_CW[no]][y + dy_tab_N_CW[no][x % 2]]+=1;
	map[x + dx_tab_N_CW[no]][y + dy_tab_N_CW[no][x % 2]].tile=value;
}

int is_magic_tile(int tile, int mode){
	if (mode==LIMITED_SET_RIVER && tile==MAGIC_RIVER) return 1;
	if (mode==LIMITED_SET_ROAD && tile==MAGIC_ROAD) return 1;
	if (mode==LIMITED_SET_ROAD_AND_RIVER && tile==MAGIC_ROAD_AND_RIVER) return 1;
	if (mode==LIMITED_SET_ALL && ( tile==MAGIC_RIVER || tile==MAGIC_ROAD || tile==MAGIC_ROAD_AND_RIVER )) return 1;
	return 0;
}

void gen_magic_river3(int mode){
	int x,y,i,j,found,limit, border=0;
		int row[32];
		int count=0,idx,gc=0,iter=0;

		do {
			gc=0;
		for (x = border; x < mapx-border ; ++x)
			for (y = border; y < mapy-border ; ++y)
				if (is_magic_tile(map[x][y].tile,mode)){
					limit=2;
					row[1]=0;
					row[0]=map[x][y].tile;
					int r=2;
					for (i=0;i<=r;i++)
							for (j=-r+i/2;j<=r-(i+1)/2;j++){
								found = is_tile_in_limited_set(get_tile_offmap(x+i,y+j+x%2*(x+i+1)%2),mode);
								row[1] |= (found >-1);
								row[1] = row[1] << 1;
								row[limit]=found;
								limit++;
								found = is_tile_in_limited_set(get_tile_offmap(x-i,y+j+x%2*(x+i+1)%2),mode);
								row[1] |= (found >-1);
								row[1] = row[1] << 1;
								row[limit]=found;
								limit++;
							}
					//pattern is computed, now find it in big table  rc_big_table[2652][26]
					count=0;
					int first=-1;
					idx=-1;
					for(i=0;i<rc_big_table_size;i++){
						if (rc_big_table[i][1] == row[1] && is_same_set(row[0],rc_big_table[i][0])) {
							int total_match=1;
							for(j=0;j<24;j++){
								if (!is_magic_tile(row[2+j],mode) && row[2+j]!=rc_big_table[i][2+j]) { total_match=0; break; }
							}
							if (total_match){
								idx=i;
								count++;
							}
							if (count==1){
								first=rc_big_table[i][0];
							}
							if (count>1 && first==rc_big_table[i][0]) count--;
						}
					}
					if (count == 1) {
						map[x][y].tile = rc_big_table[idx][0];
						map[x][y].gln = iter;
						gc++;
					}
					map[x][y].utr=count;
				}
			iter++;
			print_dec(gc);
		}while(gc>0 && iter<100);
		print_dec(iter);
}

void gen_magic_river2(){

	int x, y, i, j, k, t, ok;
	int tt[6];

	int pattern[][2][7]= {
			//{ { -1, -1, -1, -1, -1, -1, -1},{ -1, -1, -1, -1, -1, -1, -1} },

		{ { 72, -1, -1, 65, 54, -1, -1},{ -1, -1, -1, 64, -1, -1, -1} },
		{ { 72, -1, -1,227, 54, -1, -1},{ -1, -1, -1, 64, -1, -1, -1} },
		{ { 72, -1, 72, 62, -1, -1, -1},{ 65, -1, 62, -1, -1, -1, -1} },
		{ { 72, -1, -1, 54, 62, -1, -1},{ -1, -1, -1, 71, -1, -1, -1} },
		{ { 62, -1, -1,227,227, -1, -1},{ 72, -1, -1, 64, 71, -1, -1} },

		{ { 62, -1, -1, 65,227, -1, -1},{ 72, -1, -1, 64, 71, -1, -1} },
		{ { 71, -1, 61, 62, -1, -1, -1},{227,227, -1, -1, -1, -1, -1} },
		{ { 71, -1, 72, 62, -1, -1, -1},{227, -1, 62, -1, -1, -1, -1} },
		{ { 62, -1, -1,227, 62, -1, -1},{ 72, -1, -1, 64, 71, -1, -1} },
		{ { 62, -1, -1, 65, 62, -1, -1},{ 72, -1, -1, 64, 71, -1, -1} },

		{ { 61, -1, 61, 64, -1, -1, -1},{ -1, -1,227, -1, -1, -1, -1} },
		{ { 62, 62, 55, -1, -1, -1, -1},{227, 72, -1, -1, -1, -1, -1} },
		};
	int patterns_no=12;

	//for (y = 1; y < mapy - 1; ++y)
	//	for (x = 1; x < mapx - 1; ++x)
	//		tempmap[x][y]=0;

	for (y = 1; y < mapy - 1; ++y)
			for (x = 1; x < mapx - 1; ++x) {
				t=map[x][y].tile;
				if ( t==72 || t==62 || t==71  || t==61){
					for(i=0;i<6;i++)
						tt[i]=map[x + dx_tab_N_CW[i]][y + dy_tab_N_CW[i][x % 2]].tile;

					for(j=0;j<patterns_no;j++){
						if (t == pattern[j][0][0]){
							//check others
							ok=1;

							for(k=0;k<6;k++){
								//ti= map[x + dx_tab_N_CW[k]][y + dy_tab_N_CW[k][x % 2]].tile;
								if (pattern[j][0][1+k]!=-1 && pattern[j][0][1+k]!=tt[k]){
									//fail
									/*if (y==32 && j==5){
										print_dec(pattern[j][0][1+k]);
										print_dec(tt[k]);
									print_dec(k);
									print_dec(x);
									print_dec(y);
									print_dec(j);
									}*/
									ok=0;
									break;
								}
							}
							if (ok){
								if (pattern[j][1][0]!=-1)
									map[x][y].tile=pattern[j][1][0];
								for(k=0;k<6;k++){
									if (pattern[j][1][1+k]!=-1){
										map[x + dx_tab_N_CW[k]][y + dy_tab_N_CW[k][x % 2]].tile =pattern[j][1][1+k];
									}
								}
								break; //j<patterns_no
							}
						}
					}
				}
			}

	//for (y = 1; y < mapy - 1; ++y)
	//	for (x = 1; x < mapx - 1; ++x)
	//		map[x][y].utr = tempmap[x][y];


}
void gen_magic_river(){

	int x, y, i;
	int tt[6];
	for (y = mapy-2; y >  0; y--)
			for (x = mapx-2; x > 0; x--) {
				int t=map[x][y].tile;
				if (t==72 || t==62 || t==71){
					for(i=0;i<6;i++)
						tt[i]=map[x + dx_tab_N_CW[i]][y + dy_tab_N_CW[i][x % 2]].tile;
					if (t==72 && (tt[2]==65 || tt[2]==227) && tt[3]==54){
						map[x + dx_tab_N_CW[2]][y + dy_tab_N_CW[2][x % 2]].tile=64;
					}
					if (t==72 && tt[1]==72 && tt[2]==62){
						map[x][y].tile=65;
						map[x + dx_tab_N_CW[1]][y + dy_tab_N_CW[1][x % 2]].tile=62;
					}
					if (t==72 && tt[2]==54 && tt[3]==62){
						map[x + dx_tab_N_CW[3]][y + dy_tab_N_CW[3][x % 2]].tile=71;
					}
					if (t==62 &&  (( tt[2]==227 &&  tt[3]==227 ) || (tt[2]==65 && tt[3]==227 ))){
						map[x][y].tile=72;
						map[x + dx_tab_N_CW[2]][y + dy_tab_N_CW[2][x % 2]].tile=64;
						map[x + dx_tab_N_CW[3]][y + dy_tab_N_CW[3][x % 2]].tile=71;
					}
					if (t==71 && tt[2]==62 && tt[1]==61){
						map[x][y].tile=227;
						map[x + dx_tab_N_CW[1]][y + dy_tab_N_CW[1][x % 2]].tile=227;
					}
					if (t==71 && tt[2]==62 && tt[1]==72){
						map[x][y].tile=227;
						map[x + dx_tab_N_CW[1]][y + dy_tab_N_CW[1][x % 2]].tile=62;
					}
				}
			}
	//loop 2
	for (y = mapy-2; y >  0; y--)
			for (x = mapx-2; x > 0; x--) {
				int t=map[x][y].tile;
				if (t==62 ){
					for(i=0;i<6;i++)
						tt[i]=map[x + dx_tab_N_CW[i]][y + dy_tab_N_CW[i][x % 2]].tile;

					if (t==62 &&  ( tt[2]==227 || tt[2]==65)&&  tt[3]==62 ){
						map[x][y].tile=72;
						map[x + dx_tab_N_CW[2]][y + dy_tab_N_CW[2][x % 2]].tile=64;
						map[x + dx_tab_N_CW[3]][y + dy_tab_N_CW[3][x % 2]].tile=71;
					}

				}
			}
}


void gen_terrain(short tile_to_check,
		unsigned char pattern_array_SE_CCW[],
		unsigned char pattern_array_size,
		short pattern_tile[][3],
		unsigned char filterType) {

	//short tempmap[MAX_MAP_X][MAX_MAP_Y];
	int x, y;
	unsigned char i, mask;

//   srand(rawclock());
	srand((unsigned int) time((time_t *) NULL));
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			tempmap[x][y] = map[x][y].tile;
			if (map[x][y].tile == tile_to_check) {
				mask = 0;

				if ((map[x - 1][y - 1 + x % 2].tile == tile_to_check) || (check_terrain(map[x - 1][y - 1 + x % 2].tile, filterType)))
					mask += 0x20;
				if ((map[x][y - 1].tile == tile_to_check) || (check_terrain(map[x][y - 1].tile, filterType)))
					mask += 0x10;
				if ((map[x + 1][y - 1 + x % 2].tile == tile_to_check) || (check_terrain(map[x + 1][y - 1 + x % 2].tile, filterType)))
					mask += 0x08;
				if ((map[x + 1][y + x % 2].tile == tile_to_check) || (check_terrain(map[x + 1][y + x % 2].tile, filterType)))
					mask += 0x04;
				if ((map[x][y + 1].tile == tile_to_check) || (check_terrain(map[x][y + 1].tile, filterType)))
					mask += 0x02;
				if ((map[x - 1][y + x % 2].tile == tile_to_check) || (check_terrain(map[x - 1][y + x % 2].tile, filterType)))
					mask += 0x01;

				for (i = 0; i < pattern_array_size; i++)
					if (mask == pattern_array_SE_CCW[i])
						if (pattern_tile[i][0] != NOT_USED_TILE) {
							tempmap[x][y] = pattern_tile[i][rand() % 3];
							break;
						}
			}
		}
	// road trick only
/*
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			if (tempmap[x][y] == MAGIC_ROAD) {
				//tempmap[x][y]=139;
				tempmap[x][y] = 129;
				if (x % 2)
					tempmap[x - 1][y] = 37;
				else
					tempmap[x - 1][y - 1] = 37;
			}
			if (tempmap[x][y] >= total_tiles) {
				tempmap[x][y] = tile_to_check;
			}
			//if (tempmap[x][y]>=MAX_TILES_IN_PG) tempmap[x][y]=BLACK_TILE;
		}
*/
	//copy the result
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			map[x][y].tile = tempmap[x][y];

}

int generate_dialog(){

			centre_dialog(generate_dlg);
			if (do_dialog(generate_dlg, -1) == 6) {
				if (tile_mode == 1) {
					tile_cleanup();
				}

				if ((generate_dlg[2].flags & D_SELECTED) == D_SELECTED)
					gen_terrain(coast_check, coast_pattern_SE_CCW, coast_size,
							coast_pattern_tile, COAST_FILTER_INDEX); //coast

				if ((generate_dlg[3].flags & D_SELECTED) == D_SELECTED)
					gen_terrain(road_check, road_pattern_SE_CCW, road_size,
							road_pattern_tile, CITY_FILTER_INDEX); //road

				if ((generate_dlg[4].flags & D_SELECTED) == D_SELECTED){
					gen_terrain(river_check, river_pattern_SE_CCW, river_size,
							river_pattern_tile, NO_FILTER_INDEX); //river
					gen_magic_river2();
				}
/*
				if ((generate_dlg[7].flags & D_SELECTED) == D_SELECTED){
					gen_magic_river3(LIMITED_SET_ALL);
				}
*/
				draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
			}
			return D_REDRAW;
}


int ctrl_alt_m_keycallback() {
	if (key_shifts & KB_ALT_FLAG) {

		sprintf(MapStatusTxt, "Move mode\nLeft click unit");
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;

		move_init();
		edit_mode_trigger();
	}


	if (key_shifts & KB_CTRL_FLAG) {
		generate_dialog();
	}
	return D_REDRAW;
}

void do_fix_roads() {
	int x, y;

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			map[x][y].rc = find_road_mask(x, y);

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			tempmap[x][y] = find_one_way_road(x, y);

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			if (map[x][y].rc > 0) {
				map[x][y].rc = (unsigned char) tempmap[x][y];
				map[x][y].rc = filter_roads(x, y);
			}
}

void do_fix_names() {
	int x, y;
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (is_tile_name_standard(map[x][y].gln)
					&& NData_Max_Tiles[map[x][y].tile] > -1)
				map[x][y].gln = NData_Max_Tiles[map[x][y].tile];
}

void do_fix_names_hard() {
	int x, y;
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].gln > total_names
					&& NData_Max_Tiles[map[x][y].tile] > -1)
				map[x][y].gln = NData_Max_Tiles[map[x][y].tile];
}

void do_fix_terrain_types() {
	int x, y;
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (TTData_Max_Tiles[map[x][y].tile] > -1)
				map[x][y].utr = TTData_Max_Tiles[map[x][y].tile];
}

void do_fix_owner(int from, int to) {
	int new_side = 0;
	int i, x, y;

	for (i = 0; i < 6; i++) {
		if (scn_buffer[i * 2 + 1] == to)
			new_side = 1;
		if (Neutral[i] == to)
			new_side = 3;
	}

	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].own == from) {
				map[x][y].own = to;
				map[x][y].side = new_side;
			}
}

short get_id_from_2_bytes(int i) {
	return s4_buffer[i] + s4_buffer[i + 1] * 256;
}

void set_id_from_2_bytes(int i, int value) {
	s4_buffer[i] = value & 0xff;
	s4_buffer[i + 1] = value / 256;
}

void do_fix_sea_air_transport() {

	//int deploy_number_on_sea=0;
	//int deploy_number_occupied_on_sea=0;
	int axis_aux_used_sea_transports=0;
	int axis_aux_used_air_transports=0;
	int axis_core_used_sea_transports=0;
	int axis_core_used_air_transports=0;
	int allied_core_used_sea_transports=0;
	int allied_core_used_air_transports=0;
	int allied_aux_used_sea_transports=0;
	int allied_aux_used_air_transports=0;
	int i;
	//check for default air, sea transports

	if (s4_buffer[AXIS_AIR_NUMBER] > 0
			&& get_id_from_2_bytes(AXIS_AIR_TYPE) == 0)
		set_id_from_2_bytes(AXIS_AIR_TYPE, DEFAULT_AXIS_AIR_TYPE);
	if (s4_buffer[AXIS_SEA_NUMBER] > 0
			&& get_id_from_2_bytes(AXIS_SEA_TYPE) == 0)
		set_id_from_2_bytes(AXIS_SEA_TYPE, DEFAULT_AXIS_SEA_TYPE);
	if (s4_buffer[ALLIED_AIR_NUMBER] > 0
			&& get_id_from_2_bytes(ALLIED_AIR_TYPE) == 0)
		set_id_from_2_bytes(ALLIED_AIR_TYPE, DEFAULT_ALLIED_AIR_TYPE);
	if (s4_buffer[ALLIED_SEA_NUMBER] > 0
			&& get_id_from_2_bytes(ALLIED_SEA_TYPE) == 0)
		set_id_from_2_bytes(ALLIED_SEA_TYPE, DEFAULT_ALLIED_SEA_TYPE);

	// check for number of available air/sea transports vs needs
	//deploy_number_on_sea=count_deploy_hexes_on_sea();
	//deploy_number_occupied_on_sea=count_deploy_hexes_occupied_on_sea();

	axis_core_used_sea_transports=0;
	axis_core_used_air_transports=0;
	for(i=0;i<total_axis_core;i++){
		if (get_id_from_2_bytes(AXIS_AIR_TYPE)!=0)
			if (all_units[findOffset_ex(axis_core)+i].auxtnum==get_id_from_2_bytes(AXIS_AIR_TYPE))
				axis_core_used_air_transports++;
		if (get_id_from_2_bytes(AXIS_SEA_TYPE)!=0)
			if (all_units[findOffset_ex(axis_core)+i].auxtnum==get_id_from_2_bytes(AXIS_SEA_TYPE))
				axis_core_used_sea_transports++;
	}
	axis_aux_used_sea_transports=0;
	axis_aux_used_air_transports=0;
	for(i=0;i<total_axis_aux;i++){
		if (get_id_from_2_bytes(AXIS_AIR_TYPE)!=0)
			if (all_units[findOffset_ex(axis_aux)+i].auxtnum==get_id_from_2_bytes(AXIS_AIR_TYPE))
				axis_aux_used_air_transports++;
		if (get_id_from_2_bytes(AXIS_SEA_TYPE)!=0)
			if (all_units[findOffset_ex(axis_aux)+i].auxtnum==get_id_from_2_bytes(AXIS_SEA_TYPE))
				axis_aux_used_sea_transports++;
	}

	allied_core_used_sea_transports=0;
	allied_core_used_air_transports=0;
	for(i=0;i<total_allied_core;i++){
		if (get_id_from_2_bytes(ALLIED_AIR_TYPE)!=0)
			if (all_units[findOffset_ex(allied_core)+i].auxtnum==get_id_from_2_bytes(ALLIED_AIR_TYPE))
				allied_core_used_air_transports++;
		if (get_id_from_2_bytes(ALLIED_SEA_TYPE)!=0)
			if (all_units[findOffset_ex(allied_core)+i].auxtnum==get_id_from_2_bytes(ALLIED_SEA_TYPE))
				allied_core_used_sea_transports++;
	}
	allied_aux_used_sea_transports=0;
	allied_aux_used_air_transports=0;
	for(i=0;i<total_allied_aux;i++){
		if (get_id_from_2_bytes(ALLIED_AIR_TYPE)!=0)
		if (all_units[findOffset_ex(allied_aux)+i].auxtnum==get_id_from_2_bytes(ALLIED_AIR_TYPE))
			allied_aux_used_air_transports++;
		if (get_id_from_2_bytes(ALLIED_SEA_TYPE)!=0)
		if (all_units[findOffset_ex(allied_aux)+i].auxtnum==get_id_from_2_bytes(ALLIED_SEA_TYPE))
			allied_aux_used_sea_transports++;
	}

//	print_dec(s4_buffer[AXIS_SEA_NUMBER]-axis_aux_used_sea_transports);
//	print_dec(deploy_number_on_sea-deploy_number_occupied_on_sea-scn_buffer[CORE_UNITS_LIMIT]);
//	if (s4_buffer[AXIS_SEA_NUMBER]-axis_aux_used_sea_transports>deploy_number_on_sea-deploy_number_occupied_on_sea-scn_buffer[CORE_UNITS_LIMIT]){
//		print_str("Sea axis problem\n");
//	}


}

void list_conversion_of_units(int hide_names, int try_harder, int print_result) {

	//uint8_t ctry[MAX_COUNTRY];
	int i, error, result, new_country;
	int unum,/*own,*/orgtnum, auxtnum;
	unsigned char country;

	if (print_result)
		printf("Loading PGF equipment...\n");
	error = load_pgf_equipment(LOAD_CONVERSION_TABLE_ONLY, pgf_equip_file);
	if (error) {
		if (print_result)
			printf("Not found. Aborting.\n");
		return;
	}
	if (print_result)
		printf("Done.\n");
	qsort(conversion_equip, conversion_total_equip,
			sizeof(struct conversion_equipment), compare_by_name_and_country);

	//set side countries
	//memset(ctry,0,sizeof(ctry));

	//for(i=0;i<12;i++){
	//	if (scn_buffer[i]!=0) ctry[scn_buffer[i]]=1+i%2;
	//}

	for (i = 0; i < total_units; i++) {
		unum = all_units[i].unum;
		orgtnum = all_units[i].orgtnum;
		auxtnum = all_units[i].auxtnum;
		country = all_units[i].country;

		//own = all_units[i].uflag==allied_core?2:1;

		result = convert_equipment_hard(&unum, &new_country, all_units[i].unum,
				country, try_harder);
		if (print_result) {
			if (hide_names)
				printf("Er:%d  %d->%d, %d->%d\n", result, all_units[i].unum,
						unum, country, new_country);
			else
				printf(
						"Er:%d  |%s|->|%s|, |%s|->|%s|\n",
						result,
						equip[all_units[i].unum],
						equip[unum],
						country_names[country],
						new_country > 0 ?
								country_names[new_country] :
								(new_country == 0 ? country_names[country] : "*"));
		}
		if (result == CONVERSION_FOUND_NEW_COUNTRY)
			country = new_country;

		if (all_units[i].orgtnum != 0) {
			result = convert_equipment_hard(&orgtnum, &new_country,
					all_units[i].orgtnum, country, try_harder);
			if (print_result) {
				if (hide_names)
					printf("Or:%d  %d->%d, %d->%d\n", result,
							all_units[i].orgtnum, orgtnum, country,
							new_country);
				else
					printf("Or:%d  |%s|->|%s|, |%s|->|%s|\n", result,
							equip[all_units[i].orgtnum], equip[orgtnum],
							country_names[country],
							new_country > 0 ? country_names[new_country] : "*");
			}
		}

		if (all_units[i].auxtnum != 0) {
			result = convert_equipment_hard(&auxtnum, &new_country,
					all_units[i].auxtnum, country, try_harder);
			if (print_result) {
				if (hide_names)
					printf("Tr:%d  %d->%d, %d->%d\n", result,
							all_units[i].auxtnum, auxtnum, country,
							new_country);
				else
					printf("Tr:%d  |%s|->|%s|, |%s|->|%s|\n", result,
							equip[all_units[i].auxtnum], equip[auxtnum],
							country_names[country],
							new_country > 0 ? country_names[new_country] : "*");
			}
		}
	}
	return;
}

int ctrl_q_keycallback() {

	centre_dialog(rtnfix_dlg);
	if (do_dialog(rtnfix_dlg, -1) == 6) {
		if (tile_mode == 1) {
			tile_cleanup();
		}

		if ((rtnfix_dlg[2].flags & D_SELECTED) == D_SELECTED) {
			do_fix_roads();
		}
		if ((rtnfix_dlg[3].flags & D_SELECTED) == D_SELECTED) {
			do_fix_terrain_types();
		}
		if ((rtnfix_dlg[4].flags & D_SELECTED) == D_SELECTED) {
			do_fix_names();
		}
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	return D_REDRAW;
}

int draw_new_spottings() {
	compute_shade(show_ranges);
	setup_show_filters_info();
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	select_show_ranges_item();
	return D_REDRAW;
}

int ctrl_k_keycallback() {
	show_ranges = (show_ranges + 1) % 3;
	return draw_new_spottings();

}

int ctrl_j_keycallback() {
	check_and_save_mapnames();
	if (load_names(DO_NOT_SHOW_LOGS) == 0) {
		sprintf(MapStatusTxt, "Names loaded!");
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
		//d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
	}
	draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	return D_REDRAW;
}

