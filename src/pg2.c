/*
 * pg2.c
 *
 *  Created on: 2011-10-24
 *      Author: wino
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "fpge.h"
#include "pg2.h"
#include "pgf.h"
#include "tables.h"
#include "maingui.h"
#include "drawmap.h"
#include "load.h"
#include "pzc.h"
#include "filename.h"
#include "unitutil.h"

/*
 * http://luis-guzman.com/links/PG2_FilesSpec.html
 *
 00= Clear
 01= City
 02= Airfield
 03= Forest
 04= Bocage
 05= Hill
 06= Mountain
 07= Sand
 08= Swamp
 09= Ocean
 10= River
 11= Fortification
 12= Port
 13= Stream
 14= Escarpment
 15= Impassable River
 16= Rough

 Pg2 country codes from GUI97.txt
 01= Austria
 02= Belgium
 03= Bulgaria
 04= Czechoslovakia
 05= Denmark
 06= Finland
 07= France
 08= Germany
 09= Greece
 10= USA
 11= Hungary
 12= Turkey
 13= Italy
 14= Netherlands
 15= Norway
 16= Poland
 17= Portugal
 18= Romania
 19= Spain
 20= Russia
 21= Sweden
 22= Switzerland
 23= United Kingdom
 24= Yugoslavia
 25= Nationalist
 26= Republican

 Equipment class
 00=None (no unit maker)
 01=Infantry
 02=Tank
 03=Recon
 04=Anti-Tank
 05=FlaK (Panzer General II)
 06=Fortification
 07=Ground Transport (no unit maker)
 08=Artillery
 09=Air Defense
 10=Fighter
 11=Tactical Bomber
 12=Level Bomber (no unit maker)
 13=Air Transport
 14=Submarine
 15=Destroyer
 16=Battleship
 17=Carrier
 18=Naval Transport
 19=Battle Cruiser
 20=Cruiser
 21=Light Cruiser

 Target type
 0=Soft
 1=Hard
 2=Air
 3=Sea

 Movement Method
 00=Tracked
 01=Half Tracked
 02=Wheeled
 03=Leg
 04=Towed
 05=Air
 06=Deep Naval
 07=Costal
 08=All Terrain (tracked)
 09=Amphibious
 10=Naval
 11=All Terrain (Leg)
 */

//get the tiles_for_bmp row number based upon PG2 terrain type
//MAX_TERRAIN_TYPE=40 for PG, should be enough for PG2
int pg2tt_to_pgtilesrange[MAX_TERRAIN_TYPE] = { //-1 not handled = clear
		6, 8, 14, 1, 9, // airfield==2 -> 14
		7, 2, 5, 10, 0, // hill==5 -> 7
		4, 11, 13, 4, 12, // fort==11 -> 11, 13=port==12 -> 13,  Escarpment==14 -> 12
		4, 7 };

//convert PG2 terrain type to PG terrain type
int pg2tt_to_pgtt[MAX_TERRAIN_TYPE] = { //MAX_TERRAIN_TYPE=40 for PG, should be enough for PG2
		0, 21, 19, 23, 27,
		12, 13, 30, 20, 7, // 12 is a Mountain in PG and PG2 Hill
		9, 25, 8, 10, 35, // 10 is river in PG and PG2 stream
		11, 22 // 11 is river in PG and PG2 Impassable River
		};

// but 04= Czechoslovakia, we ignore this
int pg2nation_to_pgnation[MAX_COUNTRY] = {
		0, 1, 2, 3, 4,
		5, 6, 7, 8, 9,
		10, 11, 12, 13, 14,
		15, 16, 17, 18, 19,
		20, 21, 22, 23, 24,
		25, 26 };

//now is defined to 22
#define MAX_PG2_CLASS_TYPE 40

int pg2class_to_pgclass[MAX_PG2_CLASS_TYPE] = {
		-1, 0, 1, 2, 3, //FLAK ?
		5, 7, 15, 4, 6, // PG2 AIrDefence = PG1 Anti Aircraft = mobile flak?
		8, 9, 10, 16, 11,
		12, 13, 14, 17, 13, // PG2 Battle Cruiser = PG Capital ship
		13, 13 // PG2 Light Cruiser, Cruiser = PG Capital ship
		};

#define PG2_EQUIP_REC_SIZE 73

struct pg2_eqp {
	uint16_t id; //ignored
	uint8_t class, sa, ha, aa, na, gd, ad, cd, tt, ini, range, spotting;
	uint8_t range_defense_modier;
	uint8_t move_mode; //0-ground,1-air
	uint8_t movement_method; // 00=Tracked, etc
	uint8_t movement, fuel, ammunition, uk1;
	uint8_t cost; //x 12
	uint8_t uk2[3];
	uint8_t month_exp,month_avail;
	uint8_t year_avail,year_exp;
	uint8_t transport; //00=Cannot be transported  01=Can be Naval transported 02=Can be Airmobile & naval 03=can be Airborne & naval & Airmobile
	uint8_t country[4];
	uint8_t special[4];
	uint8_t bomber_size;
	char icon_name[6];
	uint16_t attack_sound, move_sound, death_sound;
	uint8_t ground_transport_weight;
	uint8_t air_and_naval_transport_weight;
	uint8_t uk3[20];
};

char pg2_gln[MAX_NAMES][MAX_PG2_NAME_SIZE];
char pg2_equip_names[MAX_UNITS][MAX_PG2_NAME_SIZE];
int total_pg2_equip_names;

int pg2_gln_map[MAX_NAMES]; // map to gln ids, -1-not set,
int total_pg2_names;

//------UI
#define PG2_FG_COLOR GUI_FG_COLOR
#define PG2_BG_COLOR GUI_BG_COLOR
#define PG2_EDIT_COLOR GUI_EDIT_COLOR

#define PG2_COL1 198+16
#define PG2_COL2 198+16+96+16

#define PG2_LINES_IN_LISTBOX 27

char pg2_new_id[256];
char pg2_old_id[256];
char pg2_unit_name[256];
char pg2_unit_country[256];
char pg2_unit_class_name[256];

struct pzc_unit pg2_equip_conversion[MAX_UNITS];

