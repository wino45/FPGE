/*
 * pg.c
 *
 *  Created on: 2011-09-04
 *      Author: wino
 */
#include <allegro.h>
#include <stdio.h>
#include "fpge.h"
#include "pg.h"
#include "filename.h"
#include "tables.h"
#include "pgf.h"
#include "ag.h"
#include "minimap.h"
#include "maingui.h"
#include "save.h"

char stm_fname[16];

int load_equip(int probe_file_only, char *fname)
{
   int i;
   FILE *inf;
   unsigned char equip_buffer[EQUIP_REC_SIZE];
   char path[MAX_PATH];

   strncpy(path,fname,MAX_PATH);
   canonicalize_filename(path,path,MAX_PATH);
   inf=fopen(path,"rb");
   if (!inf)
   {
		sprintf(path,"..\\data\\%s",fname);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf)
			//printf("Could not load equipment file\n");
			return ERROR_EQUIP_BASE+ERROR_FPGE_FILE_NOT_FOUND;
   }
   if (probe_file_only!=LOAD_CONVERSION_TABLE_ONLY){
	   total_equip=0;
	   fread(&total_equip,2,1,inf);
	   if (total_equip>MAX_UNITS)
	   {
		   fclose(inf);
		   printf("Maximum units limit (%d) is exceeded!!!\n", MAX_UNITS);
		  return ERROR_EQUIP_BASE+ERROR_FPGE_MAXIMUM_REACHED;
	   }
	   for (i=0; i<total_equip; ++i){
		 fread(&(equip[i]),EQUIP_REC_SIZE,1,inf);
		 // convert from cp1250 to utf8
		 convert_from_cp1250_to_utf8(equip_name_utf8[i],equip[i],20);
		 equip_country[i]=-1;
		 equip_flags[i]=EQUIPMENT_HARDCODED;
		 if (i==48 || i==215 || i==276 || i==330 || i==386 || i==387 || i==388) equip_flags[i]|=EQUIPMENT_IGNORES_ENTRENCHMENT;
		 if (i==388 || i==330 || i==276 || i==215 ) equip_flags[i]|=EQUIPMENT_CAN_BRIDGE_RIVERS;
		 if (i!=120 && i!=275 && (equip[i][MOV_TYPE]==MOV_TYPE_LED || equip[i][MOV_TYPE]==MOV_TYPE_TOWED || equip[i][MOV_TYPE]==MOV_TYPE_MOUNTAIN) )
			 equip_flags[i]|=EQUIPMENT_CAN_HAVE_ORGANIC_TRANSPORT;
		 }
   }else{
	   //conversion only
	   fread(&conversion_total_equip,2,1,inf);
	   if (conversion_total_equip>MAX_UNITS)
	   {
		   fclose(inf);
		   printf("Maximum units limit (%d) is exceeded!!!\n", MAX_UNITS);
		  return ERROR_EQUIP_BASE+ERROR_FPGE_MAXIMUM_REACHED;
	   }
	   for (i=0; i<conversion_total_equip; ++i){
		 fread(&(equip_buffer),EQUIP_REC_SIZE,1,inf);
		 // convert from cp1250 to utf8
		 convert_from_cp1250_to_utf8(conversion_equip[i].name,equip_buffer,20);
		 conversion_equip[i].country=-1; //any country
		 conversion_equip[i].new_id=i;
		 }
   }
   fclose(inf);
   return 0;
}

