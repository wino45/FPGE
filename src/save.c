/*
 * save.c
 *
 *  Created on: 2010-03-09
 *      Author: wino
 */

#include <stdio.h>
#include "fpge.h"
#include "filename.h"
#include "tables.h"
#include "maingui.h"
#include "save.h"
#include "load.h"

int convert_equipment_hard(int *new_unit_id, int *new_country, int equipment_ID, int country, int try_harder){
	int i,side=0;
	struct conversion_equipment search;
	struct conversion_equipment *found;
	char tmp[64];
	char no_first_token[64];
	char * pch;

	strncpy(search.name,equip_name_utf8[equipment_ID],20*3);
	strncpy(tmp,equip_name_utf8[equipment_ID],64);

	strncpy(no_first_token,"",64);
	pch = strtok (tmp," "); //first token
	//i=0;
	while (pch != NULL)
	{
	   pch = strtok (NULL, " ");
	   if (pch != NULL){
		   strncat(no_first_token," ",64);
		   strncat(no_first_token,pch,64);
		   //i++;
	   }
	   //printf ("%s\n",pch);
	}
	//if (i>0) no_first_token[strlen(no_first_token)-1]=0; //trim " " when at least one token was found
	//printf ("No first token: |%s|\n",no_first_token);

	//try with specific
	search.country=country;

	found=(struct conversion_equipment *)bsearch(&search,conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
	if (found){
		*new_unit_id=found->new_id;
		*new_country=search.country;
		return CONVERSION_FOUND;
	}else{

		if (try_harder){

			//try to find country size
			for (i = 0; i < 12; i++)
				if (scn_buffer[i] == country) {
					side = i % 2;
					break;
				}

			strncpy(search.name,equip_name_utf8[equipment_ID],20*3);
			//change country ID
			for(i=0;i<6;i++){
				search.country=scn_buffer[i*2+side];
				if (search.country!=0 && search.country<=total_countries){
					found=(struct conversion_equipment *)bsearch(&search,conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
					if (found){
						*new_unit_id=found->new_id;
						*new_country=search.country;
						return CONVERSION_FOUND_NEW_COUNTRY;
					}
				}
			}

			//change unit name
			for(i=0;i<6;i++){
				search.country=country;
				strncpy(search.name,country_names_short[scn_buffer[i*2+side]],20*3);
				strncat(search.name,no_first_token,20*3);
				//search.country=scn_buffer[i*2+side];
				//printf ("search.name: |%s|\n",search.name);
				if (scn_buffer[i*2+side]>0 && scn_buffer[i*2+side]<=total_countries){
				found=(struct conversion_equipment *)bsearch(&search,conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
				if (found){
					*new_unit_id=found->new_id;
					*new_country=search.country;
					return CONVERSION_FOUND_NEW_NAME;
				}
				}
			}

		}
		//try with generic country

		search.country=-1;
		found=(struct conversion_equipment *)bsearch(&search,conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
				if (found){
					*new_unit_id=found->new_id;
					*new_country=search.country;
					return CONVERSION_GENERIC;
				}
	}
	//printf ("No first token: |%s|\n",no_first_token);
	*new_country=0; //no country
	*new_unit_id=equipment_ID;
	return CONVERSION_NOT_FOUND;
	//return 0;
}



int convert_equipment(int equipment_ID, int country, int hide_ids, int unit_idx){
	struct conversion_equipment search;
	struct conversion_equipment *found;

	strncpy(search.name,equip_name_utf8[equipment_ID],20*3);
	//try with specific
	search.country=country;

	found=(struct conversion_equipment *)bsearch(&search,conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
	if (found){
		return found->new_id;
	}else{
		//try with generic country
		search.country=-1;
		found=(struct conversion_equipment *)bsearch(&search,conversion_equip,conversion_total_equip,sizeof(struct conversion_equipment),compare_by_name_and_country);
				if (found){
					return found->new_id;
				}else{
				//not found
					if (hide_ids)
						printf("Not found: (%d,%d) %d -> %d\n",
								all_units[unit_idx].x,all_units[unit_idx].y,equipment_ID,country);
					else
						printf("Not found: (%d,%d) %s -> %s\n",
								all_units[unit_idx].x,all_units[unit_idx].y,equip[equipment_ID],country_names[country]);
				}
	}
	return equipment_ID;
	//return 0;
}

//write the unit dat in all_units[i] to outf
void write_unit(FILE *outf,int i,int save_type)
{
	WORD unum=all_units[i].unum;
	WORD orgtnum=all_units[i].orgtnum;
	WORD auxtnum=all_units[i].auxtnum;

	char country=all_units[i].country;

	if (save_type==SAVE_WITH_UNIT_IDS_CONVERSION){
		unum=convert_equipment(unum,country,0,i);
		orgtnum=convert_equipment(orgtnum,country,0,i);
		auxtnum=convert_equipment(auxtnum,country,0,i);
	}
  fwrite(&unum,2,1,outf);
  fwrite(&orgtnum,2,1,outf);
  fwrite(&(all_units[i].country),1,1,outf);
  fwrite(&auxtnum,2,1,outf);
  fwrite(&(all_units[i].x),2,1,outf);
  fwrite(&(all_units[i].y),2,1,outf);
  fwrite(&(all_units[i].str),1,1,outf);
  fwrite(&(all_units[i].entrench),1,1,outf);
  fwrite(&(all_units[i].exp),1,1,outf);

}

void write_set(FILE *outf)
{
   int x,y,i;
   char scn_str[8+1+3+1];

   //fix up the header
   set_head[101]=mapx-1;
   set_head[103]=mapy-1;
   sprintf(scn_str,"map%02d.stm", getScenarioNumber()); //temp XXXX????

   for(i=0;i<strlen(scn_str);i++)
	   set_head[51-3+i]=scn_str[i];

   for(i=0;i<strlen(scn_str)-4;i++)
	   set_head[84-3+i]=scn_str[i];

   /*
   set_head[51]=scn_str[0];
   set_head[52]=scn_str[1];
   set_head[84]=scn_str[0];
   set_head[85]=scn_str[1];
    */

   //write the header
   for (i=0; i<sizeof(set_head); ++i)
     fputc(set_head[i],outf);
   //write section 1n GLN
   for (y=0; y<mapy; ++y)
     for (x=0; x<mapx; ++x)
     {
        fputc(map[x][y].gln&255,outf);
        fputc(map[x][y].gln/256,outf);
     }
   // road connectivity
   for (y=0; y<mapy; ++y)
     for (x=0; x<mapx; ++x)
        fputc(map[x][y].rc&255,outf);
   // country owners
   for (y=0; y<mapy; ++y)
     for (x=0; x<mapx; ++x)
        fputc(map[x][y].own&255,outf);
   // side info i.e. axis.allied.unown.neutral
   for (y=0; y<mapy; ++y)
     for (x=0; x<mapx; ++x)
        fputc(map[x][y].side&255,outf);
  // tile numbers
  for (y=0; y<mapy; ++y)
     for (x=0; x<mapx; ++x)
     {
        fputc(map[x][y].tile&255,outf);
        fputc(map[x][y].tile/256,outf);
     }
}


int save_set(int n, int side_save)
{
   FILE *outf;
   char path[MAX_PATH];

   if (side_save)
	   sprintf(path,"map%02d.se_",n);
   else
	   sprintf(path,"map%02d.set",n);

   canonicalize_filename(path,path,MAX_PATH);
   outf=fopen(path,"wb");
   if (!outf) return ERROR_SET_BASE+ERROR_FPGE_FILE_CANNOT_BE_WRITTEN;

   write_set(outf);

   fclose(outf);
   return 0;
}


void write_stm(FILE *outf){

   int x,y;

   for (y=0; y<mapy; ++y)
     for (x=0; x<mapx; ++x)
     {
        fputc(map[x][y].utr&255,outf);
        fputc(map[x][y].utr/256,outf);
     }
}

int save_stm(int n, int side_save)
{
   FILE *outf;
   char path[MAX_PATH];

   if (side_save)
	   sprintf(path,"map%02d.st_",n);
   else
	   sprintf(path,"map%02d.stm",n);

   canonicalize_filename(path,path,MAX_PATH);
   outf=fopen(path,"wb");
   if (!outf) return ERROR_STM_BASE+ERROR_FPGE_FILE_CANNOT_BE_WRITTEN;

   write_stm(outf);

   fclose(outf);
   return 0;
}

void write_scenario(FILE *outf, int save_type){
	int deploy_size,i;
	int x,y,idx;

	//prepare to write
	total_axis_core=count_axis_core();
	total_axis_aux=count_axis_aux();
	total_allied_core=count_allied_core();
	total_allied_aux=count_allied_aux();

	scn_buffer[CORE_UNITS]=total_axis_core;
	scn_buffer[AUX_UNITS]=total_axis_aux;
	scn_buffer[ALLIED_UNITS]=total_allied_core;
	scn_buffer[AUX_ALLIED_UNITS]=total_allied_aux;

	//read the victory hexes from the map
	get_victory_hexes();
	//now place in the scen buffer
	idx=VIC_HEXES;
	for (i=0; i<MAX_VICTORY_HEXES; ++i)
	{
		scn_buffer[idx+0]=victory_hexes[i].x&255;
		scn_buffer[idx+1]=(victory_hexes[i].x>>8)&255;
		scn_buffer[idx+2]=victory_hexes[i].y&255;
		scn_buffer[idx+3]=(victory_hexes[i].y>>8)&255;
		idx+=4;
	}
	// write
	for (i=0; i<DEPLOY_TABLE; ++i)
	fputc(scn_buffer[i],outf);
	deploy_size=count_deploy_hexes();    //this is the present size
	fputc((deploy_size+1)&255,outf);
	fputc(0,outf);
	//this code will filter some wrong (out of map) deployment hexes
	for (y=0; y<mapy; ++y)
		for (x=0; x<mapx; ++x)
			if (map[x][y].deploy)
			{
			fputc(x,outf); fputc(0,outf);
			fputc(y,outf); fputc(0,outf);
			}
	for (i=0; i<4; ++i)
		fputc(255,outf);
	//now use the old deploy size in the buffer to find the rest of the data
	deploy_size=4*(scn_buffer[DEPLOY_TABLE]+256*scn_buffer[DEPLOY_TABLE+1]);
	//units_start=deploy_size+0x87;  //this is the old size
	//s4_starts=deploy_size+0x77;
	//write the first section
	// for (i=0; i<s4_starts; ++i)
	//   fputc(scn_buffer[i],outf);

	//write the s4_buffer
	for (i=0; i<16; ++i)
		fputc(s4_buffer[i],outf);

	 //write core
	for(i=0; i<total_units; ++i)
	{
		if (all_units[i].uflag==axis_core)
			write_unit(outf,i,save_type);
	}

	//write aux
	for(i=0; i<total_units; ++i)
	{
		if (all_units[i].uflag==axis_aux)
			write_unit(outf,i,save_type);
	}
	//write allied
	for(i=0; i<total_units; ++i)
	{
		if (all_units[i].uflag==allied_core)
			write_unit(outf,i,save_type);
	}
	//write allied aux
	for(i=0; i<total_units; ++i)
	{
		if (all_units[i].uflag==allied_aux)
			write_unit(outf,i,save_type);
	}

}

int save_scenario(int n, int save_type)
{
	char path[MAX_PATH];
	FILE *outf;

	// open the file...
	sprintf(path,"game%03d.scn",n);
	canonicalize_filename(path,path,MAX_PATH);
	outf=fopen(path,"wb");
	if (!outf)
	{
		sprintf(MapStatusTxt,"Save of %s FAILED",path);
		return ERROR_SCN_BASE+ERROR_FPGE_FILE_CANNOT_BE_WRITTEN;
	}

	write_scenario(outf,save_type);

	fclose(outf);
	sprintf(MapStatusTxt,"%s saved",path);
	return 0;
}

int save_names()
{
	int i;
   FILE *outf;
   char path[MAX_PATH];

   strncpy(path,names_file,MAX_PATH);
   canonicalize_filename(path,path,MAX_PATH);
   outf=fopen(path,"wb");
   if (!outf) return ERROR_NAMES_BASE+ERROR_FPGE_FILE_CANNOT_BE_WRITTEN;

   fwrite(&total_names,2,1,outf);
   for(i=0;i<total_names;i++)
	   fwrite(&gln[i],MAX_NAME_SIZE,1,outf);

   fclose(outf);
   return 0;
}

int save_equip(char *file_name) //equip_file
{
	int i;
   FILE *outf;
   char path[MAX_PATH];

   strncpy(path,file_name,MAX_PATH);
   canonicalize_filename(path,path,MAX_PATH);
   outf=fopen(path,"wb");
   if (!outf) return ERROR_EQUIP_BASE+ERROR_FPGE_FILE_CANNOT_BE_WRITTEN;

   fwrite(&total_equip,2,1,outf);
   for(i=0;i<total_equip;i++)
	   fwrite(&equip[i],EQUIP_REC_SIZE,1,outf);

   fclose(outf);
   return 0;
}

