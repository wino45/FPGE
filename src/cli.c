/*
 * cli.c
 *
 *  Created on: 2010-03-09
 *      Author: wino
 */
#include <stdio.h>
#include "fpge.h"
#include "cli.h"
#include "load.h"
#include "pgf.h"
#include "config.h"
#include "callback.h"
#include "tables.h"
#include "minimap.h"
#include "filename.h"
#include "fpge_colors.h"
#include "maingui.h"
#include "randomize.h"
#include "drawmap.h"
#include "loadpng.h"
#include "pg.h"
#include "unitlist.h"
#include "lgeneral.h"

int hide_names = 0;

void CLIHelp(char *VersionName)
{
    printf("%s\n\nUsage : fpge [options] ## \nwhere ## is scenario number.\n", VersionName);
    printf("When ## equals 'ALL' each scenario from 1-40 will be processed.\n");
    printf("By adding 'X' range is expanded : ALL[XXXXX] = X-99, XX-199, XXX-299, XXXX-399, XXXXX=499\n\n");
    printf("Only one of following command line switches will be recognized at each program run, but:\n");
    //printf("-G use 24bit graphics, this option must be last, other option can be also given (default).\n");
    //printf("-P use 8bit graphics, this option must be last, other option can be also given. PGDOS only !\n");
    printf("* Save BMP options:\n");
    printf("-B[nyrliz4{gatuT}{0123}{56}{dhf}{sexkc}{mwo}{pvq@}] save BMP/PNG and exit.\n");
    printf("  Config is loaded before saving bmp.\n");
    printf("  Use any modifier to skip loading configuration. Default is 'g0so'. Modifiers:\n");
    printf("  Use any of : n-all names     y-draw terrain  r-draw roads       z-override unit hiding\n");
    printf("  Use any of : i-hide map hex grid l-toggle unit org. transport (will work for 'g' or 't' only)\n");
    printf("  Use any of : ^-scenario units counters mode  $-draw all units   #-override flags hiding\n");
    printf("  Use any of : j-show units ID in the scenario. jj-same as j, but aux units start at ID 81.\n");
    printf("  Use one of : g-ground units  a-air units     t-transport units  u-no units\n");
    printf("               T-air units priority while displaying transport units.\n");
    printf("  Use one of : 0-no names      1-non trivial   2-non river        3-river names\n");
    printf("  Use one of : 5-axis spotting 6-allied spotting\n");
    printf("  Use one of : d-show decimal  h-show hex      f-show full names\n");
    printf("  Use one of : s-show strength e-show ent      x-show experience  c-no unit counters\n");
    printf("  Use one of : k-show flags\n");
    printf("  Use one of : o-normal tiles  m-mud tiles     w-winter tiles\n");
    printf("  Use one of : p-deployment    v-victory mode  q-neutral mode     @-all of them\n");
    printf("  Use one of : 7-city problems 8-road problems\n");
    printf("  Use one of : 4-save as PNG\n");
    printf("  Use one of : A-colorize all names E-colorize ocean names - use with 'n'\n");
    printf("  Use one of : b-crop bmp PGF  bb-more crop PG DOS\n");
    printf("  Usage of any of modifiers 'nyr123' will hide all units icons unless 'z' is specified.\n");
    printf("  Example: 'fpge -Br1f 01' will draw bmp file with roads, non trivial names, display full names.\n");
    printf("* Display information options:\n");
    printf("-N display all names used in scenario.\n");
    printf("-U[d] list units. Use 'd' to display IDs. 'b' bmp vs country only mode\n");
    printf("-C list all cities.\n");
    printf("-A[s] display tile name used in map, one line per tile.\n");
    printf("-T[s] display tile type used in map, one line per tile.\n");
    printf("  For -A and -T use 's' modifier to skip lines with not used tiles in current map: -As -Ts.\n");
    printf("-S[dc] show scenario information. Use 'd' to display IDs. 'c' additional CSV output\n");
    printf("-O show names and all tiles type for each name.\n");
    printf("-M[v{gan$}{mwo}{56}{al}s] saves strategic map bmp. Modifiers:\n");
    printf("  Use one of : g-ground units  a-air units     u-no units   $-all units\n");
    printf("  Use one of : x-axis units    l-allied units\n");
    printf("  Use one of : o-normal tiles  m-mud tiles     w-winter tiles\n");
    printf("  Use one of : 5-axis spotting 6-allied spotting\n");
    printf("  Use any of : v-display victory cites         s-swap indicators\n");
    printf("-D[v{gan$}{mwo}{56}{al}] saves medium size strategic map bmp. Modifiers almost as in -M\n");
    printf("* Check map options:\n");
    printf("-r check roads. Find potential problems with roads connections.\n");
    printf("-i check rivers tiles for naming.\n");
    printf("-c check for not named city and city without owner.\n");
    printf("-o[s] check if all ocean is named properly. s-show statistics on ocean names.\n");
    printf("-u check units.\n");
    printf("-v check flags - cites owners versus sides.\n");
    printf("-y[fb] check pg.pgcam file. f-fix briefings wrong chars - convert to HTML.\n");
    printf("   b -convert PG briefings to PGF format\n");
    printf("* Other\n");
    printf("-E[dhlsb] save PGF scenario when PGF equipment.pgeqp is present in same directory.\n");
    printf("      Use 'd' to display IDs in case of unit conversion problems.\n");
    printf("      Use 'h' to allow changing of flag of the unit.\n");
    printf("      Use 'l' to list only.\n");
    printf("      Use 's' swap sides: units, icon point, Allies Move First, flags\n");
    printf("      Use 'b' save backup map only\n");
    printf("      Use 'g' save in LGeneral format instead of PGF format\n");
    printf("-F[rtnhso] fix map, same as ctrl-q\n");
    printf("  Use any of : r-road          n-names         t-terrain type\n");
    printf("  Use any of : h-names, fix harder             s-sea and air transports\n");
    printf("  Use any of : o ID_FROM ID_TO - change city owner ID_FROM to ID_TO, ex: -Fo 8 13 XX\n");
    printf("-R[ocdefbris] randomize map tiles\n");
    printf("  Use any of : o-rough         c-clear         d-desert        e-rough desert\n");
    printf("  Use any of : f-forests       b-bocage        r-roads         i-rivers\n");
    printf("  Use any of : s-swamp \n");
    printf("-X[cuAI] [ID] [MONTH] [YEAR] save resource as bmp file:\n");
    printf("  Use one of : u-units         c-units of given country\n");
    printf("  Use any of : A-annotation of bmps\n");
    printf("  Use any of : F-flip icons when needed. Use icons.txt or/and unit efile name.\n");
    printf("  Use any of : I-show aux unit icons description (-Xu only)\n");
    printf("  Optionally : numerical ID of the unit to be saved or 'UALL' for all units.\n");
    printf("  Example: To save GE units as of Dec 39 use : FPGE -Xc 8 12 39 \n");
    printf("-L[nrst] FILE load map layer from file MAPXX.SET or MAPXX.STM file.\n");
    printf("  Use any of : n-name layer    r-road layer    s-tiles layer   t-terrain type\n");
    printf("-Y create GraphViz compatible campaign tree file : '%s'.\n",fpge_pgcam_gv);
    printf("-Q use APP6 symbols for units. PGF mode only. Change tiles color on request.\n");
    printf("  Use any of : c-make tiles with colors        n-do not color tiles.\n");
    printf("  Use any of : g-make tiles gray (default is green).\n");
    printf("  Use any of : u-do not make units icons.\n");
    printf("-d Use tile matrix debug.\n");
    printf("-P Dump river patterns. Use switches to dump other patterns.\n");
    printf("  Use any of : R-check radius of 2 instead of 1 \n");
    printf("  Use any of : r-road patterns  o-road and river a-all\n");
    printf("-x Upon load of map change tiles\n");
    printf("  Use any of : s-scope          m-mode\n");
    printf("  Use any of : r-road patterns  o-road and river a-all\n");
    printf("-h display this help.\n");
}

int make_mini_tiles() {
int error;
	if (pgf_mode) {
		create_small_tiles_out_of_big();
	} else {
		if (!ag_mode) {//we allready got small tiles in AG mode
			error = load_mini_tiles();
			if (error)
				return error;
		}
	}
	return 0;
}

int prepare_mini_tiles(){
	int error;

	printf("Loading small tiles...\n");
	error=make_mini_tiles();
	if (error) return error;

	printf("Loading small units...\n");
	error = load_mini_uicons();
	if (error){
		printf("Using default small units...\n");
		build_mini_uicons();
	}
	printf("Loading small victory hexes indicators...\n");
	error = load_mini_vichex();
	if (error){
		printf("Using default small victory hexes indicators...\n");
		build_mini_vichex();
	}
return 0;
}

