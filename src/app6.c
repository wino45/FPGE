/*
 * app6.c
 *
 *  Created on: 2011-05-09
 *      Author: wino
 */
#include <allegro.h>
#include "fpge.h"
#include "fpge_colors.h"
#include "app6.h"
#include "tables.h"

BITMAP *app_components[APP_TILES];
int alt_aircraft=0;
int bb_range=5;
int gd_tank_heavy=14;
int ad_tank_heavy=11;
int mov_cavalery=5;

int draw_app6=0;
int draw_app6_units=1;
int draw_app6_on_the_fly=0;
int draw_app6_tiles=0; //0-no tiles coloring, 1-all one color, 2-mostly one color, see color below
int draw_app6_color=0; //0-green, 1 -grey


BITMAP *make_tile(){
	BITMAP *current;

	current=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
	rectfill(current,0,0,TILE_FULL_WIDTH,TILE_HEIGHT,fpge_mask_color);
	return current;
}


void init_app_components(){

	BITMAP *current;
	int current_pen_color;
	int current_bg_color;


	current_bg_color=colors_to24bits(COLOR_LIGHT_BLUE);
	current_pen_color=colors_to24bits(COLOR_BLACK);
	//
	//colors_to24bits(COLOR_BLACK);

	//RECTANGLE
	app_components[APP_RECT]=make_tile();
	current=app_components[APP_RECT];
	rectfill(current,APP_X,APP_Y,APP_X+APP_W,APP_Y+APP_H,current_bg_color);
	hline(current,APP_X,APP_Y,APP_X+APP_W,current_pen_color);
	hline(current,APP_X,APP_Y+1,APP_X+APP_W,current_pen_color);
	hline(current,APP_X,APP_Y+APP_H,APP_X+APP_W,current_pen_color);
	hline(current,APP_X,APP_Y-1+APP_H,APP_X+APP_W,current_pen_color);

	vline(current,APP_X,APP_Y,APP_Y+APP_H,current_pen_color);
	vline(current,APP_X+1,APP_Y,APP_Y+APP_H,current_pen_color);
	vline(current,APP_X-1+APP_W,APP_Y,APP_Y+APP_H,current_pen_color);
	vline(current,APP_X+APP_W,APP_Y,APP_Y+APP_H,current_pen_color);

	//CROSS
	app_components[APP_CROSS]=make_tile();
	current=app_components[APP_CROSS];
	line(current,APP_X,APP_Y,APP_X+APP_W,APP_Y+APP_H,current_pen_color);
	line(current,APP_X+1,APP_Y,APP_X+APP_W,APP_Y+APP_H-1,current_pen_color);

	line(current,APP_X,APP_Y+APP_H,APP_X+APP_W,APP_Y,current_pen_color);
	line(current,APP_X+1,APP_Y+APP_H,APP_X+APP_W,APP_Y+1,current_pen_color);

	//CAV
	app_components[APP_RECON]=make_tile();
	current=app_components[APP_RECON];
	line(current,APP_X,APP_Y+APP_H,APP_X+APP_W,APP_Y,current_pen_color);
	line(current,APP_X+1,APP_Y+APP_H,APP_X+APP_W,APP_Y+1,current_pen_color);

	//ARTY
	app_components[APP_ARTY]=make_tile();
	current=app_components[APP_ARTY];
	circlefill(current,APP_X+APP_W/2,APP_Y+APP_H/2,APP_W/10,current_pen_color);

	//ARMOUR
	app_components[APP_ARMOUR]=make_tile();
	current=app_components[APP_ARMOUR];
	hline(current,APP_X+APP_W/4,APP_Y+APP_H/4,APP_X+3*APP_W/4,current_pen_color);
	hline(current,APP_X+APP_W/4,APP_Y+3*APP_H/4,APP_X+3*APP_W/4,current_pen_color);
	arc(current,APP_X+APP_W/4,APP_Y+APP_H/2,itofix(64),itofix(-64),APP_H/4,current_pen_color);
	arc(current,APP_X+3*APP_W/4,APP_Y+APP_H/2,itofix(-64),itofix(64),APP_H/4,current_pen_color);

	//APP_MOUNTAIN
	app_components[APP_MOUNTAIN]=make_tile();
	current=app_components[APP_MOUNTAIN];
	triangle(current,APP_X+APP_W/2,APP_Y+3*APP_H/4,
	APP_X+APP_W/4,APP_Y,
	APP_X+3*APP_W/4,APP_Y,
	current_pen_color);

	//APP_WHEELS
	app_components[APP_WHEELS]=make_tile();
	current=app_components[APP_WHEELS];
	circle(current,APP_X+2*APP_W/8,APP_Y+7*APP_H/8,APP_W/15,current_pen_color);
	circle(current,APP_X+4*APP_W/8,APP_Y+7*APP_H/8,APP_W/15,current_pen_color);
	circle(current,APP_X+6*APP_W/8,APP_Y+7*APP_H/8,APP_W/15,current_pen_color);

	//APP_ANTI_TANK
	app_components[APP_ANTI_TANK]=make_tile();
	current=app_components[APP_ANTI_TANK];
	line(current,APP_X,APP_Y+APP_H,APP_X+APP_W/2,APP_Y,current_pen_color);
	line(current,APP_X+APP_W/2,APP_Y,APP_X+APP_W,APP_Y+APP_H,current_pen_color);

	//APP_ANTI_AIR_CRAFT
	app_components[APP_ANTI_AIRCRAFT]=make_tile();
	current=app_components[APP_ANTI_AIRCRAFT];
	arc(current,APP_X+APP_W/2,APP_Y+2*APP_H,itofix(42),itofix(42+44),5*APP_H/4,current_pen_color);
	//circle(current,APP_X+APP_W/2,APP_Y+2*APP_H,5*APP_H/4,current_pen_color);

	//APP_FIGHTER
	app_components[APP_FIGHTER]=make_tile();
	current=app_components[APP_FIGHTER];
	if (!alt_aircraft){
		textout_centre_ex(current,font,"F",APP_X+APP_W/2,APP_Y+APP_H/2-4,current_pen_color,fpge_mask_color);
	}
	else{
		line(current,APP_X+APP_W/4,APP_Y+APP_H/4,APP_X+3*APP_W/4,APP_Y+3*APP_H/4,current_pen_color);
		line(current,APP_X+APP_W/4,APP_Y+3*APP_H/4,APP_X+3*APP_W/4,APP_Y+APP_H/4,current_pen_color);
		arc(current,APP_X+APP_W/4,APP_Y+APP_H/2,itofix(64),itofix(-64),APP_H/4,current_pen_color);
		arc(current,APP_X+3*APP_W/4,APP_Y+APP_H/2,itofix(-64),itofix(64),APP_H/4,current_pen_color);

		line(current,APP_X+APP_W/2,APP_Y+APP_H/8,APP_X+APP_W/2,APP_Y+7*APP_H/8,current_pen_color);

		line(current,APP_X+APP_W/2,APP_Y+APP_H/8,APP_X+APP_W/2-APP_H/8,APP_Y+APP_H/4,current_pen_color);
		line(current,APP_X+APP_W/2,APP_Y+APP_H/8,APP_X+APP_W/2+APP_H/8,APP_Y+APP_H/4,current_pen_color);
	}

	//APP_BOMBER
	app_components[APP_BOMBER]=make_tile();
	current=app_components[APP_BOMBER];
	if (!alt_aircraft){
		textout_centre_ex(current,font,"B",APP_X+APP_W/2,APP_Y+APP_H/2-4,current_pen_color,fpge_mask_color);
	}
	else{
		line(current,APP_X+APP_W/4,APP_Y+APP_H/4,APP_X+3*APP_W/4,APP_Y+3*APP_H/4,current_pen_color);
		line(current,APP_X+APP_W/4,APP_Y+3*APP_H/4,APP_X+3*APP_W/4,APP_Y+APP_H/4,current_pen_color);
		arc(current,APP_X+APP_W/4,APP_Y+APP_H/2,itofix(64),itofix(-64),APP_H/4,current_pen_color);
		arc(current,APP_X+3*APP_W/4,APP_Y+APP_H/2,itofix(-64),itofix(64),APP_H/4,current_pen_color);

		line(current,APP_X+APP_W/2,APP_Y+APP_H/8,APP_X+APP_W/2,APP_Y+7*APP_H/8,current_pen_color);
		circlefill(current,APP_X+APP_W/2,APP_Y+3*APP_H/4,APP_W/12,current_pen_color);
	}

	//APP_HALF_ELLIPSE
	app_components[APP_HALF_ELLIPSE]=make_tile();
	current=app_components[APP_HALF_ELLIPSE];
	ellipsefill(current,APP_X+APP_W/2,APP_Y,3*APP_W/8+1,APP_H+1,current_pen_color);
	ellipsefill(current,APP_X+APP_W/2,APP_Y,3*APP_W/8,APP_H,current_bg_color);
	rectfill(current,0,0,TILE_FULL_WIDTH,APP_Y,fpge_mask_color);

	//APP_SUBMARINE
	app_components[APP_SUBMARINE]=make_tile();
	current=app_components[APP_SUBMARINE];
	rectfill(current,
			APP_X+3*APP_W/8,APP_Y+4*APP_H/8,
			APP_X+5*APP_W/8,APP_Y+6*APP_H/8,current_pen_color);
	triangle(current,
			APP_X+3*APP_W/8,APP_Y+4*APP_H/8,
			APP_X+3*APP_W/8,APP_Y+6*APP_H/8,
			APP_X+2*APP_W/8,APP_Y+5*APP_H/8,current_pen_color);
	triangle(current,
			APP_X+5*APP_W/8,APP_Y+4*APP_H/8,
			APP_X+5*APP_W/8,APP_Y+6*APP_H/8,
			APP_X+6*APP_W/8,APP_Y+5*APP_H/8,current_pen_color);
	vline(current,APP_X+4*APP_W/8,APP_Y,APP_Y+APP_H/2,current_pen_color);

	//APP_CIRCLE
	app_components[APP_CIRCLE]=make_tile();
	current=app_components[APP_CIRCLE];
	circlefill(current,APP_X+APP_W/2,APP_Y+APP_H/2,APP_H/2,current_bg_color);
	circle(current,APP_X+APP_W/2,APP_Y+APP_H/2,APP_H/2,current_pen_color);

	//APP_DESTROYER
	app_components[APP_DESTROYER]=make_tile();
	current=app_components[APP_DESTROYER];
	textout_centre_ex(current,font,"DD",APP_X+APP_W/2,APP_Y+APP_H/2-4,current_pen_color,fpge_mask_color);

	//APP_CRUISER
	app_components[APP_CRUISER]=make_tile();
	current=app_components[APP_CRUISER];
	textout_centre_ex(current,font,"CC",APP_X+APP_W/2,APP_Y+APP_H/2-4,current_pen_color,fpge_mask_color);

	//APP_BATTLESHIP
	app_components[APP_BATTLESHIP]=make_tile();
	current=app_components[APP_BATTLESHIP];
	textout_centre_ex(current,font,"BB",APP_X+APP_W/2,APP_Y+APP_H/2-4,current_pen_color,fpge_mask_color);

	//APP_CARRIER
	app_components[APP_CARRIER]=make_tile();
	current=app_components[APP_CARRIER];
	triangle(current,
			APP_X+2*APP_W/8,APP_Y+4*APP_H/8,
			APP_X+6*APP_W/8,APP_Y+4*APP_H/8,
			APP_X+4*APP_W/8,APP_Y+6*APP_H/8,current_pen_color);
	rectfill(current,APP_X+2*APP_W/8,APP_Y+4*APP_H/8,APP_X+4*APP_W/8,APP_Y+2*APP_H/8,current_pen_color);

	//APP_LAND_TRANSPORT
	app_components[APP_LAND_TRANSPORT]=make_tile();
	current=app_components[APP_LAND_TRANSPORT];
	int diff=2;
	circle(current,APP_X+APP_W/2,APP_Y+APP_H/2,3*APP_H/8,current_pen_color);
    vline(current,APP_X+APP_W/2,APP_Y+1*APP_H/8,APP_Y+7*APP_H/8,current_pen_color);
    line(current,
    		APP_X+2*APP_W/8+diff,APP_Y+2*APP_H/8+diff,
    		APP_X+6*APP_W/8-diff,APP_Y+6*APP_H/8-diff
    		,current_pen_color);
    line(current,
    		APP_X+2*APP_W/8+diff,APP_Y+6*APP_H/8-diff,
    		APP_X+6*APP_W/8-diff,APP_Y+2*APP_H/8+diff
    		,current_pen_color);

	//APP_AIR_TRANSPORT
	app_components[APP_AIR_TRANSPORT]=make_tile();
	current=app_components[APP_AIR_TRANSPORT];
	textout_centre_ex(current,font,"C",APP_X+APP_W/2,APP_Y+APP_H/2-4,current_pen_color,fpge_mask_color);

	//APP_SEA_TRANSPORT
	app_components[APP_SEA_TRANSPORT]=make_tile();
	current=app_components[APP_SEA_TRANSPORT];
	textout_centre_ex(current,font,"LC",APP_X+APP_W/2,APP_Y+APP_H/2-4,current_pen_color,fpge_mask_color);

	//APP_RHALF_ELLIPSE
	app_components[APP_RHALF_ELLIPSE]=make_tile();
	current=app_components[APP_RHALF_ELLIPSE];
	ellipsefill(current,APP_X+APP_W/2,APP_Y+APP_H,3*APP_W/8+1,APP_H+1,current_pen_color);
	ellipsefill(current,APP_X+APP_W/2,APP_Y+APP_H,3*APP_W/8,APP_H,current_bg_color);
	rectfill(current,0,APP_Y+APP_H,TILE_FULL_WIDTH,TILE_HEIGHT,fpge_mask_color);

	//APP_LEVEL_BOMBER
	app_components[APP_LEVEL_BOMBER]=make_tile();
	current=app_components[APP_LEVEL_BOMBER];
	if (!alt_aircraft){
		textout_centre_ex(current,font,"B",APP_X+APP_W/2,APP_Y+APP_H/2-4,current_pen_color,fpge_mask_color);
	}
	else{
		line(current,APP_X+APP_W/4,APP_Y+APP_H/4,APP_X+3*APP_W/4,APP_Y+3*APP_H/4,current_pen_color);
		line(current,APP_X+APP_W/4,APP_Y+3*APP_H/4,APP_X+3*APP_W/4,APP_Y+APP_H/4,current_pen_color);
		arc(current,APP_X+APP_W/4,APP_Y+APP_H/2,itofix(64),itofix(-64),APP_H/4,current_pen_color);
		arc(current,APP_X+3*APP_W/4,APP_Y+APP_H/2,itofix(-64),itofix(64),APP_H/4,current_pen_color);

		line(current,APP_X+APP_W/2,APP_Y+APP_H/8,APP_X+APP_W/2,APP_Y+7*APP_H/8,current_pen_color);
		circlefill(current,APP_X+APP_W/2,APP_Y+3*APP_H/4,APP_W/12,current_pen_color);
	}

	//APP_HEAVY
	app_components[APP_HEAVY]=make_tile();
	current=app_components[APP_HEAVY];
	textout_centre_ex(current,font,"H",APP_X+APP_W/2,APP_Y+APP_H/2+6,current_pen_color,fpge_mask_color);

	//APP_ENGENEER
	app_components[APP_ENGENEER]=make_tile();
	current=app_components[APP_ENGENEER];
	line(current,APP_X+2*APP_W/8,APP_Y+3*APP_H/8,APP_X+6*APP_W/8,APP_Y+3*APP_H/8,current_pen_color);
	line(current,APP_X+2*APP_W/8,APP_Y+3*APP_H/8,APP_X+2*APP_W/8,APP_Y+5*APP_H/8,current_pen_color);
	line(current,APP_X+4*APP_W/8,APP_Y+3*APP_H/8,APP_X+4*APP_W/8,APP_Y+5*APP_H/8,current_pen_color);
	line(current,APP_X+6*APP_W/8,APP_Y+3*APP_H/8,APP_X+6*APP_W/8,APP_Y+5*APP_H/8,current_pen_color);

	//APP_MOUNTAIN
	app_components[APP_MOUNTAIN]=make_tile();
	current=app_components[APP_MOUNTAIN];
	triangle(current,
			APP_X+2*APP_W/8,APP_Y+8*APP_H/8,
			APP_X+6*APP_W/8,APP_Y+8*APP_H/8,
			APP_X+4*APP_W/8,APP_Y+6*APP_H/8,current_pen_color);

	//APP_PARA
	app_components[APP_PARA]=make_tile();
	current=app_components[APP_PARA];
	triangle(current,
			APP_X+2*APP_W/8,APP_Y+4*APP_H/8,
			APP_X+6*APP_W/8,APP_Y+4*APP_H/8,
			APP_X+4*APP_W/8,APP_Y+7*APP_H/8,current_bg_color);
	line(current,APP_X+2*APP_W/8,APP_Y+4*APP_H/8,APP_X+6*APP_W/8,APP_Y+4*APP_H/8,current_pen_color);
	line(current,APP_X+2*APP_W/8,APP_Y+4*APP_H/8,APP_X+4*APP_W/8,APP_Y+7*APP_H/8,current_pen_color);
	line(current,APP_X+6*APP_W/8,APP_Y+4*APP_H/8,APP_X+4*APP_W/8,APP_Y+7*APP_H/8,current_pen_color);

	arc(current,APP_X+4*APP_W/8,APP_Y+4*APP_H/8,
    		 itofix(0),itofix(128),
    		 2*APP_W/8,current_pen_color);
	floodfill(current,APP_X+4*APP_W/8,APP_Y+3*APP_H/8,current_bg_color);

	//APP_RED_RECT
	app_components[APP_RED_RECT]=make_tile();
	current=app_components[APP_RED_RECT];
	masked_blit(app_components[APP_RECT],current,0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);
	floodfill(current,APP_X+4*APP_W/8,APP_Y+4*APP_H/8,colors_to24bits(COLOR_RED));

	/////////////////
    current_bg_color=colors_to24bits(COLOR_RED);
	//APP_RED_HALF_ELLIPSE
	app_components[APP_RED_HALF_ELLIPSE]=make_tile();
	current=app_components[APP_RED_HALF_ELLIPSE];
	ellipsefill(current,APP_X+APP_W/2,APP_Y,3*APP_W/8+1,APP_H+1,current_pen_color);
	ellipsefill(current,APP_X+APP_W/2,APP_Y,3*APP_W/8,APP_H,current_bg_color);
	rectfill(current,0,0,TILE_FULL_WIDTH,APP_Y,fpge_mask_color);

	//APP_RED_RHALF_ELLIPSE
	app_components[APP_RED_RHALF_ELLIPSE]=make_tile();
	current=app_components[APP_RED_RHALF_ELLIPSE];
	ellipsefill(current,APP_X+APP_W/2,APP_Y+APP_H,3*APP_W/8+1,APP_H+1,current_pen_color);
	ellipsefill(current,APP_X+APP_W/2,APP_Y+APP_H,3*APP_W/8,APP_H,current_bg_color);
	rectfill(current,0,APP_Y+APP_H,TILE_FULL_WIDTH,TILE_HEIGHT,fpge_mask_color);

	//APP_RED_CIRCLE
	app_components[APP_RED_CIRCLE]=make_tile();
	current=app_components[APP_RED_CIRCLE];
	circlefill(current,APP_X+APP_W/2,APP_Y+APP_H/2,APP_H/2,current_bg_color);
	circle(current,APP_X+APP_W/2,APP_Y+APP_H/2,APP_H/2,current_pen_color);

	//APP_RED_PARA
	app_components[APP_RED_PARA]=make_tile();
	current=app_components[APP_RED_PARA];
	masked_blit(app_components[APP_PARA],current,0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);
	floodfill(current,APP_X+4*APP_W/8,APP_Y+4*APP_H/8-5,colors_to24bits(COLOR_RED));
	floodfill(current,APP_X+4*APP_W/8,APP_Y+4*APP_H/8+5,colors_to24bits(COLOR_RED));

	//APP_FORT
	app_components[APP_FORT]=make_tile();
	current=app_components[APP_FORT];
	line(current,APP_X+0*APP_W/8,APP_Y+0*APP_H/8,APP_X+2*APP_W/8,APP_Y+2*APP_H/8,current_pen_color);
	line(current,APP_X+0*APP_W/8,APP_Y+8*APP_H/8,APP_X+2*APP_W/8,APP_Y+6*APP_H/8,current_pen_color);
	line(current,APP_X+8*APP_W/8,APP_Y+0*APP_H/8,APP_X+6*APP_W/8,APP_Y+2*APP_H/8,current_pen_color);
	line(current,APP_X+8*APP_W/8,APP_Y+8*APP_H/8,APP_X+6*APP_W/8,APP_Y+6*APP_H/8,current_pen_color);

	line(current,APP_X+2*APP_W/8,APP_Y+2*APP_H/8,APP_X+6*APP_W/8,APP_Y+2*APP_H/8,current_pen_color);
	line(current,APP_X+2*APP_W/8,APP_Y+6*APP_H/8,APP_X+6*APP_W/8,APP_Y+6*APP_H/8,current_pen_color);

	line(current,APP_X+2*APP_W/8,APP_Y+2*APP_H/8,APP_X+2*APP_W/8,APP_Y+6*APP_H/8,current_pen_color);
	line(current,APP_X+6*APP_W/8,APP_Y+2*APP_H/8,APP_X+6*APP_W/8,APP_Y+6*APP_H/8,current_pen_color);


}

