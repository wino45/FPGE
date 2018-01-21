/* This is the main header file for FPGE */
#ifndef _FPGE_H_
#define _FPGE_H_

#include <allegro.h>
#include <stdint.h>

#include "scen.h"
#ifdef WIN32
#include <winalleg.h>
#include <windows.h>
#else
#include <limits.h>
#define MAX_PATH PATH_MAX
#endif
#define MAX_EXT 50

//my helper function
#define Max(a,b) (a>b)?(a):(b)
#define Min(a,b) (a<b)?(a):(b)

#undef min
#undef max

#define print_hex(var)  printf("%s=%04x\n",#var,var)
#define print_dec(var)  printf("%s=%d\n",#var,var)
#define print_ldec(var)  printf("%s=%ld\n",#var,var)
#define print_str(var)  printf("%s=%s\n",#var,var)
#define print_ptr(var)  printf("%s=%p\n",#var,var)

#define BYTE unsigned char
#define WORD short
#define UWORD unsigned short
#define LONG long
#define ULONG unsigned long

#define MAX_TILES_IN_PG 237  //maximum number of map tile icons in PG. Used in various automatic actions
#define STD_MATRIX_MAX_X 20
//#define STD_MATRIX_MAX_Y 12

//#define MAX_TILES 256  //maximum number of map tile icons
#define MAX_TILES 8192  //maximum number of map tile icons

#define MAX_UICONS 6000 //maximum number of unit icons
#define MAX_MUICONS 2  //allways 2 for strategic map
#define MAX_UNITS 5000  //maximum number of units in equipment file
#define MAX_FLAGS 512   //actually PG uses 48, we need 512 to handle 256 flags due to graphics loading code
#define MAX_MINI_VIC_HEX 2   //allways 2 for strategic map
#define MAX_STRENGTH_IN_ROW 21   // from 0 to 20 to suite PGF
#define MAX_STRENGTH MAX_STRENGTH_IN_ROW*4   //actually 63
#define MAX_ICONS MAX_UICONS  //most icons that can be loaded into address 
#define MAX_SICONS 100 // stack icons

#define MAX_VICTORY_HEXES 20
#define MAX_DEPLOY_HEXES  255
#define MAX_AXIS_UNITS    80*2
#define MAX_ALLIED_UNITS 200*2
#define MAX_AXIS_AUX_UNITS    120*2
#define MAX_NAMES        20000

#define MAX_NAME_SIZE 20
#define MAX_PG2_NAME_SIZE 30
#define MAX_NAME_UTF_SIZE 40

#define STD_NAMES_PG 16

//#define MAX_MAP_X 71
//#define MAX_MAP_Y 54
#define MAX_MAP_X 1024
#define MAX_MAP_Y 1024
//#define MAX_DISPLAY_TILES 237  //maximum number of used tiles
#define TILES_HOT_KEY_NUMBER 9

#define CLASS_NUMBER 18
#define CLASS_NAME_LENGTH 16

#define BK_COLOR 0     //transparent shp color
#define HEADER_SIZE 24 //size of shp header
#define TILE_HEIGHT 50
#define TILE_WIDTH 45
#define TILE_FULL_WIDTH 60
#define TILE_HEIGHT_SHIFT 25
#define TILE_WIDTH_SHIFT 16

#define MAP_X_ADDR 101
#define MAP_Y_ADDR 103
#define MAP_STM   0x30
#define MAP_SET   0x51
#define MAP_LAYERS_START   123

#define MAX_PATTERN_SIZE  128

#define NO_FILTER_INDEX -1
#define COAST_FILTER_INDEX 0
#define CITY_FILTER_INDEX 1
#define ROAD_FILTER_INDEX 5
#define RIVER_FILTER_INDEX 6
#define FOREST_FILTER_INDEX 8

#define UNKNOWN_UTR_NAME 39

#define SHOW_LOGS 1
#define DO_NOT_SHOW_LOGS 0

#define LOAD_FILE 0
#define PROBE_FILE 1
#define SCAN_FOR_MAP_NUMBER 2
#define LOAD_CONVERSION_TABLE_ONLY 3
#define NORMAL_SAVE 4
#define SAVE_WITH_UNIT_IDS_CONVERSION 5
#define SAVE_WITH_UNIT_IDS_HARD_CONVERSION 6