void display_scenario(int display_csv) {
	int x,y,i,c,j,axvc=0,alvc=0,axaf=0,alaf=0;
	int deploy_number=0;
	int deploy_number_on_sea=0;
	int deploy_number_occupied=0;
	int axis_aux_used_sea_transports=0;
	int axis_aux_used_air_transports=0;
	int axis_core_used_sea_transports=0;
	int axis_core_used_air_transports=0;
	int allied_core_used_sea_transports=0;
	int allied_core_used_air_transports=0;
	int allied_aux_used_sea_transports=0;
	int allied_aux_used_air_transports=0;

	int static display_csv_header=1;

	printf("Scenario number : %d\n", getScenarioNumber());
	if (pgf_mode)
		printf("Scenario name : %s\n", block1_Name);
	else
		printf("Scenario name : %s\n", scn_short_description[getScenarioNumber()-1]);
	printf("Axis : ");
	for (i = 0; i < 12; i += 2)
		if ((scn_buffer[i] & 255) > 0){
			if (hide_names)
				printf("%d, ", scn_buffer[i] & 255);
			else
				printf("%d-%s, ", scn_buffer[i] & 255, country_names[scn_buffer[i]& 255]);
		}

	printf("\n");
	printf("Allied : ");
	for (i = 1; i < 12; i += 2)
		if ((scn_buffer[i] & 255) > 0){
			if (hide_names)
				printf("%d, ", scn_buffer[i] & 255);
			else
				printf("%d-%s, ", scn_buffer[i] & 255, country_names[scn_buffer[i]& 255]);
		}

	printf("\n");
	printf("Axis units orientation : %s\n",
			(scn_buffer[ORIENTATION] == 1) ? "->" : "<-");
	printf("Storm front : %s\n", (scn_buffer[STORM_FRONT] == 0) ? "No" : "Yes");
	printf("Axis offensive : %s\n", (scn_buffer[AXIS_STANCE] == 0) ? "No"
			: "Yes");
	printf("Allied offensive : %s\n", (scn_buffer[ALLIED_STANCE] == 0) ? "No"
			: "Yes");
	printf("Axis core units number : %d of %d\n", scn_buffer[CORE_UNITS], scn_buffer[CORE_UNITS_LIMIT] & 255);
	printf("Axis aux units number : %d of %d\n", scn_buffer[AUX_UNITS], scn_buffer[AUX_UNITS_LIMIT] & 255);
	printf("Allied core units number : %d of %d\n", scn_buffer[ALLIED_UNITS], scn_buffer[ALLIED_UNITS_LIMIT] & 255);
	printf("Allied aux units number : %d of %d\n", scn_buffer[AUX_ALLIED_UNITS] , scn_buffer[ALLIED_AUX_UNITS_LIMIT] & 255);

	printf("Scenario start date (dd/mm/yy) : %02d/%02d/%02d \n",
			scn_buffer[DAY] & 255, scn_buffer[MONTH] & 255, scn_buffer[YEAR]
					& 255);
	printf("Turns : %d\n", scn_buffer[TURNS] & 255);
	printf("Turns per day : %d\n", scn_buffer[TURNS_PER_DAY] & 255);
	printf("Days per turn : %d\n", scn_buffer[DAYS_PER_TURN] & 255);
	printf("Axis starting prestige : %d\n", s4_buffer[AXIS_PRESTIGE] + 256
			* s4_buffer[AXIS_PRESTIGE + 1]);
	printf("Axis prestige increase : %d\n", scn_buffer[AI_AXIS_BUCKET] + 256
			* scn_buffer[AI_AXIS_BUCKET + 1]);
	printf("Axis prestige turns : %d\n", scn_buffer[AI_AXIS_INTERVAL] & 255);
	printf("Allied starting prestige : %d\n", s4_buffer[ALLIED_PRESTIGE] + 256
			* s4_buffer[ALLIED_PRESTIGE + 1]);
	printf("Allied prestige increase : %d\n", scn_buffer[AI_ALLIED_BUCKET]
			+ 256 * scn_buffer[AI_ALLIED_BUCKET + 1]);
	printf("Allied prestige turns  : %d\n", scn_buffer[AI_ALLIED_INTERVAL]
			& 255);
	printf("Axis air transport number : %d\n", s4_buffer[AXIS_AIR_NUMBER]);
	i = s4_buffer[AXIS_AIR_TYPE] + 256 * s4_buffer[AXIS_AIR_TYPE + 1];
	if (i>0)
		printf("Axis air transport type : %d-%s\n", i, &(equip[i][NAME]));
	else
		printf("Axis air transport type : %d\n", i);
	printf("Axis sea transport number : %d\n", s4_buffer[AXIS_SEA_NUMBER]);
	i = s4_buffer[AXIS_SEA_TYPE] + 256 * s4_buffer[AXIS_SEA_TYPE + 1];
	if (i>0)
		printf("Axis sea transport type : %d-%s\n", i, &(equip[i][NAME]));
	else
		printf("Axis sea transport type : %d\n", i);
	printf("Allied air transport number : %d\n", s4_buffer[ALLIED_AIR_NUMBER]);
	i = s4_buffer[ALLIED_AIR_TYPE] + 256 * s4_buffer[ALLIED_AIR_TYPE + 1];
	if (i>0)
		printf("Allied air transport type : %d-%s\n", i, &(equip[i][NAME]));
	else
		printf("Allied air transport type : %d\n", i);
	printf("Allied sea transport number : %d\n", s4_buffer[ALLIED_SEA_NUMBER]);
	i = s4_buffer[ALLIED_SEA_TYPE] + 256 * s4_buffer[ALLIED_SEA_TYPE + 1];
	if (i>0)
		printf("Allied sea transport type : %d-%s\n", i, &(equip[i][NAME]));
	else
		printf("Allied sea transport type : %d\n", i);
	c=0;
	axvc=0;
	alvc=0;
	for (i = 0; i < MAX_VICTORY_HEXES; i++){
		if (victory_hexes[i].x > -1 && victory_hexes[i].y > -1 && victory_hexes[i].own > 0) {
			c++;
		for(j=0;j<6;j++){
			if (scn_buffer[j*2]==victory_hexes[i].own) axvc++;
			if (scn_buffer[j*2+1]==victory_hexes[i].own) alvc++;
		}
		}
	}

	axaf=0;
	alaf=0;
	//airfield must be inside map
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) 	{
		if (map[x][y].utr==19) { //airfield type
		for(j=0;j<6;j++){
			if (scn_buffer[j*2]==map[x][y].own) axaf++;
			if (scn_buffer[j*2+1]==map[x][y].own) alaf++;
		}
		}
	}
	printf("Axis airfields number : %d\n",axaf);
	printf("Allied airfields number : %d\n",alaf);
	deploy_number=count_deploy_hexes();
	deploy_number_on_sea=count_deploy_hexes_on_sea();
	deploy_number_occupied=count_deploy_hexes_occupied();
	printf("Deploy hexes number : %d\n",deploy_number);
	printf("Deploy hexes number on sea : %d\n",deploy_number_on_sea);
	printf("Deploy hexes number occupied : %d\n",deploy_number_occupied);
	printf("Victory hexes number : %d\n",c);
	printf("Axis victory hexes number : %d : ",axvc);

	for (i = 0; i < MAX_VICTORY_HEXES; i++){
			if (victory_hexes[i].x > -1 && victory_hexes[i].y > -1 && victory_hexes[i].own > 0) {
				c++;
			for(j=0;j<6;j++){
				if (scn_buffer[j*2]==victory_hexes[i].own) printf("(%d,%d)", victory_hexes[i].x, victory_hexes[i].y);
			}
			}
		}
	printf("\n");

	printf("Allied victory hexes number : %d : ",alvc);
	for (i = 0; i < MAX_VICTORY_HEXES; i++){
			if (victory_hexes[i].x > -1 && victory_hexes[i].y > -1 && victory_hexes[i].own > 0) {
				c++;
			for(j=0;j<6;j++){
				if (scn_buffer[j*2+1]==victory_hexes[i].own) printf("(%d,%d)", victory_hexes[i].x, victory_hexes[i].y);
			}
			}
		}
	printf("\n");

	printf("Victory hexes : ");
	for (i = 0; i < MAX_VICTORY_HEXES; i++)
		if (victory_hexes[i].x > -1 && victory_hexes[i].y > -1 && victory_hexes[i].own > 0){
			if (hide_names)
				printf("(%d,%d)%d-%d, ", victory_hexes[i].x, victory_hexes[i].y,
									map[victory_hexes[i].x][victory_hexes[i].y].gln,
									victory_hexes[i].own);
			else
				printf("(%d,%d)%s-%s, ", victory_hexes[i].x, victory_hexes[i].y,
					gln[map[victory_hexes[i].x][victory_hexes[i].y].gln],
					country_names[victory_hexes[i].own]);
		}
	printf("\n");
	printf("Victory hexes short: ");
	for (i = 0; i < MAX_VICTORY_HEXES; i++)
		if (victory_hexes[i].x > -1 && victory_hexes[i].y > -1 && victory_hexes[i].own > 0){
				printf("(%d,%d)", victory_hexes[i].x, victory_hexes[i].y);
		}
	printf("\n");

	axis_core_used_sea_transports=0;
	axis_core_used_air_transports=0;
	for(i=0;i<total_axis_core;i++){
		if (get_id_from_2_bytes(AXIS_AIR_TYPE)!=0)
			if (all_units[findOffset_ex(axis_core)+i].auxtnum==get_id_from_2_bytes(AXIS_AIR_TYPE))
				axis_core_used_air_transports++;
		if (get_id_from_2_bytes(AXIS_SEA_TYPE)!=0)
			if (all_units[findOffset_ex(axis_core)+i].auxtnum==get_id_from_2_bytes(AXIS_SEA_TYPE))
				axis_core_used_sea_transports++;
	}
	axis_aux_used_sea_transports=0;
	axis_aux_used_air_transports=0;
	for(i=0;i<total_axis_aux;i++){
		if (get_id_from_2_bytes(AXIS_AIR_TYPE)!=0)
			if (all_units[findOffset_ex(axis_aux)+i].auxtnum==get_id_from_2_bytes(AXIS_AIR_TYPE))
				axis_aux_used_air_transports++;
		if (get_id_from_2_bytes(AXIS_SEA_TYPE)!=0)
			if (all_units[findOffset_ex(axis_aux)+i].auxtnum==get_id_from_2_bytes(AXIS_SEA_TYPE))
				axis_aux_used_sea_transports++;
	}

	allied_core_used_sea_transports=0;
	allied_core_used_air_transports=0;
	for(i=0;i<total_allied_core;i++){
		if (get_id_from_2_bytes(ALLIED_AIR_TYPE)!=0)
			if (all_units[findOffset_ex(allied_core)+i].auxtnum==get_id_from_2_bytes(ALLIED_AIR_TYPE))
				allied_core_used_air_transports++;
		if (get_id_from_2_bytes(ALLIED_SEA_TYPE)!=0)
			if (all_units[findOffset_ex(allied_core)+i].auxtnum==get_id_from_2_bytes(ALLIED_SEA_TYPE))
				allied_core_used_sea_transports++;
	}
	allied_aux_used_sea_transports=0;
	allied_aux_used_air_transports=0;
	for(i=0;i<total_allied_aux;i++){
		if (get_id_from_2_bytes(ALLIED_AIR_TYPE)!=0)
			if (all_units[findOffset_ex(allied_aux)+i].auxtnum==get_id_from_2_bytes(ALLIED_AIR_TYPE))
				allied_aux_used_air_transports++;
		if (get_id_from_2_bytes(ALLIED_SEA_TYPE)!=0)
			if (all_units[findOffset_ex(allied_aux)+i].auxtnum==get_id_from_2_bytes(ALLIED_SEA_TYPE))
				allied_aux_used_sea_transports++;
	}
	printf("Axis used sea transports number: %d\n",axis_core_used_sea_transports+axis_aux_used_sea_transports);
	printf("Axis used sea transports number (core): %d\n",axis_core_used_sea_transports);
	printf("Axis used sea transports number (aux): %d\n",axis_aux_used_sea_transports);
	printf("Axis used air transports number: %d\n",axis_core_used_air_transports+axis_aux_used_air_transports);
	printf("Axis used air transports number (core): %d\n",axis_core_used_air_transports);
	printf("Axis used air transports number (aux): %d\n",axis_aux_used_air_transports);

	printf("Allied used sea transports number: %d\n",allied_core_used_sea_transports+allied_aux_used_sea_transports);
	printf("Allied used sea transports number (core): %d\n",allied_core_used_sea_transports);
	printf("Allied used sea transports number (aux): %d\n",allied_aux_used_sea_transports);
	printf("Allied used air transports number: %d\n",allied_core_used_air_transports+allied_aux_used_air_transports);
	printf("Allied used air transports number (core): %d\n",allied_core_used_air_transports);
	printf("Allied used air transports number (aux): %d\n",allied_aux_used_air_transports);

	if (display_csv){
		if (display_csv_header){
		printf("CSV;Scenario;Scenario name;Turns;"
				"Map X;Map Y;"
				"Axis core;Max axis core;"
				"Deploy hexes;Deploy hexes on sea;Deploy hexes occupied;"
				"Axis aux;Max axis aux;"
				"Allied core;Max allied core;"
				"Allied aux;Max allied aux;"
				"Axis victory points;Allied victory points;"
				"Axis airfields;Allied airfields;"
				"Axis sea transports;Axis air transports;"
				"Allied sea transports;Allied air transports;"
				"Max axis sea transports;Max axis air transports;"
				"Max allied sea transports;Max allied air transports;"
				"\n");
				display_csv_header=0;
		}
		printf("CSV;%d;%s;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
				getScenarioNumber(),(pgf_mode?block1_Name:scn_short_description[getScenarioNumber()-1]),scn_buffer[TURNS],
				mapx,mapy,
				scn_buffer[CORE_UNITS],
				scn_buffer[CORE_UNITS_LIMIT],
				deploy_number,deploy_number_on_sea,deploy_number_occupied,
				scn_buffer[AUX_UNITS],
				scn_buffer[AUX_UNITS_LIMIT],
				scn_buffer[ALLIED_UNITS],
				scn_buffer[ALLIED_UNITS_LIMIT],
				scn_buffer[AUX_ALLIED_UNITS],
				scn_buffer[ALLIED_AUX_UNITS_LIMIT],
				axvc,alvc,
				axaf,alaf,
				axis_core_used_sea_transports+axis_aux_used_sea_transports,
				axis_core_used_air_transports+axis_aux_used_air_transports,
				allied_core_used_sea_transports+allied_aux_used_sea_transports,
				allied_core_used_air_transports+allied_aux_used_air_transports,
				s4_buffer[AXIS_SEA_NUMBER],s4_buffer[AXIS_AIR_NUMBER],
				s4_buffer[ALLIED_SEA_NUMBER],s4_buffer[ALLIED_AIR_NUMBER]
				);
	}

}

