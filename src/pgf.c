/*
 * PGF - PG Forever load/save routines
 *
 *  Created on: 2010-06-07
 *      Author: wino
 */

#include <stdio.h>
#include "fpge.h"
#include "load.h"
#include "save.h"
#include "filename.h"
#include "fpge_colors.h"
#include "maingui.h"
#include "minimap.h"
#include "unitutil.h"
#include "tables.h"
#include "pgf.h"
#include "hash.h"
#include "app6.h"
#include "pg.h"
#include "lgeneral.h"

unsigned short UCS2_header=0xfeff;

unsigned char block1_Name[256]="CHANGEME";
unsigned char block1_Description[1024]="CHANGEME";
unsigned char block1_SET_file[256];
unsigned char block1_Max_Unit_Strength[256]="15";
unsigned char block1_Max_Unit_Experience[256]="599";
unsigned char block1_Allies_Move_First[64]="0";

unsigned short axis_experience=200;
unsigned short allied_experience=200;
unsigned short allies_move_first;

unsigned short axis_experience_table[] =
{
0,1,1,0,1,1,1,1,
1,2,2,3,2,2,2,2,
2,2,1,2,1,1,2,2,
2,2,2,2,2,3,2,2,
2,1,1,2,2,1
};

unsigned short allied_experience_table[] =
{
0,0,0,0,0,1,1,1,
1,1,1,1,1,2,2,2,
2,2,2,2,1,1,0,0,
0,1,1,1,1,2,2,2,
2,2,2,2,0,1
};

char axis_victory_table[][128]={
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t0\t3\t(27:5)",
"AXIS VICTORY\t0\t2\t",
"AXIS VICTORY\t0\t3\t(13:17)",
"AXIS VICTORY\t0\t2\t(11:7)(22:28)(43:27)",
"AXIS VICTORY\t0\t2\t",
"AXIS VICTORY\t0\t7\t(16:16)(26:4)(27:21)(39:21)(48:8)(54:14)(59:18)",
"AXIS VICTORY\t0\t3\t(50:5)(50:22)(44:35)(8:42)",
"AXIS VICTORY\t0\t2\t(37:10)(38:11)",
"AXIS VICTORY\t0\t6\t(36:14)",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t0\t1\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t0\t1\t(3:12)",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t0\t6\t(36:14)",
"AXIS VICTORY\t0\t1\t(35:14)",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t",
"AXIS VICTORY\t1\t0\t"
};

char default_purchasable_classes[]={
 1,1,1,1,1,
 1,1,0,1,1,
 1,0,0,0,0,
 1
};

unsigned short axis_prestige_allotments[PRESTIGE_ALLOTMENTS_NUMBER]={
0,0,0,0,0,0,0,0,
0,0,0,0,142,94,140,96,
0,96,122,140,0,0,0,0,
0,0,0,0,0,0,0,94,
0,140,164,0,0,0
};

unsigned short allied_prestige_allotments[PRESTIGE_ALLOTMENTS_NUMBER]={
40,40,80,98,90,168,90,140, //8
126,170,206,0,0,0,0,0, //16
150,0,0,0,202,90,120,160, //24
230,126,210,190,110,230,244,0, //32
0,0,0,230,270,170 // elements38
};

unsigned char block4[MAX_TURNS][MAX_LINE_SIZE]; //block 4 storage
int block4_lines;
unsigned char block5[MAX_SUPPLY][MAX_LINE_SIZE]; //block 5 storage
int block5_lines;
unsigned char block7[MAX_VICTORY_CON][MAX_LINE_SIZE]; //block 7 storage
int block7_lines;
unsigned char block9[MAX_CLASSES][MAX_LINE_SIZE]; //block 9 storage
int block9_lines;

int read_utf16_line_convert_to_utf8(FILE *inf, unsigned char *line){

	int gcursor=0;
	int eol=0,eof=0;
	unsigned short u1, u2;
	short digit;

	//we expect 2 bytes per char
	while(!eol){
		eof=fread(&digit,sizeof(digit),1,inf);
		if (digit == 0x0d) continue; //ignore 0d
		if (digit == 0x0a || eof==0) {
			line[gcursor]=0;
			eol=1;
			if (gcursor==0) eof-=1;// eof may be 1 or 0 -> 0 or -1
			//printf("eof=%d:>>>>>%s<<<<<\n",eof,line);

			return eof;
		}
		//normal char
		if (digit>0 && digit<0x80) {
		  //protect from longlines
		  if (gcursor<1024){
			  line[gcursor]=(unsigned char)digit;
			  gcursor++;
		  }
		}
		//big char
		if (digit>=0x80 && digit<0x7ff){
			//we encode to UTF-8
			u1 = (digit & 0x03F) | 0x80;
			u2 = ((digit & 0x007C0) | 0x3000) >> 6;
			  if (gcursor<1024-2){
				  line[gcursor]=(unsigned char)u2;
				  gcursor++;
				  line[gcursor]=(unsigned char)u1;
				  gcursor++;
			  }
			  //printf("%d=%2x%2x\n",digit,line[gcursor-2],line[gcursor-1]);
		}
		//printf("eof=%d eol=%d digit=%d\n",eof,eol,digit);
		//readkey();
	}
	//printf("eof=%d:>>>>>%s<<<<<\n",eof,line);
	return eof;
}

int load_pgf_equipment(int probe_file_only, char *fname){
	  FILE *inf;

	  char line[1024],tokens[50][256];
	  int i,cursor=0,token=0,lines;
	  //int total_victory,total_left,total_right;
	  int token_len, token_write, bmp_idx;
	  unsigned short temp;

	  char path[MAX_PATH];

      strncpy(path,fname,MAX_PATH);
	  canonicalize_filename(path,path,MAX_PATH);
	  inf=fopen(path,"rb");
	  if (!inf)
	  {
			strncpy(path,"..\\..\\default\\scenario\\",MAX_PATH);
			strncat(path,fname,MAX_PATH);
			canonicalize_filename(path,path,MAX_PATH);
			inf=fopen(path,"rb");
			if (!inf)
				return ERROR_PGF_EQUIP_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }
	  if (probe_file_only==PROBE_FILE){
		  fclose(inf);
		  return ERROR_PGF_EQUIP_BASE+ERROR_FPGE_FILE_FOUND;
	  }
	  lines=0;

	  if (probe_file_only != LOAD_CONVERSION_TABLE_ONLY)
		  total_equip=0;
	  else
		  conversion_total_equip=0;

	  while (read_utf16_line_convert_to_utf8(inf,line)>=0){
		  //count lines so error can be displayed with line number
		  lines++;
			  //strip comments
		  //printf("%d\n",0);

		  for(i=0;i<strlen(line);i++)
				  if (line[i]==0x23) { line[i]=0; break; }
			  //tokenize
			 // printf("%d\n",1);
			  token=0;
			  cursor=0;
			  for(i=0;i<strlen(line);i++)
				  if (line[i]==0x09) {tokens[token][cursor]=0;token++;cursor=0;}
				  else {tokens[token][cursor]=line[i]; cursor++;}
			  tokens[token][cursor]=0;
			  token++;
			  //printf("%d\n",2);
			  //printf("%d:",token);
			  //if (lines>110 && lines<120){
			  //for(i=0;i<token;i++)
			  //		printf("%s->", tokens[i]);
			  //printf("\n");
			  //}

			  //0x22
			  //remove quoting
			  if (tokens[1][0]==0x22){
				  //check ending quote
				  token_len=strlen(tokens[1]);

				  if (tokens[1][token_len-1]!=0x22){
					  printf("Error. Line %d. Check quotation mark in name >%s<. Line skipped.\n",lines,tokens[1]);
					  continue;
				  }
				 //remove start/end quotation marks
				  for (i=1;i<token_len-1;i++)
					  tokens[1][i-1]=tokens[1][i];
				  tokens[1][token_len-2]=0;
				  //remove double quote
				  // get new length
				  token_len=strlen(tokens[1]);
				  token_write=0;
				  for (i=0;i<token_len+1;i++){
					tokens[1][token_write]=tokens[1][i];
					if (tokens[1][i]==0x22 && tokens[1][i+1]==0x22) i++; //skip next char
					token_write++;
				  }
				  //all done

				  //printf("%s\n",tokens[1]);
			  }
			  //printf("%d\n",token);
			  if (token==33){
			  // write back to normal equipment table
			  i=atoi(tokens[0]);
			  //printf("%d\n",i);

			  if (probe_file_only != LOAD_CONVERSION_TABLE_ONLY) {
				  //printf("2\n");
				strncpy(equip[i], tokens[1], 20);
				strncpy(equip_name_utf8[i], tokens[1],40);
				//already converted
				//convert_from_cp1250_to_utf8(equip_name_utf8[i], equip[i], 20);

				equip[i][CLASS] = (unsigned char) atoi(tokens[2]);
				//attack
				equip[i][SA] = (unsigned char) atoi(tokens[3]);
				equip[i][HA] = (unsigned char) atoi(tokens[4]);
				equip[i][AA] = (unsigned char) atoi(tokens[5]);
				equip[i][NA] = (unsigned char) atoi(tokens[6]);
				//Defense
				equip[i][GD] = (unsigned char) atoi(tokens[7]);
				equip[i][AD] = (unsigned char) atoi(tokens[8]);
				equip[i][CD] = (unsigned char) atoi(tokens[9]);

				equip[i][MOV_TYPE] = (unsigned char) atoi(tokens[10]);
				if (equip[i][MOV_TYPE] == MOV_TYPE_AIR) //air
					equip[i][GAF] = 1;
				else
					equip[i][GAF] = 0;
				equip[i][INITIATIVE] = (unsigned char) atoi(tokens[11]);
				equip[i][RANGE] = (unsigned char) atoi(tokens[12]);
				equip[i][SPOTTING] = (unsigned char) atoi(tokens[13]);

				equip[i][TARGET_TYPE] = (unsigned char) atoi(tokens[14]);
				equip[i][MOV] = (unsigned char) atoi(tokens[15]);
				equip[i][FUEL] = (unsigned char) atoi(tokens[16]);
				equip[i][AMMO] = (unsigned char) atoi(tokens[17]);
				equip[i][COST] = (unsigned char) ((int) atoi(tokens[18])
						/ COST_DIVISOR);
				bmp_idx = atoi(tokens[19]);
				equip[i][BMP] = bmp_idx & 0xff;
				equip[i][BMP+1] = (bmp_idx >> 8)& 0xff;

				temp = atoi(tokens[20]);
				equip[i][ANI] = temp& 0xff;
				equip[i][ANI+1] = (temp >> 8)& 0xff;

				equip[i][MON] = (unsigned char) atoi(tokens[21]);
				equip[i][YR] = (unsigned char) atoi(tokens[22]);
				equip[i][LAST_YEAR] = (unsigned char) atoi(tokens[23]);
				equip[i][AAF] = (unsigned char) atoi(tokens[24]);
				equip_flags[i] =0;
				equip_flags[i] |= (unsigned char) atoi(tokens[25])?EQUIPMENT_IGNORES_ENTRENCHMENT:0;
				equip_flags[i] |= (unsigned char) atoi(tokens[26])?EQUIPMENT_CAN_HAVE_ORGANIC_TRANSPORT:0;
				equip[i][ALLOWED_TRANSPORT]=0;
				if (atoi(tokens[27])==1) equip[i][ALLOWED_TRANSPORT]=1; //naval transport
				if (atoi(tokens[28])==1) equip[i][ALLOWED_TRANSPORT]=2; //air mobile transport
				if (atoi(tokens[29])==1) equip[i][ALLOWED_TRANSPORT]=3; //paradrop

				equip_flags[i] |= (unsigned char) atoi(tokens[30])?EQUIPMENT_CAN_BRIDGE_RIVERS:0;
				equip_flags[i] |= (unsigned char) atoi(tokens[31])?EQUIPMENT_JET:0;

				equip_country[i] = (char) atoi(tokens[32]);

				//if (total_equip==388)
				//for(i=0;i<token;i++)
				 //printf("%s->", tokens[30]);


				total_equip++;
			}else{
				//load conversion file only
				strncpy(conversion_equip[i].name, tokens[1],20*3);
				//WRONG. File is UTF-16, we use UTF-8
				//convert_from_cp1250_to_utf8(conversion_equip[i].name, tokens[1], 20);
				conversion_equip[i].country = (char) atoi(tokens[32]);
				conversion_equip[i].new_id=i;
				conversion_total_equip++;
			}
		  }
	  }
	  fclose(inf);
	  //if (show_info) printf("All loaded.\n");
	  return 0;
}

