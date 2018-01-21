/*
 * pzc.h
 *
 *  Created on: 2011-08-24
 *      Author: wino
 */

#ifndef PZC_H_
#define PZC_H_

#include <allegro.h>

#define PZC_DLG_SIZE 8+2

struct pzc_unit {
	char name[256];
	int class;
	int target_type;
	int flag;
	int id;
};
extern int pgunit_to_pzcunit[MAX_UNITS];
extern struct pzc_unit pzc_unit_conversion[MAX_UNITS];

extern DIALOG findpzcid_dlg[PZC_DLG_SIZE];

char *pzc_list(int index, int *list_size);
int d_pzc_list_proc(int msg, DIALOG *d, int c);
int save_pzscn();
int save_pzeqp();
int load_pzc_equipment(int probe_file_only, char *fname);
int load_pg_to_pzc_conversion_file(int probe_file_only, char *fname, int mode);
int save_pgunit_to_pzcunit();
int save_pzc_units();
int save_pzc_flags();
int save_pzc_tiles();

int get_new_id(int unum, int country);

#endif /* PZC_H_ */
