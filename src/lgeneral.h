/*
 * lgeneral.h
 *
 *  Created on: 2011-09-27
 *      Author: wino
 */

#ifndef LGENERAL_H_
#define LGENERAL_H_

#define LG_U_DLG 11
extern DIALOG lgen_uicons_dlg[LG_U_DLG];

#define LG_EQP_DLG 9
extern DIALOG lgen_equip_dlg[LG_EQP_DLG];

int save_lgeneral_equ();
//int save_lgeneral_map_file(char *name);
int save_lgeneral_map();
int save_lgeneral_scenario();
int save_lgeneral_units_bmp();
int save_lgeneral_nations();

int isFlipNeeded(int idx);

#endif /* LGENERAL_H_ */
