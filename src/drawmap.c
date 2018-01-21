/*
 * drawmap.c
 *
 *  Created on: 2011-02-07
 *      Author: wino
 */

#include <allegro.h>

#include "fpge.h"
#include "fpge_colors.h"
#include "tables.h"
#include "drawmap.h"
#include "app6.h"
#include "move.h"

#define MAX_COLORIZE_PALETTE 8

//Red, Green, Blue, Yellow, Violet, Black, White, Orange
//we use PG palette
unsigned char colorize_PG[MAX_COLORIZE_PALETTE] = {22,10,1,14,5,0,15,25};

unsigned char gln_to_color[MAX_NAMES];

void do_colorize(int only_ocean){
	int x,y,i,j;

	memset(gln_to_color,0,sizeof(gln_to_color));

	//find all in map
	for (x=0;x<mapx;x++)
		for (y=0;y<mapy;y++){
			if (only_ocean){
				if (is_tile_ocean(x,y) )
					gln_to_color[Min(MAX_NAMES-1,map[x][y].gln)]=1;
			}else
				gln_to_color[Min(MAX_NAMES-1,map[x][y].gln)]=1; //all
			}
	j=0;
	for (i=0;i<MAX_NAMES;i++){
		if (gln_to_color[i]) {
			gln_to_color[i]=j;
			j=(j+1)%MAX_COLORIZE_PALETTE;
		}
	}
}

void do_colorize_ocean() {
	do_colorize(1);
}

void do_colorize_all() {
	do_colorize(0);
}

int is_tile_name_show(short x, short y){
    //int show=0;
    if (drawAllNames) return 1;
    if (drawNames)
    {
    if (!is_tile_name_standard(map[x][y].gln)){
       if (drawNames==1) return 1;
       if ((drawNames==2)&&(is_tile_name_water(x,y)==0)) return 1;
       if ((drawNames==3)&&(is_tile_name_water(x,y)==1)) return 1;
    }
    }
    return 0;
}

void make_hexes() {
	//make the victory hex
	vic_hex_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT+1);
	rectfill(vic_hex_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT+1,fpge_mask_color);
	//top part
	line(vic_hex_bmp,0,24,15,0,colors_to24bits(VIC_HEX_COLOR));
	line(vic_hex_bmp,45,0,59,24,colors_to24bits(VIC_HEX_COLOR));
	line(vic_hex_bmp,15,0,45,0,colors_to24bits(VIC_HEX_COLOR));
		//second line
	//line(vic_hex_bmp,1,24,16,0,VIC_HEX_COLOR);
	//line(vic_hex_bmp,45,1,59,25,VIC_HEX_COLOR);
	//line(vic_hex_bmp,15,1,44,1,VIC_HEX_COLOR);
	// bottom part
	line(vic_hex_bmp,0,24,15,50,colors_to24bits(VIC_HEX_COLOR));
	line(vic_hex_bmp,45,50,60,24,colors_to24bits(VIC_HEX_COLOR));
	line(vic_hex_bmp,15,50,45,50,colors_to24bits(VIC_HEX_COLOR));
	//line(vic_hex_bmp,1,24,16,49,VIC_HEX_COLOR);
	//line(vic_hex_bmp,44,49,59,24,VIC_HEX_COLOR);
	//line(vic_hex_bmp,15,48,45,48,VIC_HEX_COLOR);

	//make the neutral hex
	neutral_hex_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT+1);
	rectfill(neutral_hex_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT+1,fpge_mask_color);
	//top part
	line(neutral_hex_bmp,0,24,15,0,colors_to24bits(NEUTRAL_HEX_COLOR));
	line(neutral_hex_bmp,45,0,59,24,colors_to24bits(NEUTRAL_HEX_COLOR));
	line(neutral_hex_bmp,15,0,45,0,colors_to24bits(NEUTRAL_HEX_COLOR));

	// bottom part
	line(neutral_hex_bmp,0,24,15,50,colors_to24bits(NEUTRAL_HEX_COLOR));
	line(neutral_hex_bmp,45,50,60,24,colors_to24bits(NEUTRAL_HEX_COLOR));
	line(neutral_hex_bmp,15,50,45,50,colors_to24bits(NEUTRAL_HEX_COLOR));

	//make the deploy hex
	deploy_hex_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT+1);
	rectfill(deploy_hex_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT+1,fpge_mask_color);
	//top part
	line(deploy_hex_bmp,0,24,15,0,colors_to24bits(DEPLOY_HEX_COLOR));
	line(deploy_hex_bmp,45,0,59,24,colors_to24bits(DEPLOY_HEX_COLOR));
	line(deploy_hex_bmp,15,0,45,0,colors_to24bits(DEPLOY_HEX_COLOR));

	// bottom part
	line(deploy_hex_bmp,0,24,15,50,colors_to24bits(DEPLOY_HEX_COLOR));
	line(deploy_hex_bmp,45,50,60,24,colors_to24bits(DEPLOY_HEX_COLOR));
	line(deploy_hex_bmp,15,50,45,50,colors_to24bits(DEPLOY_HEX_COLOR));

	//make the black hex
	black_hex_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
	rectfill(black_hex_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT,fpge_mask_color);
	//top part
	line(black_hex_bmp,0,24,15,0,colors_to24bits(BLACK_HEX_COLOR));
	line(black_hex_bmp,45,0,59,24,colors_to24bits(BLACK_HEX_COLOR));
	line(black_hex_bmp,15,0,45,0,colors_to24bits(BLACK_HEX_COLOR));
	// bottom part
	line(black_hex_bmp,0,24,15,49,colors_to24bits(BLACK_HEX_COLOR));
	line(black_hex_bmp,45,49,60,24,colors_to24bits(BLACK_HEX_COLOR));
	line(black_hex_bmp,15,49,45,49,colors_to24bits(BLACK_HEX_COLOR));
	floodfill(black_hex_bmp,60/2,50/2,colors_to24bits(BLACK_HEX_COLOR));

	road_hex_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
	rectfill(road_hex_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT,fpge_mask_color);
	masked_blit(black_hex_bmp,road_hex_bmp,0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);
	rectfill(road_hex_bmp,3,TILE_HEIGHT/2-2,TILE_FULL_WIDTH-3,TILE_HEIGHT/2+2,colors_to24bits(MAGIC_ROAD_COLOR));

	river_hex_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
	rectfill(river_hex_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT,fpge_mask_color);
	masked_blit(black_hex_bmp,river_hex_bmp,0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);
	rectfill(river_hex_bmp,TILE_FULL_WIDTH/2-2,3,TILE_FULL_WIDTH/2+2,TILE_HEIGHT-3,colors_to24bits(MAGIC_RIVER_COLOR));

	road_and_river_hex_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
	rectfill(road_and_river_hex_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT,fpge_mask_color);
	masked_blit(river_hex_bmp,road_and_river_hex_bmp,0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);
	rectfill(road_and_river_hex_bmp,3,TILE_HEIGHT/2-2,TILE_FULL_WIDTH-3,TILE_HEIGHT/2+2,colors_to24bits(MAGIC_ROAD_COLOR));

	//make the problem hex
	problem_hex_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT+1);
	rectfill(problem_hex_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT+1,fpge_mask_color);
	//top part
	line(problem_hex_bmp,0,24,15,0,colors_to24bits(PROBLEM_HEX_COLOR));
	line(problem_hex_bmp,45,0,59,24,colors_to24bits(PROBLEM_HEX_COLOR));
	line(problem_hex_bmp,15,0,45,0,colors_to24bits(PROBLEM_HEX_COLOR));
	//second line
	line(problem_hex_bmp,1,24,16,0,colors_to24bits(PROBLEM_HEX_COLOR));
	line(problem_hex_bmp,44,0,58,24,colors_to24bits(PROBLEM_HEX_COLOR));
	line(problem_hex_bmp,15,1,45,1,colors_to24bits(PROBLEM_HEX_COLOR));
	// bottom part
	line(problem_hex_bmp,0,25,15,49,colors_to24bits(PROBLEM_HEX_COLOR));
	line(problem_hex_bmp,45,49,59,25,colors_to24bits(PROBLEM_HEX_COLOR));
	line(problem_hex_bmp,15,49,45,49,colors_to24bits(PROBLEM_HEX_COLOR));
	//second line
	line(problem_hex_bmp,1,25,16,49,colors_to24bits(PROBLEM_HEX_COLOR));
	line(problem_hex_bmp,44,49,58,25,colors_to24bits(PROBLEM_HEX_COLOR));
	line(problem_hex_bmp,15,48,45,48,colors_to24bits(PROBLEM_HEX_COLOR));
}

