/*
 * pacgen.c
 *
 *  Created on: 2011-01-04
 *      Author: wino
 */

#include <stdio.h>
#include "fpge.h"
#include "pacgen.h"
#include "tables.h"
#include "maingui.h"
#include "fpge_colors.h"
#include "pgf.h"
#include "drawmap.h"
#include "unitutil.h"
#include "unitlist.h"
#include "filename.h"
#include "load.h"

DIALOG pacgen_dlg[7+5+1-2]= {
/* 0 */ { d_textbox_proc,0,-16,198+16+60+2*18,30+18+12+18*4+16,GUI_FG_COLOR,GUI_BG_COLOR,0,0,0,0,0,0,0},
/* 1 */	{ d_text_proc,16,12,200,12,GUI_FG_COLOR,GUI_BG_COLOR,0,0,0,0,"Choose file to convert" },
/* 2 */	{ d_button_proc,22,30+18*4,96,18,GUI_FG_COLOR,GUI_BG_COLOR,'c',D_EXIT,0,0,"&Cancel" },
/* 3 */	{ d_button_proc,178+3*18-36,30+18*4,96,18,GUI_FG_COLOR,GUI_BG_COLOR,'s',D_EXIT,0,0,"&Load" },
/* 4 */	{ d_edit_proc,16,12+18,200,12,GUI_FG_COLOR,GUI_EDIT_COLOR,0,0,2,1,/*EDIT*/0},
/* 5 */ { d_radio_proc,16,30+18*1,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,0,D_SELECTED,95,0,"No not convert names"},
/* 6 */ { d_radio_proc,16,30+18*2,60+70,18,GUI_FG_COLOR,GUI_BG_COLOR,'p',0,95,0,"Add new names"},
/* 7 */	{ d_button_proc,200+2*18,12+18,60,18,GUI_FG_COLOR,GUI_BG_COLOR,'e',D_EXIT,0,0,"&File" },
/* 8 */ { d_text_proc,16+5*8,-8,200,12,GUI_FG_COLOR,GUI_BG_COLOR,0,0,0,0,"Load Pacific General file" },
{d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
 { NULL,0,0,0,0,0,0,0,0,0,0,NULL }
 };

char pacgen_gln[MAX_NAMES][MAX_PG2_NAME_SIZE];
int pacgen_gln_map[MAX_NAMES]; // map to gln ids, -1-not set,
int total_pacgen_names;
int pacgen_std_terrain_offset=-1;

int count_per_set[NO_OF_PACGEN_TIL_CLASSES];
int no_of_sets=NO_OF_PACGEN_TIL_CLASSES;

char class_name[NO_OF_PACGEN_TIL_CLASSES][4]={
"HQ_",
"CST", "CLR", "RD_", "RIV", "BCH", "RF_", "CTY",
"ISL", "MNT", "FRT", "LAK", "DES", "FRM", "DMT", "SF1",
"SF2" };

WORD pacgen_tt_to_pg_tile[NO_OF_PACGEN_TERRAIN_TYPES]= {
		              205, 26, 69, 69,129,146,129,146,146,131,
		              157,157, 69,129,124,131,183,131,124,131,
		              200,131,102,207, 52,183,129,157,207,124,
		               73, 73,129,128,128,158,129
                      };
int pacgen_tt_to_pg_tile_no=NO_OF_PACGEN_TERRAIN_TYPES;

unsigned char pacgen_class_2_pg_class[NO_OF_PACGEN_CLASSES]={0,1,2,3,4,5,6,7,15,12,13,14,12,13,11,8,9,10,16,17};
unsigned char pacgen_move_2_pg_move[NO_OF_PACGEN_MOVE_TYPES]={0,1,2,3,4,5,6,6,7,6,6};
unsigned char pacgen_target_type_2_pg_target_type[NO_OF_PACGEN_TARGET_TYPES]={0,1,2,3,3};

//typedef BITMAP *bmp_array[MAX_TILES];

struct ext_idx_struct {
	char name[8];
	int start;
	int stop;
} ext_idx[6];

struct idx_struct {
	char name[8];
	int size;
	int place;
};

struct pacgen_equ {
	BYTE class; //1
	BYTE sa,ha,aa,na,sub_a; //6
	BYTE gd,ad,assault_def,torpedo_def,depth_charge_def; //11
	BYTE target_type; //12
	BYTE bomber_size; //13
	BYTE uk1; //14 unknown
	BYTE ini, range, spotting; //17
	BYTE move_med; //18 0- surf, 1 - air
	BYTE move_type; //19 3-leg, 5 -air, 7-coastal-naval, etc
	BYTE mov, fuel, amo, life, uk3 ; //24
	BYTE prestige; //25
	BYTE pic_num[2]; // 27
	BYTE uk4[2]; //29 unknown
	BYTE month,year,last_year; //33
	BYTE transport; //34 0- none, 1-Naval ,2-Air Mobile,3-Airborne
	BYTE hangar_size;// 35
	BYTE countries[4];//39, bits for flags. 0x00200000 -Poland, 0x10000000 USA, etc
	BYTE special[4]; //43 special units attributes, like 14-"No buy", 1-"No AI buy" ,etc
	UWORD move_sound,attack_sound,death_sound; // 49 sound effects numbers
	BYTE fill[PACGEN_EQUIP_REC_SIZE-49];
};

struct pacgen_scn_unit {
 char unitname[40];
 BYTE  flag;//, owner 0x0e- japan, 0x1c - usa
 WORD  unit_id;
 WORD  unit_organic_transport_id;
 WORD  unit_seaair_transport_id;
 WORD unknow[2];
 WORD  X;
 WORD  Y;
 WORD  strength;
 WORD  unknown1[2];
 WORD  entranchment;
 BYTE  unknown2;
 WORD  experience;
BYTE fill[PACGEN_SCN_UNIT_REC_SIZE-(40+12*2+2)];
};

int import_pacgen_names_file(char *path) {

	FILE *inf;
	int max_names_file_size = MAX_NAMES * MAX_PG2_NAME_SIZE;
	char *end_of_line;

	inf = fopen(path, "rt");
	if (inf == NULL) {
		printf("Cannot open PacGen names file : %s\n", path);
		return ERROR_PACGEN_SCENARIO_NAMES_LOAD_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}
	total_pacgen_names = 0;

	fseek(inf, 0, SEEK_END);
	long file_size = ftell(inf);
	fseek(inf, 0, SEEK_SET);

	if (file_size > max_names_file_size) {
		printf("ERROR: Too big PacGen names file size %ld, expected at most %d. File : %s\n", file_size, max_names_file_size, path);
		fclose(inf);
		return ERROR_PACGEN_SCENARIO_NAMES_LOAD_BASE + ERROR_FPGE_PG2_FILE_TOO_BIG;
	}

	while (feof(inf) == 0) {
		fgets(pacgen_gln[total_pacgen_names], MAX_PG2_NAME_SIZE, inf);
		//clear eofl = \n
		end_of_line = strchr(pacgen_gln[total_pacgen_names], '\n');
		if (end_of_line != NULL)
			*end_of_line = 0;
		//fscanf(inf, "%s\n", pg2_gln[total_pg2_names]);
		//print_str(pg2_gln[total_pg2_names]);
		++total_pacgen_names;
	}

	fclose(inf);
	return 0;
}

int import_pacgen_string_file(char *path, char *dest_buffor, int buf_size) {

	FILE *inf;
	//int max_names_file_size = MAX_NAMES * MAX_PG2_NAME_SIZE;
	//char *end_of_line;

	inf = fopen(path, "rb");
	if (inf == NULL) {
		printf("Cannot open PacGen names file : %s\n", path);
		return ERROR_PACGEN_SCENARIO_NAMES_LOAD_BASE + ERROR_FPGE_FILE_NOT_FOUND;
	}

	fseek(inf, 0, SEEK_END);
	long file_size = ftell(inf);
	fseek(inf, 0, SEEK_SET);

	if (file_size > buf_size+1) {
		printf("ERROR: Too big PacGen names file size %ld, expected at most %d. File : %s\n", file_size, buf_size, path);
		fclose(inf);
		return ERROR_PACGEN_SCENARIO_NAMES_LOAD_BASE + ERROR_FPGE_PG2_FILE_TOO_BIG;
	}

	fread(dest_buffor,file_size,1,inf);
	dest_buffor[file_size]=0; //terminate strings
	fclose(inf);
	return 0;
}


struct carrier_str {
	int unit_id;
	int carrier_x,carrier_y;
};

int load_ext_idx(){
	FILE *inf;
	char path[MAX_PATH]="..\\data\\";
	int i;

	strncat(path,pac_ext,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf!=NULL) {

		for(i=0;i<6;i++){
			fread(&ext_idx[i],16,1,inf);
			//printf("%s %d %d\n",ext_idx[i].name,ext_idx[i].start,ext_idx[i].stop);
		}
		fclose(inf);
		return 0;
	}
	return 1;
}

int load_shp_from_pfp(char *name, int idx){

	FILE *inf;
	char path[MAX_PATH]="..\\data\\";
	char head[5];

	memset(head,0,sizeof(head));
	strncat(path,name,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf!=NULL){
		fseek(inf, idx, SEEK_END);
		fread(&head,4,1,inf);
		printf("%s\n",head);
		fclose(inf);
		return 0;
	}
	return 1;
}


int parse_txt_pfp(FILE *txt_pfp_inf, int txt_pfp_inf_off, int txt_pfp_inf_size, int group, char array[][128]){
	char *buf, line[1024];
	int ll,line_count=0,copied_line_count=0, cursor=0, line_cursor=0, group_count=0, group_state=0;

	fseek(txt_pfp_inf,txt_pfp_inf_off,SEEK_SET);
	buf= malloc(txt_pfp_inf_size);
	fread(buf,txt_pfp_inf_size,1,txt_pfp_inf);
	while(cursor<txt_pfp_inf_size){
		//read one line
		line_cursor=0;
		while(cursor<txt_pfp_inf_size && buf[cursor]!=0x0d){
			line[line_cursor]=buf[cursor];
			line_cursor++;
			cursor++;
		}
		//skip 0x0d
		if (buf[cursor]==0x0d) cursor++;
		//skip 0x0a
		if (buf[cursor]==0x0a) cursor++;
		//finish string
		line[line_cursor]=0;

		//strip comments
		for(ll=0;ll<strlen(line);ll++)
		  if (line[ll]==0x23) { line[ll]=0; break; }

		if (strlen(line)){
			if (group_state==0){
				//not yet in group, now we are in group
				group_state=1;
				group_count++;
			}

			if(group_count==group) {
				//printf("%d, >%s<\n",line_count, line);
				strncpy(array[copied_line_count],line,128);
				copied_line_count++;
			}
			line_count++;
		}else{
			if (group_state==1){
				//first empty line after group == we are not in group
				group_state=0;
			}
		}
	}
	//printf("%s",buf);

	free(buf);
	return line_count;
}

int load_pfpdata_idx(){
	FILE *pfpdata_idx_inf, *til_pfp_inf, *shp_pfp_inf, *pal_pfp_inf, *txt_pfp_inf;
	char path_idx[MAX_PATH]="..\\data\\";
	char path_til[MAX_PATH]="..\\data\\";
	char path_shp[MAX_PATH]="..\\data\\";
	char path_pal[MAX_PATH]="..\\data\\";
	char path_txt[MAX_PATH]="..\\data\\";
	char head[10],head2[10], ftoken[10], token[10];
	int x,y,i,j,k;
	int idx=0,bidx=0,size,icon_addr;
	int i_small,counter_idx;
	int color,dj;
	struct idx_struct buff;
	RGB temp_pal[256];
	int counts[NO_OF_PACGEN_TIL_CLASSES];
	BITMAP *bmp;

	int stackicn_mappings[PACGEN_STACKICN_SIZE]= //map pacgen stack icons to pg stack icons
	{0,1,2,5,3,
	 7,8,9,17,14,
	 -1,16,-1,15,13,
	 10,11,12,18,19,
	 4,6};

	strncat(path_idx,pac_pfpdata,MAX_PATH);
	strncat(path_til,pac_til,MAX_PATH);
	strncat(path_shp,pac_shp,MAX_PATH);
	strncat(path_pal,pac_pal,MAX_PATH);
	strncat(path_txt,pac_txt,MAX_PATH);

	memset(head,0,sizeof(head));
	memset(head2,0,sizeof(head2));
	memset(counts,0,sizeof(counts));

	canonicalize_filename(path_txt,path_txt,MAX_PATH);
	txt_pfp_inf=fopen(path_txt,"rb");
		if (txt_pfp_inf!=NULL) {
	canonicalize_filename(path_shp,path_shp,MAX_PATH);
	shp_pfp_inf=fopen(path_shp,"rb");
	if (shp_pfp_inf!=NULL) {
	canonicalize_filename(path_til,path_til,MAX_PATH);
	til_pfp_inf=fopen(path_til,"rb");
	if (til_pfp_inf!=NULL) {
		canonicalize_filename(path_idx,path_idx,MAX_PATH);
		pfpdata_idx_inf=fopen(path_idx,"rb");
		if (pfpdata_idx_inf!=NULL) {
			memcpy(temp_pal,pgpal,sizeof(temp_pal));
			memcpy(pgpal,pacgen_pal,sizeof(temp_pal));
			total_tiles=0;
			total_mtiles=0;
			fseek(pfpdata_idx_inf, 0, SEEK_END);
			long file_size = ftell(pfpdata_idx_inf);
			fseek(pfpdata_idx_inf, 4, SEEK_SET);
			//scan first & load palette
			printf("Scanning for tiles.\n");
			idx=0;
			for(i=0;i<file_size/16;i++){
				fread(&buff,16,1,pfpdata_idx_inf);
				strncpy(head,buff.name,8);
				//printf("%d-%d\n",ext_idx[2].start,ext_idx[2].stop);
				if (idx>=ext_idx[2].start && idx<ext_idx[2].stop)
					if ( (strcmp(&head[6],"LD")==0 || strcmp(&head[5],"LD")==0 ) && head[0]=='T'){
					//printf("%s %d %d\n",head,buff.place,buff.size);

					head[5]=0;
					for(k=0;k<no_of_sets;k++)
						if (strcmp(&head[2],class_name[k])==0) break;

					if (k==no_of_sets) break;

					fseek(til_pfp_inf,buff.place,SEEK_SET);
					fread(head2,4,1,til_pfp_inf);
					fread(&size,4,1,til_pfp_inf);
					//printf("%s %d %d\n",&head[2],k,size);

					counts[k]=size;
					}
				if (idx>=ext_idx[4].start && idx<ext_idx[4].stop){//PAL
					if ( strcmp(head,"DAY")==0 ){
						printf("Loading DAY palette.\n");
						canonicalize_filename(path_pal,path_pal,MAX_PATH);
						pal_pfp_inf=fopen(path_pal,"rb");
						if (pal_pfp_inf!=NULL){
							fseek(pal_pfp_inf,buff.place,SEEK_SET);
							fread(pgpal,256,4,pal_pfp_inf);

							unsigned char ctmp;
							for(k=0;k<256;k++){
								ctmp=pgpal[k].r;
								pgpal[k].r=pgpal[k].b;
								pgpal[k].b=ctmp;
							}

							fclose(pal_pfp_inf);
						}
					}
				}

				idx++;
			} //we have scanned file and found size of sets
			for(k=1;k<no_of_sets;k++)
				count_per_set[k]=count_per_set[k-1]+counts[k-1];

			//for(k=0;k<no_of_sets;k++)
				//printf("%s %d\n",class_name[k],count_per_set[k]);

			//now read all bmps
			idx=0;
			fseek(pfpdata_idx_inf, 4, SEEK_SET);
			for(i=0;i<file_size/16;i++){
				fread(&buff,16,1,pfpdata_idx_inf);
				strncpy(head,buff.name,8);
				strncpy(head2,buff.name,8);
				char * tok = strtok(head2, "_");
				int tokn=1;
				while (tok != NULL) {
					//printf("%s %s\n",tok,head);
					strncpy(token,tok,10);
					if (tokn==1) strncpy(ftoken,tok,10);
					tok = strtok(NULL,"_");
					if (tok!=NULL) tokn++;
				}

				//printf("%s %s\n",tok,head);
				//printf("%d-%d\n",ext_idx[2].start,ext_idx[2].stop);
				if (idx>=ext_idx[2].start && idx<ext_idx[2].stop){//TIL

					//print this only once
					if (strcmp(head,"T_CLR_DD")==0)
						printf("Loading tiles...\n");

					if (head[0]=='T')
						i_small =0;
					else
						i_small=1;

						head[5]=0;
						for(k=0;k<no_of_sets;k++)
							if (strcmp(&head[2],class_name[k])==0) break;

						if (k==no_of_sets) break;

						fseek(til_pfp_inf,buff.place,SEEK_SET);
						fread(head2,4,1,til_pfp_inf);
						fread(&size,4,1,til_pfp_inf);
						for(j=0;j<size;j++)
						{
							bidx=count_per_set[k]+j;
							bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
							for (y=0; y<TILE_HEIGHT; ++y)
							 for (x=0; x<TILE_FULL_WIDTH; ++x)
							   putpixel(bmp,x,y,fpge_mask_color);

							fseek(til_pfp_inf,buff.place+4+4+j*8,SEEK_SET);

							fread(&icon_addr,4,1,til_pfp_inf);
							icon_addr+=buff.place;
							read_header(til_pfp_inf,icon_addr);

							//printf("%d %d\n",header.lines, header.width);

							read_shp(bmp,til_pfp_inf,icon_addr);
							if (i_small){
								if (strcmp(token,"LD")==0) {mtil_bmp[bidx]=bmp;total_mtiles++;}
								if (strcmp(token,"LM")==0) mtil_bmp_mud[bidx]=bmp;
								if (strcmp(token,"LS")==0) mtil_bmp_snow[bidx]=bmp;
								if (strcmp(token,"DD")==0) dark_mtil_bmp[bidx]=bmp;
								if (strcmp(token,"DM")==0) dark_mtil_bmp_mud[bidx]=bmp;
								if (strcmp(token,"DS")==0) dark_mtil_bmp_snow[bidx]=bmp;
							}else{
								if (strcmp(token,"LD")==0) { til_bmp[bidx]=bmp;total_tiles++;}
								if (strcmp(token,"LM")==0) til_bmp_mud[bidx]=bmp;
								if (strcmp(token,"LS")==0) til_bmp_snow[bidx]=bmp;
								if (strcmp(token,"DD")==0) dark_til_bmp[bidx]=bmp;
								if (strcmp(token,"DM")==0) dark_til_bmp_mud[bidx]=bmp;
								if (strcmp(token,"DS")==0) dark_til_bmp_snow[bidx]=bmp;
							}
						}
				}// end of TIL
				if (idx>=ext_idx[1].start && idx<ext_idx[1].stop){ //SHP
					if (strcmp(head,"UNITICON")==0){
						printf("Loading UNITICON.\n");
						total_uicons=0;
						fseek(shp_pfp_inf,buff.place,SEEK_SET);
						fread(head2,4,1,shp_pfp_inf);
						fread(&size,4,1,shp_pfp_inf);
						for(j=0;j<size;j++){
							fseek(shp_pfp_inf,buff.place+4+4+j*8,SEEK_SET);
							fread(&icon_addr,4,1,shp_pfp_inf);
							icon_addr+=buff.place;
							read_header(shp_pfp_inf,icon_addr);
							int tempx=Max(header.width,TILE_FULL_WIDTH);
							int tempy=Max(header.lines,TILE_HEIGHT);
							//sometimes SHP files does not have 49x59 size but 49x181
							bmp=create_bitmap(tempx,tempy );
							for (y=0; y<tempy; ++y)
							 for (x=0; x<tempx; ++x)
							   putpixel(bmp,x,y,fpge_mask_color);
							read_shp_ex(bmp,shp_pfp_inf,icon_addr,SHP_NO_SWAP_COLOR);

							unit_bmp[j]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
							if (tempx==TILE_FULL_WIDTH)
							blit(bmp,unit_bmp[j],(header.width+1)/2-TILE_FULL_WIDTH/2,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);
							else
							stretch_blit(bmp,unit_bmp[j],header.xstart,0,header.xend-header.xstart,TILE_HEIGHT,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);

							//blit(bmp,unit_bmp[j],0,0, 0,0, TILE_FULL_WIDTH,TILE_HEIGHT);
							destroy_bitmap(bmp);
							total_uicons++;
							//printf("%d l=%d w=%d xs=%ld xe=%ld ys=%ld ye=%ld\n",j,header.lines, header.width, header.xstart, header.xend,header.ystart, header.yend);
						}
					}
					if (strcmp(head,"FLAGSLG")==0){
						printf("Loading FLAGSLG.\n");
						total_flags=0;
						fseek(shp_pfp_inf,buff.place,SEEK_SET);
						fread(head2,4,1,shp_pfp_inf);
						fread(&size,4,1,shp_pfp_inf);
						for(j=0;j<size;j++){
							fseek(shp_pfp_inf,buff.place+4+4+j*8,SEEK_SET);
							fread(&icon_addr,4,1,shp_pfp_inf);
							icon_addr+=buff.place;
							read_header(shp_pfp_inf,icon_addr);

							bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
							flag_bmp[j]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);

							for (y=0; y<TILE_HEIGHT; ++y)
							 for (x=0; x<TILE_FULL_WIDTH; ++x){
							   putpixel(bmp,x,y,fpge_mask_color);
							   putpixel(flag_bmp[j],x,y,fpge_mask_color);
							 }
							read_shp_ex(bmp,shp_pfp_inf,icon_addr,SHP_NO_SWAP_COLOR);

							//blit(bmp,flag_bmp[j],0,0,20,36,20,12);
							//blit(bmp,flag_bmp[j],0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);
							stretch_blit(bmp,flag_bmp[j],
									header.xstart,
									header.ystart,
									header.xend-header.xstart,
									header.yend-header.ystart,
									20,36,20,12);

							destroy_bitmap(bmp);
							//printf("%d l=%d w=%d xs=%ld xe=%ld ys=%ld ye=%ld\n",j,header.lines, header.width, header.xstart, header.xend,header.ystart, header.yend);

							total_flags++;
						}
						//copy flags
						for(j=0;j<size;j++){
							flag_bmp[total_flags]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
							blit(flag_bmp[j],flag_bmp[total_flags],0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);
							total_flags++;
						}
					}

					if (strcmp(ftoken,"STR")==0){
						//printf("%s, %s, %s\n",head,ftoken,token);
						counter_idx=-1;
						if (strcmp(token, "JL") == 0)//core
							{
								counter_idx = 0;
								total_strength_counters = 0;
								printf("Loading STR_*.\n");
							}
						if (strcmp(token,"JL2")==0) counter_idx=MAX_STRENGTH_IN_ROW;//aux
						if (strcmp(token,"UL")==0) counter_idx=MAX_STRENGTH_IN_ROW*2;//allied
						if (strcmp(token,"UL2")==0) counter_idx=MAX_STRENGTH_IN_ROW*3;//aux allied
						if(counter_idx!=-1){
							fseek(shp_pfp_inf,buff.place,SEEK_SET);
							fread(head2,4,1,shp_pfp_inf);
							fread(&size,4,1,shp_pfp_inf);
							for(j=0;j<size;j++){
								fseek(shp_pfp_inf,buff.place+4+4+j*8,SEEK_SET);
								fread(&icon_addr,4,1,shp_pfp_inf);
								icon_addr+=buff.place;
								read_header(shp_pfp_inf,icon_addr);

								bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
								strength_bmp[counter_idx+j+1]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);

								for (y=0; y<TILE_HEIGHT; ++y)
								 for (x=0; x<TILE_FULL_WIDTH; ++x){
								   putpixel(bmp,x,y,fpge_mask_color);
								   putpixel(strength_bmp[counter_idx+j+1],x,y,fpge_mask_color);
								 }
								read_shp_ex(bmp,shp_pfp_inf,icon_addr,SHP_NO_SWAP_COLOR);

								blit(bmp,strength_bmp[counter_idx+j+1],0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);

								if (!str0_bmp_loaded){
									if (j==9) {//make 0 from counter 10
										strength_bmp[counter_idx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
										blit(bmp,strength_bmp[counter_idx],0,0,0,0,TILE_FULL_WIDTH,TILE_HEIGHT);

										color=getpixel(strength_bmp[counter_idx],23,37);
										  rectfill(strength_bmp[counter_idx],24,38,29,46,color);
										  for(y=38;y<45;y++)
											  for(x=28;x<28+6;x++)
												  putpixel(strength_bmp[counter_idx],x,y,getpixel(strength_bmp[counter_idx],x+2,y));
									}
								}
								destroy_bitmap(bmp);
								//printf("%d l=%d w=%d xs=%ld xe=%ld ys=%ld ye=%ld\n",j,header.lines, header.width, header.xstart, header.xend,header.ystart, header.yend);

								total_flags=counter_idx+j;
							}
						}
					}

					if (strcmp(head,"STACKING")==0){
						printf("Loading STACKING.\n");
						total_sicons=0;
						fseek(shp_pfp_inf,buff.place,SEEK_SET);
						fread(head2,4,1,shp_pfp_inf);
						fread(&size,4,1,shp_pfp_inf);
						size=Min(size,(PACGEN_STACKICN_SIZE*2));
						for(j=0;j<size;j++){
							fseek(shp_pfp_inf,buff.place+4+4+j*8,SEEK_SET);
							fread(&icon_addr,4,1,shp_pfp_inf);
							icon_addr+=buff.place;
							read_header(shp_pfp_inf,icon_addr);

							dj=stackicn_mappings[j%PACGEN_STACKICN_SIZE];
							//some special cases
							if(j>=PACGEN_STACKICN_SIZE && dj==16) dj=-1; //no axis carrier

							if(dj>-1){
								if(j>=PACGEN_STACKICN_SIZE && dj>16) dj-=1; //no axis carrier, so we must shift
								if (j>=PACGEN_STACKICN_SIZE) dj+=20; //in pacgen axis stack icons are first while in pg are last, so we swap

								bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
								stack_bmp[dj]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);

								for (y=0; y<TILE_HEIGHT; ++y)
								 for (x=0; x<TILE_FULL_WIDTH; ++x){
								   putpixel(bmp,x,y,fpge_mask_color);
								   putpixel(stack_bmp[dj],x,y,fpge_mask_color);
								 }
								read_shp_ex(bmp,shp_pfp_inf,icon_addr,SHP_NO_SWAP_COLOR);

								//we flip stack icons since PG got allied icons flipped, while pacgen not
								if (j>=PACGEN_STACKICN_SIZE)
									draw_sprite(stack_bmp[dj],bmp,0,0);
								else
									draw_sprite_h_flip(stack_bmp[dj],bmp,0,0);
								total_sicons++;
								//printf("j=%d, dj=%d, total_sicons=%d\n",j,dj,total_sicons);
								//more stuff, pg got stack icons for half tracks while pacgen not, so we copy std truck
								if (j%PACGEN_STACKICN_SIZE==8){
									dj=39+(j<PACGEN_STACKICN_SIZE);
									//printf("%d\n",dj);
									stack_bmp[dj]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
									for (y=0; y<TILE_HEIGHT; ++y)
									 for (x=0; x<TILE_FULL_WIDTH; ++x){
									   putpixel(stack_bmp[dj],x,y,fpge_mask_color);
									 }

									if (j>=PACGEN_STACKICN_SIZE)
										draw_sprite(stack_bmp[dj],bmp,0,0);
									else
										draw_sprite_h_flip(stack_bmp[dj],bmp,0,0);

									total_sicons++;
									//printf("SP j=%d, dj=%d, total_sicons=%d\n",j,dj,total_sicons);
								}

								destroy_bitmap(bmp);
								//printf("%d l=%d w=%d xs=%ld xe=%ld ys=%ld ye=%ld\n",j,header.lines, header.width, header.xstart, header.xend,header.ystart, header.yend);

							}
							//printf("%d\n",j);
						}
						//printf("total_sicons=%d\n",total_sicons);
					}

				}//end of SHP

						if (idx >= ext_idx[0].start && idx < ext_idx[0].stop) { //TXT
							char array[50][128];
							int copied = 0;
							int ll;

							if (strcmp(head, "LATITUDE") == 0) {
								//Battle Generator Latitude Names
								printf("Loading LATITUDE.\n");

								//copied=
								parse_txt_pfp(txt_pfp_inf, buff.place, buff.size, 1, pacgen_weather_zones);

							}
							//country_names_short
							if (strcmp(head, "NATIONS") == 0) {
								printf("Loading NATIONS.\n");
								copied = parse_txt_pfp(txt_pfp_inf, buff.place, buff.size, 1, array);
								for (ll = 0; ll < copied; ll++) {
									strncpy(country_names_short[ll + 1], array[ll], MAX_COUNTRY_SHORT_NAME_SIZE);
									//printf("%s\n",array[ll]);
								}
							}
							//country_names
							if (strcmp(head, "BIGNATS") == 0) {
								printf("Loading BIGNATS.\n");
								copied = parse_txt_pfp(txt_pfp_inf, buff.place, buff.size, 1, array);
								for (ll = 0; ll < copied; ll++) {
									strncpy(country_names[ll + 1], array[ll], MAX_COUNTRY_NAME_SIZE);
									//printf("%s\n",array[ll]);
								}
							}
							//List of terrain types
							if (strcmp(head, "TERRAIN") == 0) {
								printf("Loading TERRAIN.\n");
								parse_txt_pfp(txt_pfp_inf, buff.place, buff.size, 1, pacgen_movement_terrain_names);
							}
							//MISC, 4th group, 11 elements, Movement Modes - 11 items.
							if (strcmp(head, "MISC") == 0) {
								printf("Loading MISC.\n");
								parse_txt_pfp(txt_pfp_inf, buff.place, buff.size, 4, pacgen_movement_type);
							}

						}


				//printf("%d\n",idx);
				idx++;
			}// end of pfpdata.idx scan
			fclose(pfpdata_idx_inf);
			fclose(til_pfp_inf);
			fclose(shp_pfp_inf);
			fclose(txt_pfp_inf);
			memcpy(pgpal,temp_pal,sizeof(temp_pal));

			return 0;
		} //(pfpdata_idx_inf!=NULL)
		fclose(til_pfp_inf);
	  } //(til_pfp_inf!=NULL)
	  fclose(shp_pfp_inf);
	} //(shp_pfp_inf!=NULL)
	fclose(txt_pfp_inf);
	}//txt_pfp_inf!=NULL
	return 1;
}

