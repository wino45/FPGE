/*
 * randomize.c
 *
 *  Created on: 2010-08-24
 *      Author: wino
 */

#include <allegro.h>

#include "fpge.h"
#include "tables.h"
#include "drawmap.h"
#include "randomize.h"

#define DX 16
#define R_HEIGHT 16
#define R_WIDTH 8*20

DIALOG randomize_dlg[RANDOMIZE_DLG_SIZE]= {
/* 0 */	{ d_textbox_proc, 10,0,250,30+12*DX, GUI_FG_COLOR,GUI_BG_COLOR, 0, 0, 0,0, 0,0,0},
/* 1 */	{ d_text_proc ,	22 ,	12 ,	156 ,	18 ,	GUI_FG_COLOR,GUI_BG_COLOR,	0 ,	0 ,	0 ,	0 ,	 "Choose elements to randomize" },
/* 2 */	{ d_check_proc ,	22 ,	30+0*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	'o' ,	0 ,	1 ,	0 ,	 "R&ough tiles" },
/* 3 */	{ d_check_proc ,	22 ,	30+1*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	'l' ,	0 ,	1 ,	0 ,	 "C&lear tiles" },
/* 4 */	{ d_check_proc ,	22 ,	30+2*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	'd' ,	0 ,	1 ,	0 ,	 "&Desert tiles" },
/* 5 */	{ d_check_proc ,	22 ,	30+3*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	'e' ,	0 ,	1 ,	0 ,	 "Rough d&esert tiles" },
/* 6 */	{ d_check_proc ,	22 ,	30+4*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	'f' ,	0 ,	1 ,	0 ,	 "&Forests tiles" },
/* 7 */	{ d_check_proc ,	22 ,	30+5*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	'b' ,	0 ,	1 ,	0 ,	 "&Bocage tiles" },
/* 7 */	{ d_check_proc ,	22 ,	30+6*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	's' ,	0 ,	1 ,	0 ,	 "&Swamp tiles" },
/* 8 */	{ d_check_proc ,	22 ,	30+7*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	'i' ,	0 ,	1 ,	0 ,	 "R&ivers" },
/* 9 */	{ d_check_proc ,	22 ,	30+8*DX ,	R_WIDTH ,	R_HEIGHT ,	GUI_FG_COLOR,GUI_BG_COLOR,	'r' ,	0 ,	1 ,	0 ,	 "&Roads" },
/* 10 */	{ d_button_proc ,	22 ,	30+10*DX ,	96 ,	18 ,	GUI_FG_COLOR,GUI_BG_COLOR,	'c' ,	D_EXIT ,	0 ,	0 ,	 "&Cancel" },
/* 11*/	{ d_button_proc ,	148 ,	30+10*DX ,	96 ,	18 ,	GUI_FG_COLOR,GUI_BG_COLOR,	'a' ,	D_EXIT ,	0 ,	0 ,	 "R&andomize" },
{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
 { NULL,              0,    0,    0,    0,    0,    0,    0,    0,       0,    0,    NULL }
 };

int is_forest_tile(short tile_to_check){
	int found=0,i;

	for(i=0;i<forest_size;i++)
		if (tile_to_check==forest_tiles[i]){
			found=1+i;
			break;
		}
	//return index+1 or 0 when not found
	return found;
}

void forest_random(int idx, int len){
	int x,y,i;
	short forest_matrix[][6] =
	{
	{138,136,138,145,136,147},
	{145,146,147,136,146,135},
	{136,137,145,146,145,138},
	{146,136,147,145,136,147},
	{145,146,137,147,146,138},
	};
	int tile_id;
	int mask;

	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x)
			for (i = 0; i < len ; ++i){
				if (tiles_display[idx+i]==map[x][y].tile){
					map[x][y].tile=forest_matrix[y%5][x%6];
					break; //i
				}
			}

	//fixing
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			tile_id = is_forest_tile(map[x][y].tile);
			if (tile_id) {

				mask = 0;

				if (is_forest_tile(map[x - 1][y - 1 + x % 2].tile))
					mask += 0x20;
				if (is_forest_tile(map[x][y - 1].tile))
					mask += 0x10;
				if (is_forest_tile(map[x + 1][y - 1 + x % 2].tile))
					mask += 0x08;
				if (is_forest_tile(map[x + 1][y + x % 2].tile))
					mask += 0x04;
				if (is_forest_tile(map[x][y + 1].tile))
					mask += 0x02;
				if (is_forest_tile(map[x - 1][y + x % 2].tile))
					mask += 0x01;

				if ((mask & forest_pattern[tile_id - 1])
						!= forest_pattern[tile_id - 1]){
					//we need to substitute 138
					map[x][y].tile = 138;
				}

				//printf("Found at : x=%d y=%d - id : %d mask : %o pattern : %o\n",x,y,tile_id,mask,forest_pattern[tile_id - 1]);
			}
		}
}


