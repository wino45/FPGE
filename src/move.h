/*
 * move.h
 *
 *  Created on: 07-04-2012
 *      Author: wino
 */

#ifndef MOVE_H_
#define MOVE_H_

#define PG_MOVE_BIN_TABLE_SIZE 288
#define PGF_MOVE_BIN_TABLE_SIZE (288+12*3*3)
#define PG_WEATHER_BIN_TABLE_SIZE 144

#define MOVE_NOT_CHECKED 255

#define MOVE_MAP_X MAX_MAP_X
#define MOVE_MAP_Y MAX_MAP_Y

extern unsigned char move_points[MOVE_MAP_X][MOVE_MAP_Y];
extern unsigned char move_directions[MOVE_MAP_X][MOVE_MAP_Y];;
extern unsigned char move_points_t[MOVE_MAP_X][MOVE_MAP_Y];
extern unsigned char move_directions_t[MOVE_MAP_X][MOVE_MAP_Y];
extern unsigned char PgStaticMoveTable[PGF_MOVE_BIN_TABLE_SIZE]; //PGF table is bigger
extern unsigned char PgStaticWeatherTable[PG_WEATHER_BIN_TABLE_SIZE];

void move_init();
int edit_mode_trigger();
int move_click();

#endif /* MOVE_H_ */
