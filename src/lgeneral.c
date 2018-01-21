/*
 * lgeneral.c
 *
 *  Created on: 2011-09-27
 *      Author: wino
 *      Based upon files : maps.c, units.c from LGeneral's lgc-pg dir
 */

//LGen : no neutral fields
//LGen : sea or air transport cannot transport organic transport (?)

#include <allegro.h>
#include <stdio.h>
#include <ctype.h>
#include "fpge.h"
#include "filename.h"
#include "fpge_colors.h"
#include "maingui.h"
#include "scenario_name.h"
#include "tables.h"
#include "lgeneral.h"

DIALOG lgen_uicons_dlg[LG_U_DLG] =
{
   /* (proc)        (x)  (y)  (w)  (h)  (fg)           (bg)        (key) (flags)     (d1) (d2)  (dp)                         (dp2) (dp3) */
   { d_textbox_proc, 0,  0,  228, 124+16+16+16, GUI_FG_COLOR, GUI_BG_COLOR,   0,    0,          0,   0,   NULL,                        NULL, NULL },
   { d_text_proc,    12, 12, 176, 8,   GUI_FG_COLOR, GUI_BG_COLOR,   0,    0,          0,   0,   "Export unit icons to LGen", NULL, NULL },
   { d_edit_proc,    72, 28, 96,  8,   GUI_FG_COLOR, GUI_EDIT_COLOR, 0,    0,         11,   1,   "pg",                        NULL, NULL },
   { d_text_proc,    12, 28, 56,  8,   GUI_FG_COLOR, GUI_BG_COLOR,   0,    0,          0,   0,   "Domain",                    NULL, NULL },
   { d_radio_proc,   12, 48, 148, 12,  GUI_FG_COLOR, GUI_BG_COLOR,   'f',    D_SELECTED, 0,   0,   "&Flip icons",                NULL, NULL },
   { d_radio_proc,   12, 64, 148, 12,  GUI_FG_COLOR, GUI_BG_COLOR,   'n',    0,          0,   0,   "&No icons flip",             NULL, NULL },
   { d_button_proc,  12, 92+16+32, 80,  20,  GUI_FG_COLOR, GUI_BG_COLOR,   'c',    D_EXIT,     0,   0,   "&Cancel",                    NULL, NULL },
   { d_button_proc, 132, 92+16+32, 80,  20,  GUI_FG_COLOR, GUI_BG_COLOR,   'e',    D_EXIT,     0,   0,   "&Export",                    NULL, NULL },
   { d_check_proc,   12, 64+16+4, 148+20, 12,  GUI_FG_COLOR, GUI_BG_COLOR, 's',    0,          1,   0,   "&Skip not used icons",       NULL, NULL },
   { d_check_proc,   12, 64+16+4+16, 148+20, 12,  GUI_FG_COLOR, GUI_BG_COLOR, 0,    0,          1,   0,   "Individual files",       NULL, NULL },
   { d_check_proc,   12, 64+16+4+16+16, 148+20, 12,  GUI_FG_COLOR, GUI_BG_COLOR, 'm',    0,          1,   0,   "&Multi column",       NULL, NULL },
   { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
   { NULL,           0,   0,   0,   0,   0,   0,   0,    0,          0,   0,   NULL,                        NULL, NULL }
};

DIALOG lgen_equip_dlg[LG_EQP_DLG] =
{
   /* (proc)        (x)  (y)  (w)  (h)  (fg)           (bg)        (key) (flags)     (d1) (d2)  (dp)                         (dp2) (dp3) */
   { d_textbox_proc, 0,  0,  228, 124, GUI_FG_COLOR, GUI_BG_COLOR,   0,    0,          0,   0,   NULL,                        NULL, NULL },
   { d_text_proc,    12, 12, 176, 8,   GUI_FG_COLOR, GUI_BG_COLOR,   0,    0,          0,   0,   "Export equipment to LGen", NULL, NULL },
   { d_edit_proc,    72, 28, 96,  8,   GUI_FG_COLOR, GUI_EDIT_COLOR, 0,    0,         11,   1,   "pg",                        NULL, NULL },
   { d_text_proc,    12, 28, 56,  8,   GUI_FG_COLOR, GUI_BG_COLOR,   0,    0,          0,   0,   "Domain",                    NULL, NULL },
   { d_check_proc,   12, 48, 148+20, 12,  GUI_FG_COLOR, GUI_BG_COLOR, 's',    0,          1,   0,   "&Skip not used icons",       NULL, NULL },
   { d_button_proc,  12, 92, 80,  20,  GUI_FG_COLOR, GUI_BG_COLOR,   'c',    D_EXIT,     0,   0,   "&Cancel",                    NULL, NULL },
   { d_button_proc, 132, 92, 80,  20,  GUI_FG_COLOR, GUI_BG_COLOR,   'e',    D_EXIT,     0,   0,   "&Export",                    NULL, NULL },
   { d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
   { NULL,           0,   0,   0,   0,   0,   0,   0,    0,          0,   0,   NULL,                        NULL, NULL }
};

int new_icon_number[MAX_UICONS];

char lgeneral_tile_type[] = {
    'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o',
    'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'c',
    'c', 'o', 'o', 'o', 'h', 'h', 'h', 'h', 'r', 'o', 'r',
    'c', 'c', 'c', 'c', 'r', 'c', 'c', 'r', 'r', 'o', 'c',
    'c', 'c', 'c', 'r', 'r', 'r', 'o', 'o', 'R', 'R', 'R',
    'R', 'r', 'r', 'r', 'r', 'r', 'R', 'R', 'R', 'R', 'R',
    'r', 'r', 'r', 'r', 'r', 'R', 'R', 'o', 'r', 'm', 'm',
    'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm',
    'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm',
    'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm',
    'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm',
    'm', 'm', 'm', 's', 't', 't', 't', 'a', 'c', 'c', '~',
    '~', 's', 's', 'f', 'f', 'f', 'f', 'c', '~', '~', '~',
    '~', 's', 'f', 'f', 'f', 'c', 'c', 'c', 'F', 'F', 'F',
    'r', 'r', 'r', '#', 'F', 'F', 'F', 'F', 'R', 'F', 'r',
    'R', 'r', '#', 'F', 'F', 'F', 'F', 'F', 'F', 'r', 'r',
    'r', '#', 'F', 'F', 'F', 'm', 'm', 'd', 'm', 'm', 'd',
    'm', 'm', 'm', 'd', 'm', 'm', 'm', 'd', 'd', 'm', 'm',
    'm', 'm', 'D', 'D', 'D', 'D', 'D', 't', 'F',

    'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm',
    'm', 'm', 'm', 'm', 'm', 'm',

    'r', 'r', 'r', 'r', 'R', 'R', 'R', 'c', 'h', 'h', 'd',
    'd', 'd', 'd'
};

/*
====================================================================
Panzer General Definitions.
====================================================================
*/
#define TARGET_TYPE_COUNT 4
char *target_types[] = {
    "soft",  "Soft",
    "hard",  "Hard",
    "air",   "Air",
    "naval", "Naval" };

enum {
    INFANTRY = 0,
    TANK, RECON, ANTI_TANK,
    ARTILLERY, ANTI_AIRCRAFT,
    AIR_DEFENSE, FORT, FIGHTER,
    TACBOMBER, LEVBOMBER, SUBMARINE,
    DESTROYER, CAPITAL, CARRIER,
    LAND_TRANS, AIR_TRANS, SEA_TRANS,
    UNIT_CLASS_COUNT
};
//17
#define UNIT_CLASSES_COUNT 17
char *unit_classes[] = {
    "inf",      "Infantry",         "infantry",
    "tank",     "Tank",             "low_entr_rate°tank",
    "recon",    "Recon",            "recon°tank",
    "antitank", "Anti-Tank",        "anti_tank",
    "art",      "Artillery",        "artillery°suppr_fire°attack_first",
    "antiair",  "Anti-Aircraft",    "low_entr_rate",
    "airdef",   "Air-Defense",      "air_defense°attack_first",
    "fort",     "Fortification",    "low_entr_rate°suppr_fire",
    "fighter",  "Fighter",          "interceptor°carrier_ok°flying",
    "tacbomb",  "Tactical Bomber",  "bomber°carrier_ok°flying",
    "levbomb",  "Level Bomber",     "flying°suppr_fire°turn_suppr",
    "sub",      "Submarine",        "swimming°diving",
    "dest",     "Destroyer",        "destroyer°swimming°suppr_fire",
    "cap",      "Capital Ship",     "swimming°suppr_fire",
    "carrier",  "Aircraft Carrier", "carrier°swimming",
    "landtrp",  "Land Transport",   "transporter",
    "airtrp",   "Air Transport",    "transporter°flying",
    "seatrp",   "Sea Transport",    "transporter°swimming",
};
#define MOVE_TYPE_COUNT 11
char *move_types[] = {
    "tracked",     "Tracked",       "pg/tracked.wav",
    "halftracked", "Halftracked",   "pg/tracked.wav",
    "wheeled",     "Wheeled",       "pg/wheeled.wav",
    "leg",         "Leg",           "pg/leg.wav",
    "towed",       "Towed",         "pg/leg.wav",
    "air",         "Air",           "pg/air.wav",
    "naval",       "Naval",         "pg/sea.wav",
    "allterrain",  "All Terrain",   "pg/wheeled.wav",
    //PGF movement types
    "amphitracked", "Amphibious Tracked",   "pg/tracked.wav",
    "amphiallterrain",  "Amphibious AllTerrain",   "pg/wheeled.wav",
    "mountain",  "Mountain",   "pg/leg.wav"
};



/*
====================================================================
Unit entries are saved to this struct.
====================================================================
*/
typedef struct {
    char name[20];
    int  class;
    int  atk_soft;
    int  atk_hard;
    int  atk_air;
    int  atk_naval;
    int  def_ground;
    int  def_air;
    int  def_close;
    int  target_type;
    int  aaf;        /* air attack flag */
    int  init;
    int  range;
    int  spot;
    int  agf;        /* air ground flag */
    int  move_type;
    int  move;
    int  fuel;
    int  ammo;
    int  cost;
    int  pic_id;
    int  month;
    int  year;
    int  last_year;
    int  nation;
    int  allowed_transport; //PW
} PG_UnitEntry;

#define NATION_COUNT 24
char *nations[] = {
    "aus", "Austria",       "0",
    "bel", "Belgia",        "1",
    "bul", "Bulgaria",      "2",
    "lux", "Luxemburg",     "3",
    "den", "Denmark",       "4",
    "fin", "Finnland",      "5",
    "fra", "France",        "6",
    "ger", "Germany",       "7",
    "gre", "Greece",        "8",
    "usa", "USA",           "9",
    "hun", "Hungary",       "10",
    "tur", "Turkey",        "11",
    "it",  "Italy",         "12",
    "net", "Netherlands",   "13",
    "nor", "Norway",        "14",
    "pol", "Poland",        "15",
    "por", "Portugal",      "16",
    "rum", "Rumania",       "17",
    "esp", "Spain",         "18",
    "so",  "Sovjetunion",   "19",
    "swe", "Sweden",        "20",
    "swi", "Switzerland",   "21",
    "eng", "Great Britain", "22",
    "yug", "Yugoslavia",    "23"
};

/*
====================================================================
Additional flags for special units.
====================================================================
*/
char *add_flags[] = {
    "6",   "jet",
    "7",   "jet",
    "9",   "jet",
    "47",  "ignore_entr",
    "108", "parachute",
    "109", "parachute",
    "168", "jet",
    "214", "bridge_eng°ignore_entr",
    "215", "parachute",
    "226", "parachute",
    "270", "parachute",
    "275", "bridge_eng°ignore_entr",
    "329", "bridge_eng°ignore_entr",
    "382", "parachute",
    "383", "parachute",
    "385", "ignore_entr",
    "386", "ignore_entr",
    "387", "bridge_eng°ignore_entr",
    "415", "parachute",
    "X"
};

char *getNotSupported(int count, int max){
	static char notSupportedBuf[256];

	snprintf(notSupportedBuf,256,"NotSupported%d",count-max);
	return notSupportedBuf;
}


void find_new_icon_numbers(){
	int i,bmp_idx,j=0;

	for(i=0;i<MAX_UICONS;i++) new_icon_number[i]=-1;

	for(i=0;i<total_equip;i++){
		bmp_idx = equip[i][BMP] + 256 * equip[i][BMP + 1];
		if (bmp_idx<MAX_UICONS && bmp_idx>-1 && new_icon_number[bmp_idx]==-1){
			new_icon_number[bmp_idx]=j;
			j++;
		}
	}
}
/*
====================================================================
Write unitclasses, target types, movement types to file.
====================================================================
*/
void units_write_classes( FILE *file )
{
    int i;
    fprintf( file, "<target_types\n" );
    for ( i = 0; i < TARGET_TYPE_COUNT; i++ )
        fprintf( file, "<%s\nname»%s\n>\n", target_types[i * 2], target_types[i * 2 + 1] );
    fprintf( file, ">\n" );
    fprintf( file, "<move_types\n" );
    for ( i = 0; i < MOVE_TYPE_COUNT; i++ )
        fprintf( file, "<%s\nname»%s\nsound»%s\n>\n",
                 move_types[i * 3], move_types[i * 3 + 1], move_types[i * 3 + 2] );
    fprintf( file, ">\n" );

	/* write unit classes, add purchase info */
	fprintf( file, "<unit_classes\n" );
	for ( i = 0; i < UNIT_CLASS_COUNT; i++ ) {
		fprintf( file, "<%s\n", unit_classes[i * 3] );
		fprintf( file, "name»%s\n", unit_classes[i * 3 + 1] );
		if (i == LAND_TRANS)
			fprintf( file, "purchase»trsp\n" );
		else if (i == FORT || i == SUBMARINE || i == DESTROYER ||
					i == CAPITAL || i == CARRIER ||
					i == AIR_TRANS || i == SEA_TRANS)
			fprintf( file, "purchase»none\n" );
		else
			fprintf( file, "purchase»normal\n" );
		fprintf( file, ">\n" );
	}
	fprintf( file, ">\n" );
}
/** Try to open file with lowercase name, then with uppercase name.
 * If both fails return NULL. Path itself is considered to be in
 * correct case, only the name after last '/' is modified. */
FILE *fopen_ic( const char *_path, const char *mode )
{
	FILE *file = NULL;
	char path[strlen(_path)+1], *start, *ptr;

	strcpy(path,_path); /* we need to copy since we modify it */

	/* start behind file separator */
	if ((start = strrchr(path,'/')) == NULL) /* Linux */
		start = strrchr(path,'\\'); /* Windows */
	if (start)
		start++;
	else
		start = path; /* only a file name */

	/* try all lower case */
	for (ptr = start; *ptr != 0; ptr++)
		*ptr = tolower(*ptr);
	if ((file = fopen(path,mode)))
		return file;

	/* try first upper case */
	start[0] = toupper(start[0]);
	if ((file = fopen(path,mode)))
		return file;

	/* try all upper case */
	for (ptr = start + 1; *ptr != 0; ptr++)
		*ptr = toupper(*ptr);
	if ((file = fopen(path,mode)))
		return file;

	return NULL;
}
/** Try to figure out nation from unit entry name. No field in PG
 * unit entry seems to hold the nation index. Bytes 30, 39, 40, 43
 * and 45 are just the same for all entries. 44 (what means ANI???)
 * seems to group certain units somehow according to class but it is
 * very jumpy and certainly not the nation id. Byte 49 varies
 * less but seems to have some other meaning, too. Pictures are also
 * not very sorted, so trying the unit name seems to be the easiest
 * approach for now:
 * Captions of non-german units start with 2-3 capital letters
 * indicating either the nation (GB, US, IT, ...) or allied usage
 * (AF,AD,...). Check for the "big" nation ids and map to
 * nation number. Generic allied units will be used by different
 * nations in scenarios but are not available for purchase (as it
 * seems that they equal some unit of the major nations).
 * Return index in global nations or -1 if no match. */
static int get_nation_from_unit_name( const char *uname )
{
	struct {
		const char *id; /* first term in name */
		int idx; /* index in global nations */
	} id_map[] = {
		{ "PO ", 15 },
		{ "GB ", 22 },
		{ "FR ", 6 },
		{ "NOR ", 14 },
		{ "LC ", 1 }, /* assign low country units to belgia */
		{ "ST ", 19 },
		{ "IT ", 12 },
		{ "Bulgarian ", 2 },
		{ "Hungarian ", 10 },
		{ "Rumanian ", 17 },
		{ "Greek ", 8 },
		{ "Yugoslav ", 23 },
		{ "US ", 9 },
        { "Finn", 5}, /* codes for Finnland ... */
        { "FIN", 5}, /* ... not used in pg but some other campaigns */
		{ "AF ", -1 },
		{ "AD ", -1 },
		{ "FFR ", 6 },
		{ "FPO ", 15 },
		{ "Katyusha ", 19 }, /* here ST is missing */
		{ "x", }
	};
	int nation_idx = 7; /* germany */
	int i = 0;

    /* only guess for PG unit database */
/*    if ( !apply_unit_mods )
        return -1;*/

	while (id_map[i].id[0] != 'x') {
		if (strncmp(id_map[i].id, uname, strlen(id_map[i].id)) == 0) {
			nation_idx = id_map[i].idx;
			break;
		}
		i++;
	}

	return nation_idx;
}
/*
====================================================================
Load PG unit entry from FPGE
 ====================================================================
*/
static int units_read_entry( int entry_numer, PG_UnitEntry *entry )
{

	if (entry_numer>total_equip) return 0;
	strncpy(entry->name,equip[entry_numer],MAX_NAME_SIZE);
	entry->class=equip[entry_numer][CLASS];
	entry->atk_soft=equip[entry_numer][SA];
	entry->atk_hard=equip[entry_numer][HA];
    entry->atk_air=equip[entry_numer][AA];
	entry->atk_naval=equip[entry_numer][NA];
	entry->def_ground=equip[entry_numer][GD];
	entry->def_air=equip[entry_numer][AD];
	entry->def_close=equip[entry_numer][CD];
	entry->target_type=equip[entry_numer][TARGET_TYPE];
	entry->aaf=equip[entry_numer][AAF];
	entry->init=equip[entry_numer][INITIATIVE];
	entry->range=equip[entry_numer][RANGE];
	entry->spot=equip[entry_numer][SPOTTING];
	entry->agf=equip[entry_numer][GAF];
	entry->move_type=equip[entry_numer][MOV_TYPE];
	entry->move=equip[entry_numer][MOV];
	entry->fuel=equip[entry_numer][FUEL];
	entry->ammo=equip[entry_numer][AMMO];
	entry->cost=equip[entry_numer][COST];
	entry->cost = (entry->cost * 120) / 10;
	entry->pic_id=equip[entry_numer][BMP]+256*equip[entry_numer][BMP+1];
	entry->month=equip[entry_numer][MON];
	entry->year=equip[entry_numer][YR];
	entry->last_year=equip[entry_numer][LAST_YEAR];

	entry->allowed_transport=equip[entry_numer][ALLOWED_TRANSPORT];

	if (equip_country[entry_numer]!= -1 )
		entry->nation = equip_country[entry_numer]-1;
	else
	{
		if (pg_mode)
			entry->nation = get_nation_from_unit_name( entry->name );
		else
			entry->nation = -1;
	}
    return 1;
}
/*
====================================================================
Replace a " with Inches and return the new string in buf.
====================================================================
*/
void string_replace_quote( char *source, char *buf )
{
    int i;
    int length = strlen( source );
    for ( i = 0; i < length; i++ )
        if ( source[i] == '"' ) {
            source[i] = 0;
            sprintf( buf, "%s Inches%s", source, source + i + 1 );
            return;
        }
    strcpy( buf, source );
}
/*
====================================================================
Fix spelling mistakes in unit names.
====================================================================
*/
static void fix_spelling_mistakes( char *name )
{
    /* replace "Jadg" with "Jagd" in unit name.
       Btw, this reminds me of a "Schinzel Wiener" in some Greek menu ;-) */
    char *pos = strstr( name, "Jadg" );
    if ( pos ) {
      printf("%s: ", name);
      memcpy( pos, "Jagd", 4 );
      printf("correct spelling to %s\n", name);
    }
}

/*
====================================================================
Convert unit database either as a new file for a campaign or
appended to single scenario.
'tac_icons' is the file name of the tactical icons.
====================================================================
*/
int units_convert_database(char *name, char *domain, int discard_not_used_icons)
{
	//int single_scen=0;

	//char tac_icons[] =  "pg.bmp";
	//char dest_path[MAX_PATH];
	//char source_path[MAX_PATH];
	//char target_name[128];
	int apply_unit_mods=0;

    int id = 0, ini_bonus, equ_idx;
    short entry_count;
    char path[MAX_PATH];
    char flags[512];
    char buf[256];
    //int i;
    PG_UnitEntry entry;
    FILE *source_file = 0, *dest_file = 0;

    printf( "Unit data base...\n" );

    if (discard_not_used_icons) {
    	find_new_icon_numbers();
    }

    //if (pg_mode) //quick and dirty
    //	apply_unit_mods=1;

    /* open dest file */

        snprintf( path, MAX_PATH, "%s", name );

    if ( ( dest_file = fopen( path, "w" ) ) == 0 ) {
        fprintf( stderr, "%s: write access denied\n", path );
        goto failure;
    }

    entry_count = total_equip-1;

    fprintf( dest_file, "@\n" ); /* only a new file needs this magic */
    /* domain */
    fprintf( dest_file, "domain»%s\n",domain );
    fprintf( dest_file, "icons»%s\nicon_type»%s\n", name,apply_unit_mods?"single":"fixed"); //?
    fprintf( dest_file, "strength_icons»%s_strength.bmp\n",domain );
    fprintf( dest_file, "strength_icon_width»16\nstrength_icon_height»12\n" );
    fprintf( dest_file, "attack_icon»%s_attack.bmp\n" ,domain);
    fprintf( dest_file, "move_icon»%s_move.bmp\n",domain );
    fprintf( dest_file, "guard_icon»%s_guard.bmp\n",domain );
    units_write_classes( dest_file );
    fprintf( dest_file, "<unit_lib\n" );
    /* first entry is RESERVED */
    memset( &entry, 0, sizeof( PG_UnitEntry ) );

    /* convert */
    while ( entry_count-- > 0 ) {

        memset( &entry, 0, sizeof( PG_UnitEntry ) );
        equ_idx= total_equip-entry_count;
        //print_dec(equ_idx);
        if ( !units_read_entry(  equ_idx, &entry ) ) {
            fprintf( stderr, "%s: unexpected end of file\n", path );
            goto failure;
        }
        /* sometimes a unit class seems to be screwed */
        if ( entry.class >= UNIT_CLASS_COUNT )
            entry.class = 0;
        /* adjust attack values according to unit class (add - for defense only) */
        switch ( entry.class ) {
            case INFANTRY:
            case TANK:
            case RECON:
            case ANTI_TANK:
            case ARTILLERY:
	        case FORT:
            case SUBMARINE:
            case DESTROYER:
            case CAPITAL:
	        case CARRIER:
                entry.atk_air = -entry.atk_air;
                break;
            case AIR_DEFENSE:
                entry.atk_soft = -entry.atk_soft;
                entry.atk_hard = -entry.atk_hard;
                entry.atk_naval = -entry.atk_naval;
                break;
            case TACBOMBER:
            case LEVBOMBER:
                if ( entry.aaf )
                    entry.atk_air = -entry.atk_air;
                break;
        }
        /* all russian tanks get an initiative bonus */
        if ( apply_unit_mods ) {
            ini_bonus = 2;
            if ( entry.class == 1 && strncmp( entry.name, "ST ", 3 ) == 0 )
            {
                entry.init += ini_bonus;
                printf( "%s: initiative bonus +%i\n", entry.name, ini_bonus );
            }
        }
        /* get flags */
        sprintf( flags, unit_classes[entry.class * 3 + 2] );

        if (equip_flags[equ_idx]&EQUIPMENT_JET) strcat( flags, "°jet" );
        if (equip_flags[equ_idx]&EQUIPMENT_CAN_BRIDGE_RIVERS) strcat( flags, "°bridge_eng" );
        if (equip_flags[equ_idx]&EQUIPMENT_IGNORES_ENTRENCHMENT) strcat( flags, "°ignore_entr" );

	/* whatever is legged or towed may use ground/air transporter */
        //PW
        /*
        if ( entry.move > 0 && (entry.move_type == 3 || entry.move_type == 4) ){
        	if (entry.allowed_transport >2 ) strcat( flags, "°parachute" );
        	if (entry.allowed_transport >1 ) strcat( flags, "°air_trsp_ok" );
        	if (entry.allowed_transport >0 ) strcat( flags, "°ground_trsp_ok" );

        }
        */
        //AGW version
		if ( entry.move < 5 && (entry.move_type == 3 || entry.move_type == 4 || entry.move_type == 10) )
			strcat( flags, "°ground_trsp_ok" );
		if ( entry.move < 5 && (entry.move_type == 3 || entry.move_type == 10) ){
			strcat( flags, "°air_trsp_ok" );
			if (entry.allowed_transport >2 ) strcat( flags, "°parachute" );
		}
		if ( entry.move > 1 && (entry.move_type == 4) )
			strcat( flags, "°air_trsp_ok" );

        /* all artillery with range 1 has no attack_first */
        if (entry.class==4 && entry.range==1)
        {
            sprintf( flags, "artillery°suppr_fire" );
            printf( "%s: overwrite flags to: artillery,suppr_fire\n",
                                                                entry.name);
        }

        /* write entry */
        fprintf( dest_file, "<%i\n", id++ );
        string_replace_quote( entry.name, buf );
        if ( apply_unit_mods )
            fix_spelling_mistakes( buf );
        //print_str(buf);
        fprintf( dest_file, "name»%s\n", buf );
        fprintf( dest_file, "nation»%s\n", (entry.nation==-1)?"none":nations[entry.nation * 3] );
        fprintf( dest_file, "class»%s\n", entry.class<UNIT_CLASSES_COUNT?unit_classes[entry.class * 3]:getNotSupported(entry.class,UNIT_CLASSES_COUNT));
        fprintf( dest_file, "target_type»%s\n", entry.target_type<TARGET_TYPE_COUNT?target_types[entry.target_type * 2]:getNotSupported(entry.target_type,TARGET_TYPE_COUNT));
        fprintf( dest_file, "initiative»%i\n", entry.init );
        fprintf( dest_file, "spotting»%i\n", entry.spot );
        fprintf( dest_file, "movement»%i\n", entry.move );
        fprintf( dest_file, "move_type»%s\n", entry.move_type<MOVE_TYPE_COUNT?move_types[entry.move_type * 3]:getNotSupported(entry.move_type,MOVE_TYPE_COUNT));
        fprintf( dest_file, "fuel»%i\n", entry.fuel );
        fprintf( dest_file, "range»%i\n", entry.range );
        fprintf( dest_file, "ammo»%i\n", entry.ammo );
        fprintf( dest_file, "<attack\ncount»1\nsoft»%i\nhard»%i\nair»%i\nnaval»%i\n>\n",
                 entry.atk_soft, entry.atk_hard, entry.atk_air, entry.atk_naval );
        fprintf( dest_file, "def_ground»%i\n", entry.def_ground );
        fprintf( dest_file, "def_air»%i\n", entry.def_air );
        fprintf( dest_file, "def_close»%i\n", entry.def_close );
        fprintf( dest_file, "flags»%s\n", flags );
        if (discard_not_used_icons){
        	fprintf( dest_file, "icon_id»%i\n", new_icon_number[entry.pic_id] );
        }else{
        	fprintf( dest_file, "icon_id»%i\n", entry.pic_id );
        }
        if ( strstr( flags, "°jet" ) )
            fprintf( dest_file, "move_sound»%s\n", "pg/air2.wav" );
        fprintf( dest_file, "cost»%i\n", entry.cost );
        fprintf( dest_file, "start_year»19%i\n", entry.year );
        fprintf( dest_file, "start_month»%i\n", entry.month );
        fprintf( dest_file, "last_year»19%i\n", entry.last_year );
        fprintf( dest_file, ">\n" );
    }
    fprintf( dest_file, ">\n" );
    fclose( source_file );
    fclose( dest_file );
    return 1;
failure:
    if ( source_file ) fclose( source_file );
    if ( dest_file ) fclose( dest_file );
    return 0;
}

int save_lgeneral_equ(){
	char buf[128]="pg";
	char ext[128]=".udb";
	char name[128]="pg.udb";

	sprintf(MapStatusTxt,"LGen udb export.");
	d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);

	lgen_equip_dlg[2].dp=buf;
	centre_dialog(lgen_equip_dlg);
	if (do_dialog(lgen_equip_dlg, -1)==6){
		strncpy(name,buf,128);
		strncat(name,ext,128);

		int change_icons = (lgen_equip_dlg[4].flags&D_SELECTED)?1:0;

		units_convert_database(name,buf,change_icons);

		snprintf(MapStatusTxt,256,"LGen %s saved.\nPick an Operation.",name);
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	}else{
		pick_msg();
	}
	return D_REDRAW;
}

int save_lgeneral_map_file(char *name, char *domain){
	int x,y,i,j;
	char type;
	FILE *fout;

	fout=fopen(name,"wt");
	if (fout==NULL){
		return 1;
	}
	fprintf(fout,"@\n");
	fprintf(fout,"terrain_db»%s.tdb\n", domain );
	fprintf(fout,"domain»pg\n" );
	fprintf(fout,"width»%i\nheight»%i\n", mapx, mapy );
	fprintf(fout, "tiles»" );

    for ( y = 0; y < mapy; y++ ) {
        for ( x = 0; x < mapx; x++ ) {
        	if (map[x][y].tile < MAX_TILES_IN_PG){
				type = lgeneral_tile_type[map[x][y].tile];
				j=0;
				for ( i = 0; i < map[x][y].tile; i++ )
					if ( lgeneral_tile_type[i] == type )
						j++;
        	}else{
        		type='c';
        		j=12;
        	}

            fprintf( fout, "%c%d", type, j );
            if ( y < mapy - 1 || x < mapx - 1 )
                fprintf( fout, "°" );
        }
    }
    fprintf( fout, "\n" );
    fprintf( fout, "names»" );
    for ( y = 0; y < mapy; y++ ) {
        for ( x = 0; x < mapx; x++ ) {
        	if (map[x][y].gln>total_names || map[x][y].gln<0)
        		fprintf( fout, "none");
        	else
        		fprintf( fout, "%s", gln[map[x][y].gln]  );
            if ( y < mapy - 1 || x < mapx - 1 )
                fprintf( fout, "°" );
        }
    }    fprintf( fout, "\n" );
	fclose(fout);
	return 0;
}
/*
====================================================================
Random generator
====================================================================
*/
int seed;
void random_seed( int _seed )
{
    seed = _seed;
}
int random_get( int low, int high )
{
    int p1 = 1103515245;
    int p2 = 12345;
    seed = ( seed * p1 + p2 ) % 2147483647;
    return ( ( abs( seed ) / 3 ) % ( high - low + 1 ) ) + low;
}
/*
====================================================================
Panzer General offers two values: weather condition and weather
region which are used to determine the weather throughout the
scenario. As the historical battle did only occur once the weather
may not change from game to game so we compute the weather of a
scenario depending on three values:
inital condition, weather region, month
Initial Condition:
    1  clear
    0  rain/snow
Regions:
    0  Desert
    1  Mediterranean
    2  Northern Europe
    3  Eastern Europe
This routine does only use Fair(Dry), Overcast(Dry), Rain(Mud) and
Snow(Ice), so there is no delay between ground and air conditions.
====================================================================
*/
void scen_create_random_weather( FILE *dest_file, int init_cond ,int region, int month, int turns )
{
    float month_mod[13] = { 0, 1.7, 1.6, 1.0, 2.0, 1.2, 0.7, 0.5, 0.6, 1.4, 1.7, 2.2, 1.7 };
    int med_weather[4] = { 0, 16, 24, 36 };
    int bad_weather[4] = { 0, 8, 12, 18 };
    int i, result;

    int weather[turns];
    memset( weather, 0, sizeof( int ) * turns );
    /* get condition and region */

    /* compute the weather */
    random_seed( month * turns + ( region + 1 ) * ( init_cond + 1 ) );
    for ( i = 0; i < turns; i++ ) {
        result = random_get( 1, 100 );
        if ( result <= (int)( month_mod[month] * bad_weather[region] ) )
            weather[i] = 2;
        else
            if ( result <= (int)( month_mod[month] * med_weather[region] ) )
                weather[i] = 1;
    }

    /* initial condition */
    weather[0] = (init_cond==1)?0:2;
    /* from december to february turn 2 (rain) into 3 (snow) */
    if ( month < 3 || month == 12 ) {
        for ( i = 0; i < turns; i++ )
            if ( weather[i] == 2 )
                weather[i]++;
    }

    /* write weather */
    fprintf( dest_file, "weather»" );
    i = 0;
    while ( i < turns ) {
        fprintf( dest_file, "%s", weather[i]==0?"fair":weather[i]==1?"clouds":weather[i]==2?"rain":"snow" );
        if ( i < turns - 1 )
            fprintf( dest_file, "°" );
        i++;
    }
    fprintf( dest_file, "\n" );
}

/** Write prestige per turn list to @file. */
int write_prestige_info( FILE *file, int turns, int *ppt )
{
	int i;

	fprintf( file, "prestige»" );
	for (i = 0; i < turns; i++)
		fprintf( file, "%d%c", ppt[i],
			(i < turns - 1) ? '°' : '\n');
	return 0;
}

int save_lgeneral_scenario_file(char *name, char *map_name, char *domain){
	int x,y;
	int i,j;
	//char type;
	FILE *dest_file;
	char line_buffer[128],dummy[128];

	dest_file=fopen(name,"wt");
	if (dest_file==NULL){
		return 1;
	}
	/* file magic */
	fprintf(dest_file,"@\n");

    /* scenario name and description */
	GetScenarioShortName(line_buffer,128);
    fprintf( dest_file, "name»%s\n", line_buffer );
    GetScenarioDescription(line_buffer,128);
    fprintf( dest_file, "desc»%s\n", line_buffer );
    snprintf(line_buffer,128,"unknow");
    fprintf( dest_file, "authors»%s\n", line_buffer );
    fprintf( dest_file, "date»%02i.%02i.19%i\n", scn_buffer[DAY]&255, scn_buffer[MONTH]&255, scn_buffer[YEAR]&255 );

    /* turn limit */
    int turns = scn_buffer[TURNS]&255;
    fprintf( dest_file, "turns»%i\n", turns );
    int turns_per_day = scn_buffer[TURNS_PER_DAY]&255;
    fprintf( dest_file, "turns_per_day»%i\n", turns_per_day );
    int days_per_turn = scn_buffer[DAYS_PER_TURN]&255;
    if ( turns_per_day == 0 && days_per_turn == 0 )
        days_per_turn = 1;
    fprintf( dest_file, "days_per_turn»%i\n", days_per_turn );

    /* domain */
    fprintf( dest_file, "domain»%s\n",domain );
    /* nations */
    fprintf( dest_file, "nation_db»%s.ndb\n",domain );
    /* units */
    fprintf( dest_file, "<unit_db\nmain»%s.udb\n>\n", domain);
    /* map */
    fprintf( dest_file, "map»%s/%s\n",domain,map_name );
    /* weather */
    scen_create_random_weather( dest_file, scn_buffer[STORM_FRONT]&255, scn_buffer[SCEN_LOCALE]&255, scn_buffer[MONTH]&255, scn_buffer[TURNS]&255 );
    /* flags */
    fprintf( dest_file, "<flags\n" );
    for (y = 0; y < mapy; ++y)
    	for (x = 0; x < mapx; ++x)
		{
			if (map[x][y].own) {
				fprintf( dest_file, "<flag\nx»%i\ny»%i\nnation»%s\nobj»%i\n>\n", x, y, nations[(map[x][y].own - 1) * 3], map[x][y].vic );
			}
		}
    fprintf( dest_file, ">\n" );

    /* players */
            fprintf( dest_file, "<players\n" );
            /* axis */

            if ( (scn_buffer[ORIENTATION]&255) == 1 )
                snprintf( dummy,128, "right" );
            else
                snprintf( dummy,128, "left" );

            /* strategy: -2 (very defensive) to 2 (very aggressive) */

            int axis_strat = scn_buffer[ALLIED_STANCE]&255; //yes ALLIED_STANCE here, was fseek( scen_file, 15, SEEK_SET );
            if ( axis_strat == 0 )
                axis_strat = 1;
            else
                axis_strat = -1;
            /* definition */

                fprintf( dest_file, "<axis\nname»Axis\n" );

                char auxstr[256];
                memset(auxstr,0,256);

                j=0;
                for(i=0;i<6;i++){
                	if (scn_buffer[i*2]){
                		j=1;
                		strncat(auxstr,nations[(scn_buffer[i*2]-1)*3],256);
                		strncat(auxstr,"°",256);
                	}
                }
                if (j) auxstr[strlen(auxstr)-1]=0; //trim last °

                fprintf( dest_file, "nations»%s\n", auxstr );

            fprintf( dest_file, "allied_players»\n" );
            int axis_ulimit = (scn_buffer[AUX_UNITS]&255)+(scn_buffer[CORE_UNITS]&255);
            fprintf( dest_file, "unit_limit»%d\n", axis_ulimit );
            //fprintf( dest_file, "core_limit»%d\n", scn_buffer[CORE_UNITS]&255 );
            fprintf( dest_file, "orientation»%s\ncontrol»human\nstrategy»%i\n", dummy, axis_strat );


    	{
    		int ppt[turns]; /* prestige per turn with dynamic size */
    		memset(ppt,0,turns * sizeof(int)); //zero
    		ppt[0]=get_id_from_2_bytes(AXIS_PRESTIGE);
    		write_prestige_info( dest_file, turns, ppt );
    	}
            fprintf( dest_file, "ai_module»default\n" );
            /* transporter */
            fprintf( dest_file, "<transporters\n" );
            /* air */
            i=get_id_from_2_bytes(AXIS_AIR_TYPE);
            if (i)
                fprintf( dest_file, "<air\nunit»%i\ncount»%i\n>\n", i - 1, s4_buffer[AXIS_AIR_NUMBER] ); //id-1, RESERVED unit is removed
            /* sea */
            i=get_id_from_2_bytes(AXIS_SEA_TYPE);
            fprintf( dest_file, "<sea\nunit»%i\ncount»%i\n>\n", i - 1, s4_buffer[AXIS_SEA_NUMBER] );
            fprintf( dest_file, ">\n" ); //trans
            fprintf( dest_file, ">\n" );//player axis


            /* allies */
            if ( (scn_buffer[ORIENTATION]&255) != 1 )
                snprintf( dummy,128, "right" );
            else
                snprintf( dummy,128, "left" );

            int allied_strat;
            if ( axis_strat == 1 )
                allied_strat = -1;
            else
                allied_strat = 1;

            fprintf( dest_file, "<allies\nname»Allies\n" );
            memset(auxstr,0,256);

            j=0;
            for(i=0;i<6;i++){
            	if (scn_buffer[i*2+1]){
            		j=1;
            		strncat(auxstr,nations[(scn_buffer[i*2+1]-1)*3],256);
            		strncat(auxstr,"°",256);
            	}
            }
            if (j) auxstr[strlen(auxstr)-1]=0; //trim last °

            fprintf( dest_file, "nations»%s\n", auxstr );


            fprintf( dest_file, "allied_players»\n" );
            int allies_ulimit = (scn_buffer[AUX_ALLIED_UNITS]&255)+(scn_buffer[ALLIED_UNITS]&255);
            fprintf( dest_file, "unit_limit»%d\n", allies_ulimit );
            fprintf( dest_file, "orientation»%s\ncontrol»cpu\nstrategy»%i\n", dummy, allied_strat );
    	{
        		int ppt[turns]; /* prestige per turn with dynamic size */
        		memset(ppt,0,turns * sizeof(int)); //zero
        		ppt[0]=get_id_from_2_bytes(ALLIED_PRESTIGE);
        		write_prestige_info( dest_file, turns, ppt );
    	}
            fprintf( dest_file, "ai_module»default\n" );

            /* transporter */
			fprintf( dest_file, "<transporters\n" );
			/* air */
			i=get_id_from_2_bytes(ALLIED_AIR_TYPE);
			if (i)
				fprintf( dest_file, "<air\nunit»%i\ncount»%i\n>\n", i - 1, s4_buffer[ALLIED_AIR_NUMBER] ); //id-1, RESERVED unit is removed
			/* sea */
			i=get_id_from_2_bytes(ALLIED_SEA_TYPE);
			fprintf( dest_file, "<sea\nunit»%i\ncount»%i\n>\n", i - 1, s4_buffer[ALLIED_SEA_NUMBER] );
            fprintf( dest_file, ">\n" );//tr
            fprintf( dest_file, ">\n" );//allied
            fprintf( dest_file, ">\n" );//players

            /* the default is that the attacker must capture
                all targets */
             fprintf( dest_file, "<result\ncheck»every_turn\n" );
             fprintf( dest_file, "<cond\n" );
             fprintf( dest_file, "<and\n<control_all_hexes\nplayer»%s\n>\n>\n",
                      (axis_strat > 0) ? "axis" : "allies" );
             fprintf( dest_file, "result»victory\n" );
             fprintf( dest_file, "message»%s\n",
                      (axis_strat > 0) ? "Axis Victory" : "Allied Victory" );
             fprintf( dest_file, ">\n" );
             fprintf( dest_file, "<else\n" );
             fprintf( dest_file, "result»defeat\n" );
             fprintf( dest_file, "message»%s\n",
                      (axis_strat > 0) ? "Axis Defeat" : "Allied Defeat" );
             fprintf( dest_file, ">\n" );
             fprintf( dest_file, ">\n" );

             /* deployment fields */
              fprintf( dest_file, "<deployfields\n<player\nid»axis\ncoordinates»default°" );
              j=0;
              for (y = 0; y < mapy; ++y)
              	for (x = 0; x < mapx; ++x)
          		{
          			if (map[x][y].deploy) {
          				fprintf( dest_file, "%s%d,%d", j ? "°" : "", x, y );
          				j++;
          			}
          		}
              fprintf( dest_file, "\n>\n" );
              fprintf( dest_file, "<player\nid»allies\ncoordinates»default\n>\n" );
              fprintf( dest_file, ">\n" );

              /* units */
              fprintf( dest_file, "<units\n" );
              for ( i = 0; i < total_units; i++ ){
            	    /* write unit */
            	    fprintf( dest_file, "<unit\n" );
            	    fprintf( dest_file, "id»%i\nnation»%s\n", all_units[i].unum-1, nations[(all_units[i].country-1) * 3] );
            	    fprintf( dest_file, "x»%i\ny»%i\n", all_units[i].x, all_units[i].y );
            	    fprintf( dest_file, "str»%i\nentr»%i\nexp»%i\n", all_units[i].str, all_units[i].entrench, all_units[i].exp );
            	    if ( all_units[i].auxtnum == 0 && all_units[i].orgtnum == 0 )
            	        fprintf( dest_file, "trsp»none\n" );
            	    else {
            	        if ( all_units[i].auxtnum )
            	            fprintf( dest_file, "trsp»%i\n", all_units[i].auxtnum - 1 );
            	        else
            	            fprintf( dest_file, "trsp»%i\n", all_units[i].orgtnum - 1 );
            	    }
            	    fprintf( dest_file, ">\n" );
              }


              fprintf( dest_file, ">\n" );

	fclose(dest_file);
	return 0;
}

int save_lgeneral_map(){
	char map_name[128];

	sprintf(MapStatusTxt,"LGen MAP saving...\nPlease wait.");
	d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);

	snprintf(map_name,128,"map%02d",getScenarioNumber());
	save_lgeneral_map_file(map_name,"pg");

	sprintf(MapStatusTxt,"LGen MAP saved.\nPick an Operation.");
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;

	return D_O_K;
}

