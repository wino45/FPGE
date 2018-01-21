/*
 * minimap.h
 *
 *  Created on: 2010-03-11
 *      Author: wino
 */

#ifndef MINIMAP_H_
#define MINIMAP_H_

#define TILE_HEIGHT_ 8
#define TILE_WIDTH_ 7
#define TILE_FULL_WIDTH_ 11


int load_mini_tiles();

int build_mini_uicons();
int load_mini_uicons();

int build_mini_vichex();
int load_mini_vichex();

void draw_mmap(BITMAP *map_to_draw,int x0, int y0, int tiles_high, int tiles_wide, int show_air, int show_vic, int show_shade, int show_units);
void make_mmap(int show_air, int show_vic, int show_shade, int show_units, int map_size);

void compute_shade(int side);

#endif /* MINIMAP_H_ */