int parse_scn_buffer(int show_info){
  int i,units_start,idx;
  int unum,x,y;

  total_deploy=scn_buffer[DEPLOY_TABLE];
  total_axis_core=scn_buffer[CORE_UNITS];
  total_allied_core=scn_buffer[ALLIED_UNITS];
  total_axis_aux=scn_buffer[AUX_UNITS];
  total_allied_aux=scn_buffer[AUX_ALLIED_UNITS];

  units_start=total_deploy*4+DEPLOY_TO_UNITS;
  idx=units_start; //place in buffer
  total_units=0;
  //read the core
  if (show_info) printf("Reading the core units...\n");
  for (i=0; i<total_axis_core; ++i,++total_units)
  {
    all_units[i].unum=scn_buffer[idx+0]+256*scn_buffer[idx+1];
    all_units[i].orgtnum=scn_buffer[idx+2]+256*scn_buffer[idx+3];
    all_units[i].country=scn_buffer[idx+4];
    all_units[i].auxtnum=scn_buffer[idx+5]+256*scn_buffer[idx+6];
    all_units[i].x=scn_buffer[idx+7]+256*scn_buffer[idx+8];
    all_units[i].y=scn_buffer[idx+9]+256*scn_buffer[idx+10];
    all_units[i].str=scn_buffer[idx+11];
    all_units[i].entrench=scn_buffer[idx+12];
    all_units[i].exp=scn_buffer[idx+13];
    all_units[i].uflag=axis_core;
    all_units[i].fuel=-1;
    all_units[i].ammo=-1;
    idx+=14;
  }

  //read the aux
  if (show_info) printf("Reading the aux units...\n");
  for (i=0; i<total_axis_aux; ++i,++total_units)
  {
    all_units[total_units].unum=scn_buffer[idx+0]+256*scn_buffer[idx+1];
    all_units[total_units].orgtnum=scn_buffer[idx+2]+256*scn_buffer[idx+3];
    all_units[total_units].country=scn_buffer[idx+4];
    all_units[total_units].auxtnum=scn_buffer[idx+5]+256*scn_buffer[idx+6];
    all_units[total_units].x=scn_buffer[idx+7]+256*scn_buffer[idx+8];
    all_units[total_units].y=scn_buffer[idx+9]+256*scn_buffer[idx+10];
    all_units[total_units].str=scn_buffer[idx+11];
    all_units[total_units].entrench=scn_buffer[idx+12];
    all_units[total_units].exp=scn_buffer[idx+13];
    all_units[total_units].uflag=axis_aux;
    all_units[total_units].fuel=-1;
    all_units[total_units].ammo=-1;
    idx+=14;
  }

  //read the allied
  if (show_info) printf("Reading the allied units...\n");
  for (i=0; i<total_allied_core; ++i,++total_units)
  {
    all_units[total_units].unum=scn_buffer[idx+0]+256*scn_buffer[idx+1];
    all_units[total_units].orgtnum=scn_buffer[idx+2]+256*scn_buffer[idx+3];
    all_units[total_units].country=scn_buffer[idx+4];
    all_units[total_units].auxtnum=scn_buffer[idx+5]+256*scn_buffer[idx+6];
    all_units[total_units].x=scn_buffer[idx+7]+256*scn_buffer[idx+8];
    all_units[total_units].y=scn_buffer[idx+9]+256*scn_buffer[idx+10];
    all_units[total_units].str=scn_buffer[idx+11];
    all_units[total_units].entrench=scn_buffer[idx+12];
    all_units[total_units].exp=scn_buffer[idx+13];
    all_units[total_units].uflag=allied_core;
    all_units[total_units].fuel=-1;
    all_units[total_units].ammo=-1;

    idx+=14;
  }

  //read the allied
  if (show_info) printf("Reading the allied aux units...\n");
  for (i=0; i<total_allied_aux; ++i,++total_units)
  {
    all_units[total_units].unum=scn_buffer[idx+0]+256*scn_buffer[idx+1];
    all_units[total_units].orgtnum=scn_buffer[idx+2]+256*scn_buffer[idx+3];
    all_units[total_units].country=scn_buffer[idx+4];
    all_units[total_units].auxtnum=scn_buffer[idx+5]+256*scn_buffer[idx+6];
    all_units[total_units].x=scn_buffer[idx+7]+256*scn_buffer[idx+8];
    all_units[total_units].y=scn_buffer[idx+9]+256*scn_buffer[idx+10];
    all_units[total_units].str=scn_buffer[idx+11];
    all_units[total_units].entrench=scn_buffer[idx+12];
    all_units[total_units].exp=scn_buffer[idx+13];
    all_units[total_units].uflag=allied_aux;
    all_units[total_units].fuel=-1;
    all_units[total_units].ammo=-1;

    idx+=14;
  }

  if (show_info) printf("#%d total units read.\n", total_units);
  //place the units in the map record
  for (i=0; i<total_units; ++i)
  {
     unum=all_units[i].unum;
     x=all_units[i].x;
     y=all_units[i].y;
     if (x>=mapx || y>=mapy || x<0 || y<0) //this is an error!
         return ERROR_SCN_BASE+ERROR_FPGE_POSITION_OUT_OF_MAP;
     if (equip[unum][GAF]) //1 if air unit
       map[x][y].auidx=i;
     else
       map[x][y].guidx=i;
  }

  // total_core=count_core();
  // total_aux=count_aux();
  // total_allied=count_allied();

  //get the victory hex coordinate and place on map
  if (show_info) printf("Reading the victory hexes..\n");
  idx=VIC_HEXES;
  for (i=0; i<MAX_VICTORY_HEXES; ++i)
  {
    victory_hexes[i].x=scn_buffer[idx+0]+256*scn_buffer[idx+1];
    victory_hexes[i].y=scn_buffer[idx+2]+256*scn_buffer[idx+3];
    if (victory_hexes[i].x>=mapx || victory_hexes[i].y>=mapy ||
    		victory_hexes[i].x<0 || victory_hexes[i].y<0)
    {
       victory_hexes[i].x=-1;
       victory_hexes[i].y=-1;
       victory_hexes[i].own=0;
    }
    else{
    	victory_hexes[i].own=map[victory_hexes[i].x][victory_hexes[i].y].own;
    	if (victory_hexes[i].own==0){
    	       victory_hexes[i].x=-1;
    	       victory_hexes[i].y=-1;
    	}
    }
    idx+=4;
  }

  for (i=0; i<MAX_VICTORY_HEXES; ++i)
  if (victory_hexes[i].x>=0)
  {
     map[victory_hexes[i].x][victory_hexes[i].y].vic=1;
  }
  if (show_info) printf("Reading the deployment hexes...\n");
  idx=DEPLOY_TABLE+2;
  for (i=0; i<total_deploy; ++i)
  {
     x=scn_buffer[idx];
     y=scn_buffer[idx+2];
     map[x][y].deploy=1;
     idx+=4;
  }
  //put the prestige and transport info in s4
  idx=scn_buffer[0x75]*4+0x77;
  for (i=0; i<16; ++i)
    s4_buffer[i]=scn_buffer[idx+i];

  return 0;
}