int save_lgeneral_scenario(){
	return save_lgeneral_scenario_CLI(TRUE);
}

int save_lgeneral_scenario_CLI(int updateGUI){
	char name[128],map_name[128];

	sprintf(MapStatusTxt,"LGen MAP saving...\nPlease wait.");
	if (updateGUI) d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);

	snprintf(map_name,128,"map%02d",getScenarioNumber());
	save_lgeneral_map_file(map_name,"pg");

	sprintf(MapStatusTxt,"LGen scen saving...\nPlease wait.");
	if (updateGUI) d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);

	GetScenarioShortName(name,128);

	save_lgeneral_scenario_file(name,map_name,"pg");

	if (updateGUI){
		sprintf(MapStatusTxt,"LGen scen saved.\nPick an Operation.");
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	}
	return D_O_K;
}

int check_icon_true_size_bottom(int icon_idx) {
	int x, y;
	int bottom = 0;
	//bottom
	for (y = TILE_HEIGHT - 1; y >= 0; --y)
		for (x = 0; x < TILE_FULL_WIDTH; ++x)
			if (getpixel(unit_bmp[icon_idx], x, y) != fpge_mask_color) {
				bottom = y;
				return bottom;
			}
	return bottom;
}

int check_icon_true_size_top(int icon_idx) {
	int x, y;
	int top = TILE_HEIGHT - 1;
	//top
	for (y = 0; y < TILE_HEIGHT; ++y)
		for (x = 0; x < TILE_FULL_WIDTH; ++x)
			if (getpixel(unit_bmp[icon_idx], x, y) != fpge_mask_color) {
				top = y;
				return top;
			}
	return top;
}