DIALOG pg2_cam_dlg[PG2_CAM_DLG_SIZE] =
{
   /* 0 */   { d_textbox_proc, 0,   0,   224, 160-96+4+16, PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      0,   0,   0,                      NULL, NULL },
   /* 1 */   { d_radio_proc,   16,  96-96+4+16,  128, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,  0,D_SELECTED,  0,   0,   (void *)"PGF format",   NULL, NULL },
   /* 2 */   { d_radio_proc,   16,  112-94+4+16, 128, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      0,   0,   (void *)"PZC format",   NULL, NULL },
   /* 3 */   { d_button_proc,  16,  136-96+4+16, 88,  16,  PG2_FG_COLOR,   PG2_BG_COLOR, 'c',D_EXIT,     0,   0,   (void *)"&Cancel",      NULL, NULL },
   /* 4 */   { d_button_proc,  120, 136-96+4+16, 88,  16,  PG2_FG_COLOR,   PG2_BG_COLOR, 'o',D_EXIT,     0,   0,   (void *)"C&onvert",     NULL, NULL },
   /* 5 */   { d_text_proc, 16 , 8 , 96, 15, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, (void *) "Select output format" },
			 { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
			 { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG findpg2id_dlg[PG2_IDCONVERT_DLG_SIZE] = {
/* 0 */{ d_textbox_proc, 0, 0, 198 + 96 + 16 + 96 + 16 + 16, 222 + 16, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, 0 },
/* 1 */{ d_pg2_list_proc, 6, 6, /*174+16*/7 * PG2_LINES_IN_LISTBOX, 222, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, (void *) pg2_list },
/* 2 */{ d_button_proc, PG2_COL1, 204, 96, 15, PG2_FG_COLOR, PG2_BG_COLOR, 'c', D_EXIT, 0, 0, (void *) "&Cancel" },
/* 3 */{ d_button_proc, PG2_COL2, 204, 96, 15, PG2_FG_COLOR, PG2_BG_COLOR, 'd', D_EXIT, 0, 0, (void *) "&Done" },
/* 4 */{ d_edit_proc, PG2_COL1, 102 - 40 + 8 + 32 + 16, 8 * 21, 15, PG2_FG_COLOR, PG2_EDIT_COLOR, 0, 0, 20, 1, (void *) pg2_new_id },
/* 5 */{ d_text_proc, PG2_COL1, 84 - 40 + 8 + 32 + 16, 96, 15, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, (void *) "PG ID:" },
/* 6 */{ d_text_proc, PG2_COL1, 84 - 80 + 8, 96, 15, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, (void *) "PG2 equ name:" },
/* 7 */{ d_text_proc, PG2_COL1, 84 - 80 + 8 + 16, 96, 15, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, (void *) pg2_old_id },
/* 8 */{ d_text_proc, PG2_COL1, 84 - 80 + 8 + 16 + 10, 8 * 21, 15, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, (void *) pg2_unit_name },
/* 9 */{ d_text_proc, PG2_COL1, 84 - 80 + 8 + 16 + 16 + 16 + 16, 8 * 21, 15, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, (void *) pg2_unit_country },
/* 10 */{ d_text_proc, PG2_COL1, 84 - 80 + 8 + 16 + 16 + 16, 8 * 21, 15, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 0, 0, (void *) pg2_unit_class_name },
/* 11 */{ d_pg2_country_check_proc, PG2_COL1, 204 - 24, 8 * 12 + 12, 15, PG2_FG_COLOR, PG2_BG_COLOR, 0, 0, 1, 0, "Ignore flag" },
		{ d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
		{ NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL }
};

DIALOG pg2_convert_dlg[PG2_DLG_SIZE] =
{
   /* 0 */   { d_textbox_proc, 0,   0,   224, 160+16, PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      0,   0,   0,                      NULL, NULL },
   /* 1 */   { d_check_proc,   16,  24,  168, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      1,   0,   (void *)"copy terrain types",   NULL, NULL },
   /* 2 */   { d_check_proc,   16,  40,  168, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      1,   0,   (void *)"copy roads links",     NULL, NULL },
   /* 3 */   { d_check_proc,   16,  56,  168, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      1,   0,   (void *)"generate tiles",       NULL, NULL },
   /* 4 */   { d_check_proc,   16,  56+16,  168, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      1,   0,   (void *)"generate road tiles",  NULL, NULL },
   /* 5 */   { d_text_proc,    16,  8,   192, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      0,   0,   (void *)"Convert PG2 scenario", NULL, NULL },
   /* 6 */   { d_radio_proc,   16,  80+16,  128, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,  0,D_SELECTED,  0,   0,   (void *)"no units",             NULL, NULL },
   /* 7 */   { d_radio_proc,   16,  96+16,  128, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      0,   0,   (void *)"convert units",        NULL, NULL },
   /* 8 */   { d_radio_proc,   16,  112+16, 128, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      0,   0,   (void *)"copy units",           NULL, NULL },
   /* 9 */   { d_button_proc,  16,  136+16, 88,  16,  PG2_FG_COLOR,   PG2_BG_COLOR, 'c',D_EXIT,     0,   0,   (void *)"&Cancel",              NULL, NULL },
   /* 10 */  { d_button_proc,  120, 136+16, 88,  16,  PG2_FG_COLOR,   PG2_BG_COLOR, 'o',D_EXIT,     0,   0,   (void *)"C&onvert",             NULL, NULL },
		     { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
		     { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

DIALOG pg2_convert_map_dlg[PG2_MAP_DLG_SIZE] =
{
   /* 0 */   { d_textbox_proc, 0,   0,   224, 160+16-48, PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      0,   0,   0,                      NULL, NULL },
   /* 1 */   { d_check_proc,   16,  24,  168, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      1,   0,   (void *)"copy terrain types",   NULL, NULL },
   /* 2 */   { d_check_proc,   16,  40,  168, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      1,   0,   (void *)"copy roads links",     NULL, NULL },
   /* 3 */   { d_check_proc,   16,  56,  168, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      1,   0,   (void *)"generate tiles",       NULL, NULL },
   /* 4 */   { d_check_proc,   16,  56+16,  168, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      1,   0,   (void *)"generate road tiles",  NULL, NULL },
   /* 5 */   { d_text_proc,    16,  8,   192, 16,  PG2_FG_COLOR,   PG2_BG_COLOR,   0,    0,      0,   0,   (void *)"Convert PG2 map", NULL, NULL },
   /* 6 */   { d_button_proc,  16,  136+16-48, 88,  16,  PG2_FG_COLOR,   PG2_BG_COLOR, 'c',D_EXIT,     0,   0,   (void *)"&Cancel",              NULL, NULL },
   /* 7 */  { d_button_proc,  120, 136+16-48, 88,  16,  PG2_FG_COLOR,   PG2_BG_COLOR, 'o',D_EXIT,     0,   0,   (void *)"C&onvert",             NULL, NULL },
		     { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
		     { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};


int c_pg_country;
int c_pg_class;
int c_pg_type;
int ignore_country_gui =0;

int equ_id_to_display_filter[MAX_UNITS];
int equ_id_to_display[MAX_UNITS];
int pg2id_to_display_number;
int pg2unit_to_pgunit[MAX_UNITS];

//lowercase only file name, not all path
void canonicalize_filename_pg2(char *dest, const char *filename, int size){
	//char buf[MAX_PATH];

	canonicalize_filename(dest, filename,  size);
	//in case of wrong filename all path will be lower cased. To minimize probability of thie event first we do canonicalize_filename.
	strlwr(get_filename(dest));

	//strncpy(dest, buf,  size);
	//just in case
	//canonicalize_filename(dest, filename,  size);
}

char *clear_lwr_string(char *desc, char * src){
	int i,j=0;

	for(i=0;i<=strlen(src);i++){ //including end of string 0
		if ((src[i]>=48 && src[i]<=57)||(src[i]>=97 && src[i]<=122)||(src[i]==0)){ //0-9, a-z
			desc[j]=src[i];
			j++;
		}
	}
	return desc;
}

char *remove_left_string(char *desc, char * src, int no){
	int i,j=0;

	for(i=0;i<=strlen(src);i++){ //including end of string 0
		if (i>no){
			desc[j]=src[i];
			j++;
		}
	}
	return desc;
}

int try_to_find_equip(int c_pg_country){
	int i,id,j,match_id=-1,match_value=0;
	char pg_equip_name_trimmed[256], pg2_equip_trimmed[256], country_lwr[256]; //pg2_unit_name

	for(i=0;i<pg2id_to_display_number;i++){
		id= equ_id_to_display[i]; //id of the filtered unit
		strncpy(pg_equip_name_trimmed,equip_name_utf8[id],256);
		strlwr(pg_equip_name_trimmed);
		strncpy(pg2_equip_trimmed, pg2_old_id,256);
		//strncpy(pd2_equip_trimmed,pg2_unit_name,256);
		strlwr(pg2_equip_trimmed);
		strncpy(country_lwr,country_names_short[c_pg_country],256);
		strlwr(country_lwr);
		if (strstr(pg_equip_name_trimmed,country_lwr)==pg_equip_name_trimmed )
			{
				remove_left_string(pg_equip_name_trimmed,pg_equip_name_trimmed,strlen(country_lwr));
			}
		clear_lwr_string(pg_equip_name_trimmed,pg_equip_name_trimmed);
		clear_lwr_string(pg2_equip_trimmed,pg2_equip_trimmed);
		//print_str(pg_equip_name_trimmed);
		//print_str(pg2_equip_trimmed);
		//try to left match
		int match_all=1;
		for(j=0;j<Min(strlen(pg_equip_name_trimmed), strlen(pg2_equip_trimmed));j++){
			if (pg_equip_name_trimmed[j]!=pg2_equip_trimmed[j]){
				match_all=0;
				if (j>match_value){ //best so far
					match_value=j;
					match_id=i;
				}
				break; //stop search
			}else{
				match_all=1;
			}
		}
		if (match_all){
			//print_dec(match_value);
			//print_dec(match_id);
			//full match
			return i;
		}

	}
	//print_dec(match_value);
	//print_dec(match_id);
	return match_id;
}

int d_pg2_country_check_proc(int msg, DIALOG *d, int c){

	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) {
		d_check_proc(msg,d,c);
		if (findpg2id_dlg[11].flags & D_SELECTED){
			ignore_country_gui=1;
			//print_dec(ignore_country_gui);
			filter_pg_units( -1, c_pg_class, c_pg_type);
		}else{
			ignore_country_gui=0;
			//print_dec(ignore_country_gui);
			filter_pg_units( c_pg_country, c_pg_class, c_pg_type);
		}
		 findpg2id_dlg[11].flags |= D_DIRTY;
		 return D_REDRAW;
	}
	if (msg == MSG_START){
		if (ignore_country_gui){
			findpg2id_dlg[11].flags |=D_SELECTED;
			filter_pg_units( -1, c_pg_class, c_pg_type);
		}else{
			findpg2id_dlg[11].flags &=~D_SELECTED;
			filter_pg_units( c_pg_country, c_pg_class, c_pg_type);
		}
		//try to find reasonable equip
		findpg2id_dlg[1].d1=try_to_find_equip(c_pg_country);
		if (findpg2id_dlg[1].d1 != -1){
			sprintf(pg2_new_id, "%d", equ_id_to_display[findpg2id_dlg[1].d1]);
		}
	}
	return d_check_proc(msg,d,c);
}

char *pg2_list(int index, int *list_size) {
	if (index < 0) {
		*list_size = pg2id_to_display_number;
		return 0;
	}
	return equip_name_utf8[equ_id_to_display[index]];
}

int d_pg2_list_proc(int msg, DIALOG *d, int c) {
	int d1;

	if (msg == MSG_START) {
		if (d->d1 > pg2id_to_display_number)
			d->d1 = -1;

		if (d->d1 > -1) {
			sprintf(pg2_new_id, "%d", equ_id_to_display[findpg2id_dlg[1].d1]);
		} else {
			strncpy(pg2_new_id, "",256);
		}
		findpg2id_dlg[4].flags |= D_DIRTY;
	}

	if (msg == MSG_CLICK || msg == MSG_CHAR) {
		d1 = d->d1;
		d_list_proc(msg, d, c);

		if (d1 > -1 || msg == MSG_CLICK) {
			sprintf(pg2_new_id, "%d", equ_id_to_display[findpg2id_dlg[1].d1]);
			findpg2id_dlg[4].flags |= D_DIRTY;

			if (msg == MSG_CLICK)
				return D_O_K;
			if (msg == MSG_CHAR) {
				if (d1 != d->d1)
					return D_USED_CHAR;
				else
					return D_O_K;
			}
		}
		return D_O_K;
	}

	return d_list_proc(msg, d, c);
}

void filter_pg_units(int pg_country, int pg_class, int pg_type) {
	int i;

	for (i = 0; i < MAX_UNITS; i++) {
		equ_id_to_display_filter[i] = -1;
		equ_id_to_display[i] = 0;
	}
	pg2id_to_display_number = 0;

	//equip[i]

	for (i = 0; i < total_equip; i++)
		if ((pg_country == equip_country[i] || pg_country == -1) &&
				pg_class == equip[i][CLASS] &&
				pg_type == equip[i][TARGET_TYPE])
			equ_id_to_display_filter[i] = i;

	for (i = 0; i < MAX_UNITS; i++)
		if (equ_id_to_display_filter[i] != -1) {
			equ_id_to_display[pg2id_to_display_number] = equ_id_to_display_filter[i];
			pg2id_to_display_number++;
		}
}

int import_pg2_equip_names_file(char *name) {

	FILE *inf;
	int max_names_file_size = MAX_UNITS * MAX_PG2_NAME_SIZE;
	char *end_of_line, path[MAX_PATH];

	strncpy(path, name, MAX_PATH);
	canonicalize_filename_pg2(path, path, MAX_PATH);
	inf = fopen(path, "rt");
	if (!inf) {
		strncpy(path,"..\\",MAX_PATH);
		strncat(path,name,MAX_PATH);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf)
		{
			printf("Cannot open PG2 equip names file : %s\n", path);
			return ERROR_PG2_EQUIP_NAMES_LOAD_BASE + ERROR_FPGE_FILE_NOT_FOUND;
		}
	}
	total_pg2_equip_names = 0;

	fseek(inf, 0, SEEK_END);
	long file_size = ftell(inf);
	fseek(inf, 0, SEEK_SET);

	if (file_size > max_names_file_size) {
		printf("ERROR: Too big PG2 equip names file size %ld, expected at most %d. File : %s\n", file_size, max_names_file_size, path);
		fclose(inf);
		return ERROR_PG2_EQUIP_NAMES_LOAD_BASE + ERROR_FPGE_PG2_FILE_TOO_BIG;
	}

	while (feof(inf) == 0) {
		fgets(pg2_equip_names[total_pg2_equip_names], MAX_PG2_NAME_SIZE, inf);
		//clear eofl = \n
		end_of_line = strchr(pg2_equip_names[total_pg2_equip_names], '\n');
		if (end_of_line != NULL)
			*end_of_line = 0;
		//fscanf(inf, "%s\n", pg2_gln[total_pg2_names]);
		//print_str(pg2_gln[total_pg2_names]);
		++total_pg2_equip_names;
	}

	fclose(inf);
	return 0;
}

int pg2_get_new_id_gui(int pg2_unum, int pg2_country, int pg2_unit_unique_name, int pg_country, int pg_class, int pg_type) {

	int need_to_save = 0;

	if (pg2_country == -1)
		pg_country = -1;

	//save current search
	c_pg_country = pg_country;
	c_pg_class = pg_class;
	c_pg_type = pg_type;

	//int pg_type = equip[pg2_unum][TARG_TYPE]; // ?? is this true

	//printf("Not found : PG2ID=%d PG2FLAG=%d PGFLAG=%d PGCLASS=%d PGTT=%d ---> OLDNAME=%s\n", pg2_unum, pg2_country, pg_country, pg_class, pg_type, pg2_equip_names[pg2_unum]);

	strncpy(pg2_old_id, pg2_equip_names[pg2_unum],256);
	if (pg2_unit_unique_name != -1)
		strncpy(pg2_unit_name, pg2_gln[pg2_unit_unique_name],256);
	else
		strncpy(pg2_unit_name, "",256);

	if (pg_country != -1)
		strncpy(pg2_unit_country, country_names[pg_country],256);
	else
		strncpy(pg2_unit_country, "*ANY*",256);

	if (pg_class != -1)
		strncpy(pg2_unit_class_name, pg_class_names[pg_class],256);
	else
		strncpy(pg2_unit_class_name, "*UNKNOWN*",256);

	filter_pg_units(pg_country, pg_class, pg_type);

	centre_dialog(findpg2id_dlg);
	if (do_dialog(findpg2id_dlg, -1) == FINDPG2ID_OK) {
		//gets(line);
		//print_str(pg2_new_id);
		if (strlen(pg2_new_id) > 0 && atoi(pg2_new_id)>0) {
			pg2unit_to_pgunit[pg2_unum] = atoi(pg2_new_id);
			need_to_save = 1;
		}
	}

	broadcast_dialog_message(MSG_DRAW, 0);
	return need_to_save;
}

int import_pg2_equip(int probe_file_only, char *fname) {
	int i, j, k;
	FILE *inf;
	struct pg2_eqp equip_buffer;
	char path[MAX_PATH];

	strncpy(path,fname,MAX_PATH);
	canonicalize_filename_pg2(path, path, MAX_PATH);
	inf = fopen(path, "rb");
	if (!inf) {
		return ERROR_PG2_EQUIP_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}

	if (probe_file_only != LOAD_CONVERSION_TABLE_ONLY) {
		total_equip=0;
		fread(&total_equip, 2, 1, inf);
		if (total_equip > MAX_UNITS) {
			fclose(inf);
			printf("Maximum units limit (%d) is exceeded!!!\n", MAX_UNITS);
			return ERROR_PG2_EQUIP_BASE + ERROR_FPGE_MAXIMUM_REACHED;
		}
		for (i = 0; i < total_equip; ++i) {
			fread(&(equip_buffer), PG2_EQUIP_REC_SIZE, 1, inf);

			strncpy(equip[i], pg2_equip_names[i], 20);
			strncpy(equip_name_utf8[i], pg2_equip_names[i], 40);

			equip[i][CLASS] = pg2class_to_pgclass[equip_buffer.class];
			//attack
			equip[i][SA] = equip_buffer.sa;
			equip[i][HA] = equip_buffer.ha;
			equip[i][AA] = equip_buffer.aa;
			equip[i][NA] = equip_buffer.na;
			//defense
			equip[i][GD] = equip_buffer.gd;
			equip[i][AD] = equip_buffer.ad;
			equip[i][CD] = equip_buffer.cd;

			equip[i][MOV_TYPE] = equip_buffer.movement_method; //conversion
			equip[i][GAF] = equip_buffer.move_mode;

			equip[i][INITIATIVE] = equip_buffer.ini;
			equip[i][RANGE] = equip_buffer.range;
			equip[i][SPOTTING] = equip_buffer.spotting;

			equip[i][TARGET_TYPE] = equip_buffer.tt;
			equip[i][MOV] = equip_buffer.movement;
			equip[i][FUEL] = equip_buffer.fuel;
			equip[i][AMMO] = equip_buffer.ammunition;
			equip[i][COST] = equip_buffer.cost;
			//no BMP
			equip[i][BMP] = 0;
			equip[i][BMP + 1] = 0;
			//no ANI
			equip[i][ANI] = 0;
			equip[i][ANI + 1] = 0;

			equip[i][MON] = equip_buffer.month_avail;
			equip[i][YR] = equip_buffer.year_avail;
			equip[i][LAST_YEAR] = equip_buffer.year_exp;
			equip_last_month[i] = equip_buffer.month_exp;

			equip[i][ALLOWED_TRANSPORT] = equip_buffer.transport;

			equip_flags[i] = 0;

			equip_flags[i] |= equip_buffer.special[0] & 0x04 ? EQUIPMENT_IGNORES_ENTRENCHMENT : 0;
			equip_flags[i] |= equip_buffer.special[0] & 0x08 ? EQUIPMENT_CAN_BRIDGE_RIVERS : 0;
			equip_flags[i] |= equip_buffer.special[0] & 0x80 ? EQUIPMENT_CANNOT_BE_PURCHASED : 0;
			equip_flags[i] |= equip_buffer.special[1] & 0x04 ? EQUIPMENT_RECON_MOVEMENT : 0;

			// convert from cp1250 to utf8
			//convert_from_cp1250_to_utf8(equip_name_utf8[i],equip[i],20);
			//equip_country[i]=-1;
			//find first country using bits patterns
			for (k = 0; k < 4; k++) {
				uint8_t mask = 0x80;
				for (j = 0; j < 8; j++) {
					if (equip_buffer.country[k] & mask) {
						int new_country = j + k * 8;
						if (new_country == 0)
							new_country = -1;

						equip_country[i] = pg2nation_to_pgnation[new_country];
						//print_dec(j+k*8);
						//we want to stop search
//						if (equip_buffer.country[0]!=0) print_dec(equip_buffer.country[0]);
//						if (equip_buffer.country[1]!=0) print_dec(equip_buffer.country[1]);
//						if (equip_buffer.country[2]!=0) print_dec(equip_buffer.country[2]);
//						if (equip_buffer.country[3]!=0) print_dec(equip_buffer.country[3]);
						k = 4;
						break;
					}
					mask = mask >> 1;
				}
			}
		}
	} else {
		//conversion only
		fread(&conversion_total_equip, 2, 1, inf);
		if (conversion_total_equip > MAX_UNITS) {
			fclose(inf);
			printf("Maximum units limit (%d) is exceeded!!!\n", MAX_UNITS);
			return ERROR_PG2_EQUIP_BASE + ERROR_FPGE_MAXIMUM_REACHED;
		}
		for (i = 0; i < conversion_total_equip; ++i) {
			fread(&(equip_buffer), PG2_EQUIP_REC_SIZE, 1, inf);
			//name
			strncpy(pg2_equip_conversion[i].name, pg2_equip_names[i], 256);

			pg2_equip_conversion[i].id = i;
			pg2_equip_conversion[i].class = equip_buffer.class;
			pg2_equip_conversion[i].target_type = equip_buffer.tt;

			pg2_equip_conversion[i].flag = -1; //default
			//find first country using bits patterns
			for (k = 0; k < 4; k++) {
				uint8_t mask = 0x80;
				for (j = 0; j < 8; j++) {
					if (equip_buffer.country[k] & mask) {
						int new_country = j + k * 8;
						if (new_country == 0)
							new_country = -1;

						pg2_equip_conversion[i].flag = new_country;
						//print_dec(j+k*8);
						//we want to stop search
						k = 4;
						break;
					}
					mask = mask >> 1;
				}
			}
			//TODO All bits country should be set here
		}
	}
	fclose(inf);
	return 0;
}

int import_pg2_map_file(char *path, int convert_terrain_type, int convert_roads_layer, int generate_tiles, int generate_road_tiles) {

	FILE *inf;
	int x, y, k;
	uint16_t xx,yy;
	struct pg2_map_cell {
		WORD tt;
		WORD road;
		char fill[3];
	} rec_buff;

	canonicalize_filename_pg2(path, path, MAX_PATH);
	inf = fopen(path, "rb");
	if (inf != NULL) {
		if (tile_mode == 1) {
			tile_cleanup();
		}
		fseek(inf, 0, SEEK_END);
		long file_size = ftell(inf);
		fseek(inf, 2, SEEK_SET);
		fread(&xx, 2, 1, inf);
		fread(&yy, 2, 1, inf);
		fseek(inf, 10, SEEK_SET);
		if (file_size >= PG2_MAP_FILE_SIZE) {
			//mapx = PG2_MAP_X;
			//mapy = PG2_MAP_Y;
			mapx=xx;
			mapy=yy;

			map_x0 = 0;
			map_y0 = 0;

			for (y = 0; y < PG2_MAP_Y; y++)
				for (x = 0; x < PG2_MAP_X; x++) {
					fread(&rec_buff, 7, 1, inf);
					memset(&map[x][y], 0, sizeof(map[x][y]));
					map[x][y].guidx = -1;
					map[x][y].auidx = -1;

					k = pg2tt_to_pgtilesrange[rec_buff.tt];
					if (k == -1)
						k = 6; //clear

					if (rec_buff.road > 0 && rec_buff.tt != 1 && rec_buff.tt != 2 && rec_buff.tt != 12) {
						if (convert_roads_layer) {
							map[x][y].rc = rec_buff.road;
							//also let the k be unchanged
						} else {
							//when road layer is not converted change to road tile
							k = 3; //special case road, only when tt is no city or port or airfield
						}
					}// no road layer ever for city, port, airfield

					if (generate_road_tiles && map[x][y].rc){
						k=3;
					}

					if (generate_tiles)
						map[x][y].tile = tiles_for_bmp[k][rand() % 3];

					if (convert_terrain_type)
						map[x][y].utr = pg2tt_to_pgtt[rec_buff.tt];
				}
		} else {
			printf("Wrong PG2 map file size %ld, expected at least %d.\n", file_size, PG2_MAP_FILE_SIZE);
			return ERROR_PG2_MAP_LOAD_BASE + ERROR_FPGE_PG2_FILE_TOO_SMALL;
		}
		fclose(inf);
	} else {
		printf("Cannot open PG2 map file : %s\n", path);
		return ERROR_PG2_MAP_LOAD_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}
	return 0;
}

int import_pg2_names_file(char *path) {

	FILE *inf;
	int max_names_file_size = MAX_NAMES * MAX_PG2_NAME_SIZE;
	char *end_of_line;

	canonicalize_filename_pg2(path, path, MAX_PATH);
	inf = fopen(path, "rt");
	if (inf == NULL) {
		printf("Cannot open PG2 names file : %s\n", path);
		return ERROR_PG2_NAMES_LOAD_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}
	total_pg2_names = 0;

	fseek(inf, 0, SEEK_END);
	long file_size = ftell(inf);
	fseek(inf, 0, SEEK_SET);

	if (file_size > max_names_file_size) {
		printf("ERROR: Too big PG2 names file size %ld, expected at most %d. File : %s\n", file_size, max_names_file_size, path);
		fclose(inf);
		return ERROR_PG2_NAMES_LOAD_BASE + ERROR_FPGE_PG2_FILE_TOO_BIG;
	}

	while (feof(inf) == 0) {
		fgets(pg2_gln[total_pg2_names], MAX_PG2_NAME_SIZE, inf);
		//clear eofl = \n
		end_of_line = strchr(pg2_gln[total_pg2_names], '\n');
		if (end_of_line != NULL)
			*end_of_line = 0;
		//fscanf(inf, "%s\n", pg2_gln[total_pg2_names]);
		//print_str(pg2_gln[total_pg2_names]);
		++total_pg2_names;
	}

	fclose(inf);
	return 0;
}

int convert_pg2_unit_to_pg_unit(struct pg2_unit *pg2_unit_cursor, int old_unit_equ_id){
	int need_to_save=0;

	if (pg2unit_to_pgunit[old_unit_equ_id]==-1){
		//do conversion
		if (pg2_get_new_id_gui(old_unit_equ_id, //PG2 unit ID
				//-1,
				pg2_unit_cursor->flag, //PG2 flag
				pg2_unit_cursor->status_2 & 0x04 ? pg2_unit_cursor->name : -1, //units unique(scenario) name ID
				pg2nation_to_pgnation[pg2_unit_cursor->flag], //PG2 flag converted to PG
				pg2class_to_pgclass[pg2_equip_conversion[old_unit_equ_id].class], //class of PG2 unit converted to PG
				//PG2 target type == PG target type so no conversion needed
				pg2_equip_conversion[old_unit_equ_id].target_type) == 1)
		{
			need_to_save = 1;
			//break;
		}
	}
	return need_to_save;

}


int import_pg2_scenario_file(char *path, int convert_terrain_type, int convert_roads_layer, int generate_tiles, int generate_road_tiles, int unit_conv_mode) {

	FILE *inf;
	int x, y, i;
	uint8_t *buf;
	WORD tmp_cash;
	char path2[MAX_PATH];
	struct pg2_map_hex_scn {
		uint8_t flag, supply, victory, deployment;
		uint16_t name;
	};

	struct pg2_map_hex_scn *map_cursor;
	struct pg2_unit *pg2_unit_cursor;

	int map_cursor_struct_size = 6;
	int current_pg2_name, current_pg2_owner;

	int scenario_data_size = 22;
	int player_size = 97;
	int players_size = 4 * player_size;
	int map_hex_size = 6;
	int map_hexs_size = map_hex_size * 45 * 40;
	int map_data_size = 140;
	int unit_size = 65;
	int max_scenario_file_size = scenario_data_size + players_size + map_hexs_size + map_data_size + unit_size * 4 * 100;

	canonicalize_filename_pg2(path, path, MAX_PATH);
	inf = fopen(path, "rb");
	if (inf == NULL) {
		printf("Cannot open PG2 scenario file : %s\n", path);
		return ERROR_PG2_SCENARIO_LOAD_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}

	if (tile_mode == 1) {
		tile_cleanup();
	}
	fseek(inf, 0, SEEK_END);
	long file_size = ftell(inf);
	fseek(inf, 0, SEEK_SET);

	if (file_size > max_scenario_file_size) {
		printf("ERROR: Too big PG2 scenario file size %ld, expected at most %d. File : %s\n", file_size, max_scenario_file_size, path);
		fclose(inf);
		return ERROR_PG2_SCENARIO_LOAD_BASE + ERROR_FPGE_PG2_FILE_TOO_BIG;
	}

	buf = malloc(file_size);

	if (buf == NULL) {
		printf("ERROR: Could not allocate memory for loading file : %s\n", path);
		fclose(inf);
		return ERROR_PG2_SCENARIO_LOAD_BASE + ERROR_FPGE_CANNOT_ALLOCATE_MEM;
	}

	int read_bytes = fread(buf, file_size, 1, inf);
	if (read_bytes != 1) {
		printf("ERROR: Could not read %ld bytes from file : %s\n", file_size, path);
		free(buf);
		fclose(inf);
		return ERROR_PG2_SCENARIO_LOAD_BASE + ERROR_FPGE_FILE_CANNOT_BE_READ;
	}
	//all ok, now parse

	//read map names
	//print_str((char*)(buf+scenario_data_size+players_size+map_hexs_size));
	replace_filename(path2, path, (char*) (buf + scenario_data_size + players_size + map_hexs_size), sizeof(path2));
	int result = import_pg2_map_file(path2, convert_terrain_type, convert_roads_layer, generate_tiles, generate_road_tiles);
	if (result) {
		free(buf);
		fclose(inf);
		return result;
	}

	// scenario name
	//print_str(pg2_gln[*(uint16_t *)(buf + 1+2+1)]);

	setScenarioNameRaw(pg2_gln[*(uint16_t *)(buf + 1+2+1)]);
	if (pgf_mode || pacgen_mode) {
		//scenario name
		strncpy(block1_Name, pg2_gln[*(uint16_t *) (buf + 1 + 2 + 1)],256);
		//scenario description
		strncpy(block1_Description, "Fix me",1024);
	} else {
		//scenario name
		convert_from_cp1250_to_utf8(scn_short_description[getScenarioNumber() - 1], pg2_gln[*(uint16_t *) (buf + 1 + 2 + 1)], SHORT_SCN_SIZE);
		//scenario description
		convert_from_cp1250_to_utf8(scn_long_description[getScenarioNumber() - 1], "Fix me", LONG_SCN_SIZE);
	}

	//some scenario params
	//print_dec(*(BYTE *)(buf+4+3));
	scn_buffer[SCEN_LOCALE] = *(BYTE *) (buf + 4 + 3);
	scn_buffer[DAY] = *(WORD *) (buf + 4 + 3 + 1);
	scn_buffer[MONTH] = *(WORD *) (buf + 4 + 3 + 1 + 2);
	scn_buffer[YEAR] = *(WORD *) (buf + 4 + 3 + 1 + 2 + 2);
	scn_buffer[TURNS_PER_DAY] = *(WORD *) (buf + 4 + 3 + 1 + 2 + 2 + 8);
	scn_buffer[DAYS_PER_TURN] = 0;
	scn_buffer[TURNS] = 40; //?

	//sides, only first 2 players will be converted
	/*
	 for(i=0;i<4;i++){
	 print_dec(*(BYTE *)(buf+scenario_data_size+i));
	 print_dec(*(BYTE *)(buf+scenario_data_size+player_size+i));
	 print_dec(*(BYTE *)(buf+scenario_data_size+2*player_size+i));
	 print_dec(*(BYTE *)(buf+scenario_data_size+3*player_size+i));
	 }
	 */

	//for(i=0;i<40;i++){
	//print_dec(*(WORD *)(buf+scenario_data_size+97-80+i*2));
	//print_dec(*(WORD *)(buf+scenario_data_size+97-80+ player_size+i*2));
	//}
	//first player
	//sides
	scn_buffer[0] = pg2nation_to_pgnation[*(BYTE *) (buf + scenario_data_size)];
	scn_buffer[2] = pg2nation_to_pgnation[*(BYTE *) (buf + scenario_data_size + 1)];
	scn_buffer[4] = pg2nation_to_pgnation[*(BYTE *) (buf + scenario_data_size + 2)];
	scn_buffer[6] = pg2nation_to_pgnation[*(BYTE *) (buf + scenario_data_size + 3)];

	tmp_cash=*(WORD *)(buf+scenario_data_size+97-80);
	s4_buffer[AXIS_PRESTIGE]= tmp_cash&0xff;
	s4_buffer[AXIS_PRESTIGE+1]= tmp_cash>>8;

	scn_buffer[AXIS_STANCE] = *(BYTE *) (buf + scenario_data_size + 5);
	s4_buffer[AXIS_AIR_NUMBER] = *(BYTE *) (buf + scenario_data_size + 6);
	s4_buffer[AXIS_SEA_NUMBER] = *(BYTE *) (buf + scenario_data_size + 7);

	//second player
	//sides
	scn_buffer[0 + 1] = pg2nation_to_pgnation[*(BYTE *) (buf + scenario_data_size + player_size)];
	scn_buffer[2 + 1] = pg2nation_to_pgnation[*(BYTE *) (buf + scenario_data_size + 1 + player_size)];
	scn_buffer[4 + 1] = pg2nation_to_pgnation[*(BYTE *) (buf + scenario_data_size + 2 + player_size)];
	scn_buffer[6 + 1] = pg2nation_to_pgnation[*(BYTE *) (buf + scenario_data_size + 3 + player_size)];

	tmp_cash=*(WORD *)(buf+scenario_data_size+97-80+player_size);
	s4_buffer[ALLIED_PRESTIGE]= tmp_cash&0xff;
	s4_buffer[ALLIED_PRESTIGE+1]= tmp_cash>>8;

	scn_buffer[ALLIED_STANCE] = *(BYTE *) (buf + scenario_data_size + 5 + player_size);
	s4_buffer[ALLIED_AIR_NUMBER] = *(BYTE *) (buf + scenario_data_size + 6 + player_size);
	s4_buffer[ALLIED_SEA_NUMBER] = *(BYTE *) (buf + scenario_data_size + 7 + player_size);

	//map names etc
	// we will use only new names
	//total_names = STD_NAMES_PG;
	for (i = 0; i < MAX_NAMES; i++)
		pg2_gln_map[i] = -1;

	for (y = 0; y < PG2_MAP_Y; y++)
		for (x = 0; x < PG2_MAP_X; x++) {
			map_cursor = (struct pg2_map_hex_scn *) (buf + scenario_data_size + players_size + (x + y * PG2_MAP_X) * map_cursor_struct_size);

			//name
			current_pg2_name = map_cursor->name - 1;
			if (current_pg2_name > 0) {
				if (pg2_gln_map[current_pg2_name] == -1) {
					//print_dec(current_pg2_name);
					//we need to copy it, clipped to MAX_NAME_SIZE=20 | 40 !!!!
					strncpy(gln[total_names], pg2_gln[current_pg2_name], MAX_NAME_SIZE);
					strncpy(gln_utf8[total_names], pg2_gln[current_pg2_name], MAX_NAME_UTF_SIZE);
					pg2_gln_map[current_pg2_name] = total_names;
					total_names++;
					mapnames_changed = 1;

					//print_str(pg2_gln[current_pg2_name]);
				}
				map[x][y].gln = pg2_gln_map[current_pg2_name];

			} else {
				//use standard name
				map[x][y].gln = NData_Max_Tiles[map[x][y].tile];
			}
			//owner
			current_pg2_owner = pg2nation_to_pgnation[map_cursor->flag & 0x1F];
			map[x][y].own = current_pg2_owner;
			current_pg2_owner = map_cursor->flag >> 5; //side
			if (current_pg2_owner == 2)
				map[x][y].side = 1;
			//if (current_pg2_owner) print_dec(current_pg2_owner);
			//victory
			map[x][y].vic = map_cursor->victory & 0x7E ? 1 : 0;
			//deployment, only for player 1
			map[x][y].deploy = map_cursor->deployment & 0x04 ? 1 : 0;

		}
	//finally save vic hexes to scn_buffer
	get_victory_hexes();

	if (unit_conv_mode != PG2_UNITMODE_NOUNITS){
		//units
		int units_from_file_size = (file_size - (scenario_data_size + players_size + map_hexs_size + map_data_size)) / unit_size;
		int axis_units = *(WORD *) (buf + scenario_data_size + 8) + *(WORD *) (buf + scenario_data_size + 8 + 2);
		int allied_units = *(WORD *) (buf + scenario_data_size + 8 + player_size) + *(WORD *) (buf + scenario_data_size + 8 + player_size + 2);
		int max_units_to_read = units_from_file_size;
		int need_to_save = 0;
		/*
		 print_dec( units_from_file_size );
		 print_dec(axis_units);
		 print_dec(allied_units);
		 */
		if (units_from_file_size != axis_units + allied_units) {
			printf("WARNING: Wrong units number in file : player1=%d player2=%d file=%d\n", axis_units, allied_units, units_from_file_size);
			max_units_to_read = Min(units_from_file_size, axis_units + allied_units );
			printf("WARNING: Will read only first %d units\n", max_units_to_read);
		}

		for (i = 0; i < max_units_to_read; i++) {
			pg2_unit_cursor = (struct pg2_unit *) (buf + scenario_data_size + players_size + map_hexs_size + map_data_size + i * unit_size);

			if (unit_conv_mode == PG2_UNITMODE_CONVERT) {
				need_to_save+=convert_pg2_unit_to_pg_unit(pg2_unit_cursor,pg2_unit_cursor->unit_equ_id);

				if (pg2_unit_cursor->transport_type >0 ){
					need_to_save+=convert_pg2_unit_to_pg_unit(pg2_unit_cursor,pg2_unit_cursor->transport_type);
				}
				if (pg2_unit_cursor->unit_appearance >0 ){
					need_to_save+=convert_pg2_unit_to_pg_unit(pg2_unit_cursor,pg2_unit_cursor->unit_appearance);
				}
			}
			//we got converted unit ID here, maybe
			if (pg2unit_to_pgunit[pg2_unit_cursor->unit_equ_id]!=-1){
				//TODO check core vs aux
				int where_add_new = add_unit_type(pg2_unit_cursor->side, 0);
				total_units++;

				all_units[where_add_new].unum = pg2unit_to_pgunit[pg2_unit_cursor->unit_equ_id];
				//print_dec(where_add_new);

				all_units[where_add_new].country = pg2nation_to_pgnation[pg2_unit_cursor->flag];
				//all_units[where_add_new].orgtnum = tnum_selected;
				//all_units[where_add_new].auxtnum = anum_selected;

				//set proposed x,y
				all_units[where_add_new].x = pg2_unit_cursor->x;
				all_units[where_add_new].y = pg2_unit_cursor->y;
				//print_dec(all_units[where_add_new].x);
				//print_dec(all_units[where_add_new].y);

				all_units[where_add_new].str = pg2_unit_cursor->strength;
				all_units[where_add_new].entrench = pg2_unit_cursor->entrenchment;
				all_units[where_add_new].exp = pg2_unit_cursor->experience_level_indicator;

				all_units[where_add_new].orgtnum =0;
				if (pg2_unit_cursor->transport_type >0 && pg2unit_to_pgunit[pg2_unit_cursor->transport_type]!=-1)
					all_units[where_add_new].orgtnum =pg2unit_to_pgunit[pg2_unit_cursor->transport_type];
				all_units[where_add_new].auxtnum =0;
				if (pg2_unit_cursor->unit_appearance >0 && pg2unit_to_pgunit[pg2_unit_cursor->unit_appearance]!=-1)
					all_units[where_add_new].auxtnum =pg2unit_to_pgunit[pg2_unit_cursor->unit_appearance];
			}
		}

		for(y=0;y<mapy;y++)
			for(x=0;x<mapx;x++){
				map[x][y].auidx=-1;
				map[x][y].guidx=-1;
			}

		for(i=0;i<total_units;i++)
			if (equip[all_units[i].unum][GAF]) //1 if air unit
				map[all_units[i].x][all_units[i].y].auidx = i;
			else
				map[all_units[i].x][all_units[i].y].guidx = i;

		//save conversion table when needed
		if (need_to_save) {
				int result = save_pg2unit_to_pgunit();
				if (!result) {
					printf("'%s' file saved.\n", pg2u2pgu_file);
				}else{
					printf("ERROR saving '%s' file.\n", pg2u2pgu_file);
				}
			}
	}// !PG2_UNITMODE_NOUNITS

	/*
	print_dec(total_units);
	print_dec(total_allied_core);
	print_dec(total_allied_aux);
	print_dec(total_axis_core);
	print_dec(total_axis_aux);
	*/
	free(buf);
	fclose(inf);

	return 0;
}

int import_pg2_map_scn_gui() {

	int i, result=0;
	char path[MAX_PATH];
	char path2[MAX_PATH];
	char ext[MAX_EXT];
	char error_str[256];

#ifdef DEBUG_PW
	strncpy(path, "c:\\panzer2\\scenario\\", MAX_PATH);
#else
	strncpy(path, ".\\", MAX_PATH);
#endif
	canonicalize_filename_pg2(path, path, MAX_PATH);
	if (file_select_ex("Select MAP/SCN file", path, "map;scn;", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {

		strncpy(ext, get_extension(path), MAX_EXT);
		strlwr(ext);
		//print_str(ext);
		if (strcmp(ext, "scn") == 0) {
			//units
			for (i = 0; i < MAX_UNITS; i++) {
				pg2_equip_conversion[i].id = -1;
				pg2unit_to_pgunit[i] = -1;
			}

			//this must be after equ load
			load_pg2_conversion_tables();

			//txt
			replace_extension(path2, path, "txt", sizeof(path2));
			result = import_pg2_names_file(path2);

			centre_dialog(pg2_convert_dlg);
			if (do_dialog(pg2_convert_dlg, -1) == PG2_DLG_OK) {

				//if (result == 0) {
					clear_all_units();

					//in case of PG2_DLG_COPY_UNITS ratio is set
					if (pg2_convert_dlg[PG2_DLG_COPY_UNITS].flags & D_SELECTED){
						for (i = 0; i < MAX_UNITS; i++) {
							pg2unit_to_pgunit[i] = i;
						}
					}
					//in case of PG2_DLG_COPY_UNITS ratio is set
					if (pg2_convert_dlg[PG2_DLG_CONVERT_UNITS].flags & D_SELECTED){
						//try to load equ from the same as FPGE dir
						//equ
						printf("Trying to load '%s' file.\n", pg2_equip_name_file);
						result=import_pg2_equip_names_file(pg2_equip_name_file);
						if (!result) {
								printf("'%s' file loaded.\n", pg2_equip_name_file);
							}
						else{
							//error, cannot continue without pg2_equip_name_file
							sprintf(error_str, "Cannot load '%s' file !",pg2_equip_name_file);
							alert(error_str, NULL, NULL, "&Continue", NULL, 'c', 0);
							sprintf(MapStatusTxt, "Cannot load '%s' file !\nPick an Operation.",pg2_equip_name_file);
							d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
							return D_REDRAW;
						}
						printf("Trying to load '%s' file in conversion mode.\n", pg2_equip_file);
						result=import_pg2_equip(LOAD_CONVERSION_TABLE_ONLY, pg2_equip_file);
						if (!result) {
								printf("'%s' file loaded in conversion mode.\n", pg2_equip_file);
							}else{
								//error, cannot continue without pg2_equip_file
								sprintf(error_str, "Cannot load '%s' file !",pg2_equip_file);
								alert(error_str, NULL, NULL, "&Continue", NULL, 'c', 0);
								sprintf(MapStatusTxt, "Cannot load '%s' file !\nPick an Operation.",pg2_equip_file);
								d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
								return D_REDRAW;
							}
					}

					//scn
					result = import_pg2_scenario_file(path,
							pg2_convert_dlg[PG2_DLG_TERRAIN_TYPE].flags & D_SELECTED ? 1:0,
							pg2_convert_dlg[PG2_DLG_ROADS].flags & D_SELECTED ? 1:0,
							pg2_convert_dlg[PG2_DLG_GEN_TILES].flags & D_SELECTED ? 1:0,
							pg2_convert_dlg[PG2_DLG_GEN_ROAD_TILES].flags & D_SELECTED ? 1:0,
							pg2_convert_dlg[PG2_DLG_CONVERT_UNITS].flags & D_SELECTED ? PG2_UNITMODE_CONVERT:
							pg2_convert_dlg[PG2_DLG_COPY_UNITS].flags & D_SELECTED ? PG2_UNITMODE_COPY:PG2_UNITMODE_NOUNITS
					);
				//}
			}
		} else {
			//map

			centre_dialog(pg2_convert_map_dlg);
			if (do_dialog(pg2_convert_map_dlg, -1) == PG2_DLG_OK-3) {
				//PG2_DLG_TERRAIN_TYPE etc will work here
			result = import_pg2_map_file(path,
					pg2_convert_map_dlg[PG2_DLG_TERRAIN_TYPE].flags & D_SELECTED ? 1:0,
					pg2_convert_map_dlg[PG2_DLG_ROADS].flags & D_SELECTED ? 1:0,
					pg2_convert_map_dlg[PG2_DLG_GEN_TILES].flags & D_SELECTED ? 1:0,
					pg2_convert_map_dlg[PG2_DLG_GEN_ROAD_TILES].flags & D_SELECTED ? 1:0);
			if (result==0)
				clear_all_units();
			}

		}

		if (result == 0) {
			main_dlg[dmHSlideIdx].d1 = get_h_slide_max();
			main_dlg[dmHSlideIdx].d2 = 0;
			vslide_max = get_v_slide_max();
			main_dlg[dmVSlideIdx].d1 = vslide_max;
			main_dlg[dmVSlideIdx].d2 = vslide_max;
			//finally draw map
			draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
		}
	}
	return D_REDRAW;
}

int import_pg2_equip_dialog() {
	char path[MAX_PATH], path2[MAX_PATH];
	char error_str[256];

	sprintf(path, ".\\%s", pg2_equip_file);
	canonicalize_filename_pg2(path, path, MAX_PATH);
	sprintf(error_str, "Load PG2 '%s' file",pg2_equip_file);
	if (file_select_ex(error_str, path, "eqp", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {

		sprintf(MapStatusTxt, "Loading PG2 '%s' and '%s' files.\nPlease wait...",pg2_equip_file,pg2_equip_name_file);
		d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);

		//txt
		replace_filename(path2, path, pg2_equip_name_file, sizeof(path2));

		int error = import_pg2_equip_names_file(path2);
		if (error) {
			sprintf(error_str, "Cannot load '%s' file !",pg2_equip_name_file);
			alert(error_str, NULL, NULL, "&Continue", NULL, 'c', 0);
			sprintf(MapStatusTxt, "Cannot load '%s' file !\nPick an Operation.",pg2_equip_name_file);
			d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
			return D_REDRAW;
		}

		//equ
		error = import_pg2_equip(LOAD_FILE, path);
		if (error) {
			sprintf(error_str, "Cannot load '%s' file !",pg2_equip_file);
			alert(error_str, NULL, NULL, "&Continue", NULL, 'c', 0);
			sprintf(MapStatusTxt, "Cannot load '%s' file !\nPick an Operation.",pg2_equip_file);
			d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
			return D_REDRAW;
		} else {
			sprintf(MapStatusTxt, "'%s' loaded\n'%s' loaded\nPick an Operation.",pg2_equip_file,pg2_equip_name_file);
			d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);
		}
	}
	return D_REDRAW;
}

int save_pg2unit_to_pgunit() {
	int i;
	char line[1024], temp_str[1024];
	char path[MAX_PATH];
	FILE *outf;

	strncpy(path, pg2u2pgu_file,MAX_PATH);

	canonicalize_filename_pg2(path, path, MAX_PATH);
	outf = fopen(path, "wb");
	if (!outf) {
		printf("ERROR: Cannot save '%s' file.\n", pg2u2pgu_file);
		return D_O_K;
	}

	fwrite(&UCS2_header, sizeof(UCS2_header), 1, outf);
	fake_UTF_write_string_with_eol(outf, "# FPGE conversion file : PG2 unit ID to PG unit ID");
	strncpy(line, "#PG2 unit ID\tPG unit ID",1024);
	fake_UTF_write_string_with_eol(outf, line);

	for (i = 0; i < MAX_UNITS; i++) {
		if (pg2unit_to_pgunit[i] != -1) {
			strncpy(line, "",1024);

			sprintf(temp_str, "%d\t", i); //PG ID
			strncat(line, temp_str,1024);

			sprintf(temp_str, "%d", pg2unit_to_pgunit[i]); //PGZ ID
			strncat(line, temp_str,1024);

			sprintf(temp_str,"\t#%s->%s",pg2_equip_conversion[i].name, equip_name_utf8[pg2unit_to_pgunit[i]]); //names
			strncat(line,temp_str,1024);

			fake_UTF_write_string_with_eol(outf, line);
		}
	}
	fclose(outf);
	return D_O_K;
}

int load_pg2_to_pg_conversion_file(int probe_file_only, char *fname, int mode) {
	FILE *inf;
	char line[1024], tokens[50][256];
	int i, cursor = 0, token = 0, lines, count = 0;
	char path[MAX_PATH];
	int error_base = ERROR_PG2UNIT_TO_PGUNIT_BASE;

	if (mode == PG2_LOAD_TT2TRID) error_base=ERROR_PG2TT_TO_PGTR_BASE;
	if (mode == PG2_LOAD_TT2TTID) error_base=ERROR_PG2TT_TO_PGTT_BASE;
	if (mode == PG2_LOAD_C2CID)   error_base=ERROR_PG2COUNTRY_TO_PGCOUNTRY_BASE;
	if (mode == PG2_LOAD_CL2CLID) error_base=ERROR_PG2CLASS_TO_PGCLASS_BASE;

	strncpy(path, fname,MAX_PATH);
	canonicalize_filename_pg2(path, path, MAX_PATH);
	inf = fopen(path, "rb");
	if (!inf) {
		return error_base + ERROR_FPGE_FILE_NOT_FOUND;
	}
	if (probe_file_only == PROBE_FILE) {
		fclose(inf);
		return error_base + ERROR_FPGE_FILE_FOUND;
	}
	lines = 0;

	while (read_utf16_line_convert_to_utf8(inf, line) >= 0) {
		//count lines so error can be displayed with line number
		lines++;

		for (i = 0; i < strlen(line); i++)
			if (line[i] == 0x23) {
				line[i] = 0;
				break;
			}

		token = 0;
		cursor = 0;
		for (i = 0; i < strlen(line); i++)
			if (line[i] == 0x09) {
				tokens[token][cursor] = 0;
				token++;
				cursor = 0;
			} else {
				tokens[token][cursor] = line[i];
				cursor++;
			}
		tokens[token][cursor] = 0;
		token++;

		if (token >= 2) {
			int idx = atoi(tokens[0]);
			int value = atoi(tokens[1]);

			if (mode == PG2_LOAD_UNITSID)
				if (idx > -1 && idx < MAX_UNITS) {
					pg2unit_to_pgunit[idx] = value;
					count++;
				}
			if (mode == PG2_LOAD_TT2TRID)
				if (idx > -1 && idx < MAX_TERRAIN_TYPE) {
					pg2tt_to_pgtilesrange[idx] = value;
					count++;
				}
			if (mode == PG2_LOAD_TT2TTID)
				if (idx > -1 && idx < MAX_TERRAIN_TYPE) {
					pg2tt_to_pgtt[idx] = value;
					count++;
				}
			if (mode == PG2_LOAD_C2CID)
				if (idx > -1 && idx < MAX_COUNTRY) {
					pg2nation_to_pgnation[idx] = value;
					count++;
				}
			if (mode == PG2_LOAD_CL2CLID)
				if (idx > -1 && idx < MAX_PG2_CLASS_TYPE) {
					pg2class_to_pgclass[idx] = value;
					count++;
				}
		}
	}
	fclose(inf);

	printf("Read %d lines, %d values set.\n", lines, count);

	return 0;
}

void load_pg2_conversion_tables() {

	int result;

	printf("Trying to load '%s' file.\n", pg2u2pgu_file);
	result = load_pg2_to_pg_conversion_file(LOAD_FILE, pg2u2pgu_file, PG2_LOAD_UNITSID);
	if (!result) {
		printf("'%s' file loaded.\n", pg2u2pgu_file);
	}

	printf("Trying to load '%s' file.\n", pg2tt2pgtr_file);
	result = load_pg2_to_pg_conversion_file(LOAD_FILE, pg2tt2pgtr_file, PG2_LOAD_TT2TRID);
	if (!result) {
		printf("'%s' file loaded.\n", pg2tt2pgtr_file);
	}
	printf("Trying to load '%s' file.\n", pg2tt2pgtt_file);
	result = load_pg2_to_pg_conversion_file(LOAD_FILE, pg2tt2pgtt_file, PG2_LOAD_TT2TTID);
	if (!result) {
		printf("'%s' file loaded.\n", pg2tt2pgtt_file);
	}
	printf("Trying to load '%s' file.\n", pg2c2pgc_file);
	result = load_pg2_to_pg_conversion_file(LOAD_FILE, pg2c2pgc_file, PG2_LOAD_C2CID);
	if (!result) {
		printf("'%s' file loaded.\n", pg2c2pgc_file);
	}
	printf("Trying to load '%s' file.\n", pg2cl2pgcl_file);
	result = load_pg2_to_pg_conversion_file(LOAD_FILE, pg2cl2pgcl_file, PG2_LOAD_CL2CLID);
	if (!result) {
		printf("'%s' file loaded.\n", pg2cl2pgcl_file);
	}
}

int convert_txt_to_brf(char *path, int out_format){
	FILE *inf, *outf;
	char path2[1024], line[8*1024], outline[8*1024];

	if (out_format){
		//PZC
		replace_extension(path2,path,"pzbrf",1024);
	}else{
		//PGF
		replace_extension(path2,path,"pgbrf",1024);
	}

	canonicalize_filename_pg2(path, path, MAX_PATH);
	inf = fopen(path, "rt");
	if (inf == NULL) {
		printf("Cannot open PG2 briefing file : %s\n", path);
		return ERROR_PG2BRIEF_FILE_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}

	canonicalize_filename_pg2(path2, path2, MAX_PATH);
	outf = fopen(path2, "wb");
	if (outf == NULL) {
		printf("Cannot open briefing file for writing : %s\n", path2);
		fclose(inf);
		return ERROR_PZCBRIEF_FILE_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}

	if (out_format) {
		//PZC
		//header
		fwrite(&UCS2_header, sizeof(UCS2_header), 1, outf);
	}
	  while (fgets(line, sizeof(line), inf) != NULL) {

		if (line[strlen(line) - 1] == 0x0a)
			line[strlen(line) - 1] = 0;
		if (line[strlen(line) - 1] == 0x0d)
			line[strlen(line) - 1] = 0;

		if (strlen(line) > 0) {
			strncpy(outline, "<p>",8*1024);
			strncat(outline, line,8*1024);
			strncat(outline, "</p>",8*1024);

			if (out_format) {
				//PZC
				fake_UTF_write_string_with_eol(outf, outline);
			} else {
				//PGF
				strncat(outline, "\r\n",8*1024);
				fputs(outline, outf);
			}
		}
	}
	fclose(inf);
	fclose(outf);
	return 0;
}

int is_file_existing(char *path){
	//print_str(path);
	//print_dec(exists(path));
	return exists(path);
}

char *change_ext(char *in, char *ext){
	static char out_buff[1024];

	if (in==NULL || strlen(in)==0){
		//return empty, do not try to add ext
		out_buff[0]=0;
	}else{
		strncpy(out_buff,in,1024);
		if (strchr(out_buff,'.')!=NULL) *strchr(out_buff,'.')=0;
		strncat(out_buff,ext,1024);
	}
	return out_buff;
}

int parse_pg2_cam_file(char *path, int out_format){
	FILE *inf, *outf;
	uint16_t no_of_scenarios,i,j,k,start_cash;
	uint8_t flag;
	unsigned char buf[636];
	char path2[1024], line[1024], tmp[1024];
	char scenario_names[52][20];

	canonicalize_filename_pg2(path, path, MAX_PATH);
	inf = fopen(path, "rb");
	if (inf == NULL) {
		printf("Cannot open PG2 campaign file : %s\n", path);
		return ERROR_PG2CAM_FILE_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}

	if (out_format){
		//PZC
		strncpy(line,"campaign.pzdat.fpge",1024);
	}else{
		//PGF
		strncpy(line,pgf_pg_pgcam,1024);
		strncat(line,".fpge",1024);
	}

	replace_filename(path2,path,line,1024);
	canonicalize_filename_pg2(path2, path2, MAX_PATH);
	outf = fopen(path2, "wb");
	if (outf == NULL) {
		printf("Cannot open campaign file for writing : %s\n", path2);
		fclose(inf);
		return ERROR_PZCCAM_FILE_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}

	printf("Writing campaign file : %s\n", line);
	//header
	fwrite(&UCS2_header, sizeof(UCS2_header), 1, outf);

	memset(scenario_names,0,sizeof(scenario_names));
	strncpy(scenario_names[50],"END",20);
	strncpy(scenario_names[51],"END",20);

	fread(&no_of_scenarios,2,1,inf);
	fread(&start_cash,2,1,inf);
	//print_dec(no_of_scenarios);
	fseek(inf,20+20,SEEK_CUR);
	for(i=0; i<no_of_scenarios; i++){
		fread(buf,636,1,inf);
		//scenario file
		strlwr(&buf[8]);
		strncpy(scenario_names[i],&buf[8],20);
		//*strchr(scenario_names[i],'.')=0;
		replace_filename(path2,path,&buf[8],1024);
		is_file_existing(path2);

		//scenario intro
		strlwr(&buf[8+60]);
		replace_filename(path2,path,&buf[8+60],1024);
		if (is_file_existing(path2)){
			convert_txt_to_brf(path2, out_format);
		}
		//print_str(&buf[8+60]);
		//briliant
		strlwr(&buf[8+80+2+40]);
		replace_filename(path2,path,&buf[8+80+2+40],1024);
		if (is_file_existing(path2)){
			convert_txt_to_brf(path2, out_format);
		}
		//print_str(&buf[8+80+2+40]);
		//victory
		strlwr(&buf[8+80+2+80+2+40]);
		replace_filename(path2,path,&buf[8+80+2+80+2+40],1024);
		if (is_file_existing(path2)){
			convert_txt_to_brf(path2, out_format);
		}
		//print_str(&buf[8+80+2+80+2+40]);
		//tactictal victory
		strlwr(&buf[8+80+2+80+2+80+2+40]);
		replace_filename(path2,path,&buf[8+80+2+80+2+80+2+40],1024);
		if (is_file_existing(path2)){
			convert_txt_to_brf(path2, out_format);
		}
		//print_str(&buf[8+80+2+80+2+80+2+40]);
		//loose
		strlwr(&buf[8+80+2+80+2+80+2+80+2+40]);
		replace_filename(path2,path,&buf[8+80+2+80+2+80+2+80+2+40],1024);
		if (is_file_existing(path2)){
			convert_txt_to_brf(path2, out_format);
		}
		//print_str(&buf[8+80+2+80+2+80+2+80+2+40]);
	}
	//print_ldec(ftell(inf));

	//Comment
	if (out_format){
		//PZC
		strncpy(line,"# This file contains Panzer Corps campaign structure",1024);
	}else{
		//PGF
		strncpy(line,"# This is a PG Forever campaign description file",1024);
	}
	fake_UTF_write_string_with_eol(outf,line);
	strncpy(line,"# Converted from PG2 with FPGE",1024);
	fake_UTF_write_string_with_eol(outf,line);
	strncpy(line,"",1024);
	fake_UTF_write_string_with_eol(outf,line);

	//Format
	if (out_format) {
		//PZC
		strncpy(line, "FormatVersion",1024);
		strncat(line, "\t",1024);
		strncat(line, "1",1024);
	} else {
		//PGF
		strncpy(line, "# Format version",1024);
		fake_UTF_write_string_with_eol(outf,line);
		strncpy(line, "1",1024);
	}
	fake_UTF_write_string_with_eol(outf,line);
	strncpy(line,"",1024);
	fake_UTF_write_string_with_eol(outf,line);

	//
	strncpy(line,"# Entry points",1024);
	fake_UTF_write_string_with_eol(outf,line);
	if (out_format){
		//PZC
		strncpy(line,"# Name\tLabel in campaign path\tDescription\tSide\tNation\tStarting prestige\tDisplay name",1024);
	}else{
		//PGF
		strncpy(line,"# Name\tLabel in campaign path\tDescription\tSide\tNation\tFree elite replacements",1024);
	}
	fake_UTF_write_string_with_eol(outf,line);

	//label
	strncpy(line,change_ext(scenario_names[0],""),1024);
	strncat(line,"\t",1024);
	//start point
	if (out_format){
		//PZC
		strncat(line,"1",1024);
	}else{
		//PGF
		strncat(line,change_ext(scenario_names[0],""),1024);
	}

	strncat(line,"\t",1024);
	//description
	strncat(line,"IDS_CAMPAIGN_DESCRIPTION1",1024);
	strncat(line,"\t",1024);
	//side
	strncat(line,"0",1024);
	strncat(line,"\t",1024);
	//nation
	//start over
	fseek(inf,2+2+20+20+31800+20+78,SEEK_SET);
	fread(&flag,1,1,inf);
	//TODO flag
	strncat(line,"0",1024);
	strncat(line,"\t",1024);
	if (out_format) {
		//PZC
		//cash
		sprintf(tmp, "%d", start_cash);
		strncat(line, tmp,1024);
		strncat(line, "\t",1024);
		//name
		strncat(line, "IDS_CAMPAIGN_NAME1",1024);
		strncat(line, "\t",1024);
		//strncat(line,"\n",1024);
	} else {
		//PGF
		//Free elite replacements
		strncat(line, "1",1024);
	}
	fake_UTF_write_string_with_eol(outf,line);
	strncpy(line,"",1024);
	fake_UTF_write_string_with_eol(outf,line);

	//outcomes,scenarios in case of PGF
	if (!out_format) {
		//PGF
		//Outcomes
		strncpy(line,"# Outcomes",1024);
		fake_UTF_write_string_with_eol(outf,line);

		strncpy(line,"BRILLIANT VICTORY",1024);
		fake_UTF_write_string_with_eol(outf,line);

		strncpy(line,"VICTORY",1024);
		fake_UTF_write_string_with_eol(outf,line);

		strncpy(line,"TACTICAL VICTORY",1024);
		fake_UTF_write_string_with_eol(outf,line);

		strncpy(line,"LOSS",1024);
		fake_UTF_write_string_with_eol(outf,line);

		strncpy(line,"",1024);
		fake_UTF_write_string_with_eol(outf,line);

		//scenarios
		strncpy(line, "# Scenarios, EDIT VC !!!",1024);
		fake_UTF_write_string_with_eol(outf, line);
		strncpy(line, "# This table specifies victory condition for each outcome, except the last one. This is because the last outcome is used by default, if no other outcomes match.",1024);
		fake_UTF_write_string_with_eol(outf, line);
		strncpy(line, "#\t\tBrilliant Victory condition\t\t\tVictory condition\t\t\tMarginal condition",1024);
		fake_UTF_write_string_with_eol(outf, line);
		strncpy(line, "# Label\tScn File\tTurns remaining\tObjectives to hold\tMandatory objectives\tTurns remaining\tObjectives to hold\tMandatory objectives\tTurns remaining\tObjectives to hold\tMandatory objectives",1024);
		fake_UTF_write_string_with_eol(outf, line);

		fseek(inf,2+2+20+20,SEEK_SET);
		for(i=0;i<no_of_scenarios;i++){
			fread(buf,636,1,inf);
			//label
			strlwr(&buf[8]);
			strncpy(line,change_ext(&buf[8],""),1024);
			strncat(line,"\t",1024);
			//scenario file
			strncat(line,change_ext(&buf[8],".pgscn"),1024);
			strncat(line,"\t",1024);
			//Fake VC
			strncat(line,"3\t0\t\t2\t0\t\t1\t0\t\t",1024);

			fake_UTF_write_string_with_eol(outf,line);
		}

		strncpy(line,"",1024);
		fake_UTF_write_string_with_eol(outf,line);
	}

	//CAMPAIGN PATH
	strncpy(line,"# Campaign path",1024);
	fake_UTF_write_string_with_eol(outf,line);
	if (out_format){
		//PZC
		strncpy(line,"#\t\t\tDecisive Victory\t\t\tMarginal Victory\t\t\tLoss",1024);
		fake_UTF_write_string_with_eol(outf,line);
	}else{
		//PGF
		strncpy(line,"#\t\t\tBrilliant Victory\t\t\tVictory\t\t\tTactical Victory\t\t\tLoss",1024);
		fake_UTF_write_string_with_eol(outf,line);
	}
	strncpy(line,"# Label\tBriefing\tScenario\tNext label\tPrestige\tBriefing\tNext label\tPrestige\tBriefing\tNext label\tPrestige\tBriefing",1024);
	fake_UTF_write_string_with_eol(outf,line);

	//start over with reading file
	fseek(inf,2+2+20+20,SEEK_SET);
	for(i=0;i<no_of_scenarios;i++){
		fread(buf,636,1,inf);

		//label
		strlwr(&buf[8]);
		strncpy(line,change_ext(&buf[8],""),1024);
		strncat(line,"\t",1024);
		//briefing
		strlwr(&buf[8+60]);
		if (out_format) {
			//PZC
			strncat(line, change_ext(&buf[8 + 60], ".pzbrf"),1024);
		} else {
			//PGF
			strncat(line, change_ext(&buf[8 + 60], ".pgbrf"),1024);
		}
		strncat(line,"\t",1024);
		//scenario
		if (out_format){
			//PZC
			strncat(line,change_ext(scenario_names[i],".pzscn"),1024);
		}
		strncat(line,"\t",1024);

		for(k=0;k<4;k++){
			//Brilliant, victory, tactical, lose
			//next label
			j=buf[8+80+2+80+2+80+2+80+2+80+30*k];
			if (j==255) j=50; // loose
			if (j==254) j=51; // end
			strncat(line,change_ext(scenario_names[j],""),1024);
			strncat(line,"\t",1024);
			//prestige
			j=buf[8+80+82*k]+buf[8+80+1+82*k]*256;
			sprintf(tmp,"%d", j);
			strncat(line,tmp,1024);
			strncat(line,"\t",1024);
			//briefing
			strlwr(&buf[8 + 80 + 2 + 40 + 82 * k]);
			if (out_format) {
				//PZC
				strncat(line, change_ext(&buf[8 + 80 + 2 + 40 + 82 * k], ".pzbrf"),1024);
			} else {
				//PGF
				strncat(line, change_ext(&buf[8 + 80 + 2 + 40 + 82 * k], ".pgbrf"),1024);
			}
			strncat(line,"\t",1024);
		}
		//strncat(line,"\n",1024);
		fake_UTF_write_string_with_eol(outf,line);
	}
	fclose(inf);
	fclose(outf);
	return 0;
}

int convert_pg2_campaign_gui() {

	//int  result;
	char path[MAX_PATH];
	//char path2[MAX_PATH];
	//char ext[MAX_EXT];

	strncpy(path, ".\\", MAX_PATH);
	canonicalize_filename_pg2(path, path, MAX_PATH);
	if (file_select_ex("Select CAM file", path, "cam", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)) {

		centre_dialog(pg2_cam_dlg);
		if (do_dialog(pg2_cam_dlg, -1) == PG2_CAM_DLG_OK)
			parse_pg2_cam_file(path,pg2_cam_dlg[1].flags&D_SELECTED?0:1);

	}
	return D_REDRAW;
}
