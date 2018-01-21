/*
 * pg2.h
 *
 *  Created on: 2011-10-24
 *      Author: wino
 */

#ifndef PG2_H_
#define PG2_H_

#define PG2_MAP_FILE_SIZE 12610
#define PG2_MAP_X 45
#define PG2_MAP_Y 40

#define PG2_MAX_FILES_IN_DAT_FILE 30000

#define PG2_LOAD_UNITSID 0
#define PG2_LOAD_TT2TRID 1
#define PG2_LOAD_TT2TTID 2
#define PG2_LOAD_C2CID 3
#define PG2_LOAD_CL2CLID 4

#define PG2_CAM_DLG_SIZE 8
extern DIALOG pg2_cam_dlg[PG2_CAM_DLG_SIZE];

#define PG2_IDCONVERT_DLG_SIZE 8+2+3+1
extern DIALOG findpg2id_dlg[PG2_IDCONVERT_DLG_SIZE];

#define PG2_DLG_SIZE (12+1)
#define PG2_MAP_DLG_SIZE (12+1-3)
extern DIALOG pg2_convert_dlg[PG2_DLG_SIZE];
extern DIALOG pg2_convert_map_dlg[PG2_MAP_DLG_SIZE];

#define PG2_DLG_OK 10
#define PG2_DLG_TERRAIN_TYPE 1
#define PG2_DLG_ROADS 2
#define PG2_DLG_GEN_TILES 3
#define PG2_DLG_GEN_ROAD_TILES 4

#define PG2_DLG_NO_UNITS 6
#define PG2_DLG_CONVERT_UNITS 7
#define PG2_DLG_COPY_UNITS 8

#define PG2_UNITMODE_NOUNITS 0
#define PG2_UNITMODE_CONVERT 1
#define PG2_UNITMODE_COPY 2

#define FINDPG2ID_OK 3

#define PG2_CAM_DLG_OK 4


struct pg2_unit {
	uint8_t status_1, status_2;
	uint8_t uk1[6];
	uint16_t name, x, y, total_unit_count, player_unit_count, unit_equ_id, transport_type;
	uint16_t unit_appearance, uk2, experience, unit_mount_code, leader_index;
	uint8_t leader_chance, experience_level_indicator;
	//uint16_t deployment_status; //??
	uint8_t player, side, flag, suppression, strength, remaining_movement, remaining_fuel, remaining_ammunition_x_2;
	uint8_t entrenchment, entrenchment_ticks;
	uint8_t uk3[5], hits;
	uint16_t direction;
	uint8_t arrival_turn_as_reinforcement, basic_or_original_strength;
	uint8_t kill_stats[8];
	uint8_t uk4[3];
};

extern uint32_t panzer2_file_icon_off_2_local_icon_idx_max;

int import_pg2_map_scn_gui();
int import_pg2_equip_dialog();

char *pg2_list(int index, int *list_size);
int d_pg2_list_proc(int msg, DIALOG *d, int c);
int d_pg2_country_check_proc(int msg, DIALOG *d, int c);

void load_pg2_conversion_tables();
int save_pg2unit_to_pgunit();
void filter_pg_units(int pg_country, int pg_class, int pg_type);
int convert_pg2_campaign_gui();
int convert_pg2_icons();

int import_pg2_icons_idx(int probe_file_only, char *fname);
int import_pg2_equip(int probe_file_only, char *fname);

int pg_dg();

#endif /* PG2_H_ */