int check_icon_true_size_left(int icon_idx) {
	int x, y;
	int left = TILE_FULL_WIDTH - 1;
	//left
	for (x = 0; x < TILE_FULL_WIDTH; ++x)
		for (y = 0; y < TILE_HEIGHT; ++y)
			if (getpixel(unit_bmp[icon_idx], x, y) != fpge_mask_color) {
				left = x;
				return left;
			}
	return left;
}

int check_icon_true_size_right(int icon_idx) {
	int x, y;
	int right = 0;
	//right
	for (x = TILE_FULL_WIDTH - 1; x >= 0; --x)
		for (y = 0; y < TILE_HEIGHT; ++y)
			if (getpixel(unit_bmp[icon_idx], x, y) != fpge_mask_color) {
				right = x;
				return right;
			}
	return right;
}

int isFlipNeeded(int icon_idx){
	int flip=0,found=0,i,j,found_eqp_id=-1;
	char buf[256]={""};
	char * pch;

	if(strcmp(icon_name_aux1[icon_idx],"")!=0){
		strncpy(buf,icon_name_aux1[icon_idx],256);
		found=1;
	}else{
		// no icon_name_aux1 name, try to find icon in efile

		for(j=0;j<total_equip;j++)
			if ((int)equip[j][BMP]+(int)equip[j][BMP+1]*256 == icon_idx){
				strncpy(buf,equip[j],256);
				found_eqp_id=j;
				found=1;
				break;
			}
	}

	if (found){
		int found_country=-1;
		//get first token from string
		pch = strtok (buf," ");
		if (pch != NULL){
			for(i=0;i<MAX_COUNTRY;i++){
				//stricmp is a case insensitive string compare. It is not POSIX compatible but supported in MinGW
				if (stricmp(pch,country_names_short[i])==0){
					//match
					if (country_side[i]==1) flip=1;
					found_country=i;
					break;
				}
			}
		}

		if (found_eqp_id!=-1 && found_country==-1)
			if (country_side[(int)equip_country[found_eqp_id]]==1) flip=1;
	}

	return flip;
}

