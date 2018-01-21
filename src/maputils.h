/*
 * maputils.h
 *
 *  Created on: 07-12-2012
 *      Author: wino
 */

#ifndef MAPUTILS_H_
#define MAPUTILS_H_

int is_tile_river(short x, short y);
int is_tile_ocean(short x, short y);
int is_tile_name_water(short x, short y);
int is_tile_name_standard(int gln);
int is_tile_a_city_tile(int tile);
int get_road_tile_index(int tile);
int is_tile_road_tile(int tile);
int is_tile_passive_road_tile(int tile);
int is_tile_passive_or_road_tile(int tile);
void debug_dump_hex_pattern(int N, int NE, int SE, int S, int SW, int NW, int val);
void debug_dump_hex_pattern_bit(int N, int NE, int SE, int S, int SW, int NW, int val);
void dump_scenario_to_csv(int dump_type,int dump_name,int dump_road,int dump_tiles,int dump_owner,int dump_victory,int dump_units,int dump_deply);
#endif /* MAPUTILS_H_ */