//read the scenario into the buffer and load
//data into various storage variables
int load_scn(int scenario_number, int show_info, int probe_file_only)
{
  FILE *inf;
  char path[MAX_PATH];
  char agheader[16];
  int ch;

  scn_size=0;
  sprintf(path,"game%03d.scn",scenario_number);
  if (show_info)
	  printf("Opening file %s\n",path);

  canonicalize_filename(path,path,MAX_PATH);
  inf=fopen(path,"rb");
  if (!inf)
  {
    //printf("Couldn't open scenario file\n");
    return ERROR_SCN_BASE+ERROR_FPGE_FILE_NOT_FOUND;
  }
  if (probe_file_only){
	fread(agheader,12,1,inf);
	agheader[12]=0;
	fclose(inf);
	if (strcmp(agheader,"ScenarioFile")==0){
		return ERROR_SCN_BASE+ERROR_FPGE_AG_FILE_FOUND;
	}
	return ERROR_SCN_BASE+ERROR_FPGE_FILE_FOUND;
  }
  while ( (ch=fgetc(inf))!=EOF && scn_size<MAX_SCN_SIZE)
  {
     scn_buffer[scn_size]=ch;
     ++scn_size;
  }
  fclose(inf);

  if (scn_size>=MAX_SCN_SIZE)
	  return ERROR_SCN_BASE+ERROR_FPGE_MAXIMUM_REACHED;

  parse_scn_buffer(show_info);

  if (show_info) printf("All loaded.\n");
  return 0;
}

