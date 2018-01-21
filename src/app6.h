/*
 * app6.h
 *
 *  Created on: 2011-05-09
 *      Author: wino
 */

#ifndef APP6_H_
#define APP6_H_

#define APP_TILES 50

#define APP_X 5
#define APP_Y 7
#define APP_W 50
#define APP_H 33

//components
#define APP_RECT 0
#define APP_CROSS 1
#define APP_RECON 2
#define APP_ARTY 3
#define APP_ARMOUR 4
#define APP_MOUNTAIN 5
#define APP_WHEELS 6
#define APP_ANTI_TANK 7
#define APP_ANTI_AIRCRAFT 8
#define APP_FORT 9
#define APP_FIGHTER 10
#define APP_BOMBER 11
#define APP_SUBMARINE 12
#define APP_HALF_ELLIPSE 13
#define APP_CIRCLE 14
#define APP_DESTROYER 15
#define APP_CRUISER 16
#define APP_BATTLESHIP 17
#define APP_CARRIER 18
#define APP_LAND_TRANSPORT 19
#define APP_AIR_TRANSPORT 20
#define APP_SEA_TRANSPORT 21
#define APP_RHALF_ELLIPSE 22
#define APP_LEVEL_BOMBER 23
#define APP_HEAVY 24
#define APP_ENGENEER 25
#define APP_PARA 26
#define APP_RED_RECT 27
#define APP_RED_HALF_ELLIPSE 28
#define APP_RED_RHALF_ELLIPSE 29
#define APP_RED_CIRCLE 30
#define APP_RED_PARA 31

extern int draw_app6;
extern int draw_app6_units;
extern int draw_app6_on_the_fly;
extern int draw_app6_tiles;
extern int draw_app6_color;

void draw_app6_unit_symbol(BITMAP *where_bmp, int x0,int y0, int ,int);
void init_app_components();

#endif /* APP6_H_ */
