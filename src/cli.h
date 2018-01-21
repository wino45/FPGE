/*
 * cli.h
 *
 *  Created on: 2010-03-09
 *      Author: wino
 */

#ifndef CLI_H_
#define CLI_H_

#define LIMITED_SET_RIVER 0
#define LIMITED_SET_ROAD 1
#define LIMITED_SET_ROAD_AND_RIVER 2
#define LIMITED_SET_ALL 3

//extern unsigned char tempmap[MAX_MAP_X][MAX_MAP_Y];
//extern unsigned char rc[MAX_MAP_X][MAX_MAP_Y];
extern unsigned char tiles_temp[MAX_TILES];

int CountBits(int v);

int make_mini_tiles();
int prepare_mini_tiles();
int cli_parsing(int argc, char *argv[]);
void CLIHelp(char *VersionName);

void listing_units(int mode, int hide_names);
void checking_units(int no_log);
void checking_flags(int no_log);
void checking_ocean(int no_log, int);
void checking_river_names(int no_log);
void handle_units_bmp_saving(int units_bmp, int units_per_country_bmp, int bmp_idx,int end_bmp_idx, int country_idx, int m, int y, int comment_bmps, int flipIcons,int,int,int);

int get_tile_offmap(int,int);
int is_same_set(int tile, int tile2);
int is_tile_in_limited_set(short tile, int set);
void display_patterns(int pattern_type, int mode);

#endif /* CLI_H_ */