int units_convert_bmps(char *name, int isFlipIcons, int discard_not_used_icons, int individual_files, int multi_column){
	int icon_idx,sum=0,t,b,r,l,ysize,xsize,k,limit,bmp_idx,s=0;
	BITMAP *u_bmp;
	int saved_icons[MAX_UICONS];
	int SDL_max_height=16384;
	char name_individual[256];

	if (discard_not_used_icons) {
		find_new_icon_numbers();
		memset(saved_icons,0,sizeof(saved_icons));
	}

	//count
	limit = total_uicons;
	if (discard_not_used_icons) limit = total_equip;

	for(k=0;k<limit;k++){
		if (discard_not_used_icons) {
			bmp_idx = equip[k][BMP] + 256 * equip[k][BMP + 1];
			//print_dec(bmp_idx);
			//print_dec(used_icons[bmp_idx]);
			if (saved_icons[bmp_idx]==0){
				icon_idx = new_icon_number[bmp_idx];
				saved_icons[bmp_idx]=1;
				s++;
			}else{
				//icon already saved
				continue;
			}
		} else {
			icon_idx = k;
		}
		t=check_icon_true_size_top(icon_idx);
		b=check_icon_true_size_bottom(icon_idx);

		ysize=(b>t?b-t+3:3); //add 3 for real pictures, 3 if empty
		sum += ysize;
	}
	if (!individual_files){
		int dx = TILE_FULL_WIDTH;
		int dy = sum;

		if (multi_column){
			int est_dx = sum / SDL_max_height +1;
			//add some height
			dx = TILE_FULL_WIDTH * ( (sum+est_dx*TILE_HEIGHT) / SDL_max_height +1);
			dy = SDL_max_height;
		}

		u_bmp = create_bitmap(dx, dy);
		rectfill(u_bmp, 0, 0, dx, dy, colors_to24bits(COLOR_BLACK)); // COLOR_BLACK is a transparent color for LGen
	}
	sum=0;
	if (discard_not_used_icons) {
		memset(saved_icons,0,sizeof(saved_icons));
	}
	int dx=0;
	for(k=0;k<limit;k++){
		if (discard_not_used_icons) {
			bmp_idx = equip[k][BMP] + 256 * equip[k][BMP + 1];
			if (saved_icons[bmp_idx]==0){
				icon_idx = new_icon_number[bmp_idx];
				saved_icons[bmp_idx]=1;
			}else{
				//icon already saved
				continue;
			}
		} else {
			icon_idx = k;
		}
		t=check_icon_true_size_top(icon_idx);
		b=check_icon_true_size_bottom(icon_idx);
		l=check_icon_true_size_left(icon_idx);
		r=check_icon_true_size_right(icon_idx);
		ysize=(b>t?b-t+3:3); //add 3 for real pictures, 3 if empty
		xsize=(r>l?r-l+1:10); // 10 if empty

		/*if (i==total_uicons-1) {
			print_dec(t);print_dec(b);print_dec(l);print_dec(r);print_dec(xsize);print_dec(ysize);
		}
		 */

		if (individual_files){
			snprintf(name_individual,256,"lg%04d.bmp",bmp_idx);
			sum = 0;
			u_bmp = create_bitmap(TILE_FULL_WIDTH, ysize);
			rectfill(u_bmp, 0, 0, TILE_FULL_WIDTH, ysize, colors_to24bits(COLOR_BLACK)); // COLOR_BLACK is a transparent color for LGen
		}

		//doFlip=0;
		if (isFlipIcons && isFlipNeeded(icon_idx)){
			//doFlip=1;
			BITMAP *tmp_bmp;
			tmp_bmp = create_bitmap(TILE_FULL_WIDTH, TILE_HEIGHT);
			rectfill(tmp_bmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT+1,fpge_mask_color);
			draw_sprite_h_flip(tmp_bmp,unit_bmp[icon_idx],0,0);
			//swap left with right limits after H-flip
			l=TILE_FULL_WIDTH-r;
			r=l+xsize;
			masked_blit(tmp_bmp,u_bmp,l,t,dx,sum+1,xsize,ysize);
			destroy_bitmap(tmp_bmp);
		}else{
			masked_blit(unit_bmp[icon_idx],u_bmp,l,t,dx,sum+1,xsize,ysize);
		}
		putpixel(u_bmp,dx,sum,make_color_fpge(0,192,248));
		putpixel(u_bmp,dx+xsize,sum,make_color_fpge(0,192,248));
		putpixel(u_bmp,dx,sum+ysize-1,make_color_fpge(0,192,248));

		if (individual_files){
			save_bmp(name_individual,u_bmp,NULL);
			destroy_bitmap(u_bmp);
		}
		sum += ysize;
		if (multi_column && sum+TILE_HEIGHT > SDL_max_height ){
			dx +=TILE_FULL_WIDTH;
			sum=0;
		}
	}
	if (!individual_files){
		save_bmp(name,u_bmp,NULL);
		destroy_bitmap(u_bmp);
	}
	return 0;
}