void randomize_tiles(int idx, int len){
	int x,y,i;

	srand((unsigned int)time((time_t *)NULL));

	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x)
			for (i = 0; i < len ; ++i){
				if (tiles_display[idx+i]==map[x][y].tile){
					map[x][y].tile=tiles_display[idx+rand()%len];
					break; //i
				}
			}
}

void randomize_patterns(int type){
	int lx,x,y,j,r,pattern_id=0,p;

	short match_pattern[][3] = {
							    //Vertical road
								{59,59,59}, //left
								{47,47,47}, //right
								//Right road
								{56,56,56}, //up
								{66,66,66}, //down
								//left road
								{67,67,67}, //down
								{57,57,57}, //up
								//vr
								{63,63,63},
								{62,62,62},
								//rr
								{65,65,65},
								{65,65,65},
								//lr
								{227,227,227},
								{54,54,54},

								};
	short change_to[][2][3] = {
							    //Vertical road
			                   {{59,69,74},{69,47,74}},
			                   {{47,74,69},{74,59,69}},
			                   //rr
			                   {{69,66,48},{56,69,48}},
			                   {{48,56,69},{66,48,69}},
			                   //lr
			                   {{48,57,74},{67,48,74}},
			                   {{74,67,48},{57,74,48}},
			                   //vr
			                   {{63,71,72},{71,62,72}},
			                   {{62,72,71},{72,63,71}},
								//rr
			                   {{65,72,64},{72,64,65}},
			                   {{65,72,64},{72,64,65}},
								  //lr
			                   {{227,64,71},{64,54,71}},
			                   {{54,71,64},{71,227,64}}
								};

	srand((unsigned int)time((time_t *)NULL));

	for (p = 0; p < 2; ++p) {
		for (x = 0; x < mapx; ++x) {
			for (y = 0; y < mapy; ++y) {
				//Vertical pattern
				pattern_id=p+type*6;
				if (y<mapy-2)
				if (map[x][y].tile==match_pattern[pattern_id][0] &&
					map[x][y+1].tile==match_pattern[pattern_id][1] &&
					map[x][y+2].tile==match_pattern[pattern_id][2]){

					r=rand()%2;
					for (j = 0; j < 3; ++j) {
						map[x][y+j].tile=change_to[pattern_id][r][j];
					}
					//y+=3;
				}
				//Right pattern
				pattern_id=p+2+type*6;
				if (x<mapx-2)
					if (map[x][y].tile==match_pattern[pattern_id][0] &&
						map[x+1][y+(x%2)].tile==match_pattern[pattern_id][1] &&
						map[x+2][y+1].tile==match_pattern[pattern_id][2]){
						r=rand()%2;
						for (j = 0; j < 3; ++j) {
							map[x+j][y+(x%2)*(j==1)+(j>1)].tile=change_to[pattern_id][r][j];
						}
					}
				//Left pattern
				pattern_id=p+4+type*6;
				lx=mapx-(x+1);
				if (lx>1)
					if (map[lx][y].tile==match_pattern[pattern_id][0] &&
						map[lx-1][y+(lx%2)].tile==match_pattern[pattern_id][1] &&
						map[lx-2][y+1].tile==match_pattern[pattern_id][2]){
						r=rand()%2;
						for (j = 0; j < 3; ++j) {
							map[lx-j][y+(lx%2)*(j==1)+(j>1)].tile=change_to[pattern_id][r][j];
						}
					}
			}
		}
	}

}

int randomize_dialog() {

	centre_dialog(randomize_dlg);
	if (do_dialog(randomize_dlg, -1) == 12) {
		if (tile_mode == 1) {
			tile_cleanup();
		}
		//printf("R\n");
		if ((randomize_dlg[2].flags & D_SELECTED) == D_SELECTED) {
			//printf("Rr\n");
			randomize_tiles(117,6);
		}
		if ((randomize_dlg[3].flags & D_SELECTED) == D_SELECTED) {
			randomize_tiles(129,3);
		}
		if ((randomize_dlg[4].flags & D_SELECTED) == D_SELECTED) {
			randomize_tiles(135,9);
		}
		if ((randomize_dlg[5].flags & D_SELECTED) == D_SELECTED) {
			randomize_tiles(123,5);
		}
		//forest
		if ((randomize_dlg[6].flags & D_SELECTED) == D_SELECTED) {
			forest_random(183,7);
			//randomize_tiles(183,7);
		}
		if ((randomize_dlg[7].flags & D_SELECTED) == D_SELECTED) {
			randomize_tiles(10*20+12,3);
		}
		if ((randomize_dlg[8].flags & D_SELECTED) == D_SELECTED) {
			randomize_tiles(8*20+19,4);
		}
		if ((randomize_dlg[9].flags & D_SELECTED) == D_SELECTED) {
			randomize_patterns(1);
		}
		if ((randomize_dlg[10].flags & D_SELECTED) == D_SELECTED) {
			randomize_patterns(0);
		}
		draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
	}
	return D_REDRAW;
}
