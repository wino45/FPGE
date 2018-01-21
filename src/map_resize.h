/*
 * map_resize.h
 *
 *  Created on: 2010-08-04
 *      Author: wino
 */

#ifndef MAP_RESIZE_H_
#define MAP_RESIZE_H_

#include <allegro.h>

#define MapResizeXIdx 4
#define MapResizeYIdx 5
#define MapResizeCurrentXIdx 6
#define MapResizeCurrentYIdx 7
#define MapResizeOffXIdx 8
#define MapResizeOffYIdx 9
#define MapResizeResizeIdx 12
#define MapResizeInfoIdx 14

#define RESIZE_DLG_SIZE 17

extern DIALOG resize_dlg[RESIZE_DLG_SIZE];

int d_box_proc_my2(int msg, DIALOG *d, int c);
int d_button_proc_center(int msg, DIALOG *d, int c);
int d_button_proc_resize(int msg, DIALOG *d, int c);

#endif /* MAP_RESIZE_H_ */
