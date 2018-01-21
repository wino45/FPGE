/*
 * move.h
 *
 *  Created on: 07-04-2012
 *      Author: wino
 */

#ifndef MOVE_H_
#define MOVE_H_

#define PG_MOVE_BIN_TABLE_SIZE (12*8*3)
#define PGF_MOVE_BIN_TABLE_SIZE (12*11*3)
#define PACGEN_MOVE_BIN_TABLE_SIZE (37*11*3)
#define MOVE_BIN_TABLE_SIZE (37*11*3)

#define PG_WEATHER_BIN_TABLE_SIZE 144
#define WEATHER_BIN_TABLE_SIZE (8*12*4)
#define PACGEN_WEATHER_BIN_TABLE_SIZE (8*12*4)

#define MOVE_NOT_CHECKED 255
#define MOVE_NO_PATH 255

#define MOVE_MAP_X MAX_MAP_X
#define MOVE_MAP_Y MAX_MAP_Y

extern unsigned char move_points[MOVE_MAP_X][MOVE_MAP_Y];
extern unsigned char move_directions_RC[MOVE_MAP_X][MOVE_MAP_Y];;
extern unsigned char move_points_transport[MOVE_MAP_X][MOVE_MAP_Y];
extern unsigned char move_directions_transport_RC[MOVE_MAP_X][MOVE_MAP_Y];
extern unsigned char move_path[MOVE_MAP_X][MOVE_MAP_Y];

extern unsigned char PgStaticMoveTable[PACGEN_MOVE_BIN_TABLE_SIZE]; //pacgen table is the biggest
extern unsigned char PgStaticWeatherTable[WEATHER_BIN_TABLE_SIZE];

extern int move_x0;
extern int move_y0;

void move_init();
int edit_mode_trigger();
int move_click();

#endif /* MOVE_H_ */