int load_pgf_pgscn(int scen_number, int show_info, int probe_file_only){

	  FILE *inf;
	  char path[MAX_PATH];
	  char line[1024],tokens[20][1024];
	  int i,j,block=0,last_line_length=-1,cursor=0,token=0,x,y,error,lines;
	  int total_victory,total_left,total_right,where_add_new;
	  unsigned char t1,t2;
	  //short digit;
	  WORD unum;

	  sprintf(path,"%03d.pgscn",scen_number);
	  canonicalize_filename(path,path,MAX_PATH);
	  if (show_info && probe_file_only==LOAD_FILE) printf("Opening file %s\n",path);
	  inf=fopen(path,"rb");
	  if (!inf)
	  {
	    //printf("Couldn't open scenario file\n");
	    return ERROR_PGF_SCN_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }
	  if (probe_file_only==PROBE_FILE){
		  fclose(inf);
		  return ERROR_PGF_SCN_BASE+ERROR_FPGE_FILE_FOUND;
	  }
	  //init
	  //units
	  clear_all_units();

	  total_deploy=0;
	  total_left=0;
	  total_right=0;

	  total_victory=0;
	  for (i=0; i<20; ++i)
	  {
		  victory_hexes[i].x=-1;
		  victory_hexes[i].y=-1;
	  }
	  //
	  lines=0;
	  block4_lines=0;
	  block5_lines=0;
	  block7_lines=0;
	  block9_lines=0;
	  memset(scn_buffer,0,sizeof(scn_buffer));

	  while (read_utf16_line_convert_to_utf8(inf,line)>=0){
		  //count lines so error can be displayed with line number
		  lines++;

			  //strip comments
			  for(i=0;i<strlen(line);i++)
				  if (line[i]==0x23) { line[i]=0; break; }
			  if (strlen(line)>0 && last_line_length==0){
				  block++;
			  }
			  last_line_length=strlen(line);
			  token=0;
			  cursor=0;
			  for(i=0;i<strlen(line);i++)
				  if (line[i]==0x09) {tokens[token][cursor]=0;token++;cursor=0;}
				  else {tokens[token][cursor]=line[i]; cursor++;}
			  tokens[token][cursor]=0;
			  token++;
			  //for(i=0;i<token;i++)
				//  printf("%s->",tokens[i]);

			  //Block#1  +: General scenario data : 2 col, 14 rows
			  if (block==1 && token>1){

				  if (token!=2)
				 	printf("Error. Line %d. Expected no of columns %d while %d columns detected.\n",lines,2,token);
				  strlwr(tokens[0]);
				  if (strcmp(tokens[0],"name")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) strncpy(block1_Name,tokens[1],256);
				  if (strcmp(tokens[0],"description")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) strncpy(block1_Description,tokens[1],1024);
				  if (strcmp(tokens[0],"set file")==0){
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) strncpy(block1_SET_file,tokens[1],256);
					  strncpy(tokens[2],"",1024);
					  j=0;
					  for(i=0;i<strlen(tokens[1]);i++)
						  if (tokens[1][i]>='0' && tokens[1][i]<='9'){
							  tokens[2][j]=tokens[1][i];
							  j++;
						  }
					  tokens[2][j]=0;
					  pgf_map_number=atoi(tokens[2]);
					  if (probe_file_only==SCAN_FOR_MAP_NUMBER){
						  fclose(inf);
						  return ERROR_PGF_SCN_BASE+ERROR_FPGE_MAP_NUMBER_FOUND;
					  }
				  }
				  if (strcmp(tokens[0],"turns")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) scn_buffer[TURNS]=(unsigned char)atoi(tokens[1]);
				  if (strcmp(tokens[0],"year")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) scn_buffer[YEAR]=(unsigned char)atoi(tokens[1]);
				  if (strcmp(tokens[0],"month")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) scn_buffer[MONTH]=(unsigned char)atoi(tokens[1]);
				  if (strcmp(tokens[0],"day")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) scn_buffer[DAY]=(unsigned char)atoi(tokens[1]);
				  if (strcmp(tokens[0],"days per turn")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) scn_buffer[DAYS_PER_TURN]=(unsigned char)atoi(tokens[1]);
				  if (strcmp(tokens[0],"turns per day")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) scn_buffer[TURNS_PER_DAY]=(unsigned char)atoi(tokens[1]);
				  if (strcmp(tokens[0],"weather zone")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) scn_buffer[SCEN_LOCALE]=(unsigned char)atoi(tokens[1]);
				  if (strcmp(tokens[0],"current weather")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) scn_buffer[STORM_FRONT]=(unsigned char)atoi(tokens[1]);
				  if (strcmp(tokens[0],"max unit strength")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) strncpy(block1_Max_Unit_Strength,tokens[1],256);
				  if (strcmp(tokens[0],"max unit experience")==0)
					  if (probe_file_only!=SCAN_FOR_MAP_NUMBER) strncpy(block1_Max_Unit_Experience,tokens[1],256);

				  allies_move_first=0;
				  if (strcmp(tokens[0],"allies move first")==0)
				  	  if (probe_file_only!=SCAN_FOR_MAP_NUMBER)
				  		  {
							  strncpy(block1_Allies_Move_First,tokens[1],64);
							  allies_move_first=atoi(block1_Allies_Move_First);
				  		  }

			  }
			  //Block#2  +: Sides : 11 col, 2 rows
			  if (block==2 && token>1){

				  if (probe_file_only==SCAN_FOR_MAP_NUMBER){
					  fclose(inf);
					  return ERROR_PGF_SCN_BASE+ERROR_FPGE_MAP_NUMBER_NOT_FOUND;
				  }

				  if (token!=11)
				 	printf("Error. Line %d. Expected no of columns %d while %d columns detected.\n",lines,11,token);
				  if (atoi(tokens[0])==0){
					  //TODO make this shifts >> below more clear and implement it in right way
					  scn_buffer[CORE_UNITS_LIMIT]=(unsigned char)atoi(tokens[2]);
					  scn_buffer[AUX_UNITS_LIMIT]=(unsigned char)atoi(tokens[3]);
					  scn_buffer[AXIS_STANCE]=(unsigned char)atoi(tokens[4]);

					  s4_buffer[AXIS_AIR_NUMBER]=(unsigned char)atoi(tokens[8]);
					  s4_buffer[AXIS_AIR_TYPE]=(unsigned char)atoi(tokens[9]);
					  s4_buffer[AXIS_AIR_TYPE+1]=(unsigned char)(atoi(tokens[9])>>8);

					  s4_buffer[AXIS_SEA_NUMBER]=(unsigned char)atoi(tokens[6]);
					  s4_buffer[AXIS_SEA_TYPE]=(unsigned char)atoi(tokens[7]);
					  s4_buffer[AXIS_SEA_TYPE+1]=(unsigned char)(atoi(tokens[7])>>8);

					  s4_buffer[AXIS_PRESTIGE]=(unsigned char)atoi(tokens[1]);
					  s4_buffer[AXIS_PRESTIGE+1]=(unsigned char)(atoi(tokens[1])>>8);

					  axis_experience = atoi(tokens[10]);
				  }
				  if (atoi(tokens[0])==1){
					  scn_buffer[ALLIED_UNITS_LIMIT]=(unsigned char)atoi(tokens[2]);
					  scn_buffer[ALLIED_AUX_UNITS_LIMIT]=(unsigned char)atoi(tokens[3]);
					  scn_buffer[ALLIED_STANCE]=(unsigned char)atoi(tokens[4]);
					  if (atoi(tokens[5])==1)
					    {
					      scn_buffer[ORIENTATION]=1;
					      scn_buffer[ORIENTATION+1]=0;
					    }
					    else
					    {
					      scn_buffer[ORIENTATION]=255;
					      scn_buffer[ORIENTATION+1]=255;
					    }

					  s4_buffer[ALLIED_AIR_NUMBER]=(unsigned char)atoi(tokens[8]);
					  s4_buffer[ALLIED_AIR_TYPE]=(unsigned char)atoi(tokens[9]);
					  s4_buffer[ALLIED_AIR_TYPE+1]=(unsigned char)(atoi(tokens[9])>>8);
					  s4_buffer[ALLIED_SEA_NUMBER]=(unsigned char)atoi(tokens[6]);
					  s4_buffer[ALLIED_SEA_TYPE]=(unsigned char)atoi(tokens[7]);
					  s4_buffer[ALLIED_SEA_TYPE+1]=(unsigned char)(atoi(tokens[7])>>8);

					  s4_buffer[ALLIED_PRESTIGE]=(unsigned char)atoi(tokens[1]);
					  s4_buffer[ALLIED_PRESTIGE+1]=(unsigned char)(atoi(tokens[1])>>8);

					  allied_experience = atoi(tokens[10]);

				  }
			  }
			  //Block#3  +: Nations: 2 col, 2 or more rows
			  if (block==3 && token>1){
				  if (token!=2){
				 	printf("Error. Line %d. Expected no of columns %d while %d columns detected.\n",lines,2,token);

				  }else{
				  t1=atoi(tokens[0]);
				  t2=atoi(tokens[1]);

				  if(t2==0) { scn_buffer[total_left*2]=t1;  total_left++;}
				  if(t2==1) { scn_buffer[total_right*2+1]=t1;  total_right++;}

				  if (total_left>6 || total_right>6)
					  printf("Error. Line %d. Too many nations (max=6 per side).\n",lines);
				  }
			  }
			  //Block#4   : Per-turn prestige allotments: 3 col, rows = no of turns
			  if (block==4 && token>1){
				  if (block4_lines>MAX_TURNS)
					  printf("Error. Line %d. Too many lines in 'Per-turn prestige allotments' block.\n",lines);
				  else{
					  strncpy(block4[block4_lines],line,MAX_LINE_SIZE);
					  block4_lines++;
				  }
			  }
			  //Block#5  ?: Supply hexes: 2 col, rows - many
			  if (block == 5 && token > 1) {
				if (block5_lines > MAX_SUPPLY)
					printf(
							"Error. Line %d. Too many lines in 'Supply hexes' block.\n",
							lines);
				else {
					strncpy(block5[block5_lines], line,MAX_LINE_SIZE);
					block5_lines++;
				}
			}
			  //Block#6  +: Victory hexes: 1 col, rows - many, limit 20
			  if (block==6 && strlen(tokens[0])>0){
				  if (token!=1)
				 	printf("Error. Line %d. Victory hexes. Expected no of columns %d while %d columns detected.\n",lines,1,token);

				  error=sscanf(tokens[0],"(%8d:%8d)",&x,&y);
				  //some protection
				  if (x>=0 && y>=0 && x<mapx && y<mapy){
					  victory_hexes[total_victory].x=x;
					  victory_hexes[total_victory].y=y;
					  victory_hexes[total_victory].own=map[x][y].own;

					  map[x][y].vic=1;
					  total_victory++;
				  }
			  }
			  //Block#7   : Victory conditions: 3 col, rows 2
			  if (block == 7 && token > 1) {
				if (block7_lines > MAX_VICTORY_CON)
					printf(
							"Error. Line %d. Too many lines in 'Victory conditions' block.\n",
							lines);
				else {
					strncpy(block7[block7_lines], line,MAX_LINE_SIZE);
					block7_lines++;
				}
			}
			  //Block#8  +: Deploy hexes: 1 col, rows - many, limit 80
			  if (block==8 && strlen(tokens[0])>0){
				  if (token!=1)
				 	printf("Error. Line %d. Expected no of columns %d while %d columns detected.\n",lines,1,token);

				  error=sscanf(tokens[0],"(%8d:%8d)",&x,&y);
				  if (error==2 && x>=0 && y>=0 && x<mapx && y<mapy){
					  map[x][y].deploy=1;
					  total_deploy++;
				  }
			  }
			  //Block#9   : Purchasable classes: 2 col, rows up to 16 ?
			  if (block == 9 && token > 1) {
				if (block9_lines > MAX_CLASSES)
					printf(
							"Error. Line %d. Too many lines in 'Purchasable classes' block.\n",
							lines);
				else {
					strncpy(block9[block9_lines], line,MAX_LINE_SIZE);
					block9_lines++;
				}
			}
			  //Block#10 +: Units: 12 col, rows - many, limit
			  if (block==10 && token>1){
				  //for(i=0;i<token;i++)
					//  printf("%s->",tokens[i]);
				  //printf(":%d:%d\n",block,token);

				  if (token!=12)
					  printf("Error. Line %d. Units. Expected no of columns %d while %d columns detected.\n",lines,12,token);

				  //0 #Hex
				  //1 #Type
				  //2 #Organic Transport Type
				  //3 #Sea/Air Transport Type
				  //4 #Side
				  //5 #Flag
				  //6 #Strength
				  //7 #Experience
				  //8 #Entrenchment
				  //9 #Fuel
				  //10 #Ammo
				  //11 #Auxiliary

				  where_add_new=add_unit_type(atoi(tokens[4]),atoi(tokens[11]));

				    error=sscanf(tokens[0],"(%8d:%8d)",&x,&y);
				    if (x>=mapx || y>=mapy || x<0 || y<0) //this is an error!
			         {
						 printf("Warning. Ignoring line %d. Check unit x,y !\n",lines);
			         }
				    else
				    {
						all_units[where_add_new].unum=atoi(tokens[1]);
						all_units[where_add_new].orgtnum=atoi(tokens[2]);
						all_units[where_add_new].country=atoi(tokens[5]);
						all_units[where_add_new].auxtnum=atoi(tokens[3]);

						all_units[where_add_new].x=x;
						all_units[where_add_new].y=y;
						all_units[where_add_new].str=atoi(tokens[6]);
						all_units[where_add_new].exp=atoi(tokens[7])/100;
						all_units[where_add_new].entrench=atoi(tokens[8]);
						all_units[where_add_new].fuel=atoi(tokens[9]);
						all_units[where_add_new].ammo=atoi(tokens[10]);

						//place on map
						 unum=all_units[where_add_new].unum;
						 if (equip[unum][GAF]) //1 if air unit
						   map[x][y].auidx=total_units;
						 else
						   map[x][y].guidx=total_units;

						total_units++;
				    }
			  }
			 //printf(":%d:%d\n",block,token);
			  //printf("%d:%s\n",block,line);
			  continue;

	  }

	  fclose(inf);

		scn_buffer[CORE_UNITS]=total_axis_core;
		scn_buffer[ALLIED_UNITS]=total_allied_core;
		scn_buffer[AUX_UNITS]=total_axis_aux;
		scn_buffer[AUX_ALLIED_UNITS]=total_allied_aux;
/*
		printf("total_units=%d\n",total_units);
		printf("total_axis_core=%d\n",total_axis_core);
		printf("total_axis_aux=%d\n",total_axis_aux);
		printf("total_allied_core=%d\n",total_allied_core);
		printf("total_allied_aux=%d\n",total_allied_aux);
*/
	 // printf("block4_lines=%d\nblock5_lines=%d\nblock7_lines=%d\nblock9_lines=%d\n",block4_lines,block5_lines,block7_lines,block9_lines);
	  if (show_info) printf("PGF scenario #%02d loaded.\n",scen_number);

	  return 0;
}

void fake_UTF_write_char(FILE *f, unsigned char write_me){
	unsigned short buff;

	buff=write_me;
	fwrite(&buff,sizeof(buff),1,f);
}

void fake_UTF_write_short(FILE *f, unsigned short buff){
	fwrite(&buff,sizeof(buff),1,f);
}

void fake_UTF_write_string(FILE *f, char *write_me){
	int i;
	unsigned short buff,b,b1;

	for(i=0;i<strlen(write_me);i++){
		buff=(unsigned char)write_me[i];
		if ((buff&0xc0)==0xc0) {//UTF-8, 2 highest bits set
			i++;
			b=((unsigned char)write_me[i])&0x3f;//get next char
			b1=buff&0x3f;
			buff=((b1<<6)&0x07C0) | b;
			//printf("%2x%2x : %4x\n",(unsigned char)write_me[i-1],(unsigned char)write_me[i],buff);
		}
		if (buff!=0x000a && buff!=0x000d) //do not save NL CR here
			fake_UTF_write_short(f,buff);
	}
}

void fake_UTF_write_string_with_eol(FILE *f, char *write_me){
    uint8_t end_of_line[4]= {0x0d,0x00,0x0a,0x00};

	fake_UTF_write_string(f,write_me);

	fwrite(&end_of_line,4,1,f);

}