#define MAX_COUNTRY 255 //was 25 as standard PG countries number
#define MAX_COUNTRY_NAME_SIZE 21
#define MAX_COUNTRY_SHORT_NAME_SIZE 5
#define MAX_TERRAIN_TYPE 40
#define TERRAIN_TYPE_SIZE 25

#define MAX_MOV_TYPE 25
#define RADIO_PG_MOV_TYPES 8
#define RADIO_PGF_MOV_TYPES 11
#define MAX_TERRAIN_MOV_TYPES 50
#define ROW_PG_TERRAIN_MOV_TYPES 12
#define ROW_PACGEN_TERRAIN_MOV_TYPES 37
#define MOVEMENT_TYPES_NO_COL 3
#define MAX_COLUMNS_DYNDLG 10

#define PGF_MOV_TYPES 11

#define MAX_UNIT_AUX_NAME_SIZE 60

/*  -------------------- Here is the Main Dialog ----------------------- */
/* ------ this section has defines for the main dialog ------------------- */
//#define SCREEN_X 640
//#define SCREEN_Y 480

//#define SCREEN_X 800
//#define SCREEN_Y 600

#define SCREEN_X 1024
#define SCREEN_Y 768

//#define SCREEN_X 1280
//#define SCREEN_Y 1024

#define TOP_LINE_Y 2
#define LINE_1_Y   28
#define LINE_2_Y   54

#define VSLIDE_W 23
#define VSLIDE_H SCREEN_Y-280

#define VSLIDE_X SCREEN_X-80
#define VSLIDE_Y 200

#define HSLIDE_W SCREEN_X - 440
#define HSLIDE_H 23
#define HSLIDE_X 210
#define HSLIDE_Y LINE_1_Y

#define TILES_WIDE (SCREEN_X-80)/TILE_WIDTH
#define TILES_HIGH (SCREEN_Y-TILE_HEIGHT/2-54)/TILE_HEIGHT

#define MAP_H (TILES_HIGH*2+1)*TILE_HEIGHT/2+1
#define MAP_W (TILES_WIDE*3/2+1)*30-15
#define MAIN_BUTTON_X SCREEN_X-50
#define EXIT_Y SCREEN_Y-10

#define BUTTON_SIZE 20
#define BUTTON_H 15
#define BUTTON_W 45

#define COLOR_WHITE 15
#define COLOR_BLACK 255
#define COLOR_BLUE 246
#define COLOR_GREEN 10
#define COLOR_RED 12
#define COLOR_PURPLE 5
#define COLOR_YELLOW 27
#define COLOR_DARK_GREEN 120
#define COLOR_DARK_DARK_GREEN 232
#define COLOR_DARK_GREEN_GRAY 38
#define COLOR_DARK_BLUE_GRAY 162
#define COLOR_LIGHT_BLUE (255-16-2)

#define HEX_COLOR 241
#define LINE_COLOR 22

#define GUI_FG_COLOR 256
#define GUI_BG_COLOR 256+1
#define GUI_EDIT_COLOR 256+2

#define FPGE_FG_COLOR 41
#define FPGE_BG_COLOR 54
#define FPGE_SCREEN_COLOR 8

//main gui colors only
#define FG_COLOR GUI_FG_COLOR
#define BG_COLOR GUI_BG_COLOR
#define SCREEN_COLOR GUI_BG_COLOR

#define MAP_COLOR 149
#define SLIDE_BG_COLOR 38
#define SLIDE_FG_COLOR 33
#define VIC_HEX_COLOR 4

#define NEUTRAL_HEX_COLOR 256-16+3
#define DEPLOY_HEX_COLOR 255
#define BLACK_HEX_COLOR 8
#define PROBLEM_HEX_COLOR 122
#define MAGIC_ROAD_COLOR 127
#define MAGIC_RIVER_COLOR 126

#define BLACK_TILE (MAX_TILES+1)
#define NOT_USED_TILE (MAX_TILES+2)
#define MAGIC_ROAD (MAX_TILES+3)
#define MAGIC_RIVER (MAX_TILES+4)
#define MAGIC_ROAD_AND_RIVER (MAX_TILES+5)

#define BYTE_MAX (256-1)
#define WORD_MAX (256*256-1)

#define PG_MODE 0
#define PGF_MODE 1
#define AG_MODE 2
#define PACGEN_MODE 3

/* ------ this section holds the functions prototypes for the main dialog */

