/*
 * tilemixer.c
 *
 *  Created on: 25-12-2012
 *      Author: wino
 */
#include <allegro.h>
#include <stdio.h>
#include "fpge.h"
#include "loadpng.h"
#include "fpge_colors.h"

BITMAP *mask=NULL;

BITMAP *load_wesnoth_tile(char tile_name[]){

	char path[PATH_MAX]="C:\\Program Files (x86)\\Battle for Wesnoth 1.10.5\\data\\core\\images\\terrain\\";

	BITMAP *loaded_png;

	strncat(path,tile_name,PATH_MAX);

	loaded_png = load_bitmap(path, NULL);

	int x,y;
	if (mask!=NULL)
	for(x=0;x<loaded_png->w;x++)
		for(y=0;y<loaded_png->h;y++)
			if (x<mask->w && y<mask->h && getpixel(mask,x,y)==0xffffff)
				putpixel(loaded_png,x,y,fpge_mask_color);
	return loaded_png;
}

int init_hex_mask(){

	mask=load_wesnoth_tile("alphamask.png");

	return 1;
}

BITMAP *mix_tile_series(char series[][256], int size, int x, int y){
	int i;
	BITMAP *map;

	map=create_bitmap(x,y);
	clear_to_color(map,fpge_mask_color);

	for(i=0; i<size;i++){
		BITMAP *loaded_png = load_wesnoth_tile(series[i]);
		if (loaded_png){
			masked_blit(loaded_png, map, 0, 0, 0, 0, loaded_png->w, loaded_png->h);
			destroy_bitmap(loaded_png);
		}
	}
	return map;
}

void load_wtiles(){
	BITMAP *map, *src;
	char ser[][256]={"water\\coast-tile.png","water\\coast-tropical-A01-n.png"};
	//char ser[][256]={"alphamask.png"};
	int size=2;
	int tile_id=73;

	init_hex_mask();

	map=create_bitmap(60,50);
	clear_to_color(map,fpge_mask_color);

	src=mix_tile_series(ser,size,72,72);

	 masked_stretch_blit(src, map, 0, 0, 72, 72, 0, 0, 60, 50);

	 destroy_bitmap(til_bmp[tile_id]);
	 til_bmp[tile_id]=map;
	 destroy_bitmap(src);
}
