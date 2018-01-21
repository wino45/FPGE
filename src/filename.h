/*
 * filename.h
 *
 *  Created on: 2010-03-10
 *      Author: wino
 */

#ifndef FILENAME_H_
#define FILENAME_H_

//FPGE
extern char fpge_config[];
extern char fpge_mapfrg[];
extern char fpge_tiles[];
extern char fpge_countries[];
extern char fpge_icons[];
extern char fpge_bmp2ctry[];
extern char fpge_mapfrgt[];
extern char fpge_mapfrg_fgm[];
extern char fpge_0str_bmp[];
extern char fpge_pgcam_gv[];

//NULP
extern char nulp_pslots[];
extern char nulp_nulp[];
extern char nulp_unit[];

//EXE
extern char exe_ag[];
extern char exe_pgwin[];
extern char exe_panzer[];
extern char exe_pgf[];

//AG
extern char ag_tileart[];

//PGDOS
extern char til_file[];
extern char mtil_file[];
extern char uicons_file[];
extern char muicons_file[];

extern char sicons_file[];
extern char equip_file[];
extern char flag_file[];
extern char mflag_file[];
extern char strength_file[];
extern char names_file[];
extern char description_file[];

//PGF
extern char pgf_equip_file[];
extern char pgf_units_bmp[];
extern char pgf_flags_bmp[];
extern char pgf_stackicn_bmp[];
extern char pgf_strength_bmp[];
extern char pgf_tacmap_dry_bmp[];
extern char pgf_tacmap_muddy_bmp[];
extern char pgf_tacmap_frozen_bmp[];
extern char pgf_pg_pgcam[];

// PacGen-FPGE
extern char pac_tt2tiles[];
extern char pac_cl2pg_cl[];
extern char pac_mt2pg_mt[];
// PacGen
extern char pac_ext[];
extern char pac_pfpdata[];
extern char pac_til[];
extern char pac_shp[];
extern char pac_pal[];
extern char pac_pacequip[];
extern char pac_pacequip_txt[];

//PZC
extern char pzc_equip_file[];
extern char pgu2pzcu_file[];
extern char pgc2pzcc_file[];
extern char pgtt2pzctt_file[];

//PG2
extern char pg2_equip_file[];
extern char pg2_equip_name_file[];
extern char pg2u2pgu_file[];
extern char pg2tt2pgtr_file[];
extern char pg2tt2pgtt_file[];
extern char pg2c2pgc_file[];
extern char pg2cl2pgcl_file[];


#define SCN_NAME_LEN 128
extern int ScenarioNumber;
extern char ScenarioName[SCN_NAME_LEN];

int getScenarioNumber();
void setScenarioNumber(int);

void getScenarioNameRaw(char *);
void setScenarioNameRaw(char *);

#endif /* FILENAME_H_ */