int save_pgf_pgscn(int scen_number, int show_info, int save_type, int hide_names, int swap_side,int side_save){
	  FILE *inf;
	  char path[MAX_PATH];

	  char line[1024];
	  char tmp_line[1024];
	  int i,x,y,written_lines_in_block;
	  int axis_prestige,allied_prestige;

      int unum;
	  int orgtnum;
	  int auxtnum;
	  //unsigned char country;
	  int country, fcountry, result, orient;

	  //save also map files
	  save_set(scen_number,side_save);
	  save_stm(scen_number,side_save);

	  sprintf(path,"%03d.pgscn",scen_number);
	  canonicalize_filename(path,path,MAX_PATH);
	  if (show_info) printf("Writing file %s\n",path);
	  inf=fopen(path,"wb");
	  if (!inf)
	  {
	    //printf("Couldn't open scenario file\n");
	    return ERROR_PGF_SCN_BASE+ERROR_FPGE_FILE_CANNOT_BE_WRITTEN;
	  }

	  //header
	  fwrite(&UCS2_header,sizeof(UCS2_header),1,inf);
	  fake_UTF_write_string_with_eol(inf,"# PG Forever scenario file");
	  fake_UTF_write_string_with_eol(inf,"");
	  //block1
	  fake_UTF_write_string_with_eol(inf,"# General scenario data");

	  //Name
	  if (pgf_mode)
		  sprintf(line,"Name\t%s",block1_Name);
	  else{
		  convert_from_cp1250_to_utf8(tmp_line,scn_short_description[scen_number-1],SHORT_SCN_SIZE);
		  sprintf(line,"Name\t%s",tmp_line);
	  }
	  fake_UTF_write_string_with_eol(inf,line);
	  //Description
	  if (pgf_mode)
		  sprintf(line,"Description\t%s",block1_Description);
	  else{
		  convert_from_cp1250_to_utf8(tmp_line,scn_long_description[scen_number-1],LONG_SCN_SIZE);
		  sprintf(line,"Description\t%s",tmp_line);
	  }
	  fake_UTF_write_string_with_eol(inf,line);
	  //SET file
	  if (pgf_mode)
		  sprintf(line,"SET file\t%s",block1_SET_file);
	  else
		  sprintf(line,"SET file\tmap%02d.set",scen_number);
	  fake_UTF_write_string_with_eol(inf,line);

	  sprintf(line,"Turns\t%d",scn_buffer[TURNS]);
	  fake_UTF_write_string_with_eol(inf,line);
	  sprintf(line,"Year\t%d",scn_buffer[YEAR]);
	  fake_UTF_write_string_with_eol(inf,line);
	  sprintf(line,"Month\t%d",scn_buffer[MONTH]);
	  fake_UTF_write_string_with_eol(inf,line);
	  sprintf(line,"Day\t%d",scn_buffer[DAY]);
	  fake_UTF_write_string_with_eol(inf,line);
	  sprintf(line,"Turns per Day\t%d",scn_buffer[TURNS_PER_DAY]);
	  fake_UTF_write_string_with_eol(inf,line);
	  sprintf(line,"Days per Turn\t%d",scn_buffer[DAYS_PER_TURN]);
	  fake_UTF_write_string_with_eol(inf,line);
	  sprintf(line,"Current Weather\t%d",scn_buffer[STORM_FRONT]);
	  fake_UTF_write_string_with_eol(inf,line);
	  sprintf(line,"Weather Zone\t%d",scn_buffer[SCEN_LOCALE]);
	  fake_UTF_write_string_with_eol(inf,line);
	  //Max Unit Strength
	  sprintf(line,"Max Unit Strength\t%s",block1_Max_Unit_Strength);
	  fake_UTF_write_string_with_eol(inf,line);
	  //Max Unit Experience
	  sprintf(line,"Max Unit Experience\t%s",block1_Max_Unit_Experience);
	  fake_UTF_write_string_with_eol(inf,line);
	  if ( (allies_move_first+swap_side)%2)
	  {
		  sprintf(line,"Allies Move First\t%d",(allies_move_first+swap_side)%2 );
		  fake_UTF_write_string_with_eol(inf,line);
	  }

	  fake_UTF_write_string_with_eol(inf,"");
	  //block2
	  fake_UTF_write_string_with_eol(inf,"# Sides");
	  fake_UTF_write_string_with_eol(inf,"# Index	Prestige	Core Limit	Aux Limit	Stance	Orientation	Sea Transports	Sea Transport Type	Air Transports	Air Transport Type	New Unit Exp");
	  sprintf(line,"%d\t%d\t%d\t%d\t%d\t%d\t%d",
			  (0+swap_side)%2,
			  (WORD)s4_buffer[AXIS_PRESTIGE]+256*s4_buffer[AXIS_PRESTIGE+1],
			  scn_buffer[CORE_UNITS_LIMIT],
			  scn_buffer[AUX_UNITS_LIMIT],
			  scn_buffer[AXIS_STANCE],
			  0,
			  s4_buffer[AXIS_SEA_NUMBER]
	  );
	  //sea transport type
	  if(s4_buffer[AXIS_SEA_TYPE]+256*s4_buffer[AXIS_SEA_TYPE+1]>0)
		  sprintf(tmp_line,"\t%d",s4_buffer[AXIS_SEA_TYPE]+256*s4_buffer[AXIS_SEA_TYPE+1]);
	  else
		  strncpy(tmp_line,"\t",256);
	  strncat(line,tmp_line,1024);
	  //air number
	  sprintf(tmp_line,"\t%d",s4_buffer[AXIS_AIR_NUMBER]);
	  strncat(line,tmp_line, 1024);
	  //air transport type
	  if(s4_buffer[AXIS_AIR_TYPE]+256*s4_buffer[AXIS_AIR_TYPE+1]>0)
		  sprintf(tmp_line,"\t%d",s4_buffer[AXIS_AIR_TYPE]+256*s4_buffer[AXIS_AIR_TYPE+1]);
	  else
		  strncpy(tmp_line,"\t",256);
	  strncat(line,tmp_line, 1024);
	  //new unit exp
	  if (pgf_mode)
		  sprintf(tmp_line,"\t%d",axis_experience);
	  else
		  sprintf(tmp_line,"\t%d",axis_experience_table[scen_number-1]*100);
	  strncat(line,tmp_line, 1024);

	  fake_UTF_write_string_with_eol(inf,line);
	  orient=0;
	  if (scn_buffer[ORIENTATION]==1) orient=1;
	  sprintf(line,"%d\t%d\t%d\t%d\t%d\t%d\t%d",
			  (1+swap_side)%2,
			  (WORD)s4_buffer[ALLIED_PRESTIGE]+256*(WORD)s4_buffer[ALLIED_PRESTIGE+1],
			  scn_buffer[ALLIED_UNITS_LIMIT],
			  scn_buffer[ALLIED_AUX_UNITS_LIMIT],
			  scn_buffer[ALLIED_STANCE],
			  (orient+swap_side)%2==1?1:-1,
			  s4_buffer[ALLIED_SEA_NUMBER]
	  );
	  //sea transport type
	  if(s4_buffer[ALLIED_SEA_TYPE]+256*s4_buffer[ALLIED_SEA_TYPE+1]>0)
		  sprintf(tmp_line,"\t%d",s4_buffer[ALLIED_SEA_TYPE]+256*s4_buffer[ALLIED_SEA_TYPE+1]);
	  else
		  strncpy(tmp_line,"\t",256);
	  strncat(line,tmp_line,1024);
	  //air number
	  sprintf(tmp_line,"\t%d",s4_buffer[ALLIED_AIR_NUMBER]);
	  strncat(line,tmp_line,1024);
	  //air transport type
	  if(s4_buffer[ALLIED_AIR_TYPE]+256*s4_buffer[ALLIED_AIR_TYPE+1]>0)
		  sprintf(tmp_line,"\t%d",s4_buffer[ALLIED_AIR_TYPE]+256*s4_buffer[ALLIED_AIR_TYPE+1]);
	  else
		  strncpy(tmp_line,"\t",256);
	  strncat(line,tmp_line,1024);
	  //new unit exp
	  if (pgf_mode)
		  sprintf(tmp_line,"\t%d",allied_experience);
	  else
		  sprintf(tmp_line,"\t%d",allied_experience_table[scen_number-1]*100);
	  strncat(line,tmp_line,1024);

	  fake_UTF_write_string_with_eol(inf,line);

	  fake_UTF_write_string_with_eol(inf,"");

	  //block3
	  fake_UTF_write_string_with_eol(inf,"# Nations");
	  fake_UTF_write_string_with_eol(inf,"# Flag	Side");
	  written_lines_in_block=0;
	  for(i=0;i<12;i++)
		  if ((scn_buffer[i] & 255) > 0){
			  sprintf(line,"%d\t%d",scn_buffer[i] & 255,(i+swap_side)%2);
			  fake_UTF_write_string_with_eol(inf,line);
			  written_lines_in_block++;
		  }
	  if (written_lines_in_block==0) fake_UTF_write_string_with_eol(inf,"None");
	  fake_UTF_write_string_with_eol(inf,"");

	  //block4
	  fake_UTF_write_string_with_eol(inf,"# Per-turn prestige allotments");
	  fake_UTF_write_string_with_eol(inf,"# Turn	Axis	Allies");
	  if (pgf_mode && block4_lines!=0){
		  //if (){
			  //it is not good idea to write "none" here
			  //fake_UTF_write_string_with_eol(inf, "None");

		//	  }
		 // else

		  int tu = scn_buffer[TURNS]&255;
		  int limit = Min(Min(block4_lines,tu),MAX_TURNS);
		  //printf("limit1=%d\n",limit);
		  //printf("block4_lines=%d\ntu=%d\nlimit=%d\n",block4_lines,tu,limit);

			  for(i=0;i<limit;i++){
				  //printf("Min i=%d\n",i);
				  fake_UTF_write_string_with_eol(inf,block4[i]);
			  }
		  //if block4_lines<scn_buffer[TURNS] fill rest with block4[block4_lines-1]

			 limit = Min(Max(tu-block4_lines,0),MAX_TURNS);
			// printf("limit2=%d\n",limit);
			 if (limit > 0){
				  char tokens[10][10];
				  int token=0;
				  int cursor=0;

				  for(i=0;i<strlen(block4[block4_lines-1]);i++)
					  if (block4[block4_lines-1][i]=='\t' )
					  {
						  tokens[token][cursor]=0;
						  if (token<10)
							  token++;
						  cursor=0;
					  }
					  else
					  {
						  tokens[token][cursor]=block4[block4_lines-1][i];
						  //printf("%c",tokens[token][cursor]);
						  if (cursor<10)
							  cursor++;
					  }
				  tokens[token][cursor]=0;
				  token++;
				  //printf("\n%d >%s<\n",token,block4[block4_lines-1]);
				  //for(i=0;i<token;i++) printf("%d %s\n",i,tokens[i]);

				  for(i=0;i<limit;i++){
					  sprintf(line, "%d\t%s\t%s",
											i+tu-limit+1,
											tokens[1],
											tokens[2]
									);
					  //printf("Max i=%d\n",i);
					  fake_UTF_write_string_with_eol(inf,line);
				  }
			 }
	  }else{
		  //write PG defaults for first 38 scenarios
		  for(i=0;i<scn_buffer[TURNS];i++){
			  if (scen_number-1<PRESTIGE_ALLOTMENTS_NUMBER && scen_number-1>=0){
				  allied_prestige=allied_prestige_allotments[scen_number-1];
				  axis_prestige=axis_prestige_allotments[scen_number-1];
			  }else{
				  allied_prestige=0;
				  axis_prestige=0;
			  }

			  //special case Ardennes
			  if (scen_number==17 && (i+1)%9==0) allied_prestige+=5000;
			  //special case Kursk
			  if (scen_number==30 && (i+1)%5==0) allied_prestige+=2000;
				sprintf(line, "%d\t%d\t%d",
						i+1,
						axis_prestige,
						allied_prestige
				);
				fake_UTF_write_string_with_eol(inf, line);
		  }
	  }
	  fake_UTF_write_string_with_eol(inf,"");

	  //block5
	  fake_UTF_write_string_with_eol(inf,"# Supply hexes");
	  fake_UTF_write_string_with_eol(inf,"# Hex	Prestige for Capturing");
	  if (pgf_mode) {
		if (block5_lines == 0)
			fake_UTF_write_string_with_eol(inf, "None");
		else
			for (i = 0; i < block5_lines; i++) {
				fake_UTF_write_string_with_eol(inf, block5[i]);
			}
	} else {
		//PGDOS
		written_lines_in_block = 0;
		for (y = 0; y < mapy; ++y)
			for (x = 0; x < mapx; ++x)
				if (map[x][y].own != 0 && map[x][y].side != 3) {
					i = 40;
					if (map[x][y].vic == 1)
						i += 40;
					sprintf(line, "(%d:%d)\t%d", x, y, i);
					fake_UTF_write_string_with_eol(inf, line);
					written_lines_in_block++;
				}
		if (written_lines_in_block == 0)
			fake_UTF_write_string_with_eol(inf, "None");
	}

	  fake_UTF_write_string_with_eol(inf,"");
	  //block6
	  fake_UTF_write_string_with_eol(inf,"# Victory hexes");
	  get_victory_hexes();//make victory_hexes[] array
	  written_lines_in_block=0;
	  for (i=0; i<MAX_VICTORY_HEXES; ++i){
		 if (victory_hexes[i].x>-1 && victory_hexes[i].y>-1 && victory_hexes[i].own > 0){
			  sprintf(line,"(%d:%d)",victory_hexes[i].x,victory_hexes[i].y);
			  fake_UTF_write_string_with_eol(inf,line);
			  written_lines_in_block++;
		 }
	  }
	  if (written_lines_in_block==0) fake_UTF_write_string_with_eol(inf,"None");

	  fake_UTF_write_string_with_eol(inf,"");
	  //block7
	  fake_UTF_write_string_with_eol(inf,"# Victory conditions");
	  //fake_UTF_write_string_with_eol("# Hex	Prestige for Capturing");
		if (pgf_mode) {
			if (block7_lines == 0)
				fake_UTF_write_string_with_eol(inf, "None");
			else
				for (i = 0; i < block7_lines; i++) {
					fake_UTF_write_string_with_eol(inf, block7[i]);
				}
		} else {
			//save rough defaults
			//TODO precise AXIS victory
			if (scen_number>0 && scen_number<38)
				sprintf(line, "%s", axis_victory_table[scen_number - 1]);
			else
				strncpy(line, "AXIS VICTORY\t1\t0\t",1024);
			fake_UTF_write_string_with_eol(inf, line);
			fake_UTF_write_string_with_eol(inf, "ALLIED VICTORY\t-1\t-1\t");
		}
	  fake_UTF_write_string_with_eol(inf,"");

	  //block8
	  fake_UTF_write_string_with_eol(inf,"# Deploy hexes");
	  //this code will filter some wrong (out of map) deployment hexes
	  written_lines_in_block=0;
	  for (y=0; y<mapy; ++y)
		 for (x=0; x<mapx; ++x)
		 if (map[x][y].deploy)
		 {
			  sprintf(line,"(%d:%d)",x,y);
			  fake_UTF_write_string_with_eol(inf,line);
			  written_lines_in_block++;
		 }
	  if (written_lines_in_block==0) fake_UTF_write_string_with_eol(inf,"None");
	  fake_UTF_write_string_with_eol(inf,"");

	   //block9
	  fake_UTF_write_string_with_eol(inf,"# Purchasable classes");
	  fake_UTF_write_string_with_eol(inf,"# Class	Can Purchase");
	  if (pgf_mode) {
		if (block9_lines == 0){
			//fake_UTF_write_string_with_eol(inf, "None");
			//write block
			for (i = 0; i < 10; i++) {
				sprintf(line, "%d\t%d",i,1);
				fake_UTF_write_string_with_eol(inf, line);
			}
			fake_UTF_write_string_with_eol(inf, "10\t1");
			fake_UTF_write_string_with_eol(inf, "15\t1");
		}
		else
			for (i = 0; i < block9_lines; i++) {
				fake_UTF_write_string_with_eol(inf, block9[i]);
			}
	} else {
		written_lines_in_block = 0;
		for (i = 0; i < 16; i++)
			if (default_purchasable_classes[i]) {
				sprintf(line, "%d\t%d", i,
						//special case scenario #1
						(scen_number==1 && i>=5 && i<=10)?0:1);
				fake_UTF_write_string_with_eol(inf, line);
				written_lines_in_block++;
			}
		if (written_lines_in_block == 0)
			fake_UTF_write_string_with_eol(inf, "None");

	}
	  fake_UTF_write_string_with_eol(inf,"");

	  //block10
	  fake_UTF_write_string_with_eol(inf,"# Units");
	  fake_UTF_write_string_with_eol(inf,"# Hex	Type	Organic Transport Type	Sea/Air Transport Type	Side	Flag	Strength	Experience	Entrenchment	Fuel	Ammo	Auxiliary?");

	  for(i=0;i<total_units;i++){
			unum=all_units[i].unum;
			orgtnum=all_units[i].orgtnum;
			auxtnum=all_units[i].auxtnum;
			country=all_units[i].country;

		  if (save_type==SAVE_WITH_UNIT_IDS_CONVERSION){
			  	unum=convert_equipment(unum,country,hide_names,i);
				orgtnum=convert_equipment(orgtnum,country,hide_names,i);
				auxtnum=convert_equipment(auxtnum,country,hide_names,i);
		 }

		  if (save_type==SAVE_WITH_UNIT_IDS_HARD_CONVERSION){
			    result=convert_equipment_hard(&unum,&fcountry,all_units[i].unum,country,1);
			    if (result==CONVERSION_FOUND_NEW_COUNTRY) country=fcountry;

			    //we do not change country based on orgtran or auxtrans
			    convert_equipment_hard(&orgtnum,&fcountry,all_units[i].orgtnum,country,1);
			    convert_equipment_hard(&auxtnum,&fcountry,all_units[i].auxtnum,country,1);
		 }

		  sprintf(line,"(%d:%d)\t%d",
				  all_units[i].x,all_units[i].y,
				  unum);
		  //orgtnum
		  if(all_units[i].orgtnum>0)
			  sprintf(tmp_line,"\t%d",orgtnum);
		  else
			  strncpy(tmp_line,"\t",256);
		  strncat(line,tmp_line, 1024);
		  //trans type
		  if(all_units[i].auxtnum>0)
			  sprintf(tmp_line,"\t%d",auxtnum);
		  else
			  strncpy(tmp_line,"\t",256);
		  strncat(line,tmp_line, 1024);
		  //side
		  sprintf(tmp_line,"\t%d",isAllied(all_units[i].uflag)?(1+swap_side)%2:swap_side);
		  strncat(line,tmp_line, 1024);
		  //flag
		  sprintf(tmp_line,"\t%d",country);
		  strncat(line,tmp_line, 1024);
		  //Strength
		  sprintf(tmp_line,"\t%d",all_units[i].str);
		  strncat(line,tmp_line, 1024);
		  //Exp
		  sprintf(tmp_line,"\t%d",((int)all_units[i].exp)*100);
		  strncat(line,tmp_line, 1024);
		  //Ent
		  sprintf(tmp_line,"\t%d",all_units[i].entrench);
		  strncat(line,tmp_line, 1024);
		  //Fuel
		  if(all_units[i].fuel>0)
			  sprintf(tmp_line,"\t%d",all_units[i].fuel);
		  else
			  strncpy(tmp_line,"\t",256);
		  strncat(line,tmp_line, 1024);
		  //Ammo
		  if(all_units[i].ammo>0)
			  sprintf(tmp_line,"\t%d",all_units[i].ammo);
		  else
			  strncpy(tmp_line,"\t",256);
		  strncat(line,tmp_line, 1024);
		  //aux?
		  sprintf(tmp_line,"\t%d",isAux(all_units[i].uflag)?1:0);
		  strncat(line,tmp_line, 1024);

		  fake_UTF_write_string_with_eol(inf,line);
	  }
	  //Leave no information in case total_units==0
	  //if (total_units==0) fake_UTF_write_string_with_eol(inf,"None");

	  fake_UTF_write_string_with_eol(inf,"");
	  fclose(inf);
	  return 0;
}