extern BITMAP *map_bmp;
extern BITMAP *left_arrow_bmp;
extern BITMAP *right_arrow_bmp;
extern BITMAP *up_arrow_bmp;
extern BITMAP *down_arrow_bmp;
extern BITMAP *vic_hex_bmp;
extern BITMAP *black_hex_bmp;
extern BITMAP *neutral_hex_bmp;
extern BITMAP *deploy_hex_bmp;
extern BITMAP *problem_hex_bmp;
extern BITMAP *road_hex_bmp;
extern BITMAP *river_hex_bmp;
extern BITMAP *road_and_river_hex_bmp;

extern BITMAP *unit_bmp[MAX_UICONS];
extern BITMAP *stack_bmp[MAX_SICONS];
extern BITMAP *save_map_bmp;
extern BITMAP *til_bmp[MAX_TILES];
extern BITMAP *dark_til_bmp[MAX_TILES];
extern BITMAP *til_bmp_mud[MAX_TILES];
extern BITMAP *dark_til_bmp_mud[MAX_TILES];
extern BITMAP *til_bmp_snow[MAX_TILES];
extern BITMAP *dark_til_bmp_snow[MAX_TILES];

extern BITMAP *mtil_bmp[MAX_TILES];
extern BITMAP *dark_mtil_bmp[MAX_TILES];
extern BITMAP *mtil_bmp_mud[MAX_TILES];
extern BITMAP *dark_mtil_bmp_mud[MAX_TILES];
extern BITMAP *mtil_bmp_snow[MAX_TILES];
extern BITMAP *dark_mtil_bmp_snow[MAX_TILES];

extern BITMAP *flag_bmp[MAX_FLAGS];
extern BITMAP *victory_flag_bmp[MAX_FLAGS];
extern BITMAP *mini_victory_hex_bmp[MAX_MINI_VIC_HEX];
extern BITMAP *strength_bmp[MAX_STRENGTH];

extern long icon_addr[MAX_ICONS]; //used to load SHP files
//RGB pgpal[];

int d_btn_proc(int,DIALOG *,int); //our function to make buttons act like
                                  //radio buttons
int d_mapbmp_proc(int,DIALOG *,int);

int d_arrow_proc(int,DIALOG *,int);

int d_hslide_proc(int,DIALOG *,int);
int d_vslide_proc(int,DIALOG *,int);

int d_GndAir_proc(int,DIALOG *,int);
int d_mapinfo_proc(int,DIALOG *,int);
int d_mapstatus_proc(int,DIALOG *,int);
int d_filterstatus_proc(int,DIALOG *,int);
int d_coreRB_proc(int,DIALOG *,int);
void remove_gunit(int,int);
void remove_aunit(int,int);
//int d_coreRB_proc(int , DIALOG *, int );
//int d_mapinfo_proc(int ,DIALOG *,int );
//int d_btn_proc(int ,DIALOG *, int );

char *add_countries(int idx,int *list_size); //unit and scen
void setup_terrain_info();
int count_vic_hexes();
int count_deploy_hexes();
int count_deploy_hexes_on_sea();
int count_deploy_hexes_occupied();
int count_deploy_hexes_occupied_on_sea();
//int load_scn(int,int);
int load_map(int );
void tile_cleanup();
void setup_show_filters_info();
int get_h_slide_max();
int get_v_slide_max();

//we won't treat equip as a structure but will use offsets into the records
#define EQUIP_REC_SIZE 50

#define NAME   0
#define CLASS 20 //class
#define SA    21 //soft attack
#define HA    22 //hard attack
#define AA    23 //air attack
#define NA    24 //naval attack
#define GD    25 //ground defence
#define AD    26 //air defence
#define CD    27 //close defence
#define TARGET_TYPE    28 //target type 0-soft,1-hard,2-air,3-naval,[4-submarine-PacGen]
#define AAF   29   //1 for tac bombers who can attack
#define INITIATIVE   31 //initiative
#define RANGE   32 //range
#define SPOTTING   33 //spotting
#define GAF   34    //the ground=0 air=1 flag
#define MOV_TYPE 35 //movement type 0-tracked,1-halftracked,2-wheeled,3-leg,4-towed,5-air,6-naval,7-all terrain
                    //PGF: amphibious tracked (8), amphibious all terrain (9) and mountain (10)
#define MOV   36  //movement
#define FUEL  37  //fuels
#define AMMO  38 //amo
#define COST  41 //cost
#define BMP   42 //icon number - 2 bytes
#define ANI   44 //animation number - 2 bytes (?)
#define MON   46
#define YR    47
#define LAST_YEAR 48
#define ALLOWED_TRANSPORT 49 //1-can have organic, 2-can have sea/air ,3-can paradrop

