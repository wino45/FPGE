/*
 * tables.h
 *
 *  Created on: 2010-03-11
 *      Author: wino
 */

#ifndef TABLES_H_
#define TABLES_H_

#include "fpge.h"
#include "load.h"

#define MOV_TYPE_TRACKED 0
#define MOV_TYPE_HALFTRACKED 1
#define MOV_TYPE_WHEELED 2
#define MOV_TYPE_LED 3
#define MOV_TYPE_TOWED 4
#define MOV_TYPE_AIR 5
#define MOV_TYPE_NAVAL 6
#define MOV_TYPE_ALLTERRAIN 7
#define MOV_TYPE_AMPTRACKED 8
#define MOV_TYPE_AMPALLTERRAIN 9
#define MOV_TYPE_MOUNTAIN 10

#define CLASS_INFANTRY 0
#define CLASS_TANK 1
#define CLASS_RECON 2
#define CLASS_ANTI_TANK 3
#define CLASS_ARTILLERY 4
#define CLASS_ANTI_AIRCARFT 5
#define CLASS_AIR_DEFENCE 6
#define CLASS_FORTIFICATION 7
#define CLASS_FIGHTER 8
#define CLASS_TACTICAL_BOMBER 9
#define CLASS_LEVEL_BOMBER 10
#define CLASS_SUBMARINE 11
#define CLASS_DESTROYER 12
#define CLASS_CAPITAL_SHIP 13
#define CLASS_AIR_CARRIER 14
#define CLASS_LAND_TRANSPORT 15
#define CLASS_AIR_TRANSPORT 16
#define CLASS_SEA_TRANSPORT  17

extern enum UTYPES ubelongs;
extern char utype_name[4][16];
extern char target_type_name[4][6];

//extern int max_colors_for_bmp;
//extern int colors_for_bmp[][3];
//extern int tiles_for_bmp[][3];

extern unsigned char road_size;
extern unsigned char road_tt;
extern short road_check;
extern unsigned char road_pattern_SE_CCW[];
extern short road_pattern_tile[][3];

extern unsigned char coast_size;
extern unsigned char coast_tt;
extern short coast_check;
extern unsigned char coast_pattern_SE_CCW[];
extern short coast_pattern_tile[][3];

extern unsigned char river_size;
extern unsigned char river_tt;
extern short river_check;
extern unsigned char river_pattern_SE_CCW[];
extern short river_pattern_tile[][3];

extern short clear_check;

extern unsigned char forest_pattern_size;
extern short forest_tiles[];
extern unsigned char forest_pattern_SE_CCW[];

extern char pg_class_names[CLASS_NUMBER][16];
extern char country_names[MAX_COUNTRY][MAX_COUNTRY_NAME_SIZE];
extern char pacgen_country_names[MAX_COUNTRY][MAX_COUNTRY_NAME_SIZE];
extern char country_names_short[MAX_COUNTRY][MAX_COUNTRY_SHORT_NAME_SIZE];
extern char pacgen_country_names_short[MAX_COUNTRY][MAX_COUNTRY_SHORT_NAME_SIZE];
extern char country_side[MAX_COUNTRY];
extern char country_active[MAX_COUNTRY];
extern char bmp_to_country[MAX_UICONS];
extern char movement_type[MAX_MOV_TYPE][25];
extern char utr_names[MAX_TERRAIN_TYPE][TERRAIN_TYPE_SIZE];
extern char movement_terrain_names[MAX_TERRAIN_MOV_TYPES][TERRAIN_TYPE_SIZE];
extern char weather_str[MOVEMENT_TYPES_NO_COL][10];
extern char pacgen_movement_terrain_names[50][128];
extern char pacgen_movement_type[50][128];
extern char pacgen_weather_zones[20][128];;
extern char months_names[12][10];
extern char weather_params[4][10];
extern char weather_zones[4][20];

#define MOVEMENT_ALL 254
#define MOVEMENT_TYPE_UNKNOWN 254
#define MOVEMENT_NOT_POSSIBLE 255

extern unsigned char map_terrain_type_to_movement_terrain[];
extern unsigned char set_head[123];
extern unsigned short cp1250_to_utf[];
extern unsigned short tiles_display_max_tiles[MAX_TILES];
extern unsigned char tiles_display[MAX_TILES_IN_PG];
extern unsigned short NData[MAX_TILES_IN_PG];
extern unsigned short TTData[MAX_TILES_IN_PG];
extern short NData_Max_Tiles[MAX_TILES];
extern short TTData_Max_Tiles[MAX_TILES];
extern unsigned int FilterTiles_Max_Tiles[MAX_TILES];
extern unsigned int FilterTiles[MAX_TILES_IN_PG];
extern unsigned int MainCategoryTiles_Max_Tiles[MAX_TILES];
extern unsigned int MainCategoryTiles[MAX_TILES_IN_PG];
extern unsigned char RandomGroupTiles_Max_Tiles[MAX_TILES];
extern unsigned char RandomGroupTiles[MAX_TILES_IN_PG];

//extern int tiles_filter[][2];
//extern short roads_tiles[];
extern short roads_passive_tiles[];
extern short ocean_tiles[];
//extern short river_tiles[];

extern unsigned char roads_passive_tiles_size;
extern unsigned char roads_tiles_size;
extern short ocean_tiles_size;
//extern short river_tiles_size;

//extern unsigned char roads_tiles_mask_RC[];
//extern int roads_tiles_mask_N_CW[][6];;

extern int stack_icons_mapping[2][2][CLASS_NUMBER];

extern signed short orginal_nupl[PG_NUPL_SIZE];

extern int colors_for_bmp[][3];
extern int max_colors_for_bmp;
extern short tiles_for_bmp[][3];

extern RGB pacgen_pal[256];
extern RGB pgpal[256];

struct tile_connection_info {
 int tile;
 char pattern_NNW_CW[7];
 int corners_NNW_CW[6];
 int bits_RC;
};

extern int max_river_conversions_patterns;
extern struct tile_connection_info river_connection_info[];
extern int pat_bits_NNE_CW[6];
extern struct tile_connection_info road_connection_info[];

extern int dx_tab_N_CW[];
extern int dy_tab_N_CW[][2];
extern int dir_bit_mask_RC[];
extern int mask_conv_N_CW_to_SE_CCW[];

extern int filter_number_ingroup[];
extern int filter_number_current_ingroup[12];
extern int filter_tt_ingroup[][MAX_TERRAIN_TYPE];

extern int rc_big_table_size;
extern int rc_big_table[][26];

void init_tables();
void init_tables_post_load();

#endif /* TABLES_H_ */