int load_bmp_tacticons(){
	FILE *inf;
	char path[MAX_PATH];
	BITMAP *new_map_bitmap;
	int x,y,dx,dy,i,j,c,pink;

	strncpy(path,"..\\graphics\\",MAX_PATH);
	strncat(path,pgf_units_bmp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (!inf){
		strncpy(path,"..\\..\\default\\graphics\\",MAX_PATH);
		strncat(path,pgf_units_bmp,MAX_PATH);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf)
			return ERROR_PGF_BMP_TACICONS_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fclose(inf);
	//all ok, load file
	if(fpge_colors_bits_shift) pink=0xe1e1ff+(fpge_colors_bits_shift==2?0xff000000:0);
	else pink=0xffe1e1;
	new_map_bitmap = load_bmp(path, NULL);
	//printf("%d %d %d\n",new_map_bitmap->w,new_map_bitmap->h, bitmap_color_depth(new_map_bitmap));

	if (new_map_bitmap->w%TILE_FULL_WIDTH) return ERROR_PGF_BMP_TACICONS_BASE+ERROR_FPGE_WRONG_BMP_SIZE_X;
	if (new_map_bitmap->h%50) return ERROR_PGF_BMP_TACICONS_BASE+ERROR_FPGE_WRONG_BMP_SIZE_Y;

	dx=new_map_bitmap->w/TILE_FULL_WIDTH;
	dy=new_map_bitmap->h/TILE_HEIGHT;
	//printf("%d %d\n",dx,dy);

	for(y=0;y<dy;y++)
		for(x=0;x<dx;x++){
			//proxy_bitmap=create_sub_bitmap(new_map_bitmap,x*dx,y*dy,60,50);
			unit_bmp[x+y*dx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			for(i=0;i<TILE_HEIGHT;i++)
				for(j=0;j<TILE_FULL_WIDTH;j++){
					c=getpixel(new_map_bitmap,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i);
					c=make_color_fpge(c&0xff,(c>>8)&0xff,(c>>16)&0xff);
					//if (x+y*dx==146) printf("%d %d %06x\n",j,i,c);
					if (c!=pink)
						putpixel(unit_bmp[x+y*dx],j,i,c);
					else
						putpixel(unit_bmp[x+y*dx],j,i,fpge_mask_color);
				}
		}

	total_uicons=dx*dy;

	destroy_bitmap(new_map_bitmap);
	return 0;
}

int load_bmp_tacmap(){
	FILE *inf;
	char path_dry[MAX_PATH], path_muddy[MAX_PATH], path_frozen[MAX_PATH];
	BITMAP *bitmap_dry, *bitmap_muddy, *bitmap_frozen;
	int x,y,dx,dy,i,j,oc,c,dc,found,pink;

	//dry
	strncpy(path_dry,"..\\graphics\\",MAX_PATH);
	strncat(path_dry,pgf_tacmap_dry_bmp,MAX_PATH);
	canonicalize_filename(path_dry,path_dry,MAX_PATH);
	inf=fopen(path_dry,"rb");
	if (!inf){
		strncpy(path_dry,"..\\..\\default\\graphics\\",MAX_PATH);
		strncat(path_dry,pgf_tacmap_dry_bmp,MAX_PATH);
		canonicalize_filename(path_dry,path_dry,MAX_PATH);
		inf=fopen(path_dry,"rb");
		if (!inf)
			return ERROR_PGF_BMP_TACMAP_DRY_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fclose(inf);

	//muddy
	strncpy(path_muddy,"..\\graphics\\",MAX_PATH);
	strncat(path_muddy,pgf_tacmap_muddy_bmp,MAX_PATH);
	canonicalize_filename(path_muddy,path_muddy,MAX_PATH);
	inf=fopen(path_muddy,"rb");
	if (!inf){
		strncpy(path_muddy,"..\\..\\default\\graphics\\",MAX_PATH);
		strncat(path_muddy,pgf_tacmap_muddy_bmp,MAX_PATH);
		canonicalize_filename(path_muddy,path_muddy,MAX_PATH);
		inf=fopen(path_muddy,"rb");
		if (!inf)
			return ERROR_PGF_BMP_TACMAP_MUDDY_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fclose(inf);

	//frozen
	strncpy(path_frozen,"..\\graphics\\",MAX_PATH);
	strncat(path_frozen,pgf_tacmap_frozen_bmp,MAX_PATH);
	canonicalize_filename(path_frozen,path_frozen,MAX_PATH);
	inf=fopen(path_frozen,"rb");
	if (!inf){
		strncpy(path_frozen,"..\\..\\default\\graphics\\",MAX_PATH);
		strncat(path_frozen,pgf_tacmap_frozen_bmp,MAX_PATH);
		canonicalize_filename(path_frozen,path_frozen,MAX_PATH);
		inf=fopen(path_frozen,"rb");
		if (!inf)
			return ERROR_PGF_BMP_TACMAP_FROZEN_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fclose(inf);

	if(fpge_colors_bits_shift) pink=0xe1e1ff+(fpge_colors_bits_shift==2?0xff000000:0);
	else pink=0xffe1e1;

	//all ok, load all files
	bitmap_dry = load_bmp(path_dry, NULL);
	bitmap_muddy = load_bmp(path_muddy, NULL);
	bitmap_frozen = load_bmp(path_frozen, NULL);

	//printf("%d %d %d\n",new_map_bitmap->w,new_map_bitmap->h, bitmap_color_depth(new_map_bitmap));

	//check if w%60==0 and h%50==0
	if (bitmap_dry->w%TILE_FULL_WIDTH) return ERROR_PGF_BMP_TACMAP_DRY_BASE+ERROR_FPGE_WRONG_BMP_SIZE_X;
	if (bitmap_dry->h%TILE_HEIGHT) return ERROR_PGF_BMP_TACMAP_DRY_BASE+ERROR_FPGE_WRONG_BMP_SIZE_Y;
	if (bitmap_muddy->w%TILE_FULL_WIDTH) return ERROR_PGF_BMP_TACMAP_MUDDY_BASE+ERROR_FPGE_WRONG_BMP_SIZE_X;
	if (bitmap_muddy->h%TILE_HEIGHT) return ERROR_PGF_BMP_TACMAP_MUDDY_BASE+ERROR_FPGE_WRONG_BMP_SIZE_Y;
	if (bitmap_frozen->w%TILE_FULL_WIDTH) return ERROR_PGF_BMP_TACMAP_FROZEN_BASE+ERROR_FPGE_WRONG_BMP_SIZE_X;
	if (bitmap_frozen->h%TILE_HEIGHT) return ERROR_PGF_BMP_TACMAP_FROZEN_BASE+ERROR_FPGE_WRONG_BMP_SIZE_Y;

	//check if they are the same size
	if (bitmap_dry->w!=bitmap_muddy->w) return ERROR_PGF_BMP_TACMAP_DRY_BASE+ERROR_FPGE_DRY_X_DIFF_MUDDY_X;
	if (bitmap_dry->w!=bitmap_frozen->w)return ERROR_PGF_BMP_TACMAP_DRY_BASE+ERROR_FPGE_DRY_X_DIFF_FROZEN_X;

	if (bitmap_dry->h!=bitmap_muddy->h) return ERROR_PGF_BMP_TACMAP_DRY_BASE+ERROR_FPGE_DRY_Y_DIFF_MUDDY_Y;
	if (bitmap_dry->h!=bitmap_frozen->h)return ERROR_PGF_BMP_TACMAP_DRY_BASE+ERROR_FPGE_DRY_Y_DIFF_FROZEN_Y;

	dx=bitmap_dry->w/TILE_FULL_WIDTH;
	dy=bitmap_dry->h/TILE_HEIGHT;
	//printf("%d %d\n",dx,dy);

	for(y=0;y<dy;y++)
		for(x=0;x<dx;x++){
			//proxy_bitmap=create_sub_bitmap(new_map_bitmap,x*dx,y*dy,60,50);
			til_bmp[x+y*dx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			dark_til_bmp[x+y*dx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			til_bmp_mud[x+y*dx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			dark_til_bmp_mud[x+y*dx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			til_bmp_snow[x+y*dx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			dark_til_bmp_snow[x+y*dx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			for(i=0;i<TILE_HEIGHT;i++)
				for(j=0;j<TILE_FULL_WIDTH;j++){
					//dry
					oc=getpixel(bitmap_dry,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i);
					c=make_color_fpge(oc&0xff,(oc>>8)&0xff,(oc>>16)&0xff);
					dc=make_color_fpge( (oc&0xff)*3/4,((oc>>8)&0xff)*3/4,((oc>>16)&0xff)*3/4);

					//if (x+y*dx==146) printf("%d %d %06x\n",j,i,c);
					if (c!=pink){
						if (draw_app6_tiles){
						//int c_idx = find_pal_element92(c);
						if (c==colors_to24bits(92) || c==colors_to24bits(75) || c==colors_to24bits(76)
						 || c==colors_to24bits(91) || c==colors_to24bits(93)|| c==colors_to24bits(90)){
						c=0;
						}
						if (    draw_app6_tiles==1||(
								  c!=colors_to24bits(42)
								&&c!=colors_to24bits(127)
								&&c!=colors_to24bits(89)
								&&c!=colors_to24bits(65)
								&&c!=colors_to24bits(49)&&c!=colors_to24bits(48)&&c!=colors_to24bits(47)&&c!=colors_to24bits(46)&&c!=colors_to24bits(45)
								)){ //42=river, 127=road, 89=forest 87-mountain 65-city 45-49-sea
							//green
							if (draw_app6_color==0){
								c &= 0xff00;
								c = c >> 1;
								c &= 0xff00;
							}
							else
								//grey
							{
								int r= (c&0xff0000)>>16;
								int g= (c&0xff00)>>8;
								int b= (c&0xff);
								int m =  ((r+g+b)/3)>>1;
								r= m<<16;
								g= m<<8;
								b=m;
								c=r+g+b;
							}

							}else{
								if (c==colors_to24bits(89)){
									//medium green
									c=make_color_fpge(0,128+32,0);
								}else{
									if (c==colors_to24bits(65)){
										//white, for city/ports
										c=make_color_fpge(255,255,255);
									}else{
										if (c==colors_to24bits(49)){
											c=make_color_fpge(131>>3,147>>3,199>>2);
										}else{
										//make brigther
									c &=0x7f7f7f;
									c =c <<1;
									}
								}
								}
							}
						}//app6

						putpixel(til_bmp[x+y*dx],j,i,c);
						putpixel(dark_til_bmp[x+y*dx],j,i,dc);
						}
					else{
						putpixel(til_bmp[x+y*dx],j,i,fpge_mask_color);
						putpixel(dark_til_bmp[x+y*dx],j,i,fpge_mask_color);
					}
					//muddy
					oc=getpixel(bitmap_muddy,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i);
					c=make_color_fpge(oc&0xff,(oc>>8)&0xff,(oc>>16)&0xff);
					dc=make_color_fpge( (oc&0xff)*3/4,((oc>>8)&0xff)*3/4,((oc>>16)&0xff)*3/4);

					//if (x+y*dx==146) printf("%d %d %06x\n",j,i,c);
					if (c!=pink){
						putpixel(til_bmp_mud[x+y*dx],j,i,c);
						putpixel(dark_til_bmp_mud[x+y*dx],j,i,dc);
						}
					else{
						putpixel(til_bmp_mud[x+y*dx],j,i,fpge_mask_color);
						putpixel(dark_til_bmp_mud[x+y*dx],j,i,fpge_mask_color);
					}
					//frozen
					oc=getpixel(bitmap_frozen,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i);
					c=make_color_fpge(oc&0xff,(oc>>8)&0xff,(oc>>16)&0xff);
					dc=make_color_fpge( (oc&0xff)*3/4,((oc>>8)&0xff)*3/4,((oc>>16)&0xff)*3/4);

					//if (x+y*dx==146) printf("%d %d %06x\n",j,i,c);
					if (c!=pink){
						putpixel(til_bmp_snow[x+y*dx],j,i,c);
						putpixel(dark_til_bmp_snow[x+y*dx],j,i,dc);
						}
					else{
						putpixel(til_bmp_snow[x+y*dx],j,i,fpge_mask_color);
						putpixel(dark_til_bmp_snow[x+y*dx],j,i,fpge_mask_color);
					}
				}
		}

	total_tiles=dx*dy;
//kill last empty tiles
	found=0;
	for(x=dx*dy-1;x>=0;x--){
		for(i=0;i<TILE_HEIGHT;i++)
			for(j=0;j<TILE_FULL_WIDTH;j++){
				oc=getpixel(til_bmp[x],j,i);
				if (oc != fpge_mask_color) { found=1; break; }
			}
		if (!found) { destroy_bitmap(til_bmp[x]); total_tiles--; }
		else break; //break i
    }

	destroy_bitmap(bitmap_dry);
	destroy_bitmap(bitmap_muddy);
	destroy_bitmap(bitmap_frozen);
	return 0;
}

int load_bmp_flags(){
	FILE *inf;
	char path[MAX_PATH];
	BITMAP *flags_bitmap,*tmp_bmp;
	int x,y,dx,dy,i,j,c,pink;

	strncpy(path,"..\\graphics\\",MAX_PATH);
	strncat(path,pgf_flags_bmp,MAX_PATH);

	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (!inf){
		strncpy(path,"..\\..\\default\\graphics\\",MAX_PATH);
		strncat(path,pgf_flags_bmp,MAX_PATH);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf)
			return ERROR_PGF_BMP_FLAGS_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fclose(inf);
	if(fpge_colors_bits_shift) pink=0xe1e1ff+(fpge_colors_bits_shift==2?0xff000000:0);
	else pink=0xffe1e1;

	//all ok, load file
	flags_bitmap = load_bmp(path, NULL);
	//printf("%d %d %d\n",new_map_bitmap->w,new_map_bitmap->h, bitmap_color_depth(new_map_bitmap));

	if (flags_bitmap->w%TILE_FULL_WIDTH) return ERROR_PGF_BMP_FLAGS_BASE+ERROR_FPGE_WRONG_BMP_SIZE_X;
	if (flags_bitmap->h%TILE_HEIGHT) return ERROR_PGF_BMP_FLAGS_BASE+ERROR_FPGE_WRONG_BMP_SIZE_Y;

	dx=flags_bitmap->w/TILE_FULL_WIDTH;
	dy=flags_bitmap->h/TILE_HEIGHT;
	//printf("%d %d\n",dx,dy);

	for(y=0;y<dy;y++)
		for(x=0;x<dx;x++){
			//proxy_bitmap=create_sub_bitmap(new_map_bitmap,x*dx,y*dy,60,50);
			tmp_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			for(i=0;i<TILE_HEIGHT;i++)
				for(j=0;j<TILE_FULL_WIDTH;j++){
					c=getpixel(flags_bitmap,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i);
					c=make_color_fpge(c&0xff,(c>>8)&0xff,(c>>16)&0xff);
					//if (x+y*dx==146) printf("%d %d %06x\n",j,i,c);
					if (c!=pink)
						putpixel(tmp_bmp,j,i,c);
					else
						putpixel(tmp_bmp,j,i,fpge_mask_color);
				}

			flag_bmp[x+y*dx]=tmp_bmp;
		}

	total_flags=dx*dy;
	//printf("%d\n",total_flags);

	destroy_bitmap(flags_bitmap);
	return 0;
}

int load_bmp_strength(){
	FILE *inf;
	char path[MAX_PATH];
	BITMAP *strength_bitmap;
	int x,y,dx,dy,i,j,c,h=0,pink;

	strncpy(path,"..\\graphics\\",MAX_PATH);
	strncat(path,pgf_strength_bmp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (!inf){
		strncpy(path,"..\\..\\default\\graphics\\",MAX_PATH);
		strncat(path,pgf_strength_bmp,MAX_PATH);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf)
			return ERROR_PGF_BMP_STRENGTH_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fclose(inf);
	if(fpge_colors_bits_shift) pink=0xe1e1ff+(fpge_colors_bits_shift==2?0xff000000:0);
	else pink=0xffe1e1;
	//all ok, load file
	//printf("01\n");
	strength_bitmap = load_bmp(path, NULL);
	//printf("%d %d %d\n",new_map_bitmap->w,new_map_bitmap->h, bitmap_color_depth(new_map_bitmap));

	if (strength_bitmap->w%TILE_FULL_WIDTH) {
		destroy_bitmap(strength_bitmap);
		return ERROR_PGF_BMP_STRENGTH_BASE+ERROR_FPGE_WRONG_BMP_SIZE_X;
	}
	if (strength_bitmap->h%TILE_HEIGHT) {
		destroy_bitmap(strength_bitmap);
		return ERROR_PGF_BMP_STRENGTH_BASE+ERROR_FPGE_WRONG_BMP_SIZE_Y;
	}
	dx=strength_bitmap->w/TILE_FULL_WIDTH;
	dy=strength_bitmap->h/TILE_HEIGHT;
	//printf("%d %d\n",dx,dy);
	//printf("001\n");

	//we will read only 4 rows
	dy=4;
	for(y=0;y<dy;y++)
		for(x=0;x<dx;x++){
			//proxy_bitmap=create_sub_bitmap(new_map_bitmap,x*dx,y*dy,60,50);
			if (x==0) h++; //skip 0 counter
			//printf("0001\n");
			strength_bmp[h]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			//printf("0001\n");

			for(i=0;i<TILE_HEIGHT;i++)
				for(j=0;j<TILE_FULL_WIDTH;j++){
					c=getpixel(strength_bitmap,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i);
					c=make_color_fpge(c&0xff,(c>>8)&0xff,(c>>16)&0xff);
					//if (x+y*dx==146) printf("%d %d %06x\n",j,i,c);
					if (c!=pink)
						putpixel(strength_bmp[h],j,i,c);
					else
						putpixel(strength_bmp[h],j,i,fpge_mask_color);
				}
			h++;
		}
	//printf("1\n");
	//0 counters are not created
	//make 0 counters out of 10 counter
	if (!str0_bmp_loaded){
		for (h = 0; h < 4; h++){
			strength_bmp[0+MAX_STRENGTH_IN_ROW*h]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);

			for (i = 0; i < TILE_HEIGHT; i++)
						for (j = 0; j < TILE_FULL_WIDTH; j++)
							putpixel(strength_bmp[h * MAX_STRENGTH_IN_ROW], j, i,
									getpixel(strength_bmp[h * MAX_STRENGTH_IN_ROW + 10], j, i));

			c=getpixel(strength_bmp[0+MAX_STRENGTH_IN_ROW*h],23,37);
			rectfill(strength_bmp[0+MAX_STRENGTH_IN_ROW*h],24,38,29,46,c);

			for(i=38;i<45;i++)
			  for(j=28;j<28+6;j++)
				  putpixel(strength_bmp[MAX_STRENGTH_IN_ROW*h],j,i,getpixel(strength_bmp[MAX_STRENGTH_IN_ROW*h],j+2,i));
		}
	}
	destroy_bitmap(strength_bitmap);
	return 0;
}

int load_bmp_stackicn(){
	FILE *inf;
	char path[MAX_PATH];
	BITMAP *stack_bitmap;
	int x,y,dx,dy,i,j,c,found,pink;

	strncpy(path,"..\\graphics\\",MAX_PATH);
	strncat(path,pgf_stackicn_bmp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (!inf){
		strncpy(path,"..\\..\\default\\graphics\\",MAX_PATH);
		strncat(path,pgf_stackicn_bmp,MAX_PATH);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf)
			return ERROR_PGF_BMP_STACKICN_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fclose(inf);
	if(fpge_colors_bits_shift) pink=0xe1e1ff+(fpge_colors_bits_shift==2?0xff000000:0);
	else pink=0xffe1e1;
	//all ok, load file
	stack_bitmap = load_bmp(path, NULL);
	//printf("%d %d %d\n",new_map_bitmap->w,new_map_bitmap->h, bitmap_color_depth(new_map_bitmap));

	if (stack_bitmap->w%TILE_FULL_WIDTH) return ERROR_PGF_BMP_STACKICN_BASE+ERROR_FPGE_WRONG_BMP_SIZE_X;
	if (stack_bitmap->h%TILE_HEIGHT) return ERROR_PGF_BMP_STACKICN_BASE+ERROR_FPGE_WRONG_BMP_SIZE_Y;

	dx=stack_bitmap->w/TILE_FULL_WIDTH;
	dy=stack_bitmap->h/TILE_HEIGHT;
	//printf("%d %d\n",dx,dy);

	for(y=0;y<dy;y++)
		for(x=0;x<dx;x++){
			//proxy_bitmap=create_sub_bitmap(new_map_bitmap,x*dx,y*dy,60,50);
			stack_bmp[x+y*dx]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
			for(i=0;i<TILE_HEIGHT;i++)
				for(j=0;j<TILE_FULL_WIDTH;j++){
					c=getpixel(stack_bitmap,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i);
					c=make_color_fpge(c&0xff,(c>>8)&0xff,(c>>16)&0xff);
					//if (x+y*dx==146) printf("%d %d %06x\n",j,i,c);
					if (c!=pink)
						putpixel(stack_bmp[x+y*dx],j,i,c);
					else
						putpixel(stack_bmp[x+y*dx],j,i,fpge_mask_color);
				}
		}

	total_sicons=dx*dy;

	//kill last empty tiles
		found=0;
		for(x=dx*dy-1;x>=0;x--){
			for(i=0;i<TILE_HEIGHT;i++)
				for(j=0;j<TILE_FULL_WIDTH;j++){
					c=getpixel(stack_bmp[x],j,i);
					if (c != fpge_mask_color) { found=1; break; }
				}
			if (!found)  { destroy_bitmap(stack_bmp[x]); total_sicons--; }
			else break; //break i
	    }
	destroy_bitmap(stack_bitmap);
	return 0;
}

void place_flags(BITMAP *bmp,int x,int y, int idx, int mode, int tile_width, int tile_height){
	int color,xx,yy,i,cursor=0,bmp_num,ctr,dx,dy;
	int h,found_class=0,ncursor;

	//printf("idx=%d\n",idx);

	if (idx == 0)
		return;
	for (i = 0; i < total_equip; ++i) {
		bmp_num=equip[i][BMP]+256*equip[i][BMP+1];
		if ( bmp_num == idx){
			//printf("bmp_num=%d\n",bmp_num);
			//printf("equip_country[%d]=%d\n",i,equip_country[i]);
			if (pgf_mode || pacgen_mode){
				ctr = equip_country[i] - 1;
			}else{
				//i-unit number
				//is this class supported in NUPL
				found_class=-1;
				ctr=-1;
				for (h = 0; h < NUPL_CLASS_IDX; ++h)
					if (nupl_class[h] == equip[i][CLASS]) {
						found_class = h;
						break;
					}
				if (found_class>-1){
					for (h = 0; h < NUPL_COUNTRY_IDX; ++h){
						ncursor=nupl_cc[h][found_class];
						while (nupl[ncursor]!= -1){
							if (nupl[ncursor]==i){
								ctr=nupl_countries[h]-1;
								break; //found !
							}
							ncursor++;
						}
						if (ctr>0) break; //do not search more
					}
				}
				else
					//class not supported in NULP
					ctr=-1;
			}
			int flag_x =20;
			int flag_y =13;

			int flags_per_row = tile_width /flag_x;
			int flags_small_rows = tile_height/flag_y-2+((tile_height%flag_y)>10?1:0); //a bit cheat here
			if (cursor<flags_per_row*2+flags_small_rows*2 || mode==1){
				//mode 0
				if (mode ==0){
					dx=( (cursor>=(flags_per_row*2)?((cursor-flags_per_row*2)%2)*(flags_per_row-1):cursor)%flags_per_row)*flag_x;
					dy=(tile_height-flag_y+1)*(cursor>(flags_per_row-1) && cursor<(flags_per_row*2))+(cursor>(flags_per_row*2-1))*( (cursor-flags_per_row*2)/2+1 )*flag_y;
					//dx=( (cursor+(cursor>(flags_per_row*2))*(flags_per_row-2)+(cursor>(flags_per_row*2+2))*(flags_per_row-2))%flags_per_row)*flag_x;
					//dy=(tile_height-flag_y+1)*(cursor>(flags_per_row-1) && cursor<(flags_per_row*2))+(cursor>(flags_per_row*2-1))*flag_y+(cursor>(flags_per_row*2+1))*flag_y;
				}
				else{
					//mode 1
					dx=(cursor % 9)*20;
					dy=(cursor / 9)*13;
				}
				if (ctr>-1){
					if (ctr<MAX_FLAGS && flag_bmp[ctr]!=NULL){
					for (xx = 0; xx < 19; ++xx) {
						for (yy = 0; yy < 12; ++yy) {
							color=getpixel(flag_bmp[ctr],20+xx,36+yy);
							if (color == fpge_mask_color)
								color = 0xffe1e1;
							color=make_color_fpge((color>>16)&0xff,(color>>8)&0xff, color&0xff);
							putpixel(bmp, x + dx + xx, y + dy + yy, color);
						}
					}
					}
//					else{
//						print_dec(ctr);
//					}
				}else{
					rectfill(bmp,x+dx,y+dy,x+dx+19,y+dy+12,colors_to24bits(COLOR_WHITE));
				}
			}
			cursor++;
		}
	}
}

void draw_unit_basic_desc(BITMAP *bmp, int k, int kk, int off, int bmp_id) {
	int row=0,cell_shift=4,spacer=4;
	char str[256];

	//name
	sprintf(str, "%s", equip_name_utf8[k]);
	textout_ex(bmp, font, str, TILE_FULL_WIDTH, off + kk * TILE_HEIGHT + row + cell_shift, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));
	row += 8;
	sprintf(str, "ID: %4d  BMP: %d", k, bmp_id);
	textout_ex(bmp, font, str, TILE_FULL_WIDTH, off + kk * TILE_HEIGHT + spacer + row + cell_shift, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));
	row += 8;
	sprintf(str, "AVAIL: %02d/%02d -> %d", equip[k][MON], equip[k][YR], 1900 + equip[k][LAST_YEAR]);
	textout_ex(bmp, font, str, TILE_FULL_WIDTH, off + kk * TILE_HEIGHT + spacer + row + cell_shift, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));
	row += 8;
	sprintf(str, "CLASS: %s", pg_class_names[equip[k][CLASS]]);
	textout_ex(bmp, font, str, TILE_FULL_WIDTH, off + kk * TILE_HEIGHT + spacer + row + cell_shift, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));
	row += 8;
	sprintf(str, "MOV: %2d %s", equip[k][MOV], movement_type[equip[k][MOV_TYPE]]);
	textout_ex(bmp, font, str, TILE_FULL_WIDTH, off + kk * TILE_HEIGHT + spacer + row + cell_shift, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));

}


void draw_units_per_country_bmp(int country, int month, int year, int flipIcons){
	int i,j,k,kk, no_of_units,c,temp_shift,off,bmp_id, flip_this_icon=0;
	BITMAP *bmp;
	char country_units_bmp_name[256],str[256];

	temp_shift=fpge_colors_bits_shift;
    fpge_colors_bits_shift=1;

	//check number of units per country
	no_of_units=0;
	for(k=0;k< total_equip; k++)
		if (equip_country[k]==country && (int)equip[k][YR]*12+equip[k][MON] <= year*12+month && equip[k][LAST_YEAR] >= year ) {
			no_of_units++;
		}
	//printf("no_of_units=%d\n",no_of_units);
	if (no_of_units>0){
		off=12+4;
		bmp=create_bitmap_ex(24,TILE_FULL_WIDTH*4,off+TILE_HEIGHT*no_of_units);
		kk=0;
		// first line
		//flag
		for(i=0;i<off;i++)
			for(j=0;j<TILE_FULL_WIDTH*4;j++){
				if (j>=TILE_FULL_WIDTH-19 && j<TILE_FULL_WIDTH && i<12 && country>0 && flag_bmp[country-1]!=NULL){
					c=getpixel(flag_bmp[country-1],j+20-(TILE_FULL_WIDTH-19),i+36);
					if (c == fpge_mask_color)
						c=0xffe1e1;
			}
			else{
				c=0xffe1e1;
			}
			//24bit color magic
			c=make_color_fpge((c>>16)&0xff,(c>>8)&0xff, c&0xff);
			putpixel(bmp,j,i,c);
			}
		//name
		if (country>0){
			sprintf(str,"%s",country_names[country]);
			textout_ex(bmp, font, str, TILE_FULL_WIDTH + 4, 4, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));
			}
		if (year<100){
			sprintf(str,"%2d/%2d",month,year);
			textout_ex(bmp, font, str, 0, 4, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));
		}
        //flag
		//masked_blit( flag_bmp[country-1],  bmp,
        //		20, 36, 1, 1, 19, 12);

		for(k=0;k< total_equip; k++)
			if (equip_country[k]==country && (int)equip[k][YR]*12+equip[k][MON] <= year*12+month && equip[k][LAST_YEAR] >= year ){
				//printf("k=%d,kk=%d\n",k,kk);

				//unit picture
				bmp_id=(int)equip[k][BMP]+equip[k][BMP+1]*256;
				// check if unit should be flip, this is per icon not per country !!
				if (flipIcons){
					flip_this_icon=0;
					if (isFlipNeeded(bmp_id)) flip_this_icon=1;
				}
				//row=0;
				for(i=0;i<TILE_HEIGHT;i++)
					for(j=0;j<TILE_FULL_WIDTH*4;j++){
						if (j<TILE_FULL_WIDTH && unit_bmp[bmp_id]!=NULL){
							c=getpixel(unit_bmp[bmp_id],(flip_this_icon?TILE_FULL_WIDTH-j-1:j),i);
							if (c == fpge_mask_color)
								c=0xffe1e1;
						}
						else
							c=0xffe1e1;
						//24bit color magic
						c=make_color_fpge((c>>16)&0xff,(c>>8)&0xff, c&0xff);
						putpixel(bmp,j,off+kk*TILE_HEIGHT+i,c);
					}
				//description
				draw_unit_basic_desc(bmp,  k,  kk,  off,  bmp_id);

				line(bmp,0,off+kk*TILE_HEIGHT,TILE_FULL_WIDTH*4,off+kk*TILE_HEIGHT,colors_to24bits(COLOR_BLACK));

				kk++;
			}
		sprintf(country_units_bmp_name,"c%03d_%02d_%02d.bmp",country,month,year);
		if (save_bmp(country_units_bmp_name, bmp, NULL))
			printf("ERROR writing file %s !!\n",country_units_bmp_name);
		else
			printf("File %s saved.\n",country_units_bmp_name);
	}else{
		printf("No units for country %d found for month %d, year %d\n",country,month,year);
	}
	 fpge_colors_bits_shift=temp_shift;
}


void draw_units_bmp(BITMAP *draw_bmp, int comment_bmps, int flipIcons, int bmp_x, int tile_width, int tile_height){
    char str[256];
    int idx,c,bmp_y,x,y,i,j,offx=0,offy=0,temp_shift,flip_this_icon=0,ioffx=0,ioffy=0;

	if (comment_bmps == 1) {
		offx = 24;
		offy = 24;
	}

	ioffx=(tile_width-TILE_FULL_WIDTH)/2;
	ioffy=(tile_height-TILE_HEIGHT)/2;

	bmp_y=total_uicons/bmp_x+((total_uicons%bmp_x)?1:0);

	temp_shift=fpge_colors_bits_shift;
    fpge_colors_bits_shift=1;

    //printf("start\n");

    //draw border numbers
	if (comment_bmps==1)
		for(x=0;x<bmp_x;x++){
			snprintf(str,256,"%d",x);
			//printf("f1\n");
			textout_centre_ex(draw_bmp, font, str, offx + tile_width / 2 + x * tile_width, (offy - 8) / 2, colors_to24bits(COLOR_WHITE), colors_to24bits(COLOR_BLACK));
			textout_centre_ex(draw_bmp, font, str, offx + tile_width / 2 + x * tile_width, bmp_y * tile_height + offy + (offy - 8) / 2, colors_to24bits(COLOR_WHITE),
						colors_to24bits(COLOR_BLACK));
			//printf("f2\n");
		}

	for(y=0;y<bmp_y;y++){
		//draw border numbers
		if (comment_bmps==1){
			snprintf(str,256,"%d",y*bmp_x);
			//printf("f1\n");
			textout_ex(draw_bmp, font, str, 0, y * tile_height + offy + tile_height / 2, colors_to24bits(COLOR_WHITE), colors_to24bits(COLOR_BLACK));
			snprintf(str,256,"%d",y*bmp_x+bmp_x-1);
			textout_ex(draw_bmp, font, str, bmp_x * tile_width + offx, y * tile_height + offy + tile_height / 2, colors_to24bits(COLOR_WHITE), colors_to24bits(COLOR_BLACK));
			//printf("f2\n");
		}
		for(x=0;x<bmp_x;x++){
			idx=y*bmp_x+x;
			if (idx<total_uicons){
				rectfill(draw_bmp, offx + x * tile_width, offy + y * tile_height, offx + (x + 1) * tile_width - 1, offy + (y + 1) * tile_height - 1, make_color_fpge(0xff, 0xe1, 0xe1));

				//draw aux texts
				if (comment_bmps & 2){
					strncpy(str,icon_name_aux1[idx],8);
					str[8]=0;
					textout_centre_ex(draw_bmp, font, str, offx + x * tile_width + tile_width / 2, offy + y * tile_height, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));

					strncpy(str,icon_name_aux2[idx],8);
					str[8]=0;
					textout_centre_ex(draw_bmp, font, str, offx + x * tile_width + tile_width / 2, offy + (y + 1) * tile_height - 8, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));
				}
				//draw icon
				if (flipIcons){
					flip_this_icon=0;
					if (isFlipNeeded(idx)) flip_this_icon=1;
				}
				for (i = 0; i < TILE_HEIGHT; i++)
					for (j = 0; j < TILE_FULL_WIDTH; j++)
						if (unit_bmp[idx] != NULL) {
							c = getpixel(unit_bmp[idx], j, i);
							if (c == fpge_mask_color) {
								c = 0xffe1e1;
								continue;
							}
							c = make_color_fpge((c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff);
							putpixel(draw_bmp, ioffx+offx + x * tile_width + (flip_this_icon?TILE_FULL_WIDTH-j-1:j), ioffy+offy + y * tile_height + i, c);
						}
				//draw flags
				if (comment_bmps & 1){
					line(draw_bmp,offx+x*tile_width+tile_width-1,offy+y*tile_height,offx+x*tile_width+tile_width-1,offy+y*tile_height+tile_height-1,colors_to24bits(COLOR_BLACK));
					line(draw_bmp,offx+x*tile_width,offy+y*tile_height+tile_height-1,offx+x*tile_width+tile_width-1,offy+y*tile_height+tile_height-1,colors_to24bits(COLOR_BLACK));
					place_flags(draw_bmp,offx+x*tile_width,offy+y*tile_height,idx,0,tile_width,tile_height);
				}
			}else{
				//blank
				rectfill(draw_bmp, offx + x * tile_width, offy + y * tile_height, offx + (x + 1) * tile_width - 1, offy + (y + 1) * tile_height - 1, make_color_fpge(0xff, 0xe1, 0xe1));
				/*
				for(i=0;i<tile_height;i++)
					for(j=0;j<tile_width;j++)
						putpixel(draw_bmp,offx+x*tile_width+j,offy+y*tile_height+i,make_color_fpge(0xff, 0xe1, 0xe1));
				*/
			}
		}
	}
	 fpge_colors_bits_shift=temp_shift;
}

void draw_one_unit_bmp(BITMAP *draw_bmp, int bmp_idx, int comment_bmps, int flip_this_icon, int tile_width, int tile_height){
	 char str[256];
    int c,i,j,temp_shift,ioffx=0,ioffy=0;

	ioffx=(tile_width-TILE_FULL_WIDTH)/2;
	ioffy=(tile_height-TILE_HEIGHT)/2;

	temp_shift=fpge_colors_bits_shift;
    fpge_colors_bits_shift=1;

    if (bmp_idx< 0 || bmp_idx>total_uicons) return;
   //icon
    rectfill(draw_bmp, 0,0,tile_width,tile_height, make_color_fpge(0xff, 0xe1, 0xe1));

	for(i=0;i<TILE_HEIGHT;i++)
		for(j=0;j<TILE_FULL_WIDTH;j++){
			c=getpixel(unit_bmp[bmp_idx],j,i);
			if (c == fpge_mask_color) c=0xffe1e1;
			c=make_color_fpge((c>>16)&0xff,(c>>8)&0xff, c&0xff);
			putpixel(draw_bmp,ioffx+(flip_this_icon?TILE_FULL_WIDTH-j-1:j),ioffy+i,c);
		}

	if (comment_bmps) {
		//blank rest
		rectfill(draw_bmp, tile_width,0,tile_width*4,tile_height, make_color_fpge(0xff, 0xe1, 0xe1));

		//for (i = 0; i < TILE_HEIGHT; i++)
		//	for (j = 0; j < TILE_FULL_WIDTH * 3; j++)
		//		putpixel(draw_bmp, TILE_FULL_WIDTH + j, i, 0xe1e1ff);

		if (comment_bmps & 1) {
			//draw flags
			place_flags(draw_bmp, tile_width, 0, bmp_idx, 1,tile_width,tile_height);
		}

		if (comment_bmps & 2) {
			//draw texts
			strncpy(str,icon_name_aux1[bmp_idx],8);
			str[8]=0;
			textout_centre_ex(draw_bmp, font, str, tile_width / 2, 0, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));

			strncpy(str,icon_name_aux2[bmp_idx],8);
			str[8]=0;
			textout_centre_ex(draw_bmp, font, str, tile_width / 2, tile_height - 8, make_color_fpge(0, 0, 0), make_color_fpge(0xff, 0xe1, 0xe1));
		}
	}
	fpge_colors_bits_shift=temp_shift;
}

int save_pgf_units_menu_bmp(){
	return save_pgf_units_bmp(0);
}

int save_pgf_units_bmp(int comment_bmps){
    char path[MAX_PATH];
    //int bmp_x,bmp_y,offx=0,offy=0;
    BITMAP *pgf_tacicons_bmp;

	strncpy(path,pgf_units_bmp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	if (file_select_ex("Save PGF 'tacicons.bmp' file",path,"bmp", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)){

		//printf("%d\n",total_uicons);
		//if (comment_bmps) { offx=20; offy=20; }
		//bmp_x=20;
		//bmp_y=total_uicons/bmp_x+(total_uicons%bmp_x?1:0);
		pgf_tacicons_bmp = create_bitmap_ex(24, 20 * TILE_FULL_WIDTH + (comment_bmps == 1) * 2 * 24, (total_uicons / 20 + ((total_uicons % 20) ? 1 : 0)) * TILE_HEIGHT + (comment_bmps == 1) * 2 * 24);

	    if (!pgf_tacicons_bmp){
			alert("ERROR: Cannot save PGF 'tacicons.bmp' file.", "Not enough memory.", NULL,"&Continue", NULL, 'c', 0);
		}else{
            sprintf(MapStatusTxt,"Saving PGF 'tacicons.bmp'\nPlease wait...");
            d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

            draw_units_bmp(pgf_tacicons_bmp,comment_bmps,0,20,TILE_FULL_WIDTH,TILE_HEIGHT);

			if (save_bmp(path, pgf_tacicons_bmp, pgpal)) {
				alert("ERROR: Cannot save PGF 'tacicons.bmp' file.", "File cannot be saved.", NULL,"&Continue", NULL, 'c', 0);
			}
			destroy_bitmap(pgf_tacicons_bmp);

			sprintf(MapStatusTxt,"'tacicons.bmp' file saved\nPick an Operation.");
			main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
            //d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
		}
	}
	return D_O_K;
}

int save_pgf_tiles_bmp(char *name, int tile_idx){
    char path[MAX_PATH], tempstr[256];
    int idx,c,bmp_x,bmp_y,x,y,i,j, temp_shift;
    BITMAP *pgf_tacmap_bmp;

    sprintf(tempstr,"Save PGF %s file",name);
	sprintf(path,".\\%s",name);
	canonicalize_filename(path,path,MAX_PATH);
	if (file_select_ex(tempstr,path,"bmp", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)){

		//("%d\n",total_tiles);

		bmp_x=20;
		bmp_y=total_tiles/bmp_x+((total_tiles%bmp_x)?1:0);
		pgf_tacmap_bmp=create_bitmap_ex(24, bmp_x*TILE_FULL_WIDTH, bmp_y*TILE_HEIGHT);

	    if (!pgf_tacmap_bmp){
	    	sprintf(tempstr,"ERROR: Cannot save %s file.",name);
			alert(tempstr, "Not enough memory.", NULL,"&Continue", NULL, 'c', 0);
		}else{
			sprintf(MapStatusTxt,"Saving %s\nPlease wait...",name);
            //sprintf(MapStatusTxt,tempstr);
            d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

            temp_shift=fpge_colors_bits_shift;
            fpge_colors_bits_shift=1;

			for(y=0;y<bmp_y;y++)
				for(x=0;x<bmp_x;x++){
					idx=y*bmp_x+x;
					if (idx<total_tiles){

						for(i=0;i<TILE_HEIGHT;i++)
							for(j=0;j<TILE_FULL_WIDTH;j++){
								switch (tile_idx) {
								case 0:
									c = getpixel(til_bmp[idx], j, i);
									break;
								case 1:
									c = getpixel(til_bmp_mud[idx], j, i);
									break;
								case 2:
									c = getpixel(til_bmp_snow[idx], j, i);
									break;
								default:
									c = 0;
									break;
								}
								if (c == fpge_mask_color) {
									c = 0xffe1e1;
								}
								c=make_color_fpge((c>>16)&0xff,(c>>8)&0xff, c&0xff);
								putpixel(pgf_tacmap_bmp,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i,c);
							}

					}else{
						//blank
						for(i=0;i<TILE_HEIGHT;i++)
							for(j=0;j<TILE_FULL_WIDTH;j++)
								putpixel(pgf_tacmap_bmp,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i,0xe1e1ff);
					}
				}

			 fpge_colors_bits_shift=temp_shift;
			if (save_bmp(path, pgf_tacmap_bmp, pgpal)) {
				sprintf(tempstr,"ERROR: Cannot save PGF %s file.",name);
				alert(tempstr, "File cannot be saved.", NULL,"&Continue", NULL, 'c', 0);
			}
			destroy_bitmap(pgf_tacmap_bmp);

			sprintf(MapStatusTxt,"%s saved.\nPick an Operation.",name);
			main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
            //d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
		}
	}
	return D_O_K;
}

int save_pgf_dry_tiles_bmp(){

	save_pgf_tiles_bmp(pgf_tacmap_dry_bmp, 0);
	save_pgf_tiles_bmp(pgf_tacmap_muddy_bmp, 1);
	save_pgf_tiles_bmp(pgf_tacmap_frozen_bmp, 2);

	return D_O_K;
}


int save_pgf_stack_bmp(){
    char path[MAX_PATH];
    int idx,c,bmp_x,bmp_y,x,y,i,j, temp_shift;
    BITMAP *pgf_stack_icons_bmp;

	strncpy(path,pgf_stackicn_bmp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	if (file_select_ex("Save PGF 'stackicn.bmp' file",path,"bmp", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)){

		//printf("%d\n",total_uicons);

		bmp_x=20;
		bmp_y=total_sicons/bmp_x+((total_sicons%bmp_x)?1:0);
		pgf_stack_icons_bmp=create_bitmap_ex(24, bmp_x*TILE_FULL_WIDTH, bmp_y*TILE_HEIGHT);

	    if (!pgf_stack_icons_bmp){
			alert("ERROR: Cannot save PGF 'stackicn.bmp' file.", "Not enough memory.", NULL,
	       "&Continue", NULL, 'c', 0);
		}else{
            sprintf(MapStatusTxt,"Saving PGF '%s'\nPlease wait...",pgf_stackicn_bmp);
            d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

            temp_shift=fpge_colors_bits_shift;
            fpge_colors_bits_shift=1;

			for(y=0;y<bmp_y;y++)
				for(x=0;x<bmp_x;x++){
					idx=y*bmp_x+x;

					if (idx<total_sicons){
						//printf("idx:total_sicons=%d:%d\n",idx,total_sicons);
						for (i = 0; i < TILE_HEIGHT; i++)
							for (j = 0; j < TILE_FULL_WIDTH; j++)
								if (stack_bmp[idx] != NULL) {
									c = getpixel(stack_bmp[idx], j, i);
									if (c == fpge_mask_color) {
										c = 0xffe1e1;
									}
									c = make_color_fpge((c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff);
									putpixel(pgf_stack_icons_bmp, x * TILE_FULL_WIDTH + j, y * TILE_HEIGHT + i, c);
								}

					}else{
						//blank
						for(i=0;i<TILE_HEIGHT;i++)
							for(j=0;j<TILE_FULL_WIDTH;j++)
								putpixel(pgf_stack_icons_bmp,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i,0xe1e1ff);
					}
				}

			 fpge_colors_bits_shift=temp_shift;
			if (save_bmp(path, pgf_stack_icons_bmp, pgpal)) {
				alert("ERROR: Cannot save PGF 'stackicn.bmp' file.", "File cannot be saved.", NULL,"&Continue", NULL, 'c', 0);
			}
			destroy_bitmap(pgf_stack_icons_bmp);

			sprintf(MapStatusTxt,"'%s' file saved\nPick an Operation.",pgf_stackicn_bmp);
			main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
            //d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
		}
	}
	return D_O_K;
}

int save_pgf_flags_bmp(){
    char path[MAX_PATH];
    int idx,c,bmp_x,bmp_y,x,y,i,j, temp_shift;
    BITMAP *pgf_stack_icons_bmp;

    strncpy(path,pgf_flags_bmp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	if (file_select_ex("Save PGF 'flags.bmp' file",path,"bmp", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)){

		//printf("%d\n",total_uicons);

		bmp_x=total_countries;
		bmp_y=2;
		//printf("total_countries=%d\n",total_countries);
		//printf("total_flags=%d\n",total_flags);
		//bmp_y=total_flags/bmp_x+(total_flags%bmp_x?1:0);
		pgf_stack_icons_bmp=create_bitmap_ex(24, bmp_x*TILE_FULL_WIDTH, bmp_y*TILE_HEIGHT);

	    if (!pgf_stack_icons_bmp){
			alert("ERROR: Cannot save PGF 'flags.bmp' file.", "Not enough memory.", NULL, "&Continue", NULL, 'c', 0);
		}else{
            sprintf(MapStatusTxt,"Saving PGF '%s'\nPlease wait...",pgf_flags_bmp);
            d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

            temp_shift=fpge_colors_bits_shift;
            fpge_colors_bits_shift=1;

			for(y=0;y<bmp_y;y++)
				for(x=0;x<bmp_x;x++){
					idx=y*bmp_x+x;
					//printf("idx=%d flag_bmp[idx]=%p\n",idx,flag_bmp[idx]);
					if (idx<total_flags){

						for(i=0;i<TILE_HEIGHT;i++)
							for(j=0;j<TILE_FULL_WIDTH;j++){
								c = fpge_mask_color;
								if (y>0){
									if (victory_flag_bmp[x]!=NULL)
										c=getpixel(victory_flag_bmp[x],j,i);
								}else{
									if (flag_bmp[x]!=NULL)
										c=getpixel(flag_bmp[x],j,i);
								}
								if (c == fpge_mask_color) {
									c = 0xffe1e1;
								}
								c = make_color_fpge((c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff);
								putpixel(pgf_stack_icons_bmp, x * TILE_FULL_WIDTH + j, y * TILE_HEIGHT + i, c);
							}

					}else{
						//blank
						for(i=0;i<TILE_HEIGHT;i++)
							for(j=0;j<TILE_FULL_WIDTH;j++)
								putpixel(pgf_stack_icons_bmp,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i,0xe1e1ff);
					}
				}

			 fpge_colors_bits_shift=temp_shift;
			if (save_bmp(path, pgf_stack_icons_bmp, pgpal)) {
				alert("ERROR: Cannot save PGF 'flags.bmp' file.", "File cannot be saved.", NULL,"&Continue", NULL, 'c', 0);
			}
			destroy_bitmap(pgf_stack_icons_bmp);

			sprintf(MapStatusTxt,"'%s' file saved\nPick an Operation.",pgf_flags_bmp);
			main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
            //d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
		}
	}
	return D_O_K;
}

int save_pgf_strength_bmp(){
    char path[MAX_PATH];
    int idx,c,bmp_x,bmp_y,x,y,i,j, temp_shift;
    int x_limit,y_limit;
    BITMAP *pgf_stack_icons_bmp;

	strncpy(path,pgf_strength_bmp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	if (file_select_ex("Save PGF 'strength.bmp' file",path,"bmp", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)){

		//printf("%d\n",total_uicons);

		bmp_x=20;
		bmp_y=8;
		//bmp_y=total_flags/bmp_x+(total_flags%bmp_x?1:0);
		pgf_stack_icons_bmp=create_bitmap_ex(24, bmp_x*TILE_FULL_WIDTH, bmp_y*TILE_HEIGHT);

	    if (!pgf_stack_icons_bmp){
			alert("ERROR: Cannot save PGF 'strength.bmp' file.", "Not enough memory.", NULL,
	       "&Continue", NULL, 'c', 0);
		}else{
            sprintf(MapStatusTxt,"Saving PGF '%s'\nPlease wait...",pgf_strength_bmp);
            d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);

            temp_shift=fpge_colors_bits_shift;
            fpge_colors_bits_shift=1;

            x_limit=15; //PG limits
            y_limit=3;

            if (pgf_mode) x_limit=20;
            if (pgf_mode || ag_mode || pacgen_mode) y_limit=4;

			for(y=0;y<bmp_y;y++)
				for(x=0;x<bmp_x;x++){
					idx=y*bmp_x+x;
					if (idx%20<x_limit && idx<y_limit*20){

						for(i=0;i<TILE_HEIGHT;i++)
							for(j=0;j<TILE_FULL_WIDTH;j++){
								c=getpixel(strength_bmp[x+y*21+1],j,i);
								if (c == fpge_mask_color) c=0xffe1e1;
								c=make_color_fpge((c>>16)&0xff,(c>>8)&0xff, c&0xff);
								//exchange aux with core : (y+(y<2?(y+2)%3-1:0) )
								putpixel(pgf_stack_icons_bmp,x*TILE_FULL_WIDTH+j,(y+(y<2?(y+2)%3-1:0) )*TILE_HEIGHT+i,c);
							}

					}else{
						if (idx%20<x_limit && idx<(y_limit+4)*20 && y>=4){
							for(i=0;i<TILE_HEIGHT;i++)
								for(j=0;j<TILE_FULL_WIDTH;j++){
									c=getpixel(strength_bmp[x+(y-4)*21+1],j,i);
									if (c == fpge_mask_color) c=0xffe1e1;
									if (c == 0x000000) c=0x7f7f7f; //PG DOS black
									if (c == 0x070B0B) c=0x7f7f7f; //AG, PG WIN black
									if (c == 0xffffff) c=0xe6e6e6; //AG, PG white

									c=make_color_fpge((c>>16)&0xff,(c>>8)&0xff, c&0xff);
									putpixel(pgf_stack_icons_bmp,x*TILE_FULL_WIDTH+j,(y+(((y/2+1)%2)?(y+2-4)%3-1:0) )*TILE_HEIGHT+i,c);
								}
						}else{//blank
						for(i=0;i<TILE_HEIGHT;i++)
							for(j=0;j<TILE_FULL_WIDTH;j++)
								putpixel(pgf_stack_icons_bmp,x*TILE_FULL_WIDTH+j,y*TILE_HEIGHT+i,0xe1e1ff);
						}
					}
				}

			 fpge_colors_bits_shift=temp_shift;
			if (save_bmp(path, pgf_stack_icons_bmp, pgpal)) {
				alert("ERROR: Cannot save PGF 'strength.bmp' file.", "File cannot be saved.", NULL,"&Continue", NULL, 'c', 0);
			}
			destroy_bitmap(pgf_stack_icons_bmp);

			sprintf(MapStatusTxt,"'%s' file saved\nPick an Operation.",pgf_strength_bmp);
			main_dlg[dmMapStatusIdx].flags |= D_DIRTY;
            //d_mapstatus_proc(MSG_DRAW,&(main_dlg[dmMapStatusIdx]),0);
		}
	}
	return D_O_K;
}


/*
//this is wrong
int bomber_special(int idx){
	short bs_id[]={20,21,22,115,116,126,132,172,
	173,239,240,241,242,306,345,346,
	347,348,133,23,24,25,26,27,
	28,176,238,243,307,308,177,349,
	350,351,352,353};

	int bs_value[]={
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,10,20,20,20,20,20,
	20,20,20,20,20,20,40,40,
	40,40,40,40};

	int bs_count=8*4+4,i;

	for (i=0;i<bs_count;i++)
		if (bs_id[i]==idx) return bs_value[i];
	return 0;
}
*/

int ignore_ent(int idx){
short ie_id[]={48,215,276,330,386,387,388};
int ie_count=7,i;

if(pacgen_mode || pgf_mode)
	return (equip_flags[idx] & EQUIPMENT_IGNORES_ENTRENCHMENT ?1:0);

for (i=0;i<ie_count;i++)
	if (ie_id[i]==idx) return 1;
return 0;
}

int can_have_org(int type, int idx){

	if (idx==120 || idx==275) return 0; //PL cav, ST cav
	if (type==3 || type==4) return 1;
	return 0;
}

int can_have_sea_tra(int class, int idx){

	if ((class>=6 && class<=14)|| class==16) return 0; //6,7,8,9,10,11,12,13,14,16
	return 1;
}

int can_bridge(int idx){
short cb_id[]={215,276,330,388};
int cb_count=4,i;

if(pacgen_mode || pgf_mode)
	return (equip_flags[idx] & EQUIPMENT_CAN_BRIDGE_RIVERS?1:0);

for (i=0;i<cb_count;i++)
	if (cb_id[i]==idx) return 1;
return 0;
}

int is_jet(int idx){
short ij_id[]={7,8,10,169};
int ij_count=4,i;

if(pacgen_mode || pgf_mode)
	return (equip_flags[idx] & EQUIPMENT_JET?1:0);

for (i=0;i<ij_count;i++)
	if (ij_id[i]==idx) return 1;
return 0;
}

int quote(char *des, char *src, int max_size) {
	//0x22
	int i, j;

	if (strchr(src, 0x22) != NULL) {
		des[0] = 0x22;
		j = 1;
		for (i = 0; i < strlen(src); i++) {
			if (src[i] == 0x22) {
				des[j] = 0x22;
				j++;
			}
			des[j] = src[i];
			j++;
		}
		des[j] = 0x22;
		des[j + 1] = 0;

		return 0;
	}

	//just copy
	strncpy(des, src, max_size);
	return 0;
}

int save_pgf_equipment(){
	char pgequip_country[MAX_UNITS];
	short pgequip_unit[MAX_UNITS];
	int cursor,i,j,pgequ,unit_id;
	char line[1024], temp_str[256], temp_str2[256];
    char path[MAX_PATH];
    FILE *outf;
    unsigned short temp;

//    if (pgf_mode){
//		alert("ERROR: Cannot save PGF equipment file.", "FPGE is in PGF mode !", NULL,
//			       "&Continue", NULL, 'c', 0);
//		return D_O_K;
//    }

    strncpy(path,".\\equipment.pgeqp",MAX_PATH);
    canonicalize_filename(path,path,MAX_PATH);
	if (file_select_ex("Save PGF equipment file",path,"pgeqp", MAX_PATH, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT)){

	outf=fopen(path,"wb");
	if (!outf){
		alert("ERROR: Cannot save PGF equipment file.", "Access to file denied", NULL,
			       "&Continue", NULL, 'c', 0);
		return D_O_K;
	}

	pgequ=total_equip;

	if (!pacgen_mode && !pgf_mode) {

			for (i = 0; i < total_equip; i++) {
				pgequip_country[i] = -1; //this unit is for all flags
				pgequip_unit[i] = i;
			}

			for (i = 0; i < NUPL_COUNTRY_IDX; i++)
				for (j = 0; j < NUPL_CLASS_IDX; j++) {
					cursor = nupl_cc[i][j];
					while (nupl[cursor] != -1) {
						if (pgequip_country[nupl[cursor]] == -1) {
							//Assign to current country
							pgequip_country[nupl[cursor]] = nupl_countries[i];
						} else {
							//add new
							//printf("pgequ=%d countr=%d unit=%d\n",pgequ,nupl_countries[i],pgequip_unit[nupl[cursor]] );
							pgequip_country[pgequ] = nupl_countries[i];
							pgequip_unit[pgequ] = pgequip_unit[nupl[cursor]];
							pgequ++;
						}
						cursor++;
					}
				}
		} else {
			//just copy
			for (i = 0; i < total_equip; i++) {
				pgequip_country[i] = equip_country[i]; //this unit is for all flags
				pgequip_unit[i] = i;
			}
		}
	//ready, just format it
	strncpy(line,
    "# ID\tName\tClass\tSoft Attack\tHard Attack\tAir Attack\tNaval Attack\t"
	"Ground Defense\tAir Defense\tClose Defense\tMovement Type\tInitiative\t"
	"Range\tSpotting\tTarget Type\tMovement\tMax Fuel\tMax Ammo\tCost\tIcon\t"
	"Animation\tMonth\tYear\tLast Year\tBomber Special\tIgnores Entrenchment\t"
	"Can Have Organic Transport\tCan Have Sea Transport\tCan Have Air Transport\t"
	"Can Paradrop\tCan Bridge Rivers\tJet\tFlag",1024);

	fwrite(&UCS2_header,sizeof(UCS2_header),1,outf);
    fake_UTF_write_string_with_eol(outf,"# PG Forever equipment table created with FPGE");
    fake_UTF_write_string_with_eol(outf,line);

	for(i=0;i<pgequ;i++){
		unit_id=pgequip_unit[i];
		strncpy(line,"",1024);
		sprintf(temp_str,"%d\t",i); //ID
		strncat(line,temp_str, 1024);
		quote(temp_str2,equip_name_utf8[unit_id],256);//Name
		sprintf(temp_str,"%s\t",temp_str2);
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][CLASS]); //class
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][SA]); //Soft
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][HA]); //Hard
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][AA]); //Air
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][NA]); //Naval
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][GD]); //Ground defence
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][AD]); //Air Defence
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][CD]); //Close Defence
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][MOV_TYPE]); //Movement type
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][INITIATIVE]); //Initiative
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][RANGE]); //Range
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][SPOTTING]); //Spotting
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][TARGET_TYPE]); //Target Type
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][MOV]); //Movement
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][FUEL]); //Max Fuel
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][AMMO]); //Max Ammo
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][COST]*COST_DIVISOR); //Cost
		strncat(line,temp_str, 1024);
		temp=(unsigned short)equip[unit_id][BMP]+256*equip[unit_id][BMP+1];
		sprintf(temp_str,"%d\t",temp); //Icon
		strncat(line,temp_str, 1024);
		temp=(unsigned short)equip[unit_id][ANI]+256*equip[unit_id][ANI+1];
		sprintf(temp_str,"%d\t",temp); //Animation
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][MON]); //Month
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][YR]); //Year
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][LAST_YEAR]); //Last Year
		strncat(line,temp_str, 1024);
		//sprintf(temp_str,"%d\t",bomber_special(unit_id)); //Bomber Special
		sprintf(temp_str,"%d\t",equip[unit_id][AAF]); //Bomber Special
		//sprintf(temp_str,"%d:%d:%d:%d\t",equip[unit_id][AAF],equip[unit_id][AAF+1],equip[unit_id][39],equip[unit_id][40]);
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",ignore_ent(unit_id)); //Ignores Entrenchment
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",can_have_org(equip[unit_id][MOV_TYPE],unit_id)); //Can Have Organic Transport
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][ALLOWED_TRANSPORT]>0); //Can Have Sea Transport
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][ALLOWED_TRANSPORT]>1); //Can Have Air Transport (?)
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",equip[unit_id][ALLOWED_TRANSPORT]>2); //Can Paradrop
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",can_bridge(unit_id)); //Can Bridge Rivers
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d\t",is_jet(unit_id)); //Jet
		strncat(line,temp_str, 1024);
		sprintf(temp_str,"%d",pgequip_country[i]); //Flag, last column
		strncat(line,temp_str, 1024);
		fake_UTF_write_string_with_eol(outf,line);
		//fflush(outf);
		//printf("%d, %d, %s\n",i,pgequip_country[i],equip_name_utf8[pgequip_unit[i]]);
	}
	fclose(outf);
	}//end of file select
	return D_O_K;
}

