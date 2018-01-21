/*
 * maputils.c
 *
 *  Created on: 07-12-2012
 *      Author: wino
 */

#include "fpge.h"
#include "tables.h"
#include "callback.h"

int is_tile_river(short x, short y) {
	/*
	 for (i = 0; i < river_tiles_size; i++) {
	 if (map[x][y].tile == river_tiles[i])
	 return 1;
	 }
	 */

	if (FilterTiles_Max_Tiles[map[x][y].tile] & (1 << RIVER_FILTER_INDEX))
		return 1;
	else
		return 0;
}

int is_tile_ocean(short x, short y) {
	int i;
	for (i = 0; i < ocean_tiles_size; i++) {
		if (map[x][y].tile == ocean_tiles[i])
			return 1;
	}
	return 0;
}

int is_tile_name_water(short x, short y) {

	if (is_tile_river(x, y))
		return 1;
	if (is_tile_ocean(x, y))
		return 1;

	return 0;
}

int is_tile_name_standard(int gln) {
	int ret = 1;
	if ((gln > 13) && (gln != 280) && (gln != 281) && (gln != 282) && (gln != 283))
		ret = 0;
	if (gln > total_names)
		ret = 0;
	return ret;
}

int is_tile_a_city_tile(int tile){
	int found=0;

	if (check_terrain(tile, CITY_FILTER_INDEX)){
			found=1;
	}
	/*
	for (i = tiles_filter[CITY_FILTER_INDEX][0];i <= tiles_filter[CITY_FILTER_INDEX][1]; i++){
		if (tile == tiles_display[i]){
			found=1;
			break;
		}
	}
	*/
	return found;
}

int get_road_tile_index(int tile){
	int j;
	for (j = 0; j < roads_tiles_size; ++j)
		if (tile == road_connection_info[j].tile)
			return j;
	return -1;
}

int is_tile_road_tile(int tile){
	int j;
	for (j = 0; j < roads_tiles_size; ++j)
		if (tile == road_connection_info[j].tile)
			return 1;
	return 0;
}

int is_tile_passive_road_tile(int tile){

	int j;
	for (j = 0; j < roads_passive_tiles_size; ++j)
		if (tile == roads_passive_tiles[j])
			return 1;
	return 0;
}

int is_tile_passive_or_road_tile(int tile){
	if (is_tile_road_tile(tile))
		return 1;
	if (is_tile_passive_road_tile(tile))
		return 1;
	return 0;
}

void debug_dump_hex_pattern(int N, int NE, int SE, int S, int SW, int NW, int val){
/*
	printf("    __     \n");
	printf(" __/%2d\\__ \n", N);
	printf("/%2d\\__/%2d\\\n", NW, NE);
	printf("\\__/%2X\\__/\n", val);
	printf("/%2d\\__/%2d\\\n", SW, SE);
	printf("\\__/%2d\\__/\n", S);
	printf("   \\__/   \n");
*/
	printf("     ___      \n");
	printf(" ___/%3d\\___ \n", N);
	printf("/%3d\\___/%3d\\\n", NW, NE);
	printf("\\___/%3X\\___/\n", val);
	printf("/%3d\\___/%3d\\\n", SW, SE);
	printf("\\___/%3d\\___/\n", S);
	printf("    \\___/    \n");
}

void debug_dump_hex_pattern_bit(int N, int NE, int SE, int S, int SW, int NW, int val){
	debug_dump_hex_pattern(N?1:0, NE?1:0, SE?1:0, S?1:0, SW?1:0, NW?1:0, val);
}