#define COST_DIVISOR 12

//similarly we will use an array to store the scenario
#define MAX_SCN_SIZE 5000
// These are offsets or starting addresses into the scenario file
#define ORIENTATION 12 //2bytes
#define AXIS_STANCE 14
#define ALLIED_STANCE 15
#define STORM_FRONT 16
#define SCEN_LOCALE 17

#define CORE_UNITS_LIMIT 18
#define AUX_UNITS_LIMIT 19
#define ALLIED_UNITS_LIMIT  20
#define ALLIED_AUX_UNITS_LIMIT  (MAX_SCN_SIZE-1)

#define TURNS 21
#define DAY   22
#define MONTH 23
#define YEAR  24
#define TURNS_PER_DAY 25
#define DAYS_PER_TURN 26
#define AI_AXIS_BUCKET 27 //2 bytes
#define AI_ALLIED_BUCKET 29 //2 bytes
#define AI_AXIS_INTERVAL 31
#define AI_ALLIED_INTERVAL 32
#define CORE_UNITS    33
#define ALLIED_UNITS  34
#define AUX_UNITS     35
#define AUX_ALLIED_UNITS  36
#define VIC_HEXES     37    //up to 20 terminated with -1
#define DEPLOY_TABLE  117
#define DEPLOY_TO_UNITS 0x87

//----
#define SHADE_MASK          0x01
#define PROBLEM_MASK        0x02
#define SELECTED_FRG_MASK   0x04
#define PLACED_FRG_MASK     0x08

/* ---------- Here we define structures ----------------------------------*/
struct shp_header
{
  int lines;     // last starting at 0 ie lines-1
  int width;
  int centerx;
  int centery;
  long xstart;
  long ystart;
  long xend;
  long yend;
};

struct EDITORTILE {
 char tdTTStr[8];
 char tdRDStr[8];
 char tdTNStr[8];
 char tdGLNStr[8];
 char tdSDStr[8];
 int tdTTStrOn;
 int tdRDStrOn;
 int tdTNStrOn;
 int tdGLNStrOn;
 int tdSDStrOn;
 int set;
};

struct MAP {
  short utr; //Terrain type
  unsigned char own; //country flag
  unsigned char vic; //is victory hex
  unsigned char rc; //road connection
  unsigned char side; //side  0- axis, 1-ally, 3-neutral ???
  unsigned char deploy; //is deployment hex
  short tile; // tile idx
  short gln; // name idx
  short auidx; //index of air unit in all_units no unit=-1
  short guidx; //index of ground unit in all_units  no unit=-1
  unsigned char shade; //set to 1 if no visibility
};

struct VICTORY_HEXES{
  WORD x,y;
  BYTE own;
};

struct DEPLOY_HEXES{
  WORD x,y;
};

enum UTYPES {
	axis_core = 0, axis_aux, allied_core, allied_aux
};
//the unit as stored in the scn file
struct UNIT {
  WORD unum;          //ID of the unit type
  WORD orgtnum;       //organic
  BYTE country;
  WORD auxtnum;      //sea/air transport
  WORD x;
  WORD y;
  BYTE str;
  BYTE entrench;
  BYTE exp;
  //PGF
  short fuel; //we need to have -1 for "no fuel set" case (PGDOS),same for "ammo"
  short ammo;
  enum UTYPES uflag; //what owner of unit
};

// called dialogs
/*
extern DIALOG ud[];
extern DIALOG sd[];
extern DIALOG td[];
extern DIALOG hd[];
extern DIALOG mm[];
extern DIALOG sr[];
extern DIALOG FindName[];
*/
//-------------------- global variable declarations --------

//extern char MapStatusTxt[];

extern struct shp_header header;
extern struct MAP  map[MAX_MAP_X][MAX_MAP_Y]; //main map storage
extern unsigned char rc[MAX_MAP_X][MAX_MAP_Y];
extern unsigned short tempmap[MAX_MAP_X][MAX_MAP_Y];
//extern unsigned short tempmap2[MAX_MAP_X][MAX_MAP_Y];
extern int mapx,mapy;  //the mapsize
extern int map_h;
extern int map_w;
extern int hslide_w;
extern int hslide_x;
extern int vslide_x;
extern int vslide_h;
extern int matrix_x;
extern int matrix_y;
extern int frg_x;
extern int frg_y;