void display_tile_name(int hide_empty) {
	int found = 0;
	int x,y,i;
	int number_of_names[MAX_NAMES];

	printf("Display tile name...\n");
	found = 0;
	for (i = 0; i < MAX_TILES_IN_PG; i++) {
		found = 0;
		for (y = 0; y < MAX_NAMES; y++)
			number_of_names[y] = 0;
		for (y = 1; y < mapy - 1; ++y)
			for (x = 1; x < mapx - 1; ++x) {
				if (map[x][y].tile == i) {
					if (map[x][y].gln < total_names)
						number_of_names[map[x][y].gln]++;
					else
						printf(
								"Name error at (%d,%d) - name out of range : %d of %d\n",
								x, y, map[x][y].gln, total_names);
					found = 1;
				}
			}
		if ((found) || (!hide_empty)) {
			printf("%d(%s): ", i, utr_names[TTData_Max_Tiles[i]]);
			//console , we use cp1250
			for (y = 0; y < MAX_NAMES; y++)
				if (number_of_names[y] > 0)
					printf("%d-%s(%d), ", y, gln[y], number_of_names[y]);
			printf("\n");
		}
	}
}

void display_terrain_type(int hide_empty) {
	int x,y,i,j;
	int found = 0;
	int tt_table[MAX_TERRAIN_TYPE];

	printf("Display terrain type...\n");
	found = 0;
	for (i = 0; i < MAX_TILES_IN_PG; i++) {
		found = 0;
		for (j = 0; j < MAX_TERRAIN_TYPE; j++)
			tt_table[j] = 0;

		for (y = 1; y < mapy - 1; ++y)
			for (x = 1; x < mapx - 1; ++x) {
				if (map[x][y].tile == i) {
					tt_table[map[x][y].utr]++;
					found = 1;
				}
			}

		if ((found) || (!hide_empty)) {
			printf("%d(%s): ", i, utr_names[TTData_Max_Tiles[i]]);
			for (y = 0; y < MAX_TERRAIN_TYPE; y++)
				if (tt_table[y] > 0)
					printf("%d-%s(%d), ", y, utr_names[y], tt_table[y]);

			printf("\n");
		}
	}
}

void checking_ocean(int no_log, int show_stats) {
	int x,y,i;
	int found = 0;
	int global_found = 0;
	int names_count[MAX_NAMES];

	memset(names_count,0,sizeof(names_count));

	if (!no_log) printf("Checking if ocean is named...\n");
	found = 0;
	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x) {
			if (is_tile_ocean(x, y))
				found++;
			if (is_tile_ocean(x, y) && is_tile_name_standard(map[x][y].gln)) {
				if (no_log) map[x][y].shade |=PROBLEM_MASK;
				global_found++;
			}
			//else
			//	if (no_log) map[x][y].shade &=~PROBLEM_MASK;
		}
	if (!no_log)
		printf(
				"Found %d ocean tiles\nFound %d named ocean tiles\nSearch for %d not named cases\n",
				found, found - global_found, global_found);


	if (!no_log) printf("Checking ocean for strange names...\n");
	found = 0;
	global_found=0;
	//get some statistics
	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x)
			if (is_tile_ocean(x, y)){
				names_count[map[x][y].gln]++;
				found++;
			}
	//print stats
	if (show_stats)
		for(i=0;i<MAX_NAMES;i++)
			if (names_count[i]>0)
				if (!no_log) printf("#%d - ID %d ( %s )\n",names_count[i],i,gln[i]);

	//search for names that are below 10% of all names of ocean
	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x){
			if (is_tile_ocean(x, y) && names_count[map[x][y].gln]<found/20 ) {
				if (global_found==0){
					if (!no_log) printf("Check ocean for strange names : \n");
				}

				if (!no_log) printf("wrong ocean name ID %d at (%d,%d) - %s\n",map[x][y].gln,x,y,gln[map[x][y].gln]);
				else map[x][y].shade |=PROBLEM_MASK;
				global_found++;
			}
			//else
			//	if (no_log) map[x][y].shade &=~PROBLEM_MASK;
		}
	if (global_found == 0)
		if (!no_log) printf("Ocean for strange names : all ok!\n");

}

void listing_city_names() {
	int x, y;
	int found = 0;
	//int i;

	printf("Listing cities names...\n");
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			found = 0;
			//for (i = tiles_filter[CITY_FILTER_INDEX][0]; i <= tiles_filter[CITY_FILTER_INDEX][1]; i++)
				if (is_tile_a_city_tile(map[x][y].tile))
					found = 1;
			if (found)
				printf("(%d,%d) %s\n", x, y, gln[map[x][y].gln]);
		}
}