void draw_tiles_matrix() {
	int i, mx, my, j = 0;
	short tile,current_tile;

	if (filter_tiles && showMatrixMode>1) j=1; //2,3
	//clear
	for (mx = 0; mx < matrix_x; mx++)
		for (my = 0; my < matrix_y; my++)
			map[mx][my].tile = BLACK_TILE;

	//draw
	for (i = 0; i < matrix_x * matrix_y; ++i) {
		tile = BLACK_TILE;
		if (sortMatrixMode){
			current_tile = i-3;
		}else{
			current_tile = tiles_display_max_tiles[i-3];
		}
		if (!filter_tiles){
			if (i==0) current_tile=MAGIC_ROAD;
			if (i==1) current_tile=MAGIC_RIVER;
			if (i==2) current_tile=MAGIC_ROAD_AND_RIVER;
		}

		if (filter_tiles) {
			if (FilterTiles_Max_Tiles[current_tile] & (1 << filter_last)){
				if (filter_number_current_ingroup[filter_last]>1){
					if (TTData_Max_Tiles[current_tile]==filter_tt_ingroup[filter_last][filter_number_current_ingroup[filter_last]-2]){
						tile = current_tile;
					}
				}else{
					tile = current_tile;
				}
			}
		} else {
			if (i < total_tiles+3)
				tile = current_tile;
		}

		mx = j % (matrix_x);
		my = j / (matrix_x);
		if (tile != BLACK_TILE && j<matrix_x*matrix_y) {
			map[mx][my].tile = tile;

			j++;
			if (filter_tiles) {
				if (showMatrixMode > 1)
					j++; //2,3
				if (showMatrixMode == 3 && j % 20 == 1)
					j += 20; //1
				if (showMatrixMode == 1 && j % 20 == 0)
					j += 20; //3
			}
		}
	}
}


void draw_map(BITMAP *map_to_draw,int x0, int y0, int tiles_high, int tiles_wide){
	draw_map_shift(map_to_draw,x0,y0,tiles_high,tiles_wide,mapShiftMode);
}

int map_shift_x(int mode) {
	int shift_x = 0;

	switch (mode) {
	case 0:
		shift_x =0;
		break;
	case 1:
		shift_x = -TILE_WIDTH_SHIFT;
		break;
	case 2:
		shift_x = -(TILE_FULL_WIDTH - TILE_WIDTH_SHIFT);
		break;
	default:
		shift_x = -TILE_WIDTH_SHIFT;
		break;
	}

	return shift_x;
}

int map_shift_y(int mode) {
	int shift_y = 0;

	if (mode) shift_y = -TILE_HEIGHT_SHIFT;

	return shift_y;
}

int get_label_color(int idx) {
	int bg_color;

	switch (all_units[idx].uflag) {
	case axis_core:
		bg_color = COLOR_BLACK;
		break;
	case axis_aux:
		bg_color = COLOR_DARK_BLUE_GRAY;
		break;
	case allied_core:
		bg_color = COLOR_DARK_GREEN;
		break;
	case allied_aux:
		bg_color = COLOR_PURPLE;
		break;
	default:
		bg_color = COLOR_BLACK;
		break;
	}
	return bg_color;
}