void dump_scenario_to_csv(int dump_type,int dump_name,int dump_road,int dump_tiles,int dump_owner,int dump_victory,int dump_units, int dump_deploy){
	int x,y;
	char line[MAX_LINE_SIZE+1],tmp[MAX_LINE_SIZE+1];

	snprintf(line,MAX_LINE_SIZE,"x,y");
	if (dump_type){
		snprintf(tmp,MAX_LINE_SIZE,",ttype");
		strncat(line,tmp,MAX_LINE_SIZE);
	}
	if (dump_name){
		snprintf(tmp,MAX_LINE_SIZE,",name");
		strncat(line,tmp,MAX_LINE_SIZE);
	}
	if (dump_road){
		snprintf(tmp,MAX_LINE_SIZE,",road");
		strncat(line,tmp,MAX_LINE_SIZE);
	}
	if (dump_tiles){
		snprintf(tmp,MAX_LINE_SIZE,",tile");
		strncat(line,tmp,MAX_LINE_SIZE);
	}
	if (dump_owner){
		snprintf(tmp,MAX_LINE_SIZE,",owner");
		strncat(line,tmp,MAX_LINE_SIZE);
	}
	if (dump_victory){
		snprintf(tmp,MAX_LINE_SIZE,",victory");
		strncat(line,tmp,MAX_LINE_SIZE);
	}
	if (dump_deploy){
		snprintf(tmp,MAX_LINE_SIZE,",deploy");
		strncat(line,tmp,MAX_LINE_SIZE);
	}
	if (dump_units){
		snprintf(tmp,MAX_LINE_SIZE,",unitid,str,exp,entrench,country,unitflag,auxtnum,orgtnum");
		strncat(line,tmp,MAX_LINE_SIZE);
	}
	printf("%s\n",line);

	for (x=0;x<mapx;x++)
		for (y=0;y<mapy;y++){
			snprintf(line,MAX_LINE_SIZE,"%2d,%2d",x,y);
			if (dump_type){
				snprintf(tmp,MAX_LINE_SIZE,",%2d",map[x][y].utr);
				strncat(line,tmp,MAX_LINE_SIZE);
			}
			if (dump_name){
				snprintf(tmp,MAX_LINE_SIZE,",%4d",map[x][y].gln);
				strncat(line,tmp,MAX_LINE_SIZE);
			}
			if (dump_road){
				snprintf(tmp,MAX_LINE_SIZE,",%3d",map[x][y].rc);
				strncat(line,tmp,MAX_LINE_SIZE);
			}
			if (dump_tiles){
				snprintf(tmp,MAX_LINE_SIZE,",%3d",map[x][y].tile);
				strncat(line,tmp,MAX_LINE_SIZE);
			}
			if (dump_owner){
				snprintf(tmp,MAX_LINE_SIZE,",%2d",map[x][y].own);
				strncat(line,tmp,MAX_LINE_SIZE);
			}
			if (dump_victory){
				snprintf(tmp,MAX_LINE_SIZE,",%2d",map[x][y].vic);
				strncat(line,tmp,MAX_LINE_SIZE);
			}
			if (dump_deploy){
				snprintf(tmp,MAX_LINE_SIZE,",%2d",map[x][y].deploy);
				strncat(line,tmp,MAX_LINE_SIZE);
			}
			if (dump_units){
				int idx =-1;
				if (map[x][y].guidx>=0){
					idx = map[x][y].guidx;
				}
				if (map[x][y].auidx>=0){
					idx = map[x][y].auidx;
				}
				if (idx>0){
					snprintf(tmp,MAX_LINE_SIZE,",%3d,%2d,%2d,%2d,%2d,%1d,%3d,%3d",
							all_units[idx].unum,
							all_units[idx].str,
							all_units[idx].exp,
							all_units[idx].entrench,
							all_units[idx].country,
							all_units[idx].uflag,
							all_units[idx].auxtnum,
							all_units[idx].orgtnum
							);
				}else{
					snprintf(tmp,MAX_LINE_SIZE,",%3d,%2d,%2d,%2d,%2d,%1d,%3d,%3d",0,0,0,0,0,0,0,0);
				}
				strncat(line,tmp,MAX_LINE_SIZE);
			}
			printf("%s\n",line);
	}
}