void check_city_names(int no_log) {
	int x, y;
	int found = 0;
	int global_found = 0;

	if (!no_log) printf("Checking cities names...\nCheck no named cites tiles:\n");
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			found = 0;
			//for (i = tiles_filter[CITY_FILTER_INDEX][0]; i <= tiles_filter[CITY_FILTER_INDEX][1]; i++)
				if (is_tile_a_city_tile(map[x][y].tile) && is_tile_name_standard(map[x][y].gln) )
					found = 1;

			if (found) {
				if (!no_log) printf("(%d,%d)\n", x, y);
				else map[x][y].shade |=PROBLEM_MASK;
				global_found = 1;
			}
		}

	if (global_found == 0)
		if (!no_log) printf("All ok!\n");

	global_found=0;
	if (!no_log) printf("Check cites without owner:\n");
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			found = 0;
			//for (i = tiles_filter[CITY_FILTER_INDEX][0]; i <= tiles_filter[CITY_FILTER_INDEX][1]; i++)
				if (is_tile_a_city_tile(map[x][y].tile)	&& map[x][y].own == 0 )
					found = 1;

			if (found) {
				if (!no_log) printf("(%d,%d)\n", x, y);
				else map[x][y].shade |=PROBLEM_MASK;
				global_found = 1;
			}
		}
	if (global_found == 0)
		if (!no_log) printf("All ok!\n");
}

void listing_units(int mode, int hide_names) {
	int i;

	switch (mode) {
	case 0:
		printf("Listing units:\n");
		break;
	case 1:
		printf("Listing units (BMP,COUNTRY):\n");
		break;
	}
	for (i = 0; i < total_units; i++) {
		if (mode==0){
		if (hide_names)
			printf("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s\n", i + 1,
					all_units[i].unum, all_units[i].country, all_units[i].str,
					all_units[i].entrench, all_units[i].exp, all_units[i].x,
					all_units[i].y, all_units[i].orgtnum, all_units[i].auxtnum,
					utype_name[all_units[i].uflag]
			//,equip[all_units[i].unum][CLASS]
			);
		else
			printf(
					"%d, %s, %s, %d, %d, %d, %d, %d, %s, %s, %s\n",
					i + 1,
					//console output, no UTF8 here (?)
					&(equip[all_units[i].unum][NAME]),
					country_names[all_units[i].country],
					all_units[i].str,
					all_units[i].entrench,
					all_units[i].exp,
					all_units[i].x,
					all_units[i].y,
					(all_units[i].orgtnum > 0) ? (char *) &(equip[all_units[i].orgtnum][NAME])
							: "-",
					(all_units[i].auxtnum > 0) ? (char *) &(equip[all_units[i].auxtnum][NAME])
							: "-", utype_name[all_units[i].uflag]);
		}
		if (mode==1){
			printf("%d, %d\n",
					(int)equip[all_units[i].unum][BMP]+256*equip[all_units[i].unum][BMP+1],
					all_units[i].country
			);
			if (all_units[i].orgtnum > 0)
				printf("%d, %d\n",
						(int)equip[all_units[i].orgtnum][BMP]+256*equip[all_units[i].orgtnum][BMP+1],
						all_units[i].country
				);
			if (all_units[i].auxtnum > 0)
				printf("%d, %d\n",
						(int)equip[all_units[i].auxtnum][BMP]+256*equip[all_units[i].auxtnum][BMP+1],
						all_units[i].country
				);
		}
	}
}

void checking_river_names(int no_log) {
	int x,y;
	int global_found = 0;

	if (!no_log) printf("Checking rivers names...\nCheck no named river tiles:\n");
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			if (is_tile_river(x, y) && is_tile_name_standard(map[x][y].gln)) {

				if (!no_log) printf("(%d,%d)\n", x, y);
				else map[x][y].shade |= PROBLEM_MASK;
				global_found = 1;
			}
			//else
			//	if (no_log) map[x][y].shade &=~PROBLEM_MASK;
		}
	if (global_found == 0)
		if (!no_log) printf("All ok!\n");
}

void list_names(){
	int x,y;
	int number_of_names[MAX_NAMES];

	for (x = 0; x < MAX_NAMES; ++x)
		number_of_names[x] = 0;
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].gln < MAX_NAMES)
				number_of_names[map[x][y].gln]++;
	for (x = 0; x < MAX_NAMES; ++x)
		//console , we use cp1250
		if (number_of_names[x] > 0)
			printf("%d - %d %s\n", x, number_of_names[x], gln[x]);
}

void save_pgf_scenario(int hide_names, int try_harder, int swap_side,int side_save){
	int error;
	//if (!pgf_mode){
		error=load_pgf_equipment(LOAD_CONVERSION_TABLE_ONLY,pgf_equip_file);
		if (error) return;
		qsort(conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
		save_pgf_pgscn(getScenarioNumber(), SHOW_LOGS, try_harder?SAVE_WITH_UNIT_IDS_HARD_CONVERSION:SAVE_WITH_UNIT_IDS_CONVERSION, hide_names, swap_side,side_save);
	//}else{
	//	printf("ERROR. Cannot save PGF scenario while in PGF mode.\n");
	//}
}

void show_names_vs_tiles() {
	int x,y,i,j;
	int number_of_names[MAX_NAMES];

	for (x = 0; x < MAX_NAMES; ++x)
		number_of_names[x] = 0;
	for (y = 0; y < mapy; ++y)
		for (x = 0; x < mapx; ++x)
			if (map[x][y].gln < MAX_NAMES)
				number_of_names[map[x][y].gln]++;
	for (i = 0; i < MAX_NAMES; ++i)
		//console, we use cp1250
		if (number_of_names[i] > 0) {
			//clean
			for (j = 0; j < MAX_TILES; ++j)
				tiles_temp[j] = 0;
			//scan for tiles type
			for (y = 0; y < mapy; ++y)
				for (x = 0; x < mapx; ++x)
					if (map[x][y].gln == i)
						tiles_temp[map[x][y].tile]++;
			//
			printf("%d - %d %s : ", i, number_of_names[i], gln[i]);
			for (j = 0; j < MAX_TILES; ++j)
				if (tiles_temp[j] > 0)
					printf("%d-%s(%d), ", j, utr_names[
					                                   TTData_Max_Tiles[j]>-1?TTData_Max_Tiles[j]:UNKNOWN_UTR_NAME
					                                   ],
							tiles_temp[j]);
			printf("\n");
		}
}

int CountBits(int v){
	//Counting bits set, Brian Kernighan's way

	unsigned int c; // c accumulates the total bits set in v
	for (c = 0; v; c++)
	{
	  v &= v - 1; // clear the least significant bit set
	}
	return c;
}

int isRoadOrRiverTile(int tileToCheck){
	//int i;

	return check_terrain_mask(tileToCheck,(1<<ROAD_FILTER_INDEX)+(1<<RIVER_FILTER_INDEX));

	//for(i=0;i<10;i++) //10 tiles to check
	//	if (tiles_display[17+7*20+i]==tileToCheck) return 1;

	//return 0;
}


void checking_road_connections()
{
	int x,y;
	int global_found = 0;

	printf("Checking roads connection...\nCheck:\n");
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x) {
			rc[x][y] = map[x][y].rc;
			map[x][y].rc = find_road_mask(x, y);
		}

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			tempmap[x][y] = find_one_way_road(x, y);

	// copy temp to rc and filter it
	for (y=1; y<mapy-1; ++y)
		for (x=1; x<mapx-1; ++x)
			if (map[x][y].rc>0){
				map[x][y].rc=(unsigned char)tempmap[x][y];
				//filter it
				map[x][y].rc=filter_roads(x,y);
			}

	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			if (map[x][y].rc != rc[x][y]) {
				printf("(%d,%d)\n", x, y);
				global_found = 1;
			}

	if (global_found == 0)
		printf("All ok!\n");

	printf("Checking roads connection - bridges...\n");
	global_found=0;
	for (y = 1; y < mapy - 1; ++y)
		for (x = 1; x < mapx - 1; ++x)
			if ( isRoadOrRiverTile(map[x][y].tile) && CountBits(map[x][y].rc)<2 ){
				printf("B(%d,%d)\n", x, y);
				global_found=1;
			}

	if (global_found == 0)
		printf("All ok!\n");

}