int parse_set_file(FILE *inf, int offset){

	int x,y,c=0;

	int found = 0, i, neutIndex = 0;

	//get the map size
	fseek(inf, offset+MAP_X_ADDR, SEEK_SET);
	fread(&mapx, 2, 1, inf);
	fseek(inf, offset+MAP_Y_ADDR, SEEK_SET);
	fread(&mapy, 2, 1, inf);
	++mapx;
	++mapy; //PG sets this to one less than size i.e. last index
	if (mapx > MAX_MAP_X || mapy > MAX_MAP_Y)
		return ERROR_SET_BASE+ERROR_FPGE_MAXIMUM_REACHED;
		//exit(121);
	//get STM
	fseek(inf, offset+MAP_STM, SEEK_SET);
	fread(stm_fname, 14, 1, inf);
	stm_fname[14]=0;
	//stm_number=atoi(stm);
	//printf("nn=%d\n",nn);
	//get the tiles numbers
	fseek(inf, offset+MAP_LAYERS_START + 5 * mapx * mapy, SEEK_SET);
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x){
			c+=fread(&(map[x][y].tile), 2, 1, inf);
			if (map[x][y].tile>total_tiles || map[x][y].tile<0) printf("WARNING: Wrong tile type at %d,%d !\n",x,y);
		}
	if (c!=mapx*mapy) printf("WARNING: SET file too short !\n");
	//get the country(flag) info
	fseek(inf, offset+MAP_LAYERS_START + 3 * mapx * mapy, SEEK_SET);
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x){
			fread(&(map[x][y].own), 1, 1, inf);
			if (map[x][y].own>total_countries) printf("WARNING: Wrong flag type at %d,%d !\n",x,y);
		}
	//get the gln numbers
	fseek(inf, offset+MAP_LAYERS_START + 0 * mapx * mapy, SEEK_SET);
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x){
			fread(&(map[x][y].gln), 2, 1, inf);
			if (map[x][y].gln>total_names || map[x][y].gln<0) printf("WARNING: Wrong name at %d,%d ! Bigger then max loaded name.\n",x,y);
		}
	//get the side info

	//clear neutral array
	for (i = 0; i < 6; ++i)
		Neutral[i] = 0;

	fseek(inf, offset+MAP_LAYERS_START + 4 * mapx * mapy, SEEK_SET);
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x) {
			fread(&(map[x][y].side), 1, 1, inf);
			if (map[x][y].side==2 ) printf("WARNING: Wrong side==2 at %d,%d !\n",x,y);
			if (map[x][y].side>3) printf("WARNING: Wrong side>3 at %d,%d !\n",x,y);
			//printf("map[x][y].side=%d, map[x][y].own==%d\n",map[x][y].side,map[x][y].own);
			if ((map[x][y].side == 3) && (map[x][y].own > 0)) {
				//printf("map[%d][%d].own=%d\n",x,y,map[x][y].own);
				if (neutIndex < 6) {
					found = 0;
					for (i = 0; i < 6; ++i)
						if (Neutral[i] == map[x][y].own)
							found = 1;

					if (!found) {
						Neutral[neutIndex] = map[x][y].own;
						//printf("%d\n",Neutral[neutIndex]);
						neutIndex++;
					}
				}
			}
		}

	//get the road connectivity
	fseek(inf,offset+MAP_LAYERS_START + 2 * mapx * mapy, SEEK_SET);
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x){
			fread(&(map[x][y].rc), 1, 1, inf);
			if (map[x][y].rc&(~0xBB)) printf("WARNING: Wrong road at %d,%d !\n",x,y);

		}
	//some clean up
	//make sure all unit indexes are set to -1 (no unit)
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x) {
			map[x][y].guidx = -1;
			map[x][y].auidx = -1;
		}

	return 0;
}


