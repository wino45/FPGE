/*
 * unit.h
 *
 *  Created on: 2010-05-04
 *      Author: wino
 */

#ifndef UNIT_H_
#define UNIT_H_

//display the unit info for the unit selected in the box
//idx is for equip list
#define INFO_X 195
#define INFO_Y 360
#define INFO_SPACE 15
#define INFO_W 220+26
//#define INFO_H 100
#define INFO_H INFO_SPACE*8-2

#define U_BUTTON_W 96

#define FILTER_COUNTRIES_ALL 0
#define FILTER_COUNTRIES_SCENARIO_ONLY 1

#define udDoneBtnIdx 1
#define udSetBtnIdx 2
#define udListBoxIdx 21
#define udAllUnitIdx 28
#define udScenarioCountryUnitsOnlyIdx 30
#define udExcludeReservedIdx udScenarioCountryUnitsOnlyIdx+1

extern int d_my_textbox_proc(int msg,DIALOG *d,int c);
extern void display_unit_info(int idx, int x0, int y0, int fg, int bg);
extern void make_countries_table(int);
extern int get_country_from_current_list(int idx);

extern int scenario_units_only_filter;
extern int exclude_reserved_units_filter;

#define UNIT_DLG_SIZE 35

extern DIALOG unit_dlg[UNIT_DLG_SIZE];

#endif /* UNIT_H_ */
