/*
 * unitlist.h
 *
 *  Created on: 2010-05-03
 *      Author: wino
 */

#ifndef UNITLIST_H_
#define UNITLIST_H_

#define ULD_DLG 16
#define UL_BUTTON_W 80

extern DIALOG unitslist_dlg[ULD_DLG];

int d_ullist_proc(int msg, DIALOG *d, int c);
void find_xy(int idx, unsigned char isAir, int *x, int *y);
int findOffset_ex(enum UTYPES unit_type_ex);

#endif /* UNITLIST_H_ */