int save_lgeneral_units_bmp(){
	char buf[128]="pg";
	char ext[128]=".bmp";
	char name[128]="pg.bmp";

	sprintf(MapStatusTxt,"LGen bmp export.");
	d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);

	lgen_uicons_dlg[2].dp=buf;
	centre_dialog(lgen_uicons_dlg);
	if (do_dialog(lgen_uicons_dlg, -1)==7){
		strncpy(name,buf,128);
		strncat(name,ext,128);
		int doFlip = (lgen_uicons_dlg[4].flags&D_SELECTED)?1:0;
		int change_icons = (lgen_uicons_dlg[8].flags&D_SELECTED)?1:0;
		int individual_icons = (lgen_uicons_dlg[9].flags&D_SELECTED)?1:0;
		int multi_column = (lgen_uicons_dlg[10].flags&D_SELECTED)?1:0;
		units_convert_bmps(name, doFlip, change_icons,individual_icons,multi_column);

		snprintf(MapStatusTxt,256,"LGen %s saved.\nPick an Operation.",name);
		main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
	}else{
		pick_msg();
	}
	return D_REDRAW;
}

int save_nations_db(char *name, char *target_name, char *domain){
    int i;
    FILE *file;

    /* nation database */
    printf( "Nation database...\n" );

    if ( ( file = fopen( name, "w" ) ) == 0 ) {
        fprintf( stderr, "%s: access denied\n", name );
        return 1;
    }
    fprintf( file, "@\n" );
    fprintf( file, "icons»%s\n", target_name );
    fprintf( file, "icon_width»20\nicon_height»13\n" );
    /* domain */
    fprintf( file, "domain»%s\n",domain );
    fprintf( file, "<nations\n" );
    for ( i = 0; i < NATION_COUNT; i++ )
        fprintf( file, "<%s\nname»%s\nicon_id»%s\n>\n", nations[i * 3], nations[i * 3 + 1], nations[i * 3 + 2] );
    fprintf( file, ">\n" );
    fclose( file );
    return 0;
}