void draw_app6_unit_symbol(BITMAP *where_bmp, int x0,int y0, int idx, int side){

	int class = equip[idx][CLASS];
	int movement_type=equip[idx][MOV_TYPE];
	int range =equip[idx][RANGE];
	int gd=equip[idx][GD];
	int ad=equip[idx][AD];

	rectfill(where_bmp,x0,y0,x0+TILE_FULL_WIDTH,y0+TILE_HEIGHT,fpge_mask_color);


	if (class==CLASS_INFANTRY) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);

		if (equip_flags[idx]&EQUIPMENT_CAN_BRIDGE_RIVERS)
			masked_blit(app_components[APP_ENGENEER],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		else{
			//for bad equip files...
		if (((equip_flags[idx]&EQUIPMENT_CAN_HAVE_ORGANIC_TRANSPORT) && equip[idx][MOV]<mov_cavalery)||movement_type == MOV_TYPE_MOUNTAIN){
			masked_blit(app_components[APP_CROSS],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		}else
			masked_blit(app_components[APP_RECON],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		}

		if (movement_type == MOV_TYPE_MOUNTAIN)
		    masked_blit(app_components[APP_MOUNTAIN],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);

		if (equip[idx][ALLOWED_TRANSPORT]==3)
			masked_blit(app_components[side?APP_RED_PARA:APP_PARA],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);

	}
	if (class==CLASS_TANK) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_ARMOUR],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (gd>=gd_tank_heavy && ad>=ad_tank_heavy)
			masked_blit(app_components[APP_HEAVY],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}

	if (class==CLASS_RECON) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_RECON],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_WHEELED || movement_type == MOV_TYPE_HALFTRACKED  || movement_type == MOV_TYPE_ALLTERRAIN)
			masked_blit(app_components[APP_WHEELS],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_TRACKED)
			masked_blit(app_components[APP_ARMOUR],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}

	if (class==CLASS_ANTI_TANK) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_ANTI_TANK],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_TRACKED)
			masked_blit(app_components[APP_ARMOUR],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_WHEELED || movement_type == MOV_TYPE_HALFTRACKED)
			masked_blit(app_components[APP_WHEELS],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	if (class==CLASS_ARTILLERY) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_ARTY],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_TRACKED)
			masked_blit(app_components[APP_ARMOUR],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_WHEELED || movement_type == MOV_TYPE_HALFTRACKED)
			masked_blit(app_components[APP_WHEELS],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	if (class==CLASS_ANTI_AIRCARFT) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_ANTI_AIRCRAFT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_TRACKED)
			masked_blit(app_components[APP_ARMOUR],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_WHEELED || movement_type == MOV_TYPE_HALFTRACKED)
			masked_blit(app_components[APP_WHEELS],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	if (class==CLASS_AIR_DEFENCE) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_ANTI_AIRCRAFT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_ARTY],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_TRACKED)
			masked_blit(app_components[APP_ARMOUR],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (movement_type == MOV_TYPE_WHEELED || movement_type == MOV_TYPE_HALFTRACKED)
			masked_blit(app_components[APP_WHEELS],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}

	if (class==CLASS_FORTIFICATION) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_FORT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (range>=0)
			masked_blit(app_components[APP_ARTY],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);

	}

	if (class==CLASS_FIGHTER) {
		if (alt_aircraft)
			masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		else
			masked_blit(app_components[side?APP_RED_RHALF_ELLIPSE:APP_RHALF_ELLIPSE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);

		masked_blit(app_components[APP_FIGHTER],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}

	if (class==CLASS_TACTICAL_BOMBER ) {
		if (alt_aircraft)
			masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		else
			masked_blit(app_components[side?APP_RED_RHALF_ELLIPSE:APP_RHALF_ELLIPSE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);

		masked_blit(app_components[APP_BOMBER],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}

	if (class==CLASS_LEVEL_BOMBER) {
		if (alt_aircraft)
			masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		else
			masked_blit(app_components[side?APP_RED_RHALF_ELLIPSE:APP_RHALF_ELLIPSE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);

		masked_blit(app_components[APP_LEVEL_BOMBER],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_HEAVY],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}

	if (class==CLASS_SUBMARINE) {
		masked_blit(app_components[side?APP_RED_HALF_ELLIPSE:APP_HALF_ELLIPSE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_SUBMARINE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	if (class==CLASS_DESTROYER) {
		masked_blit(app_components[side?APP_RED_CIRCLE:APP_CIRCLE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_DESTROYER],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	if (class==CLASS_CAPITAL_SHIP) {
		masked_blit(app_components[side?APP_RED_CIRCLE:APP_CIRCLE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		if (range>=bb_range)
			masked_blit(app_components[APP_BATTLESHIP],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		else
			masked_blit(app_components[APP_CRUISER],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}

	if (class==CLASS_AIR_CARRIER) {
		masked_blit(app_components[side?APP_RED_CIRCLE:APP_CIRCLE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_CARRIER],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	if (class==CLASS_LAND_TRANSPORT) {
		masked_blit(app_components[side?APP_RED_RECT:APP_RECT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_LAND_TRANSPORT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	if (class==CLASS_AIR_TRANSPORT) {
		masked_blit(app_components[APP_RHALF_ELLIPSE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_AIR_TRANSPORT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	if (class==CLASS_SEA_TRANSPORT) {
		masked_blit(app_components[APP_CIRCLE],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
		masked_blit(app_components[APP_SEA_TRANSPORT],where_bmp,0,0,x0,y0,TILE_FULL_WIDTH,TILE_HEIGHT);
	}
	//Modifiers
	}
