/*
 * drawmap.h
 *
 *  Created on: 2011-02-07
 *      Author: wino
 */

#ifndef DRAWMAP_H_
#define DRAWMAP_H_

void draw_tiles_matrix();

void draw_map_shift(BITMAP *,int, int, int, int, int);
void draw_map(BITMAP *,int,int,int,int);

void make_hexes();
int map_shift_x(int mode);
int map_shift_y(int mode);


#endif /* DRAWMAP_H_ */