void initialize_pacgen_countries_table(){
	int i;

	for(i=0;i<32;i++){
		//strncpy(country_names[i],pacgen_country_names[i],MAX_COUNTRY_NAME_SIZE);
		//strncpy(country_names_short[i],pacgen_country_names_short[i],MAX_COUNTRY_SHORT_NAME_SIZE);
		country_active[i]=1;
	}
}

void initialize_pacgen_std_names(){
	int i;

	pacgen_std_terrain_offset=total_names;
	//print_dec(pacgen_std_terrain_offset);
	for(i=0;i<37;i++){
		strncpy(gln[total_names], pacgen_movement_terrain_names[i], MAX_NAME_SIZE);
		strncpy(gln_utf8[total_names], pacgen_movement_terrain_names[i], MAX_NAME_UTF_SIZE);
		total_names++;
		mapnames_changed = 1;
	}
}


int import_pacgen_scenario() {
	FILE *inf;
	char path[MAX_PATH];
	char path1[MAX_PATH];
	char file_name[MAX_PATH];
	unsigned char scenario_details_part1[75];
	unsigned char scenario_details_part2[91];
	int x,y,i,j,k,no_of_names;
	int ufound=0,utotal=0,notufound=0;
	int where_add_new,unum,carrier_shift;
	WORD mx,my;
	unsigned char rec_buff[PACGEN_MAP_HEX_SIZE];
	unsigned char pacgen_scn_unit_buf[PACGEN_SCN_UNIT_REC_SIZE];
	int expected_file_size;
	struct carrier_str carrrier_id[(PACGEN_MAX_CORE_UNITS+PACGEN_MAX_AUX_UNITS)*2];
	int loop=1;
    int unit_id, orgtran_id, seaair_id;

	for (i=0;i<(PACGEN_MAX_CORE_UNITS+PACGEN_MAX_AUX_UNITS)*2;i++) carrrier_id[i].unit_id=-1;
//	return D_REDRAW;

	if (tile_mode == 1) {
		tile_cleanup();
	}

	strncpy(path, ".\\", sizeof(path) - 1);
	canonicalize_filename(path,path,MAX_PATH);
	pacgen_dlg[4].dp=&path;
	centre_dialog(pacgen_dlg);
    while (loop){
    	i=do_dialog(pacgen_dlg,-1);
    	if (i==2 || i==3 || i==-1) loop=0;
    	if (i==7) {
    		file_select_ex("Select MAP or SCN file",path,"map;scn;",MAX_PATH,OLD_FILESEL_WIDTH,OLD_FILESEL_HEIGHT);
    	}
    }

	//strncpy(path, ".\\", sizeof(path) - 1);
	//if (file_select_ex("Select MAP file", path, "map;scn;", 1024, OLD_FILESEL_WIDTH,
	//		OLD_FILESEL_HEIGHT))
    if (i == 3)
	{
    	canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (inf!=NULL) {
			fseek(inf, 0, SEEK_END);
			long file_size = ftell(inf);
			fseek(inf, 0, SEEK_SET);
			fread(&mx,2,1,inf);
			fread(&my,2,1,inf);

			//printf("file size: %ld\n",file_size);
			//printf("file size - map: %ld\n",file_size-(4+PACGEN_MAP_HEX_SIZE*mx*my+105));
			int map_rest=file_size-(4+PACGEN_MAP_HEX_SIZE*mx*my+101);
			//printf("map rest: %d\n",map_rest);

			int rest=file_size-(4+PACGEN_MAP_HEX_SIZE*mx*my+105 + 0x291a);
			expected_file_size=4+mx*my*37+101+0x291a+218*6*200+132;
			//printf("file size - map-units offset: %d\n",rest);
			//printf("file size - map-units offset / 218: %d:%d\n",rest/218, rest %218);

			if (file_size == expected_file_size) {
				printf("Standard scenario\n");
			}else{
				if (map_rest==0){
					printf("Standard map\n");
					rest=0;
				}else{
					printf("User made scenario\n");
					fread(&no_of_names,4,1,inf);
					fseek(inf, no_of_names*30, SEEK_CUR);
					expected_file_size+=4+no_of_names*30+48;
					if (file_size == expected_file_size) {
						printf("ok\n");
					}else{
						printf("nok : %ld\n",expected_file_size-file_size);
					}
				}
			}

			strncpy(file_name,get_filename(path),MAX_PATH);
			char *str_ptr=strchr(file_name,'.');
			if (str_ptr!=NULL) *str_ptr=0;
			setScenarioNameRaw(file_name);

			if (file_size>=4+PACGEN_MAP_HEX_SIZE*mx*my){

				//names
				replace_extension(path1,path,"tst",MAX_PATH);
				strncpy(file_name,"s",MAX_PATH);
				strncat(file_name,get_filename(path1),MAX_PATH);
				replace_filename(path1,path,file_name,MAX_PATH);
				canonicalize_filename(path1,path1,MAX_PATH);
				//print_str(path1);
				import_pacgen_names_file(path1);

				//now scenario title and description
				replace_extension(path1,path,"tit",MAX_PATH); //tit for title
				canonicalize_filename(path1,path1,MAX_PATH);
				import_pacgen_string_file(path1, block1_Name,sizeof(block1_Name));

				replace_extension(path1,path,"des",MAX_PATH); //des for description
				canonicalize_filename(path1,path1,MAX_PATH);
				import_pacgen_string_file(path1, block1_Description,sizeof(block1_Description));

				for (i = 0; i < MAX_NAMES; i++)
					pacgen_gln_map[i] = -1;

				mapx=mx;
				mapy=my;
				//printf("Start reading map (%d,%d)\n",mapx,mapy);
				map_x0=0;
				map_y0=0;

				for(y=0;y<my;y++)
					for(x=0;x<mx;x++)
					{
						fread(&rec_buff,PACGEN_MAP_HEX_SIZE,1,inf);
						memset(&map[x][y],0, sizeof(map[x][y]));
						map[x][y].guidx=-1;
						map[x][y].auidx=-1;
						//printf("%d,",rec_buff[15]);
						//if (x==mx-1)printf("\n");
						if (pacgen_mode){
							map[x][y].tile=count_per_set[rec_buff[15-1]+1]+rec_buff[15-3];
							map[x][y].rc = rec_buff[15-4];
							map[x][y].utr = TTData_Max_Tiles[pacgen_tt_to_pg_tile[rec_buff[15]]];
							map[x][y].own = rec_buff[15+1]!=0xff?rec_buff[15+1]+1:0;
							if (rec_buff[3]) map[x][y].vic=1;
							map[x][y].deploy=rec_buff[15+8]?1:0;
						}
						else{
							map[x][y].tile=pacgen_tt_to_pg_tile[rec_buff[15]];
							map[x][y].rc = rec_buff[15-4];
							map[x][y].own = rec_buff[15+1]!=0xff?rec_buff[15+1]+1:0;
							if (rec_buff[3]) map[x][y].vic=1;
							map[x][y].deploy=rec_buff[15+8]?1:0;
						}
						//printf("%d,",map[x][y].tile);
						//if (x==mx-1)printf("\n");
						if (pacgen_dlg[6].flags & D_SELECTED){
							int current_pacgen_name = rec_buff[0]+rec_buff[1]*256;
							if (current_pacgen_name != 0x7fff && current_pacgen_name<total_pacgen_names) {
								if (pacgen_gln_map[current_pacgen_name] == -1) {
									//print_dec(current_pg2_name);
									//we need to copy it, clipped to MAX_NAME_SIZE=20 | 40 !!!!
									strncpy(gln[total_names], pacgen_gln[current_pacgen_name], MAX_NAME_SIZE);
									strncpy(gln_utf8[total_names], pacgen_gln[current_pacgen_name], MAX_NAME_UTF_SIZE);
									pacgen_gln_map[current_pacgen_name] = total_names;
									total_names++;
									mapnames_changed = 1;


									//print_str(pg2_gln[current_pg2_name]);
								}
								map[x][y].gln = pacgen_gln_map[current_pacgen_name];
								/*
								if(rec_buff[3]){
								printf("(%02d,%2d) :",x,y);
								for(i=0;i<37;i++)
									printf("%02x,",rec_buff[i]);
								printf("\n");
								}
								*/
						}else{
								if (current_pacgen_name == 0x7fff && pacgen_std_terrain_offset>-1){
										//first names are just names of the terrain
										//print_dec(rec_buff[15]);
										map[x][y].gln = pacgen_std_terrain_offset+rec_buff[15];
									}
							}
						}
					}

				clear_all_units();

//				printf("file size - map-units offset: %d-%d=%d\n",rest,1200*PACGEN_SCN_UNIT_REC_SIZE+128,rest - (1200*PACGEN_SCN_UNIT_REC_SIZE+128));
				//check if this file is a scenario file
				if (rest==(int)6*(PACGEN_MAX_CORE_UNITS+PACGEN_MAX_AUX_UNITS)*PACGEN_SCN_UNIT_REC_SIZE+128){
					  printf("Reading scenario details.\n");

					struct conversion_equipment search;
					struct conversion_equipment *found;

					memset(scn_buffer,0,MAX_SCN_SIZE);
					memset(s4_buffer,0,16);

					fseek(inf, 101, SEEK_CUR);
					fread(scenario_details_part1,75,1,inf);
					fseek(inf, 0x291a-75-91, SEEK_CUR);
					fread(scenario_details_part2,91,1,inf);

					for(i=0;i<2;i++) //two sides only
					{
						//copy sides
						for(j=0;j<scenario_details_part1[6*5+i];j++)
							scn_buffer[j*2+i]=scenario_details_part1[6*j+i]+1;
						k=6*6+1; //index to next info
						//starting cash
						s4_buffer[AXIS_PRESTIGE+i*2]=scenario_details_part1[k+i*2];
						s4_buffer[AXIS_PRESTIGE+i*2+1]=scenario_details_part1[k+i*2+1];
						k+=6*2+6;
						//number of air sea transport
						s4_buffer[AXIS_AIR_NUMBER+i] = scenario_details_part1[k+i];
						s4_buffer[AXIS_SEA_NUMBER+i] = scenario_details_part1[k+i+6];
					}
					//icon orientations
					scn_buffer[ORIENTATION]=scenario_details_part1[6*5];
					//weather
					scn_buffer[STORM_FRONT]=scenario_details_part1[6*11+1];
					scn_buffer[DAY]=scenario_details_part1[6*11+3];
					scn_buffer[MONTH]=scenario_details_part1[6*11+4];
					scn_buffer[YEAR]=scenario_details_part1[6*11+5];

					scn_buffer[TURNS]=scenario_details_part1[6*11+6];
					scn_buffer[TURNS_PER_DAY]=scenario_details_part1[6*11+7];
					scn_buffer[DAYS_PER_TURN]=scenario_details_part1[6*11+8];

					for(i=0;i<2;i++) //two sides only
					{
						//air type
						//TODO convert, something is wrong here
						if (s4_buffer[AXIS_AIR_NUMBER+i]>0){
							//s4_buffer[AXIS_AIR_TYPE+i*2]=scenario_details_part1[11*6+1+i*2];
							//s4_buffer[AXIS_AIR_TYPE+i*2+1]=scenario_details_part1[11*6+1+i*2+1];
						}
						//sea type
						if (s4_buffer[AXIS_SEA_NUMBER+i]>0){
							//s4_buffer[AXIS_SEA_TYPE+i*2]=scenario_details_part1[11*6+1+i*2+6*2];
							//s4_buffer[AXIS_SEA_TYPE+i*2+1]=scenario_details_part1[11*6+1+i*2+6*2+1];
						}
					}
					//TODO
					for(i=0;i<75;i++) printf("%5d", scenario_details_part1[i]);
					printf("\n");
					for(i=0;i<91;i++) printf("%5d", scenario_details_part2[i]);
					printf("\n");


					//clean units numbers
					clear_all_units();

					printf("Reading units\n");
					for(i=0;i<PACGEN_MAX_CORE_UNITS+PACGEN_MAX_AUX_UNITS;i++)
						for(j=0;j<6;j++){
							//load units
							fread(&pacgen_scn_unit_buf,PACGEN_SCN_UNIT_REC_SIZE,1,inf);
							unit_id = (unsigned int)pacgen_scn_unit_buf[41]+
									  (unsigned int)pacgen_scn_unit_buf[41+1]*256;

							orgtran_id = (unsigned int)pacgen_scn_unit_buf[43]+
									     (unsigned int)pacgen_scn_unit_buf[43+1]*256;

							seaair_id = (unsigned int)pacgen_scn_unit_buf[45]+
									    (unsigned int)pacgen_scn_unit_buf[45+1]*256;

							//32 max country in pacgen

							if (pacgen_scn_unit_buf[55]>0 && unit_id !=0xffff){
								/*
								printf("name,X,Y,own : >%s<,%d,%d,%d\n",strchr(pacgen_scn_unit_buf,' ')+1,
										pacgen_scn_unit_buf[51]+pacgen_scn_unit_buf[52]*256,
										pacgen_scn_unit_buf[53]+pacgen_scn_unit_buf[54]*256,
										pacgen_scn_unit_buf[55]);
								 */
								//printf("%d,%d,%d\n",unit_id,orgtran_id,seaair_id);

							if (pacgen_scn_unit_buf[0]>='0' && pacgen_scn_unit_buf[0]<='9')
								strncpy(search.name,strchr(pacgen_scn_unit_buf,' ')+1,39);
							else
								strncpy(search.name,pacgen_scn_unit_buf,39);
							search.name[39]=0;
							search.new_id = 0;
							search.old_id = unit_id;
							search.country = pacgen_scn_unit_buf[40]+1;

							//printf("search : name country old new : %s,%d,%d,%d\n",search.name,search.country,search.old_id,search.new_id);

							found=(struct conversion_equipment *)bsearch(
									&search,
									conversion_equip,
									conversion_total_equip,
									sizeof(struct conversion_equipment),
									compare_by_old_id_and_country
									);
							if (!found){
									found=(struct conversion_equipment *)bsearch(
																		&search,
																		conversion_equip,
																		conversion_total_equip,
																		sizeof(struct conversion_equipment),
																		compare_by_old_id
																		);

									//if (found)
									//	printf("search again: name: %s old country: %d new country : %d oldID: %d newID: %d\n",search.name,search.country,found->country,search.old_id,found->new_id);


								}

								utotal++;
								if (found){
									//place unit

									  if (j%2==1){ //allied
									    	if (i>=PACGEN_MAX_CORE_UNITS){ //aux
									    		carrier_shift=PACGEN_MAX_CORE_UNITS*2+PACGEN_MAX_AUX_UNITS;
									    		where_add_new=total_axis_core+total_axis_aux+total_allied_core+total_allied_aux;
									    		add_unit(where_add_new);
									    		all_units[where_add_new].uflag=allied_aux;
									    		total_allied_aux++;
									    	}
									    	else{
									    		carrier_shift=PACGEN_MAX_CORE_UNITS+PACGEN_MAX_AUX_UNITS;
									    		where_add_new=total_axis_core+total_axis_aux+total_allied_core;
									    		add_unit(where_add_new);
									    		all_units[where_add_new].uflag=allied_core;
									    		total_allied_core++;
									    	}
									    }
									    //if (j%2==0)
									    else{ //AXIS
									    	if (i>=PACGEN_MAX_CORE_UNITS){ //aux
									    		carrier_shift=PACGEN_MAX_CORE_UNITS;
									    		where_add_new=total_axis_core+total_axis_aux;
									    		add_unit(where_add_new);
									    		all_units[where_add_new].uflag=axis_aux;
									    		total_axis_aux++;
									    	}
									    	else{
									    		carrier_shift=0;
									    		where_add_new=total_axis_core;
									    		add_unit(where_add_new);
									    		all_units[where_add_new].uflag=axis_core;
									    		total_axis_core++;
									    	}
									    }
									    carrier_shift=(j%2==0?0:PACGEN_MAX_CORE_UNITS+PACGEN_MAX_AUX_UNITS);
									/*
									if (j%2==0){
							    		where_add_new=total_axis_core;
							    		add_unit(where_add_new);
							    		all_units[where_add_new].uflag=axis_core;
							    		total_axis_core++;
									}
									else{
							    		where_add_new=total_axis_core+total_axis_aux+total_allied_core;
							    		add_unit(where_add_new);
							    		all_units[where_add_new].uflag=allied_core;
							    		total_allied_core++;
									}
									*/
										//printf("where_add_new=%d type=%d\n",where_add_new,all_units[where_add_new].uflag);
									    unum = found->new_id;
									    all_units[where_add_new].unum=unum;
									   // all_units[where_add_new].orgtnum=atoi(tokens[2]);

									    if (found->country!=-1)
									    	all_units[where_add_new].country=found->country;
									    else
									    	all_units[where_add_new].country=pacgen_scn_unit_buf[40]+1;

									    // all_units[where_add_new].auxtnum=atoi(tokens[3]);
									    //error=sscanf(tokens[0],"(%d:%d)",&x,&y);

									    x=pacgen_scn_unit_buf[51]+pacgen_scn_unit_buf[52]*256;
									    y=pacgen_scn_unit_buf[53]+pacgen_scn_unit_buf[54]*256;

									    for(k=0;k<8;k++)
									    	if (pacgen_scn_unit_buf[112+k]<255) {
									    		carrrier_id[carrier_shift+pacgen_scn_unit_buf[112+k]].unit_id=i;
									    		carrrier_id[carrier_shift+pacgen_scn_unit_buf[112+k]].carrier_x=x;
									    		carrrier_id[carrier_shift+pacgen_scn_unit_buf[112+k]].carrier_y=y;
									    	}
									    //change x,y when on carrier
									    if (carrrier_id[carrier_shift+i].unit_id>-1){
										    //set carrier x,y
										    x=carrrier_id[carrier_shift+i].carrier_x;
										    y=carrrier_id[carrier_shift+i].carrier_y;
									    }
									    //set first x,y
										all_units[where_add_new].x = x;
										all_units[where_add_new].y = y;
									    //find new x,y
										find_xy(where_add_new,equip[found->new_id][GAF],&x,&y);
										//set new x,y
										all_units[where_add_new].x = x;
										all_units[where_add_new].y = y;

								        if (equip[found->new_id][GAF]) //1 if air unit
								          map[x][y].auidx=where_add_new;
								        else
								          map[x][y].guidx=where_add_new;

									    all_units[where_add_new].str=pacgen_scn_unit_buf[55];
									    all_units[where_add_new].entrench=pacgen_scn_unit_buf[55+2+4];
									    all_units[where_add_new].exp=pacgen_scn_unit_buf[55+2+4+2+1];

									    all_units[where_add_new].fuel=-1;
									    all_units[where_add_new].ammo=-1;

									    if (orgtran_id>-1 && orgtran_id<conversion_total_equip && orgtran_id!=9999){
											search.new_id = 0;
											search.old_id = orgtran_id;

											found=(struct conversion_equipment *)bsearch(
													&search,
													conversion_equip,
													conversion_total_equip,
													sizeof(struct conversion_equipment),
													compare_by_old_id_and_country
													);
											if (!found) {
											found =
													(struct conversion_equipment *) bsearch(
															&search,
															conversion_equip,
															conversion_total_equip,
															sizeof(struct conversion_equipment),
															compare_by_old_id);
										}
											if (found){
									    		all_units[where_add_new].orgtnum=found->new_id;
									    		//printf("Found organic transport of unit : >%s< old orgtran ID =%d, country=%d\n, new orgtran ID =%d\n",pacgen_scn_unit_buf,search.old_id,search.country,found->new_id);
									    	}else{
												printf("Search failed for organic transport of unit : >%s< orgtran ID =%d, country=%d\n",pacgen_scn_unit_buf,search.old_id,search.country);
									    	}
									    }

									    if (seaair_id>-1 && seaair_id<conversion_total_equip && seaair_id!=9999){
											search.new_id = 0;
											search.old_id = seaair_id;

											found=(struct conversion_equipment *)bsearch(
													&search,
													conversion_equip,
													conversion_total_equip,
													sizeof(struct conversion_equipment),
													compare_by_old_id_and_country
													);
											if (!found) {
											found =
													(struct conversion_equipment *) bsearch(
															&search,
															conversion_equip,
															conversion_total_equip,
															sizeof(struct conversion_equipment),
															compare_by_old_id);
											}
									    	if (found){
									    		all_units[where_add_new].auxtnum=found->new_id;
									    		//printf("Found sea&air transport of unit : >%s< old sea&air ID =%d, country=%d\n, new sea&air ID =%d\n",pacgen_scn_unit_buf,search.old_id,search.country,found->new_id);
									    	}else{
												printf("Search failed for sea&air transport of unit : >%s< sea&air ID =%d, country=%d\n",pacgen_scn_unit_buf,search.old_id,search.country);
									    	}
									    }
								    	//printf("NEW : %d,%d,%d\n",all_units[where_add_new].unum,all_units[where_add_new].orgtnum,all_units[where_add_new].auxtnum);

									    total_units++;

									ufound++;
									//printf("Found old new : %d, %d, %d\n",found->old_id,found->new_id,found->country);
								}else{
									notufound++;
									printf("Search failed for : >%s< - >%s<, %d, %d\n",pacgen_scn_unit_buf,search.name,search.old_id,search.country);
								}
								/*
								for(k=0;k<conversion_total_equip;k++)
									if ( search.old_id == conversion_equip[k].old_id && search.country==conversion_equip[k].country){
										   printf("conversion_equip : name country old new : %d,%s,%d,%d,%d\n",
												   k,conversion_equip[k].name,
													conversion_equip[k].country,
													conversion_equip[k].old_id,
													conversion_equip[k].new_id);
										   break;
									}
								*/
							}
						}

					scn_buffer[CORE_UNITS]=total_axis_core;
					scn_buffer[ALLIED_UNITS]=total_allied_core;
					scn_buffer[AUX_UNITS]=total_axis_aux;
					scn_buffer[AUX_ALLIED_UNITS]=total_allied_aux;

					printf("Read : axis core %d aux %d, allied core %d aux %d\n",total_axis_core,total_axis_aux,total_allied_core,total_allied_aux);
				}
				  // printf("t=%d f=%d nf=%d\n",utotal,ufound,notufound);

				main_dlg[dmHSlideIdx].d1=get_h_slide_max();
				main_dlg[dmHSlideIdx].d2=0;
				vslide_max=get_v_slide_max();
				main_dlg[dmVSlideIdx].d1=vslide_max;
				main_dlg[dmVSlideIdx].d2=vslide_max;

				//printf("dm[dmHSlideIdx].d1=%d dm[dmHSlideIdx].d2=%d dm[dmVSlideIdx].d1=%d\n",dm[dmHSlideIdx].d1,dm[dmHSlideIdx].d2,dm[dmVSlideIdx].d1);

				//finally draw map
				draw_map(map_bmp,map_x0,map_y0,tiles_high,tiles_wide);

			}else{
				printf("Wrong PacGen map file size %ld, expected at least %d.\n",file_size,4+PACGEN_MAP_HEX_SIZE*mx*my);
			}
			fclose(inf);
		}else{
			printf("Cannot open PacGen map or scenario file : %s\n",path);
		}
	}
	//printf("End\n");
	return D_REDRAW;

}

