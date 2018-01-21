/*
 * pacgen.h
 *
 *  Created on: 2011-01-04
 *      Author: wino
 */

#ifndef PACGEN_H_
#define PACGEN_H_

#define NO_OF_PACGEN_TIL_CLASSES 17
#define NO_OF_PACGEN_TERRAIN_TYPES 37
#define NO_OF_PACGEN_CLASSES 20
#define NO_OF_PACGEN_MOVE_TYPES 11
#define NO_OF_PACGEN_TARGET_TYPES 5

#define LOAD_PACGEN_MODE_TT2TILES 0
#define LOAD_PACGEN_CLASSES 1
#define LOAD_PACGEN_MOVE_TYPES 2

#define PACGEN_MAP_HEX_SIZE 37
#define PACGEN_EQUIP_REC_SIZE 168
#define PACGEN_SCN_UNIT_REC_SIZE 218
#define PACGEN_STACKICN_SIZE 22

#define PACGEN_MAX_CORE_UNITS 80
#define PACGEN_MAX_AUX_UNITS 120

//extern int count_per_set[NO_OF_PACGEN_TIL_CLASSES];;
extern int no_of_sets;

extern DIALOG pacgen_dlg[7+5+1-2];

int load_ext_idx();
int load_pfpdata_idx();

int import_pacgen_scenario();
int load_pacgen_tt2tiles();
int load_pacgen_cl2pg_cl();
int load_pacgen_mt2pg_mt();

int load_pacgen_equip();
void initialize_pacgen_countries_table();
void initialize_pacgen_std_names();

#endif /* PACGEN_H_ */