//extern unsigned char equip[MAX_UNITS][EQUIP_REC_SIZE];
extern BYTE scn_buffer[MAX_SCN_SIZE];
extern int scn_size; //bytes we actually read
extern int show_unit_layer_type; //toggle depending on whether air or ground selected
//extern char unit_names[][16]; //unit type names: infantry tank etc.
//extern char country_names[25][16];

extern enum EDIT_FUNCTION {
	edit_none,
	edit_str,
	edit_exp,
	edit_place,
    edit_ent,
    edit_vic,
    edit_own,
    edit_ter,
    edit_tile,
    edit_deploy,
    edit_neutral,
    edit_rimp,
    edit_rexp,
    edit_frg_select,
    edit_frg_place,
    edit_transport,
    move_mode
} edit_op;

extern int unit_type_selected; //picked in the ud dialog
extern int unum_selected;
extern int tnum_selected;
extern int cnum_selected; //the current country
extern int anum_selected; //aux transport

#define EQUIPMENT_HARDCODED                  0x0001
#define EQUIPMENT_BOMBER_SPECIAL             0x0002
#define EQUIPMENT_IGNORES_ENTRENCHMENT       0x0004
#define EQUIPMENT_CAN_HAVE_ORGANIC_TRANSPORT 0x0008
#define EQUIPMENT_CAN_HAVE_SEA_TRANSPORT     0x0010
#define EQUIPMENT_CAN_HAVE_AIR_TRANSPORT     0x0020
#define EQUIPMENT_CAN_PARADROP               0x0040
#define EQUIPMENT_CAN_BRIDGE_RIVERS          0x0080
#define EQUIPMENT_JET                        0x0100
#define EQUIPMENT_CANNOT_BE_PURCHASED        0x0200
#define EQUIPMENT_RECON_MOVEMENT             0x0400

#define MAX_LINE_SIZE 256
#define MAX_MAP_FRG 500
#define FRG_X_SIZE 9
#define FRG_Y_SIZE 9

extern int total_units;  //on map
extern int total_equip; //units in panzequp
extern int conversion_total_equip;
extern unsigned char equip[MAX_UNITS][EQUIP_REC_SIZE];
extern unsigned char equip_name_utf8[MAX_UNITS][20*2];
extern unsigned char icon_name_aux1[MAX_UNITS][MAX_UNIT_AUX_NAME_SIZE*2];
extern unsigned char icon_name_aux2[MAX_UNITS][MAX_UNIT_AUX_NAME_SIZE*2];

extern char equip_country[MAX_UNITS];
extern WORD equip_flags[MAX_UNITS];
extern int8_t equip_last_month[MAX_UNITS];

struct conversion_equipment{
	unsigned char name[20*3];
	char country;
	int new_id;
	int old_id;
};

extern struct conversion_equipment conversion_equip[MAX_UNITS];

extern struct UNIT all_units[MAX_AXIS_UNITS+MAX_ALLIED_UNITS+MAX_AXIS_AUX_UNITS];
extern char uselected[16];
extern char tselected[16];
extern char cselected[16];
extern char aselected[16];
extern int map_mouse_x,map_mouse_y; // coordinates on map
extern int ustr_selected;
extern int uent_selected;
extern int uexp_selected;

//extern int scn_number;
extern char scn_file_name[128];
extern int map_number;
extern int stm_number;
extern int pgf_map_number;

extern int tile_mode;
//extern int mapx,mapy;
extern int filter_tiles,filter_start,filter_stop,filter_last;
extern int rexp_left_mouse_click,rexp_x,rexp_y;
extern struct VICTORY_HEXES victory_hexes[];
extern struct EDITORTILE EditorTile[];
extern int total_names;
extern char gln[][20];
extern unsigned char gln_utf8[][20*2];

extern int map_x0,map_y0;

//extern struct UNIT all_units[];
extern int tiles_wide;
extern int tiles_high;
extern int vslide_max;
extern int top_spacer;
extern int bottom_spacer;


// offsets in s4_buffer
#define AXIS_PRESTIGE   0
#define ALLIED_PRESTIGE 2
#define AXIS_AIR_NUMBER 4
#define ALLIED_AIR_NUMBER 5
#define AXIS_SEA_NUMBER 6
#define ALLIED_SEA_NUMBER 7
#define AXIS_AIR_TYPE 8
#define ALLIED_AIR_TYPE 10
#define AXIS_SEA_TYPE 12
#define ALLIED_SEA_TYPE 14