int load_pacgen_file(char *filename_to_load, int file_type, int key_limit, int error_base){

	FILE *inf;

		char line[1024],tokens[50][256];
		int id,i,cursor=0,token=0,lines;
		char path[MAX_PATH];

		strncpy(path,filename_to_load,MAX_PATH);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (inf == NULL) return error_base+ERROR_FPGE_FILE_NOT_FOUND;

		lines=0;

		//count lines so error can be displayed with line number
		while(read_utf16_line_convert_to_utf8(inf,line)>=0){

			lines++;

			//strip comments
			for(i=0;i<strlen(line);i++)
				if (line[i]==0x23) { line[i]=0; break; }
			//tokenize
			token=0;
			cursor=0;
			for(i=0;i<strlen(line);i++)
				if (line[i]==0x09) {tokens[token][cursor]=0;token++;cursor=0;}
				else {tokens[token][cursor]=line[i]; cursor++;}
			tokens[token][cursor]=0;
			token++;

			id=atoi(tokens[0]);
			if (id>0 && id <=key_limit && token>=2){
				switch(file_type){
					case LOAD_PACGEN_MODE_TT2TILES: pacgen_tt_to_pg_tile[id-1]=atoi(tokens[1]); break;
					case LOAD_PACGEN_CLASSES: pacgen_class_2_pg_class[id]=atoi(tokens[1]); break;
					case LOAD_PACGEN_MOVE_TYPES: pacgen_move_2_pg_move[id]=atoi(tokens[1]); break;
				}

			}else{
				if (id!=0)
					printf("Wrong line : %s\n",line);
			}
		}
		fclose(inf);

		//for(i=0;i<NO_OF_PACGEN_TERRAIN_TYPES;i++)
		//	printf("%d %d\n",i,pacgen_tt_to_pg_tile[i]);

		return 0;
}


