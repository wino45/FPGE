/*
 * class.h
 *
 *  Created on: 2011-07-15
 *      Author: wino
 */

#ifndef CLASS_H_
#define CLASS_H_

#include <allegro.h>

#define ClassInfantryChkBoxIdx 3

#define CLASS_DLG (5+2+6+7+3)

extern DIALOG class_dlg[CLASS_DLG];

int d_class_box_proc(int msg,DIALOG *d,int c);
int d_class_cancelbtn_proc(int msg,DIALOG *d,int c);

#endif /* CLASS_H_ */
