/*
 * pg.h
 *
 *  Created on: 2011-09-04
 *      Author: wino
 */

#ifndef PG_H_
#define PG_H_

int load_equip(int , char *fname);
int parse_scn_buffer(int show_info);
int parse_set_file(FILE *inf, int offset);
int parse_stm_file(FILE *inf, int offset);

int load_scn(int , int , int);
int load_scenario(int scenario_number,int show_info);
int load_names();

int load_tiles_layer(char *name);
int load_roads_layer(char *name);
int load_names_layer(char *name);
int load_terrain_type_layer(char *name);

int save_equip_dialog();
int convert_brf();

#endif /* PG_H_ */