int load_pacgen_tt2tiles(){
	return load_pacgen_file(pac_tt2tiles,LOAD_PACGEN_MODE_TT2TILES,NO_OF_PACGEN_TERRAIN_TYPES,ERROR_TT2TILES_FILE_BASE);
}

int load_pacgen_cl2pg_cl(){
	return load_pacgen_file(pac_cl2pg_cl,LOAD_PACGEN_CLASSES,NO_OF_PACGEN_CLASSES,ERROR_PACGEN_CL2PG_CL_FILE_BASE);
}

int load_pacgen_mt2pg_mt(){
	return load_pacgen_file(pac_mt2pg_mt,LOAD_PACGEN_MOVE_TYPES,NO_OF_PACGEN_MOVE_TYPES,ERROR_PACGEN_MT2PG_MT_FILE_BASE);
}


int load_pacgen_equip()
{
   int i,j,k,ax,pic_num;
   unsigned int c;
   short no_of_equip;
   struct pacgen_equ pacgen_equip;
   char flipped[MAX_UICONS];

   FILE *inf,*inf2;
	char path[MAX_PATH]="..\\data\\";
	char path2[MAX_PATH]="..\\data\\";
	char name[1024];

	memset(flipped,0,sizeof(flipped));
	strncat(path,pac_pacequip,MAX_PATH);
	strncat(path2,pac_pacequip_txt,MAX_PATH);

   //inf=fopen(equip_file,"rb");
   canonicalize_filename(path,path,MAX_PATH);
   inf=fopen(path,"rb");
   if (inf!=NULL)
   {
	   canonicalize_filename(path2,path2,MAX_PATH);
	   inf2=fopen(path2,"rt");
	   if (inf2!=NULL)
	      {
		   fread(&no_of_equip,2,1,inf);
		   total_equip=0;
		   conversion_total_equip=0;
		    //printf("no_of_equip=%d\n",no_of_equip);
		   //printf("sizeof(pacgen_equip)=%d\n",sizeof(pacgen_equip));
		   //int max=0;
		   for(i=0;i<no_of_equip;i++){
			   fread(&pacgen_equip,PACGEN_EQUIP_REC_SIZE,1,inf);
			   //printf("fread(pacgen_equip,PACGEN_EQUIP_REC_SIZE,1,inf)=%d\n",readbytes);
			   fgets(name,sizeof(name),inf2);
			   if (name[strlen(name)-1]=='\n') name[strlen(name)-1]=0;

			   //if (strlen(name)>max) max=strlen(name);

			   c = (int)pacgen_equip.countries[0]+
				   (int)(pacgen_equip.countries[1]<<8)+
				   (int)(pacgen_equip.countries[2]<<16)+
				   (int)(pacgen_equip.countries[3]<<24);

			   //check if given equipment is axis or allied.
			   ax=0;
			   //int mask=0x44006280; //
			   int mask=0x47006284;

			   if (c & mask) ax=1;
			   if (c & ~mask) ax+=2;

			   //if (i==667) printf("Code i=%d c=%x\n",i,c);
			   //if (ax==3) printf("Code i=%d\n",i);

			   //j=-2;
			   if (c==0) c=0x80000000;

			   for(j=0;j<32;j++)
				   if (c & (1<<j))
					   {
					   //if (i==667) printf("%d %d\n",total_equip,j+1);

				   memset(equip[total_equip],0,EQUIP_REC_SIZE);
				   equip_flags[total_equip]=0;
				   strncpy(equip[total_equip],name,19); //copy up to 19 chars
				   convert_from_cp1250_to_utf8(equip_name_utf8[total_equip],equip[total_equip],20);
				   //TODO: convert country
				   if (c==0x80000000)
					   equip_country[total_equip]=-1;
				   else
					   equip_country[total_equip]=j+1;

				   equip[total_equip][CLASS]=pacgen_class_2_pg_class[pacgen_equip.class];
				   //special case for Armory efile "Coastal Battery" are in battle ship class due to design of efile,
				   //http://panzercentral.com/forum/viewtopic.php?f=27&t=47084#p669578
				   if (equip[total_equip][CLASS]==CLASS_CAPITAL_SHIP && strstr(equip[total_equip],"Coastal Battery")!=NULL)
					   equip[total_equip][CLASS]=CLASS_FORTIFICATION;

				   equip[total_equip][SA]=pacgen_equip.sa;
				   equip[total_equip][HA]=pacgen_equip.ha;
				   equip[total_equip][AA]=pacgen_equip.aa;
				   equip[total_equip][NA]=pacgen_equip.na;
				   equip[total_equip][GD]=pacgen_equip.gd;
				   equip[total_equip][AD]=pacgen_equip.ad;
				   equip[total_equip][CD]=pacgen_equip.assault_def;
				   equip[total_equip][TARGET_TYPE]=pacgen_target_type_2_pg_target_type[pacgen_equip.target_type];
				   equip[total_equip][INITIATIVE]=pacgen_equip.ini;
				   equip[total_equip][RANGE]=pacgen_equip.range;
				   equip[total_equip][SPOTTING]=pacgen_equip.spotting;
				   //special case for Armory efile "recon seaplanes" are in fighter class due to design of efile,
				   //http://panzercentral.com/forum/viewtopic.php?f=27&t=47084#p669578

				   if (equip[total_equip][CLASS]==CLASS_FIGHTER &&  equip[total_equip][SPOTTING]>4)
					   equip[total_equip][CLASS]=CLASS_TACTICAL_BOMBER;
				   equip[total_equip][GAF]=pacgen_equip.move_med;
				   equip[total_equip][MOV_TYPE]=pacgen_move_2_pg_move[pacgen_equip.move_type];
				   equip[total_equip][MOV]=pacgen_equip.mov;
				   equip[total_equip][FUEL]=pacgen_equip.fuel;
				   equip[total_equip][AMMO]=pacgen_equip.amo;
				   equip[total_equip][COST]=pacgen_equip.prestige;
				   equip[total_equip][BMP]=pacgen_equip.pic_num[0];
				   equip[total_equip][BMP+1]=pacgen_equip.pic_num[1];
				   equip[total_equip][MON]=pacgen_equip.month;
				   equip[total_equip][YR]=pacgen_equip.year;
				   equip[total_equip][LAST_YEAR]=pacgen_equip.last_year-1; // -1 by Radoye  : http://panzercentral.com/forum/viewtopic.php?f=27&t=47084&view=unread#p669575
				   equip[total_equip][ALLOWED_TRANSPORT]=pacgen_equip.transport;

				   //conversion table
					convert_from_cp1250_to_utf8(conversion_equip[total_equip].name,equip[total_equip],20);
					conversion_equip[total_equip].country=equip_country[total_equip]; //any country
					conversion_equip[total_equip].new_id=total_equip; //New ID
					conversion_equip[total_equip].old_id=i; //Original ID

					for(k=0;k<4;k++){
						switch (pacgen_equip.special[k]){
							case 5://Pure Tac
								equip[total_equip][AAF]=1;
								break;
							case 3://Engineer
								equip_flags[total_equip]|=EQUIPMENT_CAN_BRIDGE_RIVERS;
								break;
							case 4://Bunker Killer
							case 12://Ranger
								equip_flags[total_equip]|=EQUIPMENT_IGNORES_ENTRENCHMENT;
								break;
						}
					}

/*
					printf("conversion_equip : name: %s country: %d oldID: %d newID: %d\n",
							conversion_equip[total_equip].name,
							conversion_equip[total_equip].country,
							conversion_equip[total_equip].old_id,
							conversion_equip[total_equip].new_id);
*/
					conversion_total_equip++;
				    total_equip++;
			   }

			   if (c != 0 ){
				   //total_equip-1 makes sense only when there was at least one new equipment
				   pic_num=(int)equip[total_equip-1][BMP]+equip[total_equip-1][BMP+1]*256;
				   //print_dec(pic_num);
				   //flip icon when necessary, we do not want to flip it twice...
				   if (ax==1 && flipped[pic_num]==0 && unit_bmp[pic_num]!=NULL){
					   //print_str("flip");
					   BITMAP *u_bmp_tmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
					   rectfill(u_bmp_tmp,0,0,TILE_FULL_WIDTH,TILE_HEIGHT,fpge_mask_color);
					   draw_sprite_h_flip(u_bmp_tmp,unit_bmp[pic_num],0,0);
					   destroy_bitmap(unit_bmp[pic_num]);
					   unit_bmp[pic_num]=u_bmp_tmp;
					   flipped[pic_num]=1;
				   }
			   }
			   //printf("%s\n",name);

			   //printf("%d %d %d %d %d %d %d\n", pacgen_equip.countries,  pacgen_equip.move_type, pacgen_equip.life, pacgen_equip.prestige, pacgen_equip.pic_num[0], pacgen_equip.attack_sound,pacgen_equip.death_sound);
			   //printf("pr=%d pic_num=%d uk4=%d as=%d\n", equip.prestige, 256*equip.pic_num[1]+ equip.pic_num[0], equip.uk4[0],equip.attack_sound);
		   }

		   //sort conversion table
		   qsort(conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_old_id_and_country);

		   //print_dec(conversion_total_equip);
		   //print_dec(total_equip);


		   //all ok
			fclose(inf2);
			fclose(inf);
			return 0;
	      }
	   fclose(inf);
	   return 1;
   }
   return 1;
}