#define DEFAULT_AXIS_AIR_TYPE 29
#define DEFAULT_AXIS_SEA_TYPE 299
#define DEFAULT_ALLIED_AIR_TYPE 354
#define DEFAULT_ALLIED_SEA_TYPE 291

extern BYTE s4_buffer[16];

extern int drawNames;
extern int drawAllNames;
extern int drawTerrain;
extern int drawRoads;
extern int showDecimal;
extern int showFilter;
extern int filterTerrain;
extern int filterName;
extern int showCounter;
extern int drawGndTransport;
extern int showWeather;
extern int showHex;
extern int showHexMatrix;
extern int showMatrixMode;
extern int sortMatrixMode;
extern int showHexColor;
extern int showHexMatrixColor;
extern int graphical_overide;
extern int scenarioUnitsMode;
extern int displayAllUnits;
extern int displayUnitsOrder;
extern int graphical_overide_hex; //special hex display
extern int debug_tile_matrix;
extern int show_ranges;
extern int show_problems;
extern int show_debug_problems;
extern int colorizeNames;
extern int mapShiftMode;
//edit_op == edit_tile && debug_tile_matrix
extern int drawRoadsDebug;
extern int showDecimalDebug;
extern int drawNamesDebug;
extern int drawAllNamesDebug;
extern int colorizeNamesDebug;
extern int drawTerrainDebug;

extern char MonthStr[SCENARIO_STR_SIZE];
extern char YearStr[SCENARIO_STR_SIZE];

extern unsigned char Neutral[];
//mini tiles
extern char mtil_file[];
extern int total_tiles;
extern int total_mtiles;
//extern BITMAP *mtil_bmp[MAX_TILES];
//extern BITMAP *dark_mtil_bmp[MAX_TILES];

extern char muicons_file[];
extern int total_uicons;
extern int total_muicons;
extern int total_sicons;
extern BITMAP *munit_bmp[MAX_MUICONS];

extern char mflag_file[];
extern int total_mflags;
extern int total_flags;
extern int total_countries;

extern int total_strength_counters;

extern RGB pacgen_pal[];
extern RGB pgpal[];
extern int pix_pos,max_pix_x,max_pix_y;
//extern void set_in_radius(int x, int y, int r, int value);

//extern struct UNIT all_units[];
extern int total_deploy; //deployment hexes
//gr
extern int card_old;
extern int w_old;
extern int h_old;

//units in the scenario
extern int total_axis_core;
extern int total_allied_core;
extern int total_axis_aux;
extern int total_allied_aux;

int count_axis_core();
int count_axis_aux();
int count_allied_core();
int count_allied_aux();

//GUI
extern char MapInfoTxt[];
extern char MapStatusTxt[];
extern char AboutTxt1[];
extern char AboutTxt2[];
extern char AboutTxt3[];
extern char FilterStatusTxt[];
extern char EquipmentFileToSave[];

extern int backup_not_saved;

//int d_mapinfo_proc(int ,DIALOG *,int);

void get_victory_hexes();
int filter_roads(int,int);
int find_road_mask(int x, int y);
int find_one_way_road(int x, int y);

void initialize_dm();
int is_tile_river(short x, short y);
int is_tile_ocean(short x, short y);
int is_tile_name_water(short x, short y);
int is_tile_name_standard(int gln);
int is_tile_a_city_tile(int tile);
int load_scenario_dialog();

void do_place_status();
int str_bmp_offset(int idx);

#define MAX_SCENARIOS 40
#define SHORT_SCN_SIZE 14
#define LONG_SCN_SIZE 160
extern unsigned char scn_active[];
extern unsigned char scn_short_description[][SHORT_SCN_SIZE];
extern unsigned char scn_long_description[][LONG_SCN_SIZE];

extern int pg_mode;
extern int pgf_mode;
extern int ag_mode;
extern int pacgen_mode;
extern int nupl_present;
extern int bin_tables_present;
extern int mapnames_changed;
extern int str0_bmp_loaded;

extern int fpge_gui_fg_color;
extern int fpge_gui_bg_color;
extern int fpge_gui_edit_color;
extern int swap_indicators;
extern int place_frg_dialog;

int isAllied(enum UTYPES uflag);
int isNotAllied(enum UTYPES uflag);
int isAux(enum UTYPES uflag);
int isNotAux(enum UTYPES uflag);

void pick_msg();

#endif