void checking_units(int no_log){
	int i,j,ok,offset;
	unsigned char mov_type;
	short utr;
	//int global_found = 0;
	int found=0,unum;

	if (!no_log) printf("Checking units...\n");

	//clear
//	if (no_log)
//		for (y = 0; y < mapy ; ++y)
//			for (x = 0; x < mapx ; ++x)
//				map[x][y].shade &=~PROBLEM_MASK;

	// naval not at sea
	for (i=0; i<total_units;i++){
		mov_type = equip[all_units[i].unum][MOV_TYPE];
		utr = map[all_units[i].x][all_units[i].y].utr;
		if (
			 mov_type == MOV_TYPE_NAVAL && utr !=4 && utr !=5 &&
			 utr !=6 && utr !=7 && utr !=8
			 && utr !=29 && utr !=37
		){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check naval unit(s) on ground :\n");
			}
			if (!no_log) printf("naval@gnd ID %d at (%d,%d) - %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Naval units at ground : all ok!\n");

	// ground at sea
	found=0;
	for (i=0; i<total_units;i++){
		mov_type = equip[all_units[i].unum][MOV_TYPE];
		utr = map[all_units[i].x][all_units[i].y].utr;
		if ( mov_type != MOV_TYPE_NAVAL && mov_type != MOV_TYPE_AIR &&
			 ( utr ==4 || utr ==5 || utr ==6 || utr ==7 ) && all_units[i].auxtnum ==0
					 ){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check ground unit(s) at sea :\n");
			}
			if (!no_log) printf("gnd@sea ID %d at (%d,%d) - %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Ground units at sea : all ok!\n");

	// ground at escarpment
	found=0;
	for (i=0; i<total_units;i++){
		mov_type = equip[all_units[i].unum][MOV_TYPE];
		utr = map[all_units[i].x][all_units[i].y].utr;
		if ( mov_type != MOV_TYPE_NAVAL && mov_type != MOV_TYPE_AIR &&
			 ( utr ==34 || utr ==35 || utr ==36  )
					 ){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check ground unit(s) at escarpment or rough desert :\n");
			}
			if (!no_log) printf("@escarp ID %d at (%d,%d) - %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Ground units at escarpment or rough desert : all ok!\n");
	//towed ground unit at swamp
	found=0;
	for (i=0; i<total_units;i++){
		mov_type = equip[all_units[i].unum][MOV_TYPE];
		utr = map[all_units[i].x][all_units[i].y].utr;
		if ( mov_type == MOV_TYPE_TOWED && utr ==20 ){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check towed unit at swamp :\n");
			}
			if (!no_log) printf("@swamp ID %d at (%d,%d) - %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Towed unit at swamp : all ok!\n");

	// ground at ground but with sea transport
	found=0;
	for (i=0; i<total_units;i++){
		mov_type = equip[all_units[i].unum][MOV_TYPE];
		utr = map[all_units[i].x][all_units[i].y].utr;
		if ( mov_type != MOV_TYPE_NAVAL && mov_type != MOV_TYPE_AIR &&
			  utr !=4 && utr !=5 && utr !=6 && utr !=7  && all_units[i].auxtnum !=0
			 && equip[all_units[i].auxtnum][MOV_TYPE] == MOV_TYPE_NAVAL
					 ){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check ground unit(s) at ground with sea transport :\n");
			}
			if (!no_log) printf("sea trans@gnd ID %d at (%d,%d) - %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Ground units at ground with sea transport : all ok!\n");

	//
	//organic transport is allowed for this class
	found=0;
	for (i=0; i<total_units;i++){
		mov_type = equip[all_units[i].unum][MOV_TYPE];
		utr = map[all_units[i].x][all_units[i].y].utr;
		if ( all_units[i].orgtnum >0 && ((mov_type !=MOV_TYPE_LED && mov_type !=MOV_TYPE_TOWED && mov_type!=MOV_TYPE_MOUNTAIN)
				||(mov_type==MOV_TYPE_TOWED && equip[all_units[i].unum][CLASS]==CLASS_FORTIFICATION)) ){
			//printf("mov_type= %d\n",mov_type);
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check ground unit(s) not suitable for having organic transport :\n");
			}
			if (!no_log) printf("org not allowed, ID %d at (%d,%d) - %s with %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]),&(equip[all_units[i].orgtnum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Ground units with organic transport : all ok!\n");

	//6) for land transport is used unit other eqp class than land transport
	//organic transport is in organic transport class
	found=0;
	for (i=0; i<total_units;i++){
		mov_type = equip[all_units[i].unum][MOV_TYPE];
		utr = map[all_units[i].x][all_units[i].y].utr;
		if ( all_units[i].orgtnum >0 && equip[all_units[i].orgtnum][CLASS]!=CLASS_LAND_TRANSPORT){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check ground unit(s) having wrong organic transport :\n");
			}
			if (!no_log) printf("wrong org, ID %d at (%d,%d) - %s with %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]),&(equip[all_units[i].orgtnum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Ground units having wrong organic transport : all ok!\n");

	//unit got country set
	found=0;
	for (i=0; i<total_units;i++){
		if ( all_units[i].country <1 || all_units[i].country>total_countries){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Units having wrong country :\n");
			}
			if (!no_log) printf("wrong country, ID %d at (%d,%d) - %d\n",i+1,all_units[i].x,all_units[i].y,all_units[i].country);
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}else{
			//country within bounds
			if (equip_country[all_units[i].unum]!=-1){
				offset=0;
				if (all_units[i].uflag==allied_core || all_units[i].uflag==allied_aux ) offset=1;
				ok=0;
				for(j=0;j<6;j++){
					if(scn_buffer[j*2+offset]==all_units[i].country){
						ok=1;
						break;
					}
				}
				//check neutral
				if(!ok){
					for(j=0;j<6;j++){
						if(Neutral[j]==all_units[i].country){
							ok=1;
							break;
						}
				}}


				if (!ok){
					if (found==0){
						found=1;
						//global_found=1;
						if (!no_log) printf("Units having wrong country :\n");
					}
					if (!no_log)
						printf("wrong country of equipment, ID %d at (%d,%d) %s country %d, equip %d is %d\n",
								i+1,
								all_units[i].x,
								all_units[i].y,
								((all_units[i].uflag==allied_core || all_units[i].uflag==allied_aux)?"allied":"axis"),
								all_units[i].country,
								all_units[i].unum,
								equip_country[all_units[i].unum]);
					else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
				}
			}
		}
	}
	if (found == 0)
		if (!no_log) printf("Units having wrong country : all ok!\n");


	//7) land/air/transport unit used as primary unit
	found=0;
	for (i=0; i<total_units;i++){
		unum=all_units[i].unum;
		if ( equip[unum][CLASS]>CLASS_AIR_CARRIER){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check unit(s) having transport class as main unit:\n");
			}
			if (!no_log) printf("transport as unit, ID %d at (%d,%d) - %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Unit(s) having transport class as main unit: all ok!\n");

	//2) unit transported by air in case this unit has land transport
	found=0;
	for (i=0; i<total_units;i++){
		if ( all_units[i].auxtnum>0 && equip[all_units[i].auxtnum][CLASS]==CLASS_AIR_TRANSPORT && all_units[i].orgtnum>0){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check unit(s) being air transported but having organic transport :\n");
			}
			if (!no_log) printf("in air with org trans, ID %d at (%d,%d) - %s in %s with %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]),
					&(equip[all_units[i].auxtnum][NAME]),&(equip[all_units[i].orgtnum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Unit(s) being air transported but having organic transport : all ok!\n");

	//4) for air/naval transport is used unit other eqp class than air/naval transport
	found=0;
	for (i=0; i<total_units;i++){
		unum=all_units[i].auxtnum;
		if ( unum>0 && equip[unum][CLASS]<CLASS_AIR_TRANSPORT){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check unit(s) having air/sea transport unit not designed for air/sea transport :\n");
			}
			if (!no_log) printf("wrong sea/air, ID %d at (%d,%d) - %s in %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]),
					&(equip[all_units[i].auxtnum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Unit(s) having air/sea transport unit not designed for air/sea transport : all ok!\n");

	//1) unit transported by air transport in case this unit is not available in eqp for air transporting
	found=0;
	for (i=0; i<total_units;i++){
		unum=all_units[i].auxtnum;
		if ( unum>0 && equip[unum][CLASS]==CLASS_AIR_TRANSPORT && equip[all_units[i].unum][ALLOWED_TRANSPORT]<2){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check unit(s) having air transport unit not allowed to be air transported :\n");
			}
			if (!no_log) printf("air not allowed, ID %d at (%d,%d) - %s in %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]),
					&(equip[all_units[i].auxtnum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Unit(s) having air transport unit not allowed to be air transported : all ok!\n");

	//3) unit transported by naval transport in case this unit is not available in eqp for naval transporting
	found=0;
	for (i=0; i<total_units;i++){
		unum=all_units[i].auxtnum;
		if ( unum>0 && equip[unum][CLASS]==CLASS_SEA_TRANSPORT && equip[all_units[i].unum][ALLOWED_TRANSPORT]<1){
			if (found==0){
				found=1;
				//global_found=1;
				if (!no_log) printf("Check unit(s) having sea transport unit not allowed to be sea transported :\n");
			}
			if (!no_log) printf("sea not allowed, ID %d at (%d,%d) - %s in %s\n",i+1,all_units[i].x,all_units[i].y,&(equip[all_units[i].unum][NAME]),
					&(equip[all_units[i].auxtnum][NAME]));
			else map[all_units[i].x][all_units[i].y].shade |=PROBLEM_MASK;
		}
	}
	if (found == 0)
		if (!no_log) printf("Unit(s) having sea transport unit not allowed to be sea transported : all ok!\n");

}

void checking_flags(int no_log){
	int x,y,found=0;

	if (!no_log) printf("Checking flags...\n");
	//axis
	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x){
			//if (no_log) map[x][y].shade &=~PROBLEM_MASK;
			if (map[x][y].side==0 && map[x][y].own > 0)
				if (map[x][y].own!=scn_buffer[0] && map[x][y].own!=scn_buffer[2] &&
					map[x][y].own!=scn_buffer[4] && map[x][y].own!=scn_buffer[6] &&
					map[x][y].own!=scn_buffer[8] && map[x][y].own!=scn_buffer[10])
					{
						if (!no_log) printf("Check axis flag at (%d,%d)\n",x,y);
						else map[x][y].shade |=PROBLEM_MASK;
						found=1;
					}
		}
	//allied
	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x)
			if (map[x][y].side==1 && map[x][y].own > 0)
				if (map[x][y].own!=scn_buffer[1] && map[x][y].own!=scn_buffer[3] &&
					map[x][y].own!=scn_buffer[5] && map[x][y].own!=scn_buffer[7] &&
					map[x][y].own!=scn_buffer[9] && map[x][y].own!=scn_buffer[11])
					{
						if (!no_log)
							printf("Check allied flag at (%d,%d)\n",x,y);
						else
							map[x][y].shade |=PROBLEM_MASK;
						found=1;
					}
	//neutral
	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x)
			if (map[x][y].side==3 && map[x][y].own > 0)
				if (map[x][y].own==scn_buffer[1] || map[x][y].own==scn_buffer[3] ||
					map[x][y].own==scn_buffer[5] || map[x][y].own==scn_buffer[7] ||
					map[x][y].own==scn_buffer[9] || map[x][y].own==scn_buffer[11] ||
					map[x][y].own==scn_buffer[0] || map[x][y].own==scn_buffer[2] ||
					map[x][y].own==scn_buffer[4] || map[x][y].own==scn_buffer[6] ||
					map[x][y].own==scn_buffer[8] || map[x][y].own==scn_buffer[10])
					{
						if (!no_log) printf("Check neutral flag at (%d,%d)\n",x,y);
						else map[x][y].shade |=PROBLEM_MASK;
						found=1;
					}
	if (found == 0)
		if (!no_log) printf("Checking flags : all ok!\n");
}
/*
void checking_terrain_types(){
	int x,y,found=0;

	printf("Checking tiles vs terrain type...\n");
	//axis
	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx ; ++x)
			if (TTData_Max_Tiles[map[x][y].tile] > -1)
				map[x][y].utr = TTData_Max_Tiles[map[x][y].tile];
					 {printf("Check axis flag at (%d,%d)\n",x,y); found=1;}

	if (found == 0)
		printf("Checking flags : all ok!\n");
}
*/

int is_in_set(short tile,int limit, short table[]){
	int j, found =-1;

	for(j=0;j<limit;j++){
		if (tile == table[j]){
			found=table[j];
			break;
		}
	}
	return found;
}

int is_same_set(int tile, int tile2){

	if (is_tile_in_limited_set(tile,LIMITED_SET_RIVER)>-1 && is_tile_in_limited_set(tile2,LIMITED_SET_RIVER)>-1) return 1;
	if (is_tile_in_limited_set(tile,LIMITED_SET_ROAD)>-1 && is_tile_in_limited_set(tile2,LIMITED_SET_ROAD)>-1) return 1;
	if (is_tile_in_limited_set(tile,LIMITED_SET_ROAD_AND_RIVER)>-1 && is_tile_in_limited_set(tile2,LIMITED_SET_ROAD_AND_RIVER)>-1) return 1;

	return 0;
}

int is_tile_in_limited_set(short tile, int set){
	short river_tiles[]= {52,53,54,55,61,62,63,64,65,71,72,227,228,229,MAGIC_RIVER};
	short road_tiles[]=  {37,47,48,49,56,57,58,59,66,67,68, 69, 74,MAGIC_ROAD};
	short river_and_road_tiles[]= {30,32,40,41,60,70,223,224,225,226,MAGIC_ROAD_AND_RIVER};
	int river_tiles_no=15;
	int road_tiles_no=14;
	int river_and_road_tiles_no=11;
	int found=-1;

	if (tile<0) return tile;

	switch(set){
		case LIMITED_SET_RIVER:
			return is_in_set(tile,river_tiles_no,river_tiles);
		case LIMITED_SET_ROAD:
			return is_in_set(tile,road_tiles_no,road_tiles);
		case LIMITED_SET_ROAD_AND_RIVER:
			return is_in_set(tile,river_and_road_tiles_no,river_and_road_tiles);
		case LIMITED_SET_ALL:
			if ( (found=is_in_set(tile,river_tiles_no,river_tiles))>-1) return found;
			if ( (found=is_in_set(tile,road_tiles_no ,road_tiles))>-1) return found;
			return is_in_set(tile,river_and_road_tiles_no,river_and_road_tiles);
		default:
			return -1;
	}
}

int get_tile_offmap(int x, int y){

	if (x<0 || x>= mapx || y<0 || y>= mapy) return -2; //offmap tile
	return map[x][y].tile;
}

//gawk "{ print \"{\" $0 \"},\" }" pat_all_s.txt
//fpge -P ALL | grep "^[ 0-9][0-9]\{2\}" | sort | uniq -w 12 | wc -l
void display_patterns(int pattern_mode, int mode){
	int x,y,i,j,found,limit, border=0;
	int row[32];

	for (x = border; x < mapx-border ; ++x)
		for (y = border; y < mapy-border ; ++y)
					if (is_tile_in_limited_set(map[x][y].tile,pattern_mode)!=-1){
						row[1]=0;
						//printf("%3d->",map[x][y].tile);
						row[0]=map[x][y].tile;
						if (mode==1){
							limit=8;
							for(i=0;i<6;i++){
								found = is_tile_in_limited_set(map[x + dx_tab_N_CW[i]][y + dy_tab_N_CW[i][x % 2]].tile,pattern_mode);
								row[1] |= (found >-1);
								row[1] = row[1] << 1;
								row[2+i]=found;
							//printf("%3d%c",found,i<5?',':'\n');
							}
						}
						if (mode==2){
							limit=2;
							int r=2;
							for (i=0;i<=r;i++)
									for (j=-r+i/2;j<=r-(i+1)/2;j++){
										found = is_tile_in_limited_set(get_tile_offmap(x+i,y+j+x%2*(x+i+1)%2),pattern_mode);
										row[1] |= (found >-1);
										row[1] = row[1] << 1;
										row[limit]=found;
										limit++;
										found = is_tile_in_limited_set(get_tile_offmap(x-i,y+j+x%2*(x+i+1)%2),pattern_mode);
										row[1] |= (found >-1);
										row[1] = row[1] << 1;
										row[limit]=found;
										limit++;
									}
						}

						printf("%3d,0x%08x,",row[0],row[1]);
						for(i=2;i<limit;i++) printf("%3d%c",row[i],i<limit-1?',':'\n');

					}
}

void  run_and_marks_rivers(int mode, int scope, int pattern_mode){
	int x,y;

	int *tempmap2 = malloc(mapx * mapy * sizeof(int));
	//tempmap+x+mapx*y

	for (y = 0; y < mapy ; ++y)
		for (x = 0; x < mapx; ++x){
			tempmap2[x+mapx*y]=map[x][y].tile;

			if (scope==0 && is_tile_in_limited_set(map[x][y].tile, LIMITED_SET_RIVER)>=0)
				map[x][y].tile=MAGIC_RIVER;
			if (scope==1 && is_tile_river(x,y))
				map[x][y].tile=MAGIC_RIVER;
			if (scope==2 ){
				if (is_tile_in_limited_set(map[x][y].tile,LIMITED_SET_RIVER)>=0) map[x][y].tile=MAGIC_RIVER;
				if (is_tile_in_limited_set(map[x][y].tile,LIMITED_SET_ROAD)>=0) map[x][y].tile=MAGIC_ROAD;
				if (is_tile_in_limited_set(map[x][y].tile,LIMITED_SET_ROAD_AND_RIVER)>=0) map[x][y].tile=MAGIC_ROAD_AND_RIVER;
			}
		}

	if (mode==0){
		gen_terrain(MAGIC_RIVER, river_pattern_SE_CCW, river_size,river_pattern_tile, NO_FILTER_INDEX); //river
		gen_magic_river2();
	}

	if (mode==1)
		gen_magic_river3(pattern_mode);

	if (mode==2){
			gen_magic_river3(LIMITED_SET_RIVER);

			gen_terrain(MAGIC_RIVER, river_pattern_SE_CCW, river_size,river_pattern_tile, NO_FILTER_INDEX); //river
			gen_magic_river2();
		}

	for (y = 0; y < mapy ; ++y)
			for (x = 0; x < mapx; ++x){
				if (tempmap2[x+mapx*y]!=map[x][y].tile)
					map[x][y].shade |= PROBLEM_MASK;
				else
					map[x][y].tile=tempmap2[x+mapx*y];
			}

	free(tempmap2);
	show_debug_problems=1;
}

void handle_units_bmp_saving(int units_bmp, int units_per_country_bmp, int bmp_idx, int end_bmp_idx, int country_idx, int m, int y, int comment_bmps, int flipIcons, int bmp_x, int tile_width, int tile_height) {
	char bmp_name[256];
	BITMAP *bmp;
	int j,k,limit, flip_this_icon=0;

	if (units_bmp) {
		if (bmp_idx != 0) {
			if (bmp_idx >0){
				if (end_bmp_idx <= bmp_idx) {
						//one unit(s) only
						limit = bmp_idx;
					} else {
						//unit range
						limit = Min(end_bmp_idx,total_uicons-1);
					}
			}
			else{
				//all units
				bmp_idx=0;
				limit=total_uicons-1;
			}
			for(k=bmp_idx;k<=limit;k++){
				bmp = create_bitmap_ex(24, tile_width + 3 * tile_width * (comment_bmps > 0), tile_height);
				if (flipIcons){
					flip_this_icon=0;
					if (isFlipNeeded(k)) flip_this_icon=1;
				}
				draw_one_unit_bmp(bmp,k,comment_bmps,flip_this_icon,tile_width,tile_height);
				snprintf(bmp_name, 256, "u%04d.bmp", k);
				if (save_bmp(bmp_name, bmp, NULL))
					printf("ERROR writing file %s !!\n", bmp_name);
				else
					printf("File %s saved.\n", bmp_name);

				destroy_bitmap(bmp);
			}
		} else {
			//whole stuff, bmp_idx == 0
			bmp = create_bitmap_ex(24, bmp_x * tile_width + (comment_bmps == 1) * 2 * 24, (total_uicons / bmp_x + ((total_uicons % bmp_x) ? 1 : 0)) * tile_height + (comment_bmps == 1) * 2 * 24);
			draw_units_bmp(bmp, comment_bmps, flipIcons, bmp_x,tile_width,tile_height);
			if (save_bmp(pgf_units_bmp, bmp, NULL))
				printf("ERROR writing file %s !!\n", pgf_units_bmp);
			else
				printf("File %s saved.\n", pgf_units_bmp);
			destroy_bitmap(bmp);
		}
	}
	if (units_per_country_bmp) {
		if (country_idx != 0) {
			//one country only
			if (country_idx == -1 || (country_idx <= total_countries && country_idx > 0))
				draw_units_per_country_bmp(country_idx, m, y, flipIcons);
		} else {
			for (j = 0; j <= total_countries; j++)
				draw_units_per_country_bmp(j == 0 ? -1 : j, m, y, flipIcons);
		}
	}
}

int cli_parsing(int argc, char *argv[]) {
	int i, j, y, m, error=0;
	int bmp_idx=0;
	int end_bmp_idx=0;
	int country_idx=0;

	int hide_empty = 0;
//	int global_found = 0;
	int save_PNG = 0;
	int show_spotting = 3;
	int show_units = 0;
	char param=0;
	char path[MAX_PATH];

	int comment_bmps=0;
	int fix_roads=0;
	int units_bmp=0;
	int units_per_country_bmp=0;
	int fix_names=0;
	int fix_names_hard=0;
	int fix_terrain=0;
	int fix_sea_air_transport=0;
	int fix_owner=0;
	//int fix_units=0;
	int try_harder=0;
	int export_to_lgeneral=0;
	int swap_side=0;
	int list_only=0;
	int display_csv=0;
	int fix_brefings=0;
	int shift_mode=0;
	int displayUnitsOrderSave = displayUnitsOrder;
	int tmp_fpge_colors_bits_shift;
	int show_stats=0;
	int side_save=0;
	int list_units_mode=0;
	int flip_icons=0;
	int pattern_range_mode=1;
	int pattern_mode=LIMITED_SET_RIVER;
	int tmode=0;
	int tscope=0;
	//layers
	//int layer_tiles=0;
	//int layer_roads=0;
	//int layer_terrain_type=0;

	//BITMAP *bmp;

//	int x, y, j;
//	int number_of_names[MAX_NAMES];

	//if (argc>1) print_str(argv[1]);
	//if (argc>2) print_str(argv[2]);
	//if (argc>3) print_str(argv[3]);

	if (strcmp(argv[1],"-h")!=0 && argv[1][1]!='X'){
		setScenarioNumber(atoi(argv[argc-1]));
		//read_map(scn_number);
		//printf("Loading scenario #%02d...\n",scn_number);
		if (!pacgen_mode){
			error=load_scenario(getScenarioNumber(),SHOW_LOGS);
			if (error){
				printf("%s\n",load_error_str);
				return error;
			}else{
				printf("Scenario #%02d loaded.\n",getScenarioNumber());
			}
		}

	}
	//check for other params
	if ( (argc>2)||( argv[1][1]=='h'||argv[1][1]=='X'||argv[1][1]=='Y') ){
		//read only one
		if (argv[1][0]!='-') {
			printf("Wrong parameter : %s",argv[1]);
			return 2;
		}
		if (strlen(argv[1])>2) param=argv[1][2];
		switch (param){
			case 's': hide_empty=1; break;
			case 'd': hide_names=1; break;
		}
		param=argv[1][1];
		switch (param) {
			case 'd':
				debug_tile_matrix=1;
				break;
			case 'B':
				if (strlen(argv[1]) > 2) {
					hide_empty = 1;
					for (i = 2; i < strlen(argv[1]); i++) {
						switch (argv[1][i]) {
						//---4----9-
						//----------
						//---------
						//-------
						case 'b':
							shift_mode++;
							break;
						case 'R':
						case '8':
							show_problems=1;
							//mark_road_problems();
							break;
						case 'C':
						case '7':
							show_problems=2;
							//mark_city_problems();
							break;
						case 'U':
							show_problems=3;
							break;
						case 'V':
							show_problems=4;
							break;
						case 'O':
							show_problems=5;
							break;
						case 'I':
							show_problems=6;
							break;
						case 'A':
							colorize_names=1;
							break;
						case 'E':
							colorize_names=2;
							break;
						case 'g':
							show_unit_layer_type = 0;
							break;
						case 'a':
							show_unit_layer_type = 1;
							break;
						case 't':
							show_unit_layer_type = 2;
							break;
						case 'u':
							show_unit_layer_type = 3;
							break;
						case 'T':
							show_unit_layer_type = 4;
							break;
						case '0':
							drawNames = 0;
							break;
						case '1':
							drawNames = 1;
							break;
						case '2':
							drawNames = 2;
							break;
						case '3':
							drawNames = 3;
							break;
						case '4':
							save_PNG=1;
							break;
						case 'n':
							drawAllNames = 1;
							break;
						case 'y':
							drawTerrain = 1;
							break;
						case 'r':
							drawRoads = 1;
							break;
						case 'd':
							showDecimal = 0;
							break;
						case 'h':
							showDecimal = 1;
							break;
						case 'f':
							showDecimal = 2;
							break;
						case 's':
							showCounter = 0;
							break;
						case 'e':
							showCounter = 1;
							break;
						case 'x':
							showCounter = 2;
							break;
						case 'k':
							showCounter = 3;
							break;
						case 'c':
							showCounter = -1;
							break;
						case 'l':
							drawGndTransport = 1;
							break;
						case 'o':
							showWeather = 0;
							break;
						case 'm':
							showWeather = 1;
							break;
						case 'w':
							showWeather = 2;
							break;
						case 'i':
							showHex = 0;
							break;
						case 'v':
							graphical_overide_hex |= 0x01;
							break;
						case 'p':
							graphical_overide_hex |= 0x02;
							break;
						case 'q':
							graphical_overide_hex |= 0x04;
							break;
						case '@':
							graphical_overide_hex = 7;
							break;
						case 'z':
							graphical_overide |= 0x01;
							break;
						case '#':
							graphical_overide |= 0x02;
							break;
						case '^':
							scenarioUnitsMode = 1;
							break;
						case '$':
							displayAllUnits = 1;
							break;
						case 'j':
							displayUnitsOrder += 1;
							break;
						case '5':
							show_ranges = 0;
							compute_shade(show_ranges);
							break;
						case '6':
							show_ranges = 1;
							compute_shade(show_ranges);
							break;
						}
					}
				}

				if (!hide_empty)
					if (load_config() == 0)
						printf("Reading FPGE config file succeeded.\n");

				mark_problems();
/*
				  if(argc > 3){
				  		//printf("%s\n",argv[2]);
				  }
*/
				tmp_fpge_colors_bits_shift=fpge_colors_bits_shift;

				if (save_PNG){
					printf("Saving PNG file\n");
					fpge_colors_bits_shift=2;
				}
				else
					printf("Saving BMP file\n");
				printf("Please wait...\n");

				set_pal();

				save_map_bmp = create_bitmap_ex(fpge_colors_bits,
						mapx * TILE_WIDTH + (TILE_FULL_WIDTH-TILE_WIDTH) + 2*map_shift_x(shift_mode),
						mapy * TILE_HEIGHT + TILE_HEIGHT/2 + 2* map_shift_y(shift_mode));

				draw_map_shift(save_map_bmp, 0, 0, mapy, mapx,shift_mode);

				sprintf(path, ".\\%s%02d.%s",(argc>3?argv[2]:"map"),getScenarioNumber(),(save_PNG?"png":"bmp"));
				canonicalize_filename(path,path,MAX_PATH);

				if (save_PNG) {
					if (save_png(path, save_map_bmp, 0)) {
						printf(
								"ERROR: Cannot save PNG file. File cannot be saved.\n");
					}
				} else {
					if (save_bmp(path, save_map_bmp, pgpal)) {
						printf(
								"ERROR: Cannot save BMP file. File cannot be saved.\n");
					}
				}

				destroy_bitmap(save_map_bmp);
				displayUnitsOrder=displayUnitsOrderSave;
				fpge_colors_bits_shift=tmp_fpge_colors_bits_shift;
				return 1;
			case 'F':
				if (strlen(argv[1]) > 2) {
					hide_empty = 1;
					for (i = 2; i < strlen(argv[1]); i++) {
						switch (argv[1][i]) {
						case 'r':
							fix_roads=1;
							break;
						case 'n':
							fix_names=1;
							break;
						case 'h':
							fix_names_hard=1;
							break;
						case 't':
							fix_terrain=1;
							break;
						case 's':
							fix_sea_air_transport=1;
							break;
						case 'o':
							fix_owner=1;
							break;
						}
					}
					if (fix_roads) {
						printf("Fixing roads.\n");
						do_fix_roads();
					}
					if (fix_names) {
						printf("Fixing simple names.\n");
						do_fix_names();
					}
					if (fix_names_hard) {
						printf("Fixing simple names (harder).\n");
						do_fix_names_hard();
					}
					if (fix_terrain) {
						printf("Fixing terrain types.\n");
						do_fix_terrain_types();
					}
					if (fix_sea_air_transport) {
						printf("Fixing sea and air transports.\n");
						do_fix_sea_air_transport();
					}
					if (fix_owner) {
						printf("Fixing owner. From %d -> %d\n",atoi(argv[2]),atoi(argv[3]));
						do_fix_owner(atoi(argv[2]),atoi(argv[3]));
					}

					if (fix_roads || fix_names || fix_terrain || fix_names_hard || fix_sea_air_transport || fix_owner){
						printf("Saving scenario.\n");
						save_scenario_procedure();
					}else
						printf("Nothing to fix. Not saving.\n");

				}
				return 1;
			case 'R':
				if (strlen(argv[1]) > 2) {
					for (i = 2; i < strlen(argv[1]); i++) {
						switch (argv[1][i]) {
						case 'o':
							printf("Randomizing rough tiles...\n");
							randomize_tiles(1);
							break;
						case 'c':
							printf("Randomizing clear tiles...\n");
							randomize_tiles(2);
							break;
						case 'd':
							printf("Randomizing desert tiles...\n");
							randomize_tiles(3);
							break;
						case 'e':
							printf("Randomizing rough desert tiles...\n");
							randomize_tiles(4);
							break;
						case 'f':
							printf("Randomizing forests tiles...\n");
							forest_random();
							//randomize_tiles(183,7);
							break;
						case 'b':
							printf("Randomizing bocage tiles...\n");
							randomize_tiles(5);
							break;
						case 's':
							printf("Randomizing swamp tiles...\n");
							randomize_tiles(6);
							break;
						case 'r':
							printf("Randomizing roads patterns...\n");
							randomize_patterns(0);
							break;
						case 'i':
							printf("Randomizing river patterns...\n");
							randomize_patterns(1);
							break;
						}
						save_scenario_procedure();
					}
				}
				return 1;
			case 'X':

				flip_icons=0;

				if (strlen(argv[1]) > 2) {
					for (i = 2; i < strlen(argv[1]); i++) {
						switch (argv[1][i]) {
						case 'A':
							comment_bmps|=1;
							break;
						case 'I':
							comment_bmps|=2;
							break;
						case 'F':
							flip_icons=1;
							break;
						case 'u':
							units_bmp=1;
							units_per_country_bmp=0;
							break;
						case 'c':
							units_per_country_bmp=1;
							units_bmp=0;
							break;
						}
					}//for
					//printf("argc=%d\n",argc);
					bmp_idx=0;
					end_bmp_idx=0;
					country_idx=0;


					if (units_bmp && argc>=3)
						bmp_idx=atoi(argv[2]);

					if (units_bmp && argc>=4)
						end_bmp_idx=atoi(argv[3]);

					if (units_per_country_bmp && argc>=3)
						country_idx=atoi(argv[2]);

					//print_dec(flip_icons);
					//print_dec(end_bmp_idx);

					//display all units, do not filter by month and year
					y=100;
					m=12;
					if (argc==5) {
						m=atoi(argv[3]);
						y=atoi(argv[4]);
						//printf("c=%d, y=%d, m=%d\n",i,y,m);
					}
					handle_units_bmp_saving(units_bmp, units_per_country_bmp, bmp_idx, end_bmp_idx, country_idx,  m,  y, comment_bmps, flip_icons, 20,TILE_FULL_WIDTH , TILE_HEIGHT);
				}
				return 1;
			case 'E':
				if (strlen(argv[1]) > 2) {
				for (i = 2; i < strlen(argv[1]); i++) {
					switch (argv[1][i]) {
					case 'g':
						export_to_lgeneral=1;
						break;
					case 'h':
						try_harder=1;
						break;
					case 's':
						swap_side=1;
						break;
					case 'l':
						list_only=1;
						break;
					case 'd':
						hide_names=1;
						break;
					case 'b':
						side_save = 1;
						break;
					}
					}
				}
				if (list_only){
					list_conversion_of_units(hide_names,try_harder,1); //display conversion
				}else{
					if (export_to_lgeneral){
						save_lgeneral_scenario_CLI(0);
					}else{
						save_pgf_scenario(hide_names,try_harder,swap_side,side_save);
					}
				}
				return 1;
			case 'N':
				list_names();
				return 1;
			case 'O':
				show_names_vs_tiles();
				return 1;
			case 'r':
				checking_road_connections();
				return 1;
			case 'i':
				checking_river_names(0);
				return 1;
			case 'U':

				if (strlen(argv[1]) > 2) {
				for (i = 2; i < strlen(argv[1]); i++) {
					switch (argv[1][i]) {
					case 'b':
						list_units_mode=1;
						break;
						}
					}
				}
				listing_units(list_units_mode,hide_names);
				return 1;
			case 'c':
				check_city_names(0);
				return 1;
			case 'C':
				listing_city_names();
				return 1;
			case 'o':
				if (strlen(argv[1]) > 2) {
				for (i = 2; i < strlen(argv[1]); i++) {
					switch (argv[1][i]) {
					case 's':
						show_stats=1;
						break;
						}
					}
				}
				checking_ocean(0,show_stats);
				return 1;
			case 'M':
			case 'D':
				show_unit_layer_type = 0;
				graphical_overide_hex = 0;
				if (strlen(argv[1]) > 2) {
					for (i = 2; i < strlen(argv[1]); i++) {
						switch (argv[1][i]) {
						case 's':
							swap_indicators =1;
							break;
						case 'g':
							show_unit_layer_type = 0;
							break;
						case 'a':
							show_unit_layer_type = 1;
							break;
						case 'u':
							show_unit_layer_type = 3;
							break;
						case '$':
							displayAllUnits = 1;
							break;
						case 'v':
							graphical_overide_hex = 1;
							break;
						case 'o':
							showWeather = 0;
							break;
						case 'm':
							showWeather = 1;
							break;
						case 'w':
							showWeather = 2;
							break;
						case '5':
							show_spotting = 1;
							break;
						case '6':
							show_spotting = 2;
							break;
						case 'x':
							show_units = 1;
							break;
						case 'l':
							show_units = 2;
							break;
						}
					}
				}

				if (param=='M'){
					error=prepare_mini_tiles();
					if (error) return error;

					printf("Saving strategy map...\n");

					make_mmap(show_unit_layer_type, graphical_overide_hex, show_spotting,show_units,0); //0 small size
				}

				if (param=='D'){
					printf("Saving strategy medium map...\n");
					make_mmap(show_unit_layer_type, graphical_overide_hex, show_spotting,show_units,1); //1 medium size
				}

				return 1;
			case 'T':
				display_terrain_type(hide_empty);
				return 1;
			case 'A':
				display_tile_name(hide_empty);
				return 1;
			case 'h':
				CLIHelp(MapStatusTxt);
				return 1;
			case 'S':
				if (strlen(argv[1]) > 2) {
					for (i = 2; i < strlen(argv[1]); i++) {
						switch (argv[1][i]) {
						case 'c':
							display_csv = 1;
							break;
						}
					}
				}
				display_scenario(display_csv);
				return 1;
			case 'u':
				checking_units(0);
				return 1;
			case 'v':
				checking_flags(0);
				return 1;
			case 'Y':
				parse_pgcam(SHOW_LOGS,LOAD_FILE);
				return 1;
			case 'y':
				if (strlen(argv[1]) > 2) {
					for (i = 2; i < strlen(argv[1]); i++) {
						switch (argv[1][i]) {
						case 'f':
							fix_brefings = 1;
							break;
						case 'b':
							printf("Converting PG briefings to PGF format.\n");
							convert_brf();
							return 1;
						}
					}
				}
				check_pgcam(SHOW_LOGS,LOAD_FILE,fix_brefings);
				return 1;
			case 'L': //layers
				//printf("argc=%d\n",argc);
				if (argc!=4){
					printf("ERROR. Wrong number of arguments\n");
				}else{
					//printf("1=%s 2=%s 3=%s\n",argv[1],argv[2],argv[3]);
					j=0;
					if (strlen(argv[1]) > 2) {
						for (i = 2; i < strlen(argv[1]); i++) {
							switch (argv[1][i]) {
							case 's':
								printf("Loading tile layer.\n");
								j+=load_tiles_layer(argv[2])==0?1:0;
								break;
							case 'r':
								printf("Loading roads layer.\n");
								j+=load_roads_layer(argv[2])==0?1:0;
								break;
							case 't':
								printf("Loading terrain type layer.\n");
								j+=load_terrain_type_layer(argv[2])==0?1:0;
								break;
							case 'n':
								printf("Loading names layer.\n");
								j+=load_names_layer(argv[2])==0?1:0;
								break;
							}
						}
					}
					if (j>0) {
						printf("Saving scenario.\n");
						save_scenario_procedure();
					}
				}
				return 1;
		case 'P':
			if (strlen(argv[1]) > 2) {
				for (i = 2; i < strlen(argv[1]); i++) {
					switch (argv[1][i]) {
					case 'R':
						pattern_range_mode = 2;
						break;
					case 'r':// LIMITED_SET_RIVER is default
						pattern_mode = LIMITED_SET_ROAD;
						break;
					case 'o':
						pattern_mode = LIMITED_SET_ROAD_AND_RIVER;
						break;
					case 'a':
						pattern_mode = LIMITED_SET_ALL;
						break;
					}
				}
			}
			display_patterns(pattern_mode, pattern_range_mode); //0-river
			return 1;
		case 'x':
			if (strlen(argv[1]) > 2) {
				for (i = 2; i < strlen(argv[1]); i++) {
					switch (argv[1][i]) {
					case 'm':
						tmode++;
						break;
					case 's':
						tscope++;
						break;
					case 'r':// LIMITED_SET_RIVER is default
						pattern_mode = LIMITED_SET_ROAD;
						break;
					case 'o':
						pattern_mode = LIMITED_SET_ROAD_AND_RIVER;
						break;
					case 'a':
						pattern_mode = LIMITED_SET_ALL;
						break;
					}
				}
			}
			run_and_marks_rivers(tmode,tscope,pattern_mode);
			break;
		}
		}
	return 0;
}
