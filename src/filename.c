/*
 * filename.c
 *
 *  Created on: 2010-03-11
 *      Author: wino
 */
#include <string.h>
#include <stdio.h>

#include "fpge.h"
#include "filename.h"


//FPGE
char fpge_config[]="fpge.cfg";
char fpge_mapfrg[]="mapfrg.txt";
char fpge_tiles[]="tiles.txt";
char fpge_countries[]="countries.txt";
char fpge_icons[]="icons.txt";
char fpge_bmp2ctry[]="bmp2ctry.txt";
char fpge_mapfrgt[]="mapfrgt.fgt"; //text file
char fpge_mapfrg_fgm[]="mapfrg.fgm"; //binary file
char fpge_0str_bmp[]="0str.bmp";
char fpge_pgcam_gv[]="pgcam.gv";

//NULP
char nulp_pslots[]="p_slots.txt";
char nulp_nulp[]="nulp.txt";
char nulp_unit[]="unit.txt";

//EXE
char exe_ag[]="ag.exe";
char exe_pgwin[]="pg.exe";
char exe_panzer[]="panzer.exe";
char exe_pgf[]="pgforever.exe";

//AG
char ag_tileart[]="tileart.dat";

//PGDOS
char til_file[]="tacmap.shp";
char mtil_file[]="stratmap.shp";
char uicons_file[]="tacicons.shp";
char muicons_file[]="sticons.shp";

char sicons_file[]="stackicn.shp";
char equip_file[]="panzequp.eqp";
char flag_file[]="flags.shp";
char mflag_file[]="vichex.shp";
char strength_file[]="strength.shp";
char names_file[]="mapnames.str";
char description_file[]="scenstat.bin";

//PGF
char pgf_equip_file[]="equipment.pgeqp";
char pgf_units_bmp[]="tacicons.bmp";
char pgf_flags_bmp[]="flags.bmp";
char pgf_stackicn_bmp[]="stackicn.bmp";
char pgf_strength_bmp[]="strength.bmp";
char pgf_tacmap_dry_bmp[]="tacmap_dry.bmp";
char pgf_tacmap_muddy_bmp[]="tacmap_muddy.bmp";
char pgf_tacmap_frozen_bmp[]="tacmap_frozen.bmp";
char pgf_pg_pgcam[]="pg.pgcam";

//PacGen-FPGE
char pac_tt2tiles[]="tt2tiles.txt";
char pac_cl2pg_cl[]="paccl2pgcl.txt";
char pac_mt2pg_mt[]="pacmt2pgmt.txt";
//PacGen
char pac_ext[]="ext.idx";
char pac_pfpdata[]="pfpdata.idx";
char pac_til[]="til.pfp";
char pac_shp[]="shp.pfp";
char pac_pal[]="pal.pfp";
char pac_pacequip[]="pacequip.eqp";
char pac_pacequip_txt[]="pacequip.txt";

//PZC
char pzc_equip_file[]="equipment.pzeqp";
char pgu2pzcu_file[]="pgu2pzcu.txt";
char pgc2pzcc_file[]="pgc2pzcc.txt";
char pgtt2pzctt_file[]="pgtt2pzctt.txt";

//PG2
char pg2_equip_file[]="equip97.eqp";
char pg2_equip_name_file[]="equip97.txt";
char pg2u2pgu_file[]="pg2u2pgu.txt";

char pg2tt2pgtr_file[]="pg2tt2pgtr.txt";
char pg2tt2pgtt_file[]="pg2tt2pgtt.txt";
char pg2c2pgc_file[]="pg2c2pgc.txt";
char pg2cl2pgcl_file[]="pg2cl2pgcl.txt";

int ScenarioNumber;
char ScenarioName[SCN_NAME_LEN];


int getScenarioNumber(){
	return ScenarioNumber;
}

void setScenarioNumber(int NewScenarioNumber){
   ScenarioNumber=NewScenarioNumber;
}

void getScenarioNameRaw(char *ScenarioNameBuffer){
	strncpy(ScenarioNameBuffer,ScenarioName,SCN_NAME_LEN);
}

void setScenarioNameRaw(char *ScenarioNameBuffer){
	strncpy(ScenarioName,ScenarioNameBuffer,SCN_NAME_LEN);
}

void getScenarioName(char *ScenarioNameBuffer, int gameMode){
	if (ScenarioNumber>-1){
		if (gameMode==PG_MODE||gameMode==AG_MODE||gameMode==PACGEN_MODE) //pacgen_mode should not be triggered
			sprintf(ScenarioNameBuffer,"game%03d.scn",ScenarioNumber);
		if (gameMode==PGF_MODE)
			sprintf(ScenarioNameBuffer,"%03d.pgscn",ScenarioNumber);
	}
	else{
		if (gameMode==PGF_MODE && strlen(ScenarioName)>0)
			sprintf(ScenarioNameBuffer,"%s.pgscn",ScenarioName);
		else
			sprintf(ScenarioNameBuffer,"game%03d.scn",ScenarioNumber);
	}
}