int save_nations_bmp(char *name){
    int i;
	BITMAP *n_bmp;

	n_bmp = create_bitmap(20, 13*NATION_COUNT);
	rectfill(n_bmp, 0, 0, 20, 13*NATION_COUNT, colors_to24bits(COLOR_BLACK)); // COLOR_BLACK is a transparent color for LGen

	for ( i = 0; i < NATION_COUNT; i++ )
		masked_blit(flag_bmp[i],n_bmp,20,36,0,i*13,20,13);
	save_bmp(name,n_bmp,NULL);
	destroy_bitmap(n_bmp);
    return 0;
}

int save_lgeneral_nations(){
	char name[128]="pg_nations.bmp";
	char nameDB[128]="pg.ndb";

	sprintf(MapStatusTxt,"LGen pg.ndb saving...\nPlease wait.");
	d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);

	save_nations_db(nameDB,name,"pg");

	sprintf(MapStatusTxt,"LGen pg_nations.bmp saving...\nPlease wait.");
	d_mapstatus_proc(MSG_DRAW, &(main_dlg[dmMapStatusIdx]), 0);

	save_nations_bmp(name);

	sprintf(MapStatusTxt,"LGen ndb, bmp saved.\nPick an Operation.");
	main_dlg[dmMapStatusIdx].flags |= D_DIRTY;

	return D_O_K;
}