int parse_stm_file(FILE *inf, int offset){
	int x,y,c=0;

	fseek(inf,offset, SEEK_SET);

	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x){
			c+=fread(&(map[x][y].utr), 2, 1, inf);
			if (map[x][y].utr>39 || map[x][y].utr<0)
				printf("WARNING: Wrong terrain type at %d,%d !\n",x,y);
		}

	if (c!=mapy*mapx) printf("WARNING: STM file too short !\n");
	return 0;
}

// read SET and STM files
int load_map(int set_file_number) {
	char path[MAX_PATH];
	//char map_set[32];
	FILE *inf;
	int error=0;

	sprintf(path, "map%02d.set", set_file_number);
	canonicalize_filename(path,path,MAX_PATH);
	inf = fopen(path, "rb");
	if (!inf)
		return ERROR_SET_BASE+ERROR_FPGE_FILE_NOT_FOUND;

	error=parse_set_file(inf,0);
	fclose(inf);

	if (error)
		return error;

	// read the utrs from the stm file
	//sprintf(fname, "Map%02d.stm", stm_number);
	strncpy(path, stm_fname,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf = fopen(path, "rb");
	if (!inf) {
		//printf("Couldn't open stm file\n");
		return ERROR_STM_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}

	parse_stm_file(inf,0);
	fclose(inf);

	return 0;
}

int load_scenario(int scenario_number, int show_info){
	int error=ERROR_PGF_SCN_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	int error1=ERROR_PGF_EQUIP_BASE+ERROR_FPGE_FILE_NOT_FOUND;

	if (show_info) printf("Loading scenario #%02d...\n",scenario_number);

	if (pgf_mode){
		//first try PGF scenario file
		error=load_pgf_pgscn(scenario_number,DO_NOT_SHOW_LOGS,PROBE_FILE);
		error1=load_pgf_equipment(PROBE_FILE, pgf_equip_file);
	}
	//printf("%d %d\n",error,error1);
	if (error==ERROR_PGF_SCN_BASE+ERROR_FPGE_FILE_NOT_FOUND || error1==ERROR_PGF_EQUIP_BASE+ERROR_FPGE_FILE_NOT_FOUND)
	{
		if (show_info) printf("PGF scenario not found..\n");

		if (pgf_mode){//exit if PGF scenario not found and in PGF mode
			sprintf(load_error_str, "Error loading PGF scenario #%02d.",scenario_number);
			return error;
		}

		//load PGDOS
		setScenarioNumber(scenario_number);
		map_number=scenario_number;

		ag_mode=0;
		//check AG scenario
		error = load_scn(getScenarioNumber(), DO_NOT_SHOW_LOGS, PROBE_FILE);
		if (error == ERROR_SCN_BASE+ERROR_FPGE_AG_FILE_FOUND) {
			//AG load
			if (show_info) printf("AG scenario found..\n");

			//read equipment
			  error=load_equip(LOAD_FILE,equip_file);
			  if (error) {
				  sprintf(load_error_str, "Error loading equipment file : %s.",equip_file);
				  return error;
			  }
			//clear the map
			memset(&map, 0, sizeof(map));

			error = load_ag_scn(scenario_number, show_info);

			if (error) {
				//if (error == ERROR_AG_SCN_BASE + ERROR_FPGE_FILE_NOT_FOUND)
				sprintf(load_error_str, "Error loading AG game%02d.scn file.",
						map_number);
				return error;
			}
			ag_mode=1;

		} else {
			//read equipment
			  error=load_equip(LOAD_FILE,equip_file);
			  if (error) {
				  sprintf(load_error_str, "Error loading equipment file : %s.",equip_file);
				  return error;
			  }

			//start of PGDOS load
			if (show_info)
				printf("Loading map #%02d...\n", map_number);
			//clear the map
			memset(&map, 0, sizeof(map));
			error = load_map(map_number);

			if (error) {
				if (error == ERROR_SET_BASE + ERROR_FPGE_FILE_NOT_FOUND)
					sprintf(load_error_str, "Could not open map%02d.set file.",
							map_number);
				if (error == ERROR_STM_BASE + ERROR_FPGE_FILE_NOT_FOUND)
					sprintf(load_error_str, "Could not open map%02d.stm file.",
							map_number);
				return error;
			} else {

				error = load_scn(getScenarioNumber(), show_info, LOAD_FILE);
				if (error) {
					sprintf(load_error_str,
							"Could not open PGDOS scenario game%03d.scn file.",
							getScenarioNumber());
					return error;
				}
				pgf_mode = 0;
			}
		} //end of PGDOS load
	} else {
		//read equipment
		  error=load_pgf_equipment(LOAD_FILE, pgf_equip_file);
		  if (error) {
			  sprintf(load_error_str, "Error loading equipment file : %s.",pgf_equip_file);
			  return error;
		  }else{
			  if (show_info)
			  	printf("PGF equipment file loaded.\n");
		  }

		//load PGF
		error = load_pgf_pgscn(scenario_number, DO_NOT_SHOW_LOGS,SCAN_FOR_MAP_NUMBER);

		setScenarioNumber(scenario_number);

		//map_number should have proper value
		if (error == ERROR_PGF_SCN_BASE + ERROR_FPGE_MAP_NUMBER_NOT_FOUND) {
			//error
			sprintf(load_error_str,
					"Could not find map number in PGF %03d.pgscn file.",
					scenario_number);
			return error;
		} else {
			//all ok

			//clear the map
			memset(&map, 0, sizeof(map));
			map_number = pgf_map_number;

			if (show_info)
				printf("Loading PGF map #%02d...\n", map_number);
			error = load_map(map_number);

			if (error) {
				if (error == ERROR_SET_BASE + ERROR_FPGE_FILE_NOT_FOUND)
					sprintf(load_error_str, "Could not open PGF map%02d.set file.",
							map_number);
				if (error == ERROR_STM_BASE + ERROR_FPGE_FILE_NOT_FOUND)
					sprintf(load_error_str, "Could not open PGF map%02d.stm file.",
							map_number);
				return error;
			} else {

				error = load_pgf_pgscn(getScenarioNumber(), show_info, LOAD_FILE);
				if (error) {
					sprintf(load_error_str,
							"Could not open PGF scenario %03d.pgscn file.",
							getScenarioNumber());
					return error;
				}
				//if (show_info) printf("PGF scenario #%02d loaded.\n",scn_number);
			}
			pgf_mode = 1;
		}
	}
	//compute scan ranges
	compute_shade(show_ranges);
	return 0;
}


int load_description(int show_info) {
	FILE *inf;
	char path[MAX_PATH];

	strncpy(path,description_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf = fopen(path, "rb");
	if (!inf) {
		sprintf(path,"..\\data\\%s",description_file);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf){
			if (show_info) printf("Could not open scenario description file %s.\n", description_file);
			return ERROR_DESCRIPTION_BASE+ERROR_FPGE_FILE_NOT_FOUND;
		}
	}
	fread(scn_active, MAX_SCENARIOS, 1, inf);
	fread(scn_short_description, SHORT_SCN_SIZE, MAX_SCENARIOS, inf);
	fread(scn_long_description, LONG_SCN_SIZE, MAX_SCENARIOS, inf);

	fclose(inf);
	return 0;
}

int load_names(int show_info) {
	FILE *inf;
	int i;
	char path[MAX_PATH];

	strncpy(path,names_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf = fopen(path, "rb");
	if (!inf) {
		sprintf(path,"..\\data\\%s",names_file);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf)
		{
			if (show_info) printf("Could not open names file %s.\n", names_file);
			return ERROR_NAMES_BASE+ERROR_FPGE_FILE_NOT_FOUND;
		}
	}
	fread(&total_names, 2, 1, inf);
	total_names &= 0xffff; //hack
	if (total_names > MAX_NAMES) {
		if (show_info) printf("WARNING. Loading only %d out of %d names from file %s.\n",
				MAX_NAMES, total_names, names_file);
		total_names = MAX_NAMES;
	}
	for (i = 0; i < total_names; ++i) {
		fread(&(gln[i][0]), MAX_NAME_SIZE, 1, inf);
		convert_from_cp1250_to_utf8(gln_utf8[i], gln[i], MAX_NAME_SIZE);
	}
	fclose(inf);
	mapnames_changed = 0;
	return 0;
}


int check_set(char *path){

	FILE *inf;
	uint16_t x,y;

	inf=fopen(path,"rb");
	if (inf==NULL)
		return 1;
	//check size
	fseek(inf,0,SEEK_END);
	long file_size = ftell(inf);
	if (file_size != mapx * mapy * 7 + 0x7b )
	{
		fclose(inf);
		return 2;
	}

	fseek(inf,MAP_X_ADDR,SEEK_SET);
	fread(&x,2,1,inf);
	fread(&y,2,1,inf);
	//printf("x %d\n",x);
	//printf("y %d\n",y);
	if ( (x+1)!=mapx || (y+1)!=mapy)
	{
		fclose(inf);
		return 3;
	}

	fclose(inf);
	return 0;
}

int check_stm(char *path){

	FILE *inf;

	inf=fopen(path,"rb");
	if (inf==NULL)
		return 1;
	//check size
	fseek(inf,0,SEEK_END);
	long file_size = ftell(inf);
	if (file_size != mapx * mapy * 2 )
	{
		fclose(inf);
		return 2;
	}

	fclose(inf);
	return 0;
}


int load_tiles_layer(char *name){
	int res,x,y;
	FILE *inf;
	char path[MAX_PATH];

	strncpy(path,name,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	if ( (res=check_set(path)) ){
		printf("ERROR loading file %s, error=%d\n",path,res);
		return res;
	}
	else{
		inf=fopen(path,"rb");
		if (inf==NULL)
			return 1;

		fseek(inf, MAP_LAYERS_START + 5 * mapx * mapy, SEEK_SET);
		for (y = 0; y < mapy; ++y)
			for (x = 0; x < mapx; ++x){
				fread(&(map[x][y].tile), 2, 1, inf);
				if (map[x][y].tile>total_tiles || map[x][y].tile<0)
					printf("WARNING: Wrong tile type at %d,%d !\n",x,y);
			}
		fclose(inf);
	}
	return 0;
}

int load_roads_layer(char *name){
	int res,x,y;
	FILE *inf;
	char path[MAX_PATH];

	strncpy(path,name,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	if ( (res=check_set(path)) ){
		printf("ERROR loading file %s, error=%d\n",path,res);
		return res;
	}
	else{
		inf=fopen(path,"rb");
		if (inf==NULL) return 1;

		fseek(inf,MAP_LAYERS_START + 2 * mapx * mapy, SEEK_SET);
		for (y = 0; y < mapy; ++y)
			for (x = 0; x < mapx; ++x){
				fread(&(map[x][y].rc), 1, 1, inf);
				if (map[x][y].rc&(~0xBB)) printf("WARNING: Wrong road at %d,%d !\n",x,y);
			}
		fclose(inf);
	}
	return 0;
}

int load_names_layer(char *name){
	int res,x,y;
	FILE *inf;
	char path[MAX_PATH];

	strncpy(path,name,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	if ( (res=check_set(path)) ){
		printf("ERROR loading file %s, error=%d\n",path,res);
		return res;
	}
	else{
		inf=fopen(path,"rb");
		if (inf==NULL) return 1;

		fseek(inf, MAP_LAYERS_START + 0 * mapx * mapy, SEEK_SET);
		for (y = 0; y < mapy; ++y)
			for (x = 0; x < mapx; ++x){
				fread(&(map[x][y].gln), 2, 1, inf);
				if (map[x][y].gln>total_names || map[x][y].gln<0)
					printf("WARNING: Wrong name at %d,%d !\n",x,y);
			}

		fclose(inf);
	}
	return 0;
}

int load_terrain_type_layer(char *name){
	int res,x,y;
	FILE *inf;
	char path[MAX_PATH];

	strncpy(path,name,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	if ( (res=check_stm(path)) ){
		printf("ERROR loading file %s, error=%d\n",path,res);
		return res;
	}
	else{
		inf=fopen(path,"rb");
		if (inf==NULL) return 1;

		for (y = 0; y < mapy; ++y)
			for (x = 0; x < mapx; ++x){
				fread(&(map[x][y].utr), 2, 1, inf);
				if (map[x][y].utr>=MAX_TERRAIN_TYPE || map[x][y].utr<0)
					printf("WARNING: Wrong terrain type at %d,%d !\n",x,y);
			}

		fclose(inf);
	}
	return 0;
}


//save equip dialog
int save_equip_dialog() {
	char path[MAX_PATH];
	char error_str[256];

	//TODO
	sprintf(path, ".\\%s", equip_file);
	canonicalize_filename(path, path, MAX_PATH);
	if (file_select_ex("Save PG 'panzequp.eqp' file", path, "eqp", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {

		 sprintf(MapStatusTxt,"Saving PG 'panzequp.eqp'\nPlease wait...");
         d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

		int error = save_equip(path);

		if (error) {
			strncpy(error_str, "Cannot save 'panzequp.eqp' file !",256);
			alert(error_str, NULL, NULL, "&Continue", NULL, 'c', 0);
		}
		else{
	           sprintf(MapStatusTxt,"'panzequp.eqp' saved\nPick an Operation.");
	            d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

		}
	}
	return D_REDRAW;
}

int convert_brf(){

	int converted=0;
	int i,j,k,kk,end_of_string;
	char in_file[MAX_PATH], out_file[MAX_PATH], html_buf[200];
	unsigned char buf[200];
	FILE *inf,*outf;

	for(i=1;i<200;i++){
		snprintf(in_file,MAX_PATH,"..\\scnbrief\\bdb%04d.brf",i);
		canonicalize_filename(in_file,in_file,MAX_PATH);
		inf=fopen(in_file,"rb");
		if (inf!=NULL){
			fread(&j,4,1,inf);
			if (j>0){
				snprintf(out_file,MAX_PATH,".\\%04d.pgbrf",i);
				canonicalize_filename(out_file,out_file,MAX_PATH);
				outf=fopen(out_file,"wb");
					if (outf!=NULL && j>0){
						fputs("<p>",outf);
						for(k=0;k<j;k++){
							fread(buf,200,1,inf);
							end_of_string=0;
							for(kk=0;kk<200;kk++){
								if (buf[kk]>31 && end_of_string==0) {
									  if (buf[kk]>127 || is_html_quotable(buf[kk])){
										  snprintf(html_buf,200,"&#%d;",buf[kk]);
										  fputs(html_buf,outf);
									  }
									  else{
										  fputc(buf[kk],outf);
										  //fprintf(outf,"%c",buf[kk]);
									  }
									//fputc(buf[kk],outf);
								}
								if (buf[kk]==10 && end_of_string==0) {
									fputs("</p>\n<p>",outf);
									//print_dec(i);
								}
								if (buf[kk]==0 && end_of_string==0) {
									fputc(' ',outf);
									end_of_string=1;
								}
							}
						}
						fputs("</p>\n",outf);
						fclose(outf);
						converted++;
					}
			}
			fclose(inf);
		}
	}
	return converted;
}