int create_small_tiles_out_of_big(){
	int i;
	total_mtiles=total_tiles;
//60
	for (i=0; i<total_mtiles; ++i)
	  {
	   mtil_bmp[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	   dark_mtil_bmp[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	   mtil_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	   dark_mtil_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	   mtil_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	   dark_mtil_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);

	   stretch_blit( til_bmp[i],  mtil_bmp[i], 0, 0, til_bmp[i]->w, til_bmp[i]->h, 0, 0, mtil_bmp[i]->w, mtil_bmp[i]->h);
	   stretch_blit( dark_til_bmp[i],  dark_mtil_bmp[i], 0, 0, dark_til_bmp[i]->w, dark_til_bmp[i]->h, 0, 0, dark_mtil_bmp[i]->w, dark_mtil_bmp[i]->h);

	   stretch_blit( til_bmp_mud[i],  mtil_bmp_mud[i], 0, 0, til_bmp_mud[i]->w, til_bmp_mud[i]->h, 0, 0, mtil_bmp_mud[i]->w, mtil_bmp_mud[i]->h);
	   stretch_blit( dark_til_bmp_mud[i],  dark_mtil_bmp_mud[i], 0, 0, dark_til_bmp_mud[i]->w, dark_til_bmp_mud[i]->h, 0, 0, dark_mtil_bmp_mud[i]->w, dark_mtil_bmp_mud[i]->h);

	   stretch_blit( til_bmp_snow[i],  mtil_bmp_snow[i], 0, 0, til_bmp_snow[i]->w, til_bmp_snow[i]->h, 0, 0, mtil_bmp_snow[i]->w, mtil_bmp_snow[i]->h);
	   stretch_blit( dark_til_bmp_snow[i],  dark_mtil_bmp_snow[i], 0, 0, dark_til_bmp_snow[i]->w, dark_til_bmp_snow[i]->h, 0, 0, dark_mtil_bmp_snow[i]->w, dark_mtil_bmp_snow[i]->h);

	  }
	return 0;
}

/* function to pass to hash_free_table */
void strfree( void *d )
{
      /* any additional processing goes here (if you use structures as data) */
      /* free the datapointer */
      free(d);
}

int parse_pgcam( int show_info,  int probe_file_only){

	  FILE *inf, *outf;
	  char path[MAX_PATH], brfnametmp[256], color_code[256], br_color_code[256], scenario_node[256];
	  char line[1024],tokens[20][256];
	  int j,i,block=0,last_line_length=-1,cursor=0,token=0, sub_graph_counter=0;
	  int lines;

	  hash_table table;

	  snprintf(path,MAX_PATH,pgf_pg_pgcam);
	  canonicalize_filename(path,path,MAX_PATH);
	  if (show_info && probe_file_only==LOAD_FILE)
		  printf("Opening file %s\n",path);
	  inf=fopen(path,"rb");
	  if (!inf)
	  {
	    //printf("Couldn't open scenario file\n");
	    return ERROR_PGCAM_FILE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }
	  if (probe_file_only==PROBE_FILE){
		  fclose(inf);
		  return ERROR_PGCAM_FILE_BASE+ERROR_FPGE_FILE_FOUND;
	  }

	  hash_construct_table(&table,211);

	  snprintf(path,MAX_PATH,fpge_pgcam_gv);
	  canonicalize_filename(path,path,MAX_PATH);
	  outf=fopen(path,"wb");
	  if (!outf)
	  {
	    //printf("Couldn't open scenario file\n");
		  fclose(inf);
		  return ERROR_PGCAM_FILE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }

	  fprintf(outf,"digraph G { \n");

	  while (read_utf16_line_convert_to_utf8(inf,line)>=0){
		  //count lines so error can be displayed with line number
		  lines++;

			  //strip comments
			  for(i=0;i<strlen(line);i++)
				  if (line[i]==0x23) { line[i]=0; break; }
			  if (strlen(line)>0 && last_line_length==0){
				  block++;
			  }
			  last_line_length=strlen(line);
			  token=0;
			  cursor=0;
			  for(i=0;i<strlen(line);i++)
				  if (line[i]==0x09) {tokens[token][cursor]=0;token++;cursor=0;}
				  else {tokens[token][cursor]=line[i]; cursor++;}
			  tokens[token][cursor]=0;
			  token++;

			  //entry points
			  if (block == 2 && token > 1){

				  fprintf(outf,"\"%s\" [shape=diamond, fillcolor=crimson, style=filled, color=black ]\n",tokens[0]);
				  fprintf(outf,"\"%s\" -> \"%s\" %s\n",tokens[0],tokens[1],"[color=crimson]");
			  }
			  //Block#5  path
			  if (block == 5 && token > 1) {
				  //header
				  strncpy(scenario_node,tokens[0],256);
				  //check brefing
				  if (strlen(tokens[1])>0){
					  strncat(scenario_node,"+",256);

					  fprintf(outf,"subgraph cluster_%d { color=white \n",sub_graph_counter);

					  //cheat
					  fprintf(outf,"\"%s\" [shape=ellipse, style=filled, label=\"%s\", color=darkgoldenrod1]\n",tokens[0],tokens[1]); //darkgoldenrod1
					  //link
					  fprintf(outf,"\"%s\" -> \"%s\" [color=darkgoldenrod1]\n",tokens[0],scenario_node);
					  //scenario node
					  fprintf(outf,"\"%s\" [shape=box, fillcolor=darkgoldenrod1, style=filled, color=black, label=\"%s\"]\n",scenario_node,tokens[0]); //+

					  fprintf(outf,"}\n");
					  sub_graph_counter++;
				  }else{
					  fprintf(outf,"\"%s\" [shape=box, fillcolor=darkgoldenrod1, style=filled, color=black, label=\"%s\"]\n",scenario_node,tokens[0]); //chartreuse2
				  }

				  for (j = 0; j < 3; j++) {
				switch (j) {
				case 0:
					strncpy(color_code, "[color=blue]", 256);
					strncpy(br_color_code, "[color=blue2]", 256);
					break;
				case 1:
					strncpy(color_code, "[color=green]", 256);
					strncpy(br_color_code, "[color=green2]", 256);
					break;
				case 2:
					strncpy(color_code, "[color=red]", 256);
					strncpy(br_color_code, "[color=red2]", 256);
					break;
				}
				  if (strlen(tokens[3*(j+1)+2])>0){
					  strncpy(brfnametmp,tokens[3*(j+1)+2],256);
					  if (hash_lookup(brfnametmp,&table)){
						  //printf("%s juz jest\n",brfnametmp);
						  while(hash_lookup(brfnametmp,&table)){
							  strncat(brfnametmp,"+",256);
						  }
						  hash_insert( brfnametmp, strdup(brfnametmp), &table );
						  //printf("%s nie ma\n",brfnametmp);
					  }else{
						  hash_insert( brfnametmp, strdup(brfnametmp), &table );
					  }

					  fprintf(outf,"\"%s\" [shape=ellipse, style=filled, label=\"%s\"] %s\n",brfnametmp,tokens[3*(j+1)+2],br_color_code); //darkgoldenrod1
					  parse_pgbrf(outf,tokens[3*(j+1)+2],brfnametmp,color_code);
					  fprintf(outf,"\"%s\" -> \"%s\" %s\n",scenario_node,brfnametmp,color_code);
					  if (strlen(tokens[3*(j+1)])>0)
						  fprintf(outf,"\"%s\" -> \"%s\" %s\n",brfnametmp,tokens[3*(j+1)],color_code);
				  }else
					  fprintf(outf,"\"%s\" -> \"%s\" %s\n",scenario_node,tokens[3*(j+1)],color_code);
				  }
			}
	  }
	  //end node
	  fprintf(outf,"\"END\" [shape=diamond, fillcolor=crimson, style=filled, color=black ]\n");

	  fprintf(outf,"}\n");
	  fclose(inf);
	  fclose(outf);

      hash_free_table( &table, strfree );

	  if (show_info) {
		  printf("PGF campaign file loaded.\n");
		  printf("'%s' file written. To make pgcam.png use : dot %s -Tpng > pgcam.png\n",fpge_pgcam_gv,fpge_pgcam_gv);
	  }

	  return 0;
}

int parse_pgbrf(FILE *outf, char *path, char *node_name, char *color){

	  FILE *inf;
	  char str[1024];
	  int str_idx=0;
	  int state=0;
	  int buf;

	  inf=fopen(path,"rb");
	  if (!inf)
	  {
	    //printf("Couldn't open scenario file\n");
	    return ERROR_PGBRF_FILE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }
	    do {
	    	buf = fgetc (inf);
	    	if (state==0 && buf=='<')  {state=1; continue;}
	    	if (state==1 && buf=='a')  state=2;
	    	if (state==1 && buf!='a')  state=0;
	    	if (state==2 && buf=='>')  state=0;
	    	if (state==2){
	    		if (buf=='h') { state=3;continue;}
	    	}
	    	if (state==3){
	    		if (buf=='r') { state=4;continue;}
	    		state=2;
	    	}
	    	if (state==4){
	    		if (buf=='e') { state=5;continue;}
	    		state=2;
	    	}
	    	if (state==5){
	    		if (buf=='f') { state=6;continue;}
	    		state=2;
	    	}
	    	if (state==6){
	    		if (buf=='\"') { state=7;continue;}
	    	}
	    	if (state==7){
	    		if (buf=='\"') {
	    			str[str_idx]=0;
	    			fprintf(outf,"\"%s\" -> \"%s\" %s\n",node_name,str,color);
	    			//printf("\n");
	    			state=0;
	    			str_idx=0;
	    			continue;
	    		}
	    	}
	    	if (state==7 && buf != EOF){
	    		str[str_idx]=(char)buf;
	    		str_idx++;
	    		//printf("%c",buf);
	    	}
	    		//printf("%c%d",buf,state);
	    } while (buf != EOF);
	  fclose(inf);
	  return 0;
}

int check_pgbrf(int show_info,char *path){

	  FILE *inf;
	  int buf,fsize,result=0;

	  inf=fopen(path,"rb");
	  if (!inf)
	  {
	    if (show_info) printf("WARN ! : Couldn't open briefing file %s\n",path);
	    return ERROR_PGBRF_FILE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }

	  fseek(inf,0,SEEK_END);
	  fsize=ftell(inf);
	  fseek(inf,0,SEEK_SET);
	  if (fsize>BRF_FILE_SIZE_WARN)
	  {
		  if (show_info) printf("WARN ! : briefing file longer then expected, file %s, size %d\n",path,fsize);
		  result=ERROR_PGBRF_FILE_BASE+ERROR_FPGE_BRF_LONG;
		  fclose(inf);
		  return result;
	  }

		do {
			buf = fgetc(inf);
			if (buf > 127 || buf == 39) {
				if (show_info) printf("WARN ! : illegal char in file %s, should be &#%d;\n",path, buf);
				result = ERROR_PGBRF_FILE_BASE + ERROR_FPGE_WRONG_BRF_CHAR;
			}

			//printf("%c%d",buf,state);
		} while (buf != EOF);
	  fclose(inf);
	  return result;
}


int check_pgscn(int show_info,char *path){

	  FILE *inf;
	  int result=0;

	  inf=fopen(path,"rb");
	  if (!inf)
	  {
	    if (show_info) printf("WARN ! : Couldn't open scenario file %s\n",path);
	    return ERROR_PGF_SCN_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }

	  fclose(inf);
	  return result;
}

int is_html_quotable(unsigned char c){
	return (strchr("<>\"&'/[]",c)!=NULL);
}

int fix_pgbrf(int show_info, char *path){

	  FILE *inf;
	  int fsize,result=0,i;
	  unsigned char *buff;

	  if (show_info) printf("Trying to fix briefing file %s\n",path);

	  inf=fopen(path,"rb");
	  if (!inf)
	  {
	    if (show_info) printf("WARN ! : Couldn't open briefing file %s\n",path);
	    return ERROR_PGBRF_FILE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }

	  fseek(inf,0,SEEK_END);
	  fsize=ftell(inf);
	  fseek(inf,0,SEEK_SET);

	  if (fsize>BRF_FILE_SIZE_WARN)
	  {
		  if (show_info) printf("WARN ! : briefing file longer then expected, file %s, size %d\n",path,fsize);
		  result=ERROR_PGBRF_FILE_BASE+ERROR_FPGE_BRF_LONG;
		  fclose(inf);
		  return result;
	  }

	  buff = malloc(fsize);
	  fread(buff,fsize,1,inf);
	  fclose(inf);

	  inf=fopen(path,"wb");
	  if (!inf)
	  {
		free(buff);
	    if (show_info) printf("ERROR ! : Couldn't open briefing file %s for writing\n",path);
	    return ERROR_PGBRF_FILE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }

	  for(i=0;i<fsize;i++){
		  if (buff[i]>127 || is_html_quotable(buff[i]))
			  fprintf(inf,"&#%d;",buff[i]);
		  else
			  fprintf(inf,"%c",buff[i]);
	  }
	  free(buff);
	  fclose(inf);
	  if (show_info) printf("Done.\n");

	  return result;
}


int check_pgcam( int show_info, int probe_file_only, int fix_brefings){

	  FILE *inf;
	  char path[MAX_PATH];
	  char line[1024],tokens[20][256];
	  int j,i,block=0,last_line_length=-1,cursor=0,token=0;
	  int lines,result;
	  int show_fix_info=0;
	  int print_ok=0; //not finished

	  hash_table table;

	  snprintf(path,MAX_PATH,pgf_pg_pgcam);
	  canonicalize_filename(path,path,MAX_PATH);
	  if (show_info && probe_file_only==LOAD_FILE)
		  printf("Opening file %s\n",path);
	  inf=fopen(path,"rb");
	  if (!inf)
	  {
	    //printf("Couldn't open scenario file\n");
	    return ERROR_PGCAM_FILE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	  }
	  if (probe_file_only==PROBE_FILE){
		  fclose(inf);
		  return ERROR_PGCAM_FILE_BASE+ERROR_FPGE_FILE_FOUND;
	  }

	  hash_construct_table(&table,211);

	  while (read_utf16_line_convert_to_utf8(inf,line)>=0){
		  //count lines so error can be displayed with line number
		  lines++;

			  //strip comments
			  for (i = 0; i < strlen(line); i++)
				if (line[i] == 0x23) {
					line[i] = 0;
					break;
				}
			  if (strlen(line)>0 && last_line_length==0){
				  block++;
			  }
			  last_line_length=strlen(line);
			  token=0;
			  cursor=0;
			  for (i = 0; i < strlen(line); i++)
				if (line[i] == 0x09) {
					tokens[token][cursor] = 0;
					token++;
					cursor = 0;
				} else {
					tokens[token][cursor] = line[i];
					cursor++;
				}
			  tokens[token][cursor]=0;
			  token++;

			  //scenario block
			  if (block == 4 && token > 1) {
				  result=check_pgscn(show_info,tokens[1]);
				  //insert existing scenario
				  if (0==result){
					  hash_insert( strdup(tokens[0]), strdup(tokens[0]), &table );
					  if (print_ok)
						  printf("%s\n",line);
				  }
				  //if (result&&show_info)
					  //printf("Non existing file : %s\n",tokens[1]);
			  }

			  //Block#5  path
			  if (block == 5 && token > 1) {
				  //check if line is pointing to existing scenario
				  if (hash_lookup(tokens[2],&table)){

					  if (print_ok)
						  printf("%s\n",line);

					  //check briefings for invalid characters
					  if (strlen(tokens[1])>0){
						  result=check_pgbrf(show_info,tokens[1]);
						  if (result&&fix_brefings)
							  fix_pgbrf(show_fix_info,tokens[1]);
					  }

					  for(j=0;j<3;j++)
						  if (strlen(tokens[3*(j+1)+2])>0) {
							  result=check_pgbrf(show_info,tokens[3*(j+1)+2]);
							  if (result&&fix_brefings)
								  fix_pgbrf(show_fix_info,tokens[3*(j+1)+2]);
						 }
				  }
			}
	  }

	  fclose(inf);

      hash_free_table( &table, strfree );

	  if (show_info) {
		  printf("PGF campaign file checked.\n");
	  }

	  return 0;
}

int convert_brf_to_pgbrf(){

	char buf[200];
	//alert("", "", NULL, "&Continue", NULL, 'c', 0);
	if (!pg_mode){
		alert("This option will work in PG mode only !", NULL, NULL, "&Continue", NULL, 'c', 0);
		return D_O_K;
	}
	snprintf(buf,200,"Converted %d BRF files.",convert_brf());
	alert(buf, NULL, NULL, "&Continue", NULL, 'c', 0);
	return D_O_K;
}

