/*
 * menu.h
 *
 *  Created on: 16-07-2010
 *      Author: wino
 */

#ifndef MENU_H_
#define MENU_H_

extern MENU fpge_menu[];

void select_weather_item();
void select_unique_name_item();
void select_display_name_item();
void select_hex_item();
void select_flag_item();
void select_all_units_item();
void select_unit_order_item();
void select_scenario_units_item();
void select_show_ranges_item();
void select_gnd_transport_item();
void select_road_item();
void select_terrain_item();
void select_all_names_item();
void setup_menu_ticks();
int onExit();
int onUndo();

#endif /* MENU_H_ */