int get_label_value(int idx) {
	int index;

	switch (all_units[idx].uflag) {
	case axis_core:
		index = idx + 1;
		break;
	case axis_aux:
		index = idx + 1 - total_axis_core;
		if (displayUnitsOrder==2) index+=80;
		break;
	case allied_core:
		index = idx + 1 - total_axis_core - total_axis_aux;
		break;
	case allied_aux:
		index = idx + 1 - total_axis_core - total_axis_aux - total_allied_core;
		if (displayUnitsOrder==2) index+=80;
		break;
	default:
		index = 0;
		break;
	}
	return index;
}


void draw_map_shift(BITMAP *map_to_draw,int x0, int y0, int tiles_high, int tiles_wide, int shift)
{
  int x,y,xs,ys,idx,sidx,value,cc,sim,is_air_unit_show,rc;
  int seq=0,eq=0;
  int clr, gidx,aidx,geq,aeq;
  int clr2;
  int bmp_num, line_color, str_bmp_off;
  int shift_x=0, shift_y=0;
  BITMAP *tile_to_draw;
  int i;

  if (shift){
	  shift_x=map_shift_x(shift);
	  shift_y=map_shift_y(shift);
  }

  if (colorizeNames==2) do_colorize_ocean();
  if (colorizeNames==1) do_colorize_all();

  //blank all
  //FPGE_SCREEN_COLOR
  clr=MAP_COLOR;
  if (tile_mode==1) clr=FPGE_SCREEN_COLOR;
  rectfill(map_to_draw,0,0,map_to_draw->w,map_to_draw->h,colors_to24bits(clr));
  //blank off the edges

  /*
    //top row
    rectfill(map_to_draw,0,0,map_to_draw->w,TILE_HEIGHT/2,colors_to24bits(MAP_COLOR));
    //bottom row
    rectfill(map_to_draw,0,map_to_draw->h-TILE_HEIGHT/2,map_to_draw->w,map_to_draw->h,colors_to24bits(MAP_COLOR));
    //left column
    rectfill(map_to_draw,0,0,TILE_WIDTH/2,map_to_draw->h,colors_to24bits(MAP_COLOR));
    //right column
    rectfill(map_to_draw,map_to_draw->w-TILE_WIDTH/2,0,map_to_draw->w,map_to_draw->h,colors_to24bits(MAP_COLOR));

    //left column
    //rectfill(map_to_draw,0,0,map_to_draw->w+10,map_to_draw->h,colors_to24bits(MAP_COLOR));
    //right column
    //rectfill(map_to_draw,map_to_draw->w-10,map_to_draw->w,map_to_draw->w,map_to_draw->h,colors_to24bits(MAP_COLOR));
  */

  //draw the tiles

   //not very optimal, fix me
   total_axis_core=count_axis_core();
   total_axis_aux=count_axis_aux();
   //total_allied=count_allied();

   //draw tiles first
   for (y=y0-1; y<y0+tiles_high+1; ++y)
     for (x=x0-1; x<x0+tiles_wide+1; ++x)
       if (x>=0 && x<mapx && y>=0 && y<mapy)
       {
	   // show the tile
			xs=(x-x0)*TILE_WIDTH+shift_x;
			ys=(y-y0)*TILE_HEIGHT+shift_y;
			if ((x)%2) ys+=TILE_HEIGHT/2;

			if(map[x][y].tile>=total_tiles){
				switch (map[x][y].tile) {
					case BLACK_TILE:
						tile_to_draw = black_hex_bmp;
						break;
					case MAGIC_ROAD:
						tile_to_draw = road_hex_bmp;
						break;
					case MAGIC_RIVER:
						tile_to_draw = river_hex_bmp;
						break;
					case MAGIC_ROAD_AND_RIVER:
						tile_to_draw = road_and_river_hex_bmp;
						break;
					default:
						tile_to_draw = black_hex_bmp;
						break;
				}
				masked_blit(tile_to_draw,map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT);
			}else{
				if ((map[x][y].shade&SHADE_MASK) && show_ranges != 2 && edit_op!=edit_tile){
					switch(showWeather){
						case 0:tile_to_draw = dark_til_bmp[map[x][y].tile]; break;
						case 1:tile_to_draw = dark_til_bmp_mud[map[x][y].tile];break;
						case 2:tile_to_draw = dark_til_bmp_snow[map[x][y].tile];break;
						default:tile_to_draw =0;break;
					}
				}else{
					switch(showWeather){
						case 0:tile_to_draw = til_bmp[map[x][y].tile];break;
						case 1:tile_to_draw = til_bmp_mud[map[x][y].tile];break;
						case 2:tile_to_draw = til_bmp_snow[map[x][y].tile];break;
						default:tile_to_draw =0;break;
					}
				}
				masked_blit(tile_to_draw,map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT);

			}
		//if (y<3 && x<21)
		//masked_blit(strength_bmp[x+y*MAX_STRENGTH_IN_ROW],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT);
   } //end of draw til info



	if ((showHex && edit_op!=edit_tile)||(showHexMatrix && edit_op==edit_tile)){
		int clrIdx = (edit_op==edit_tile)?showHexMatrixColor : showHexColor;
		int clrArray[] = {HEX_COLOR,COLOR_WHITE,COLOR_BLACK, COLOR_RED, COLOR_YELLOW, COLOR_BLUE };
		int clrArray2[] = {MAP_COLOR,COLOR_WHITE,COLOR_BLACK, COLOR_RED, COLOR_YELLOW, COLOR_BLUE };

		clr=clrArray[clrIdx];
		clr2=(edit_op==edit_tile)?clrArray2[clrIdx]:-1;

		for (y=y0-1; y<y0+tiles_high+1; ++y)
		   for (x=x0-1; x<x0+tiles_wide+1; ++x)
		   {
			if (x >= 0 && x < mapx && y >= 0 && y < mapy) {
						if ((clr2 > 0) && (x < matrix_x) && (y	<= matrix_y))
							cc = clr2;
						else
							cc = clr;
						xs = (x - x0) * TILE_WIDTH+shift_x;
						ys = (y - y0) * TILE_HEIGHT+shift_y;
						if ((x) % 2)
							ys += 25;
						//draw upper lines of hex
						if (draw_app6)
							cc = colors_to24bits(COLOR_DARK_DARK_GREEN);
						else
							cc = colors_to24bits(cc);
						line(map_to_draw, xs + 0, ys + 24, xs + 15, ys + 0, cc);
						line(map_to_draw, xs + 45, ys + 0, xs + 59, ys + 24, cc);
						line(map_to_draw, xs + 15, ys, xs + 45, ys, cc);
					}
		   }
	}
	//problems
	if ((show_problems ||show_debug_problems) && edit_op!=edit_tile)
	for (y=y0-1; y<y0+tiles_high+1; ++y)
	   for (x=x0-1; x<x0+tiles_wide+1; ++x){
           xs=(x-x0)*TILE_WIDTH+shift_x;
           ys=(y-y0)*TILE_HEIGHT+shift_y;
           if (x%2) ys+=TILE_HEIGHT/2;
		   //road
	    	if (map[x][y].shade&PROBLEM_MASK)
	    		masked_blit(problem_hex_bmp,map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT+1);
	  }
	//selected fragment
	if (edit_op==edit_frg_select || edit_op==edit_frg_place)
	for (y=y0-1; y<y0+tiles_high+1; ++y)
	   for (x=x0-1; x<x0+tiles_wide+1; ++x){
           xs=(x-x0)*TILE_WIDTH+shift_x;
           ys=(y-y0)*TILE_HEIGHT+shift_y; // +(x%2)*TILE_HEIGHT/2
           if (x%2) ys+=TILE_HEIGHT/2;
	    	if ((map[x][y].shade&SELECTED_FRG_MASK) || (map[x][y].shade&PLACED_FRG_MASK))
	    		masked_blit(problem_hex_bmp,map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT+1);
	  }

    if (edit_op==edit_vic || edit_op==edit_deploy || edit_op==edit_neutral || graphical_overide_hex>0 )
    {
       for (y=y0-1; y<y0+tiles_high+1; ++y)
        for (x=x0-1; x<x0+tiles_wide+1; ++x)
         if (x>=0 && x<mapx && y>=0 && y<mapy)
         {
            // show the special hex
            xs=(x-x0)*TILE_WIDTH+shift_x;
            ys=(y-y0)*TILE_HEIGHT+shift_y;
            if ((x)%2) ys+=TILE_HEIGHT/2;
            if (map[x][y].vic&&((edit_op==edit_vic)||((graphical_overide_hex&0x01)==0x01)))
            {
              masked_blit(vic_hex_bmp,map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT+1);
            }
            if (map[x][y].deploy&&((edit_op==edit_deploy)||((graphical_overide_hex&0x02)==0x02)))
            {
              masked_blit(deploy_hex_bmp,map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT+1);
            }
            if (map[x][y].side==3 && ((edit_op==edit_neutral)||((graphical_overide_hex&0x04)==0x04)))
            {
              masked_blit(neutral_hex_bmp,map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT+1);
            }

         }
     }

    //draw roads, flags and units, id in scenario, counters
    if (edit_op!=edit_tile)
    for (y=y0-1; y<y0+tiles_high+1; ++y)
      for (x=x0-1; x<x0+tiles_wide+1; ++x)
        if (x>=0 && x<mapx && y>=0 && y<mapy)
        {
          xs=(x-x0)*TILE_WIDTH+shift_x;
          ys=(y-y0)*TILE_HEIGHT+shift_y;
          if ((x)%2) ys+=TILE_HEIGHT/2;
  		// we need this to make no units be displayed in names modes
  		if ((drawNames)||(drawAllNames)||(drawTerrain)||(drawRoads)) {

  			if(drawRoads){
  				 line_color = colors_to24bits(LINE_COLOR);
  				 if (map[x][y].rc&0x01) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+TILE_FULL_WIDTH/2,ys,line_color);
  				 if (map[x][y].rc&0x02) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*1.7,ys+TILE_HEIGHT/4,line_color);
  				 if (map[x][y].rc&0x08) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*1.7,ys-1+(-(float)TILE_HEIGHT/4.0+TILE_HEIGHT),line_color);

  				 if (map[x][y].rc&0x10) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT,line_color);
  				 if (map[x][y].rc&0x20) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*0.3,ys-1+(-(float)TILE_HEIGHT/4.0+TILE_HEIGHT),line_color);
  				 if (map[x][y].rc&0x80) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*0.3,ys+TILE_HEIGHT/4,line_color);
  			 }

  		}
  		if ((graphical_overide>0)||(!((drawNames)||(drawAllNames)||(drawTerrain)||(drawRoads)))) {
  			//put on the flag...if it is a victory we need to change
  			if ((graphical_overide==0)||(graphical_overide&0x02)==0x02){
  			if (map[x][y].own && map[x][y].vic)
  				masked_blit(victory_flag_bmp[map[x][y].own-1],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT);
  			if (map[x][y].own && map[x][y].vic==0)
  				masked_blit(flag_bmp[map[x][y].own-1],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT);
  		}
  		idx=-1;
  		sidx=-1;
  		is_air_unit_show=-1;
  		//new way
  		aidx=-1;
  		gidx=-1;
  		aeq=-1;
  		geq=-1;
  		//ground
  		if ((graphical_overide==0)||(graphical_overide&0x01)==0x01){
  			if (map[x][y].guidx>=0){
  			  if ((drawGndTransport)&&(all_units[map[x][y].guidx].orgtnum>0))
  				{
  				  gidx=map[x][y].guidx;
  				  geq=all_units[map[x][y].guidx].orgtnum;
  				}
  			  else
  				{
  				  gidx=map[x][y].guidx;
  				  geq=all_units[map[x][y].guidx].unum;
  				}
  			//show transport
  			if ( (show_unit_layer_type==2 || show_unit_layer_type == 4 ) && all_units[map[x][y].guidx].auxtnum>0)
  			   {
					gidx=map[x][y].guidx;
					geq=all_units[map[x][y].guidx].auxtnum;
  			   }
  			}//ground unit present

  			//air
  			if (map[x][y].auidx >= 0) {
  								aidx = map[x][y].auidx;
  								aeq = all_units[map[x][y].auidx].unum;
  							}

  			if (show_unit_layer_type == 0 || show_unit_layer_type == 2) {
  								if (gidx != -1) {
  									idx = gidx;
  									eq = geq;
  									is_air_unit_show = 0;
  									if (displayAllUnits) {
  										sidx = aidx;
  										seq = aeq;
  									}
  								} else
  									if (displayAllUnits) {
										idx = aidx;
										eq = aeq;
										is_air_unit_show = 1;
  								}
  							}
  			if (show_unit_layer_type == 1 || show_unit_layer_type == 4) {
  								if (aidx != -1) {
  									idx = aidx;
  									eq = aeq;
  									is_air_unit_show = 1;
  									if (displayAllUnits) {
  										sidx = gidx;
  										seq = geq;
  									}
  								} else
  									if (displayAllUnits) {
										idx = gidx;
										eq = geq;
										is_air_unit_show = 0;
  								}
  							}

  			//draw unit description
  			if (idx >-1 && show_unit_layer_type!=3){
  				bmp_num=equip[eq][BMP]+256*equip[eq][BMP+1];
  				//masked_blit(unit_bmp[bmp_num],map_to_draw,0,0,xs,ys-(equip[eq][TARG_TYPE]==2?16:0),TILE_FULL_WIDTH,TILE_HEIGHT);
  				if (draw_app6_on_the_fly){
  					draw_app6_unit_symbol(map_to_draw,xs,ys, eq, (all_units[idx].uflag==allied_core||all_units[idx].uflag==allied_aux ? 1:0)  );
  				}else{
  				if (scn_buffer[ORIENTATION]==1 || draw_app6)
  					draw_sprite(map_to_draw,unit_bmp[bmp_num],xs,ys-(equip[eq][TARGET_TYPE]==2?11:0));
  				else
  					draw_sprite_h_flip(map_to_draw,unit_bmp[bmp_num],xs+1,ys-(equip[eq][TARGET_TYPE]==2?11:0));
  				}

  				if (sidx>-1){ //draw stacked unit
  					sim=stack_icons_mapping
  					[(all_units[sidx].uflag==allied_core || all_units[sidx].uflag==allied_aux)?0:1]
  					[equip[seq][MOV_TYPE]>CLASS_TANK?0:1]
  					[equip[seq][CLASS]];
  					if (scn_buffer[ORIENTATION]==1)
  						draw_sprite(map_to_draw,stack_bmp[sim],xs,ys+3);
  					else
  						draw_sprite_h_flip(map_to_draw,stack_bmp[sim],xs+1,ys+3);
  					//masked_blit(stack_bmp[sim],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT);
  				}
  				//display unit ID# in scenario
  				if (displayUnitsOrder) {
					char buf[32];
					sprintf(buf, " %d ", get_label_value(idx));
					if (drawTerrain || drawAllNames || drawNames)
						is_air_unit_show = 1; //do not move labels when other info is displayed
					textout_centre_ex(map_to_draw, font, buf, xs + (sidx==-1?TILE_FULL_WIDTH / 2:TILE_FULL_WIDTH / 4),
							ys + 2 + 20	* is_air_unit_show - (sidx==-1?0:2),
							colors_to24bits(COLOR_WHITE), colors_to24bits(get_label_color(idx)));
					if (sidx>-1){
						sprintf(buf, " %d ", get_label_value(sidx));
						textout_centre_ex(map_to_draw, font, buf, xs + (is_air_unit_show?TILE_FULL_WIDTH / 2:0)+TILE_FULL_WIDTH / 4,
								ys + 2 + 20	* is_air_unit_show - 2+ (is_air_unit_show?0:8),
								colors_to24bits(COLOR_WHITE), colors_to24bits(get_label_color(idx)));					}
  				}
  				if (showCounter!=-1 && showCounter<3){
  					//counter
  					value=(showCounter==0?all_units[idx].str:0)+(showCounter==1?all_units[idx].exp:0)+(showCounter==2?all_units[idx].entrench:0);
  					//value=(showCounter==0?equip[all_units[idx].unum][TARG_TYPE]:0)+(showCounter==1?all_units[idx].exp:0)+(showCounter==2?all_units[idx].entrench:0);
  					str_bmp_off=str_bmp_offset(idx);
  					masked_blit(strength_bmp[str_bmp_off + value],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT);
  				}
  				if (showCounter==3 && all_units[idx].country>0 && all_units[idx].country<=total_countries)
  					masked_blit(flag_bmp[all_units[idx].country-1],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH,TILE_HEIGHT);
  			//}
  			}
  		} //overide unit show
  		} // end of draw unit
  	  } //end of draw units


	 // draw names
	if (edit_op != edit_tile)
		if ((drawNames) || (drawAllNames) || (drawTerrain))
			for (y = y0 - 1; y < y0 + tiles_high + 1; ++y)
				for (x = x0 - 1; x < x0 + tiles_wide + 1; ++x)
					if (x >= 0 && x < mapx && y >= 0 && y < mapy) {
						char buf[256];
						xs = (x - x0) * TILE_WIDTH + shift_x;
						ys = (y - y0) * TILE_HEIGHT + shift_y;
						if ((x) % 2)
							ys += TILE_HEIGHT / 2;

						if ((drawAllNames) || (drawNames)) {
							if ((!showFilter) || (map[x][y].gln == filterName)) {
								//text_mode(251); // bordo
								int bg_color = COLOR_BLACK; //black
								int fg_color = COLOR_WHITE; //white

								if (is_tile_river(x, y))
									bg_color = COLOR_BLUE; // blue if water hex
								else {
									if (is_tile_ocean(x, y)) {
										if (colorizeNames == 2) {
											bg_color = colorize_PG[gln_to_color[map[x][y].gln]];
										} else
											bg_color = COLOR_BLUE; // blue if water hex
									}
								}
								if (colorizeNames == 1)
									bg_color = colorize_PG[gln_to_color[map[x][y].gln]];
								if (bg_color == 14 || bg_color == 15) //yellow or white
									fg_color = COLOR_BLACK;

								switch (showDecimal) {
								case 0:
									sprintf(buf, "%d", map[x][y].gln);
									break;
								case 1:
									sprintf(buf, "%X", map[x][y].gln);
									break;
								case 2:
									sprintf(buf, "%s", gln_utf8[map[x][y].gln]);
									break;
								}
								if (is_tile_name_show(x, y))
									textout_centre_ex(map_to_draw, font, buf, xs + TILE_FULL_WIDTH / 2, ys + 2, colors_to24bits(fg_color), colors_to24bits(bg_color));
							}
						}
						if (drawTerrain) {
							if ((!showFilter) || (map[x][y].utr == filterTerrain)) {
								//text_mode(COLOR_BLUE); //blue
								int fg_color = COLOR_WHITE;
								int bg_color = COLOR_BLUE;
								switch (showDecimal) {
								case 0:
									sprintf(buf, "%d", map[x][y].utr);
									break;
								case 1:
									sprintf(buf, "%X", map[x][y].utr);
									break;
								case 2:
									if (map[x][y].utr >= 0 && map[x][y].utr < MAX_TERRAIN_TYPE)
										sprintf(buf, "%s", utr_names[map[x][y].utr]);
									else {
										//fg_color=COLOR_BLACK;
										bg_color = COLOR_RED;
										sprintf(buf, "%s", "WRONG !");
									}
									break;
								}
								textout_centre_ex(map_to_draw, font, buf, xs + TILE_FULL_WIDTH / 2, ys + 2 + 10, colors_to24bits(fg_color), colors_to24bits(bg_color));
							}
						}
					}
	 //move
	if (edit_op == move_mode) {
		//print_str("move_mode");
		char buf[256];
		int fg_color;
		int bg_color;
		int line_color;
		for (y = y0 - 1; y < y0 + tiles_high + 1; ++y)
			for (x = x0 - 1; x < x0 + tiles_wide + 1; ++x)
				if (x >= 0 && x < mapx && y >= 0 && y < mapy) {

					xs=(x-x0)*TILE_WIDTH+shift_x;
					ys=(y-y0)*TILE_HEIGHT+shift_y;

					if (move_points_transport[x][y] < MOVE_NOT_CHECKED) {

						if ((x)%2) ys+=TILE_HEIGHT/2;
						fg_color = COLOR_PURPLE;
						bg_color = COLOR_WHITE;

						sprintf(buf, "%d", move_points_transport[x][y]);
						textout_centre_ex(map_to_draw, font, buf,
								xs + TILE_FULL_WIDTH / 2,
								ys + 2 + 10,
								colors_to24bits(fg_color),
								colors_to24bits(bg_color));

						 line_color = colors_to24bits(fg_color);
						 if (move_directions_transport_RC[x][y]&0x01) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+TILE_FULL_WIDTH/2,ys,line_color);
						 if (move_directions_transport_RC[x][y]&0x02) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*1.7,ys+TILE_HEIGHT/4,line_color);
						 if (move_directions_transport_RC[x][y]&0x08) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*1.7,ys-1+(-(float)TILE_HEIGHT/4.0+TILE_HEIGHT),line_color);

						 if (move_directions_transport_RC[x][y]&0x10) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT,line_color);
						 if (move_directions_transport_RC[x][y]&0x20) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*0.3,ys-1+(-(float)TILE_HEIGHT/4.0+TILE_HEIGHT),line_color);
						 if (move_directions_transport_RC[x][y]&0x80) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*0.3,ys+TILE_HEIGHT/4,line_color);
				}
					if (move_points[x][y] < MOVE_NOT_CHECKED ) {
						//xs=(x-x0)*TILE_WIDTH+shift_x;
						//ys=(y-y0)*TILE_HEIGHT+shift_y;
						if ((x)%2) ys+=TILE_HEIGHT/2;
						fg_color = COLOR_WHITE;
						bg_color = COLOR_BLACK;

						sprintf(buf, "%d", move_points[x][y]);
						textout_centre_ex(map_to_draw, font, buf,
								xs + TILE_FULL_WIDTH / 2,
								ys + 2,
								colors_to24bits(fg_color),
								colors_to24bits(bg_color));

						 line_color = colors_to24bits(fg_color);
		  				 if (move_directions_RC[x][y]&0x01) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+TILE_FULL_WIDTH/2,ys,line_color);
		  				 if (move_directions_RC[x][y]&0x02) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*1.7,ys+TILE_HEIGHT/4,line_color);
		  				 if (move_directions_RC[x][y]&0x08) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*1.7,ys-1+(-(float)TILE_HEIGHT/4.0+TILE_HEIGHT),line_color);

		  				 if (move_directions_RC[x][y]&0x10) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT,line_color);
		  				 if (move_directions_RC[x][y]&0x20) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*0.3,ys-1+(-(float)TILE_HEIGHT/4.0+TILE_HEIGHT),line_color);
		  				 if (move_directions_RC[x][y]&0x80) line(map_to_draw,xs+TILE_FULL_WIDTH/2,ys+TILE_HEIGHT/2,xs+(float)TILE_FULL_WIDTH/2.0*0.3,ys+TILE_HEIGHT/4,line_color);
					}
					if (move_path[x][y]<MOVE_NO_PATH){
						line_color = colors_to24bits(COLOR_BLUE);
						int tcx=xs+TILE_FULL_WIDTH/2;
						int tcy=ys+TILE_HEIGHT/2;
						if (move_path[x][y]==0) line(map_to_draw,tcx,tcy,tcx,tcy-TILE_HEIGHT,line_color);
						if (move_path[x][y]==1) line(map_to_draw,tcx,tcy,tcx+TILE_WIDTH,tcy-TILE_HEIGHT/2,line_color);
						if (move_path[x][y]==2) line(map_to_draw,tcx,tcy,tcx+TILE_WIDTH,tcy+TILE_HEIGHT/2,line_color);
						if (move_path[x][y]==3) line(map_to_draw,tcx,tcy,tcx,tcy+TILE_HEIGHT,line_color);
						if (move_path[x][y]==4) line(map_to_draw,tcx,tcy,tcx-TILE_WIDTH,tcy+TILE_HEIGHT/2,line_color);
						if (move_path[x][y]==5) line(map_to_draw,tcx,tcy,tcx-TILE_WIDTH,tcy-TILE_HEIGHT/2,line_color);
					}
				}
	} //move_mode

	if (edit_op == edit_tile && debug_tile_matrix) {
		char buf[256];

		for (y = y0 - 1; y < y0 + tiles_high + 1; ++y)
			for (x = x0 - 1; x < x0 + tiles_wide + 1; ++x)
				if (x >= 0 && x < mapx && y >= 0 && y < mapy && map[x][y].tile!=BLACK_TILE) {

					xs = (x - x0) * TILE_WIDTH + shift_x;
					ys = (y - y0) * TILE_HEIGHT + shift_y;
					if ((x) % 2)
						ys += TILE_HEIGHT / 2;

					if (drawRoadsDebug) {
						if ( (FilterTiles_Max_Tiles[map[x][y].tile] & (1 << ROAD_FILTER_INDEX)) && ( drawRoadsDebug == 1 ) ) {
							line_color = colors_to24bits(LINE_COLOR);
							rc = 0;
							for (i = 0; i < roads_tiles_size; i++)
								if (road_connection_info[i].tile == map[x][y].tile) {
									//rc = roads_tiles_mask_RC[i];
									rc =road_connection_info[i].bits_RC;

									if (rc & 0x01)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + TILE_FULL_WIDTH / 2, ys, line_color);
									if (rc & 0x02)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 1.7, ys + TILE_HEIGHT / 4, line_color);
									if (rc & 0x08)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 1.7, ys - 1 + (-(float) TILE_HEIGHT / 4.0 + TILE_HEIGHT), line_color);
									if (rc & 0x10)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT, line_color);
									if (rc & 0x20)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 0.3, ys - 1 + (-(float) TILE_HEIGHT / 4.0 + TILE_HEIGHT), line_color);
									if (rc & 0x80)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 0.3, ys + TILE_HEIGHT / 4, line_color);
									break;
								}
						}
						if ((FilterTiles_Max_Tiles[map[x][y].tile] & (1 << RIVER_FILTER_INDEX)) && ( drawRoadsDebug ==2)) {
							line_color = colors_to24bits(COLOR_BLUE);
							rc = 0;
							for (i = 0; i < max_river_conversions_patterns; i++)
								if (river_connection_info[i].tile == map[x][y].tile) {
									rc = river_connection_info[i].bits_RC;

									if (rc & 0x01)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + TILE_FULL_WIDTH / 2, ys, line_color);
									if (rc & 0x02)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 1.7, ys + TILE_HEIGHT / 4, line_color);
									if (rc & 0x08)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 1.7, ys - 1 + (-(float) TILE_HEIGHT / 4.0 + TILE_HEIGHT), line_color);
									if (rc & 0x10)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT, line_color);
									if (rc & 0x20)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 0.3, ys - 1 + (-(float) TILE_HEIGHT / 4.0 + TILE_HEIGHT), line_color);
									if (rc & 0x80)
										line(map_to_draw, xs + TILE_FULL_WIDTH / 2, ys + TILE_HEIGHT / 2, xs + (float) TILE_FULL_WIDTH / 2.0 * 0.3, ys + TILE_HEIGHT / 4, line_color);
									break;

								}
						}

						//patterns
						if (drawRoadsDebug == 3) {
							int pattern=0;

							//draw also patterns
							if (FilterTiles_Max_Tiles[map[x][y].tile] & (1 << ROAD_FILTER_INDEX)){
								//search for road pattern
								for(i=0;i<road_size;i++){
									if (road_pattern_tile[i][0]==map[x][y].tile || road_pattern_tile[i][1]==map[x][y].tile || road_pattern_tile[i][2]==map[x][y].tile){
										pattern=road_pattern_SE_CCW[i];
										break;
									}
								}
							}

							if (FilterTiles_Max_Tiles[map[x][y].tile] & (1 << COAST_FILTER_INDEX)){
								//search for coast pattern
								for(i=0;i<coast_size;i++){
									if (coast_pattern_tile[i][0]==map[x][y].tile || coast_pattern_tile[i][1]==map[x][y].tile || coast_pattern_tile[i][2]==map[x][y].tile){
										pattern=coast_pattern_SE_CCW[i];
										break;
									}
								}
							}

							if (FilterTiles_Max_Tiles[map[x][y].tile] & (1 << RIVER_FILTER_INDEX)){
								//search for river pattern
								for(i=0;i<river_size;i++){
									if (river_pattern_tile[i][0]==map[x][y].tile || river_pattern_tile[i][1]==map[x][y].tile || river_pattern_tile[i][2]==map[x][y].tile){
										pattern=river_pattern_SE_CCW[i];
										break;
									}
								}
							}

							if(pattern){
								line_color = colors_to24bits(PROBLEM_HEX_COLOR);

								// SE_CCW
								int dx=3;
								int dy=2;
								if (pattern & 0x01){
									line(map_to_draw,xs + 0+dx ,ys + 24,xs + 15+dx,ys + 49-dy,line_color);
									line(map_to_draw,xs + 1+dx ,ys + 24,xs + 16+dx,ys + 49-dy,line_color);
								}
								if (pattern & 0x02) {
									line(map_to_draw, xs + 15, ys + 50-dx, xs + 45, ys + 50-dx, line_color);
									line(map_to_draw, xs + 15, ys + 49-dx, xs + 44, ys + 49-dx, line_color);
								}
								if (pattern & 0x04) {
									line(map_to_draw, xs + 45-dx, ys + 50-dy, xs + 60-dx, ys + 24, line_color);
									line(map_to_draw, xs + 44-dx, ys + 50-dy, xs + 59-dx, ys + 24, line_color);
								}
								if (pattern & 0x08) {
									line(map_to_draw, xs + 45-dx, ys + 0+dy, xs + 60-dx, ys + 24, line_color);
									line(map_to_draw, xs + 44-dx, ys + 0+dy, xs + 59-dx, ys + 24, line_color);
								}
								if (pattern & 0x10) {
									line(map_to_draw, xs + 15, ys + 0+dx, xs + 45, ys + 0+dx, line_color);
									line(map_to_draw, xs + 15, ys + 1+dx, xs + 44, ys + 1+dx, line_color);
								}
								if (pattern & 0x20) {
									line(map_to_draw, xs + 0+dx, ys + 24, xs + 15+dx, ys + 0+dy, line_color);
									line(map_to_draw, xs + 1+dx, ys + 24, xs + 16+dx, ys + 0+dy, line_color);
								}
							}
						}
					} //draw roads
					if (drawNamesDebug) {

						int bg_color = COLOR_BLACK; //black
						int fg_color = COLOR_WHITE; //white
						int gln = NData_Max_Tiles[map[x][y].tile];

						switch (showDecimalDebug) {
						case 0:
							sprintf(buf, "%d", gln);
							break;
						case 1:
							sprintf(buf, "%X", gln);
							break;
						case 2:
							sprintf(buf, "%s", gln_utf8[gln]);
							break;
						}
							textout_centre_ex(map_to_draw, font, buf, xs + TILE_FULL_WIDTH / 2, ys + 2, colors_to24bits(fg_color), colors_to24bits(bg_color));
					}

					if (drawAllNamesDebug) {
						int bg_color = COLOR_BLACK; //black
						int fg_color = COLOR_WHITE; //white
						int tile = map[x][y].tile;

						switch (colorizeNamesDebug) {
						case 0:
							switch (showDecimalDebug) {
								case 0:
								case 2:
									sprintf(buf, "%d", tile);
									break;
								case 1:
									sprintf(buf, "%X", tile);
									break;
							}
							break;
						case 1:
							sprintf(buf, "%04x", FilterTiles_Max_Tiles[tile]);
							break;
						case 2:
							sprintf(buf, "%04x", MainCategoryTiles_Max_Tiles[tile]);
							break;
						}
						textout_centre_ex(map_to_draw, font, buf, xs + TILE_FULL_WIDTH / 2, ys + 2+20, colors_to24bits(fg_color), colors_to24bits(bg_color));

						if (colorizeNamesDebug == 0){
							for (i = 0; i < ocean_tiles_size; i++){
								if (ocean_tiles[i]==map[x][y].tile){
									strncpy(buf,"O",256);
									textout_centre_ex(map_to_draw, font, buf, xs + TILE_FULL_WIDTH / 2, ys + 2+30, colors_to24bits(fg_color), colors_to24bits(bg_color));
									break;
								}
							}
						}
						if (colorizeNamesDebug == 1) {
							for (i = 0; i < roads_passive_tiles_size; i++) {
								if (roads_passive_tiles[i] == map[x][y].tile) {
									strncpy(buf, "R", 256);
									textout_centre_ex(map_to_draw, font, buf, xs + TILE_FULL_WIDTH / 2, ys + 2 + 30, colors_to24bits(fg_color), colors_to24bits(bg_color));
									break;
								}
							}
						}
					}

					if (drawTerrainDebug) {

						//text_mode(COLOR_BLUE); //blue
						int fg_color = COLOR_WHITE;
						int bg_color = COLOR_BLUE;
						int utr = TTData_Max_Tiles[map[x][y].tile];

						switch (showDecimalDebug) {
						case 0:
							sprintf(buf, "%d", utr);
							break;
						case 1:
							sprintf(buf, "%X", utr);
							break;
						case 2:
							if (utr >= 0 && utr < MAX_TERRAIN_TYPE)
								sprintf(buf, "%s", utr_names[utr]);
							else {
								bg_color = COLOR_RED;
								sprintf(buf, "%s", "WRONG !");
							}
							break;
						}
						textout_centre_ex(map_to_draw, font, buf, xs + TILE_FULL_WIDTH / 2, ys + 2 + 10, colors_to24bits(fg_color), colors_to24bits(bg_color));
					}
				}
	} //edit_op == edit_tile
	vsync();
}
