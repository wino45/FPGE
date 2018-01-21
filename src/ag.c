/*
 * ag.c
 *
 *  Created on: 2010-08-05
 *      Author: wino
 */
#include <stdio.h>
#include "fpge.h"
#include "load.h"
#include "ag.h"
#include "save.h"
#include "fpge_colors.h"
#include "minimap.h"
#include "filename.h"
#include "pg.h"

#define MAX_AG_CPAL 10

char RLE_palette_name[MAX_AG_CPAL][5];
unsigned char RLE_palette_table[MAX_AG_CPAL][256][3];
int RLE_palette_index;



int little_endian(unsigned char *size_buf)
{
	return (size_buf[0]<<24)+(size_buf[1]<<16)+(size_buf[2]<<8)+size_buf[3];
}

unsigned short  little_endian_ushort_swap(unsigned short value)
{
	return ((value>>8)&0xFF) + ((value<<8)&0xFF00);
}

int paint(BITMAP *map,int sx,int sy, int cp, int length, int pal_no, int max_x){
	int i, pixel_color;
	//printf("%d %d",pixel_color,length);

	pixel_color=cp;
	if (cp != fpge_mask_color) {
		pixel_color = make_color_fpge(
				RLE_palette_table[pal_no][cp][0],
				RLE_palette_table[pal_no][cp][1],
				RLE_palette_table[pal_no][cp][2]);
	}
	//if (sx==0 && sy==0) printf("cp=%6x pc=%6x\n",cp,pixel_color);
	for(i=sx;i<sx+length; i++)
		putpixel(map,i,sy,pixel_color);

	return length;
}

void make_flag_ag(BITMAP *temp_bmp, int i) {
	int x, y, cp;
	BITMAP *tmp_bmp;

	tmp_bmp = create_bitmap(TILE_FULL_WIDTH, TILE_HEIGHT);
	// zero out the bitmap with the transparent color
	for (y = 0; y < TILE_HEIGHT; ++y)
		for (x = 0; x < TILE_FULL_WIDTH; ++x)
			putpixel(tmp_bmp, x, y, fpge_mask_color);
	// flags in AG and PG got different size
	for (y = 0; y < temp_bmp->h; ++y)
		for (x = 0; x < temp_bmp->w; ++x){
			cp=getpixel(temp_bmp, x, y);
			putpixel(tmp_bmp, x + 20, y + 36,cp);
		}

	flag_bmp[i]=tmp_bmp;
	//20,36
	total_flags++;
}

void make_terrain_ag(BITMAP *temp_bmp, int i) {
	int x, y, cp, dc, c;

	c=make_color_fpge(195,195,119);

	til_bmp[i] = create_bitmap(TILE_FULL_WIDTH, TILE_HEIGHT);
	dark_til_bmp[i] = create_bitmap(TILE_FULL_WIDTH, TILE_HEIGHT);
	til_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
	dark_til_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
	til_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
	dark_til_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);

	//more to do
	for (y = 0; y < TILE_HEIGHT; ++y)
		for (x = 0; x < TILE_FULL_WIDTH; ++x){
			cp=getpixel(temp_bmp, x, y);
			dc=make_color_fpge(((cp>>16)&0xff)*3/4,((cp>>8)&0xff)*3/4, (cp&0xff)*3/4);
			if (cp==fpge_mask_color) dc=fpge_mask_color;

			putpixel(til_bmp[i], x, y, cp);
			putpixel(dark_til_bmp[i], x, y, dc);

			if (cp==c){
				putpixel(til_bmp_mud[i], x, y, make_color_fpge(211,178,105));
				putpixel(dark_til_bmp_mud[i], x, y, make_color_fpge(162,134,85));

				putpixel(til_bmp_snow[i], x, y, make_color_fpge(231,239,239));
				putpixel(dark_til_bmp_snow[i], x, y,  make_color_fpge(174,178,178));
			}else{
				putpixel(til_bmp_mud[i], x, y, cp);
				putpixel(dark_til_bmp_mud[i], x, y, dc);

				putpixel(til_bmp_snow[i], x, y, cp);
				putpixel(dark_til_bmp_snow[i], x, y, dc);
			}
		}

	total_tiles++;
}

void make_small_tiles_ag(BITMAP *temp_bmp, int i) {
	int x, y, cp, dc;
	int c;

	c=make_color_fpge(207,207,119);

	mtil_bmp[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	dark_mtil_bmp[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	mtil_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	dark_mtil_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	mtil_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	dark_mtil_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);

	   for (y=0; y<TILE_HEIGHT_; ++y)
	     for (x=0; x<TILE_FULL_WIDTH_; ++x){
	    	 cp=getpixel(temp_bmp, x, y);
	    	 if (fpge_colors_bits_shift)
	    		 dc=make_color_fpge((cp&0xff)*3/4,((cp>>8)&0xff)*3/4, ((cp>>16)&0xff)*3/4);
	    	 else
	    		 dc=make_color_fpge(((cp>>16)&0xff)*3/4,((cp>>8)&0xff)*3/4, (cp&0xff)*3/4);
	    	 if (cp==fpge_mask_color) dc=fpge_mask_color;

	    	 putpixel(mtil_bmp[i],x,y,cp);
	    	 putpixel(dark_mtil_bmp[i],x,y,dc);

				if (cp==c){
					putpixel(mtil_bmp_mud[i], x, y, make_color_fpge(211,178,105));
					putpixel(dark_mtil_bmp_mud[i], x, y, make_color_fpge(162,134,85));

					putpixel(mtil_bmp_snow[i], x, y, make_color_fpge(231,239,239));
					putpixel(dark_mtil_bmp_snow[i], x, y, make_color_fpge(174,178,178));
				}else{
					putpixel(mtil_bmp_mud[i], x, y, cp);
					putpixel(dark_mtil_bmp_mud[i], x, y, dc);

					putpixel(mtil_bmp_snow[i], x, y, cp);
					putpixel(dark_mtil_bmp_snow[i], x, y, dc);
				}
		   }
	total_mtiles++;
}

void make_units_ag(BITMAP *temp_bmp, int i) {
	int x, y, cp;

	unit_bmp[i] = create_bitmap(TILE_FULL_WIDTH, TILE_HEIGHT);

	//more to do
	for (y = 0; y < TILE_HEIGHT; ++y)
		for (x = 0; x < TILE_FULL_WIDTH; ++x){
			cp=getpixel(temp_bmp, x, y);
			putpixel(unit_bmp[i], x, y, cp);
		}

	total_uicons++;
}

void make_stack_ag(BITMAP *temp_bmp, int i) {
	int x, y, cp;

	stack_bmp[i] = create_bitmap(TILE_FULL_WIDTH, TILE_HEIGHT);

	//more to do
	for (y = 0; y < TILE_HEIGHT; ++y)
		for (x = 0; x < TILE_FULL_WIDTH; ++x){
			cp=getpixel(temp_bmp, x, y);
			putpixel(stack_bmp[i], x, y, cp);
		}

	total_sicons++;
}

void make_strength_ag(BITMAP *temp_bmp, int i) {
	int x, y, cp, h;

	//i=0..14 in one row, expand to 1-15
	//i=0..59

	h = (i%15+1)+ (i/15)*MAX_STRENGTH_IN_ROW;

	strength_bmp[h] = create_bitmap(TILE_FULL_WIDTH, TILE_HEIGHT);

	//clear all
	for (y = 0; y < TILE_HEIGHT; ++y)
		for (x = 0; x < TILE_FULL_WIDTH; ++x)
			putpixel(strength_bmp[h], x, y, fpge_mask_color);

	//move to  22,36
	for (y = 0; y < temp_bmp->h; ++y)
		for (x = 0; x < temp_bmp->w; ++x){
			cp=getpixel(temp_bmp, x, y);
			putpixel(strength_bmp[h], x + 22, y + 36,cp);
		}


	if (!str0_bmp_loaded){
		if (i%15==9) //tile 10, make tile 0
		{
			h=(i/15)*MAX_STRENGTH_IN_ROW;
			strength_bmp[h] = create_bitmap(TILE_FULL_WIDTH, TILE_HEIGHT);
			//copy current tile eg 10 to 0
			for (y = 0; y < TILE_HEIGHT; ++y)
				for (x = 0; x < TILE_FULL_WIDTH; ++x)
					putpixel(strength_bmp[h], x, y, fpge_mask_color);

			for (y = 0; y < temp_bmp->h; ++y)
				for (x = 0; x < temp_bmp->w; ++x){
					cp=getpixel(temp_bmp, x, y);
					putpixel(strength_bmp[h], x + 22, y + 36,cp);
				}

			//kill 1 from 10
			cp=getpixel(strength_bmp[h],23,37);
			rectfill(strength_bmp[h],24,38,29,46,cp);
			//move 0 to center
		  for(y=38;y<45;y++)
			  for(x=28;x<28+6;x++)
				  putpixel(strength_bmp[h],x,y,getpixel(strength_bmp[h],x+2,y));


		}
	}
}

int ag_find_pallete_id(char *pal_name){
	int i;

	for(i=0;i<RLE_palette_index;i++)
		if (strcmp(pal_name,RLE_palette_name[i])==0) return i;

	return 0;
}

#define CC_NUMBER 600

int load_tileart(){
	FILE *inf;
	char path[MAX_PATH];
	char buf[5];
	char picture_name[5];
	char pal_name[5];
	unsigned char size_buf[4];
	int size_of_block;
	int flags; //unknown flags
	unsigned short x,y,count;
	BITMAP *temp_bmp;

	buf[4]=0; //eof string hack
	picture_name[4]=0;
	pal_name[4]=0;
	unsigned short no_of_bytes_in_line;
	unsigned char pixel_color, mode, length, no_of_pixels_to_copy;
	int cy,cx,i,k,pic_no,current_pal_no;
	unsigned short pal_r,pal_g,pal_b,pal_no;
	int cc=0,iter;

	strncpy(path,ag_tileart,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (!inf){
		sprintf(path,"%s%s","..\\art\\",ag_tileart);
		canonicalize_filename(path,path,MAX_PATH);
		inf=fopen(path,"rb");
		if (!inf)
			return ERROR_AG_TILEART_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fread(buf,4,1,inf);//magic bytes, can be 00000000 or 'Vers'
	//printf(">%s\n",buf);

	RLE_palette_index=0;
	total_flags=0;
	//no strength total
	total_tiles=0;
	total_sicons=0;
	total_uicons=0;
	total_mtiles=0;

	for(iter=0;iter<2;iter++){

	while(fread(buf,4,1,inf)==1)//blocks read
	{
		fread(size_buf,sizeof(size_buf),1,inf); //size
		size_of_block=little_endian(size_buf);
		//printf("%s %d\n",buf,size_of_block);
		if (strcmp(buf,"CPal")==0 && iter==0){
			fread(RLE_palette_name[RLE_palette_index],4,1,inf);
			size_of_block-=4;
			RLE_palette_name[RLE_palette_index][4]=0; //end of string
			//move 18
			fseek(inf,10,SEEK_CUR);
			size_of_block-=10;
			//
			for(i=0;i<256;i++){
				fread(&pal_no,sizeof(pal_no),1,inf);
				pal_no=little_endian_ushort_swap(pal_no);
				fread(&pal_r,sizeof(pal_r),1,inf);
				fread(&pal_g,sizeof(pal_g),1,inf);
				fread(&pal_b,sizeof(pal_b),1,inf);

				//ew read 2 bytes, Hi byte=Lo byte so we get rid of it
				pal_r&=0xff;
				pal_g&=0xff;
				pal_b&=0xff;

				size_of_block-=2*4;
				RLE_palette_table[RLE_palette_index][pal_no][0]=pal_r;
				RLE_palette_table[RLE_palette_index][pal_no][1]=pal_g;
				RLE_palette_table[RLE_palette_index][pal_no][2]=pal_b;

				//printf("%d %d %d %d\n",pal_no,pal_g,pal_g,pal_b);
			}
			RLE_palette_index++;
		}
		if (strcmp(buf,"RLEi")==0 && iter==1){
			//picture read more
			fread(picture_name,4,1,inf);
			fread(pal_name,4,1,inf);
			current_pal_no=ag_find_pallete_id(pal_name);
			//if (current_pal_no==-1) printf("pal not found %d\n",RLE_palette_index);

			fread(&flags,sizeof(flags),1,inf);
			fread(&x,sizeof(x),1,inf);
			x=little_endian_ushort_swap(x);
			fread(&y,sizeof(y),1,inf);
			y=little_endian_ushort_swap(y);
			fread(&count,sizeof(count),1,inf);

			size_of_block-=(4+4+4+2+2+2);
			//if (cc==CC_NUMBER) printf(">%s< >%s< x:%d y:%d palno:%d\n",picture_name,pal_name,x,y,current_pal_no);

			temp_bmp=create_bitmap(x,y);
			/*for(cx=0;cx<x;cx++)
				for(cy=0;cy<y;cy++)
					putpixel(temp_bmp,cx,cy,fpge_mask_color);
			 */
			cy=0;
			while (size_of_block>0){

				fread(&no_of_bytes_in_line,sizeof(no_of_bytes_in_line),1,inf);
				no_of_bytes_in_line=little_endian_ushort_swap(no_of_bytes_in_line);
				//if (cc==CC_NUMBER) printf(">>>> line size %d\n",no_of_bytes_in_line);
				size_of_block-=no_of_bytes_in_line;
				//fseek(inf,no_of_bytes_in_line-2,SEEK_CUR);

				cx=0;
				for(i=0;i<no_of_bytes_in_line-2;i++){
					fread(&mode,sizeof(mode),1,inf);
					//printf("mode:%2x i=%d\n",mode,i);
					//readkey();
					//handle RLE transparency
					if(mode ==0xFF) {
						fread(&length,sizeof(length),1,inf);
						i++;
						//if (cc==CC_NUMBER) printf("trans mode:%2x length:%2d %6x\n",mode,length,fpge_mask_color);
						//readkey();
						//pixel_color=;
						cx+=paint(temp_bmp,cx,cy,fpge_mask_color,length,current_pal_no,x);
						//if (cc==CC_NUMBER) printf("cx:%2d\n",cx);
						continue; //break for
					}
					//handle copy pixels
					if(mode >0x80){
						no_of_pixels_to_copy=mode-0x80;
						//if (cc==CC_NUMBER) printf("liter mode:%2x length:%2d\n",mode,no_of_pixels_to_copy);

						for(k=0;k<no_of_pixels_to_copy;k++){
							fread(&pixel_color,sizeof(pixel_color),1,inf);
							i++;
							//if (cc==CC_NUMBER) printf("k:%2d color:%2x\n",k,pixel_color);
							cx+=paint(temp_bmp,cx,cy,pixel_color,1,current_pal_no,x);
							//if (cc==CC_NUMBER) printf("cx:%2d\n",cx);
						}
						//exit(0);
					}else{
					//finally handle RLE for non transparency pixels
						fread(&length,sizeof(length),1,inf);
						i++;
						//if (cc==CC_NUMBER) printf("RLE mode:%2x color:%2d\n",mode,length);
						//readkey();
						pixel_color=mode;
						cx+=paint(temp_bmp,cx,cy,length,mode,current_pal_no,x);
						//if (cc==CC_NUMBER) printf("cx:%2d\n",cx);
						}
				}
				//if (cx>x) printf("hmm x:%d cx:%d\n",x,cx);
				cy++;

			}
			//if (cc==CC_NUMBER) printf("pix %6x\n",getpixel(temp_bmp,0,0));
			//get number
			pic_no=atoi(&picture_name[1]);
			//printf("pic_no : %d\n",pic_no);
			switch(picture_name[0]){
				case 'f':
					make_flag_ag(temp_bmp,pic_no);
					break;
				case 'm':
					make_terrain_ag(temp_bmp,pic_no);
				    break;
				case 'n':
					make_strength_ag(temp_bmp,pic_no);
					break;
				case 's':
					make_stack_ag(temp_bmp,pic_no);
					break;
				case 'u':
					make_units_ag(temp_bmp,pic_no);
					break;
				case 't':
					//small tiles
					make_small_tiles_ag(temp_bmp,pic_no);
					break;
			}
			destroy_bitmap(temp_bmp);
		}
		//printf("size of block : %d\n",size_of_block);
		//if (cc==CC_NUMBER) break;
		cc++;

		//this is needed to skip unknown blocks
		fseek(inf,size_of_block,SEEK_CUR);
	}
	//printf("%d\n",RLE_palette_index);
	fseek(inf,4,SEEK_SET);
	}
	fclose(inf);
	return 0;
}

int load_ag_nupl(){

		char path[MAX_PATH];
		FILE *inf;
		//int error=0;
		long offset=-1;

		sprintf(path,"%s%s","..\\",exe_ag);
		canonicalize_filename(path,path,MAX_PATH);
		inf = fopen(path, "rb");
		if (!inf){
			sprintf(path,"%s%s","..\\",exe_pgwin);
			canonicalize_filename(path,path,MAX_PATH);
			inf = fopen(path, "rb");
			if (!inf)
				return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
		}
		fseek(inf, 0, SEEK_END);
		long file_size = ftell(inf);
		//printf("%d\n",file_size);
		//FPGE will understand only three ag.exe and one PG.exe sizes:
		if (file_size==2161664) offset=1877568; //ag.exe
		if (file_size==2167611) offset=1883344; //ag.exe
		if (file_size==2167296) offset=1883344; //ag.exe
		if (file_size==2135040) offset=1845024; //pg.exe
		if (offset==-1) {
			fclose(inf);
			return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_UNKNOWN_EXE;
		}
		fseek(inf,offset,SEEK_SET);

		int blocks_read = fread(nupl,sizeof(WORD),AG_NUPL_SIZE,inf);
		if (blocks_read!=AG_NUPL_SIZE){
			fclose(inf);
			return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_WRONG_NULP;
		}
		//for(i=0;i<20;i++) printf("%d %d\n",i,nulp[i]);
		//int i;
		//for(i=AG_NUPL_SIZE-19;i<AG_NUPL_SIZE;i++) printf("%d %d\n",i,nupl[i]);
		fclose(inf);

		parse_nupl();

		//test
		/*
		for(i=0;i<NULP_CLASS_IDX;i++){
		cur=nulp_cc[4][i];
		while (nulp[cur]!=-1){
			printf("%d %s %d\n",i,equip_name_utf8[ nulp[cur]], nulp[cur]);
			cur++;
		}
		}
		*/
		return 0;
}

int load_ag_scenario_desc(){

		char path[MAX_PATH];
		char ag_desc_long[AG_LONG_DES];
		char ag_desc_short[AG_SHORT_DES];
		FILE *inf;
		int i,j,k,n;
		long offset=-1;

		sprintf(path,"%s%s","..\\",exe_ag);
		canonicalize_filename(path,path,MAX_PATH);
		inf = fopen(path, "rb");
		if (!inf){
			sprintf(path,"%s%s","..\\",exe_pgwin);
			canonicalize_filename(path,path,MAX_PATH);
			inf = fopen(path, "rb");
			if (!inf)
				return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
		}

		fseek(inf, 0, SEEK_END);
		long file_size = ftell(inf);
		//printf("%d\n",file_size);
		//FPGE will understand only three ag.exe and one PG.exe sizes:
		//if (file_size==2161664) offset=1877568; //ag.exe
		if (file_size==2167611) offset=1822944+8; //ag.exe

		if (file_size==2167296) offset=1822944+8; //ag.exe
		if (file_size==2135040) offset=1750688; //pg.exe
		if (offset==-1) {
			fclose(inf);
			return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_UNKNOWN_EXE;
		}

		for (n=0;n<MAX_SCENARIOS;n++){

			memset(scn_long_description[n],0,LONG_SCN_SIZE);

			fseek(inf,offset+n*256,SEEK_SET);

			int blocks_read = fread(ag_desc_long,AG_LONG_DES,1,inf);
			if (blocks_read!=1){
				fclose(inf);
				return ERROR_AG_PG_EXE_BASE+ERROR_FPGE_WRONG_AG_LONG_DESC;
			}

			//trim date
			for(i=0;i<AG_LONG_DES;i++)
				if (ag_desc_long[i]==0x0d) ag_desc_long[i]=' ';

			k=0;
			j=0;
			for(i=0;i<AG_LONG_DES;i++){
				if (ag_desc_long[i]!=' ' || j!=(int)' ')
				{
					scn_long_description[n][k]=ag_desc_long[i];
					j=(int)ag_desc_long[i];
					k++;
				}

				if (ag_desc_long[i]==0) i=AG_LONG_DES;
			}

			//printf("%s\n---\n",scn_long_description[scn_number-1]);
			//printf("%d\n",n);
		}

		//now short desc.
		if (file_size==2167611) offset=1845804; //ag.exe
		if (file_size==2167296) offset=1845804; //ag.exe


		if (file_size==2135040) { offset=1813712+8; } //pg.exe
		fseek(inf,offset,SEEK_SET);
		fread(ag_desc_short,AG_SHORT_DES,1,inf);
		i=0;
		j=0;
		k=0;
		memset(scn_short_description[i],0,SHORT_SCN_SIZE);

		while (i<40){
			if (ag_desc_short[k]==0){
				i++;
				memset(scn_short_description[i],0,SHORT_SCN_SIZE);
				j=0;
				while (ag_desc_short[k]==0) k++;
			}else{
				scn_short_description[i][j]=ag_desc_short[k];
				k++;
				j++;
			}
		}
		fclose(inf);

		return 0;
}



int save_ag_scenario(int n, int save_type)
{
	char path[MAX_PATH];
	FILE *outf;
	char head[16];

	// int count;

	// open the file for writing
	sprintf(path,"game%03d.scn",n);
	outf=fopen(path,"wb");
	if (!outf)
	{
		sprintf(MapStatusTxt,"Save of %s FAILED",path);
		return ERROR_AG_SAVE_SCENARIO_BASE+ERROR_FPGE_FILE_CANNOT_BE_WRITTEN;
	}

	strncpy(head,"ScenarioFile",16);
	fwrite(head,12,1,outf);
	write_scenario(outf,save_type);

	strncpy(head,"SetFile",16);
	fwrite(head,7,1,outf);
	write_set(outf);

	strncpy(head,"MapFile",16);
	fwrite(head,7,1,outf);
    write_stm(outf);

	fclose(outf);
	sprintf(MapStatusTxt,"%s saved",path);
	return 0;
}

int save_ag(int save_type){
	  char backup_temp[80];

	if (backup_not_saved) {
		sprintf(backup_temp, "copy game%03d.scn game0%03d.bak", getScenarioNumber(),
				getScenarioNumber());
		system(backup_temp);
		backup_not_saved = 0;
	}
	return save_ag_scenario(getScenarioNumber(),save_type);
}

int load_ag_scn(int scenario_number, int show_info){
	FILE * inf;
	char head[16],path[MAX_PATH];
	unsigned char d,c,a,al,ala;
	unsigned short x,y;

	int offset_scenario=12;
	int offset_set;
	int offset_stm;

    int ch;

	//printf("load_ag_scn: %s\n",fname);
	sprintf(path,"game%03d.scn",scenario_number);

	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf == NULL) return ERROR_AG_SCN_BASE+ERROR_FPGE_FILE_NOT_FOUND;

	fread(head,12,1,inf);
	head[12]=0;

	//printf("HEAD: %s\n",head);

	fseek(inf, offset_scenario+DEPLOY_TABLE , SEEK_SET);
	fread(&d,sizeof(d),1,inf);
	fseek(inf, offset_scenario+CORE_UNITS , SEEK_SET);
	fread(&c,sizeof(c),1,inf);
	fseek(inf, offset_scenario+ALLIED_UNITS , SEEK_SET);
	fread(&a,sizeof(al),1,inf);
	fseek(inf, offset_scenario+AUX_UNITS , SEEK_SET);
	fread(&al,sizeof(a),1,inf);
	fseek(inf, offset_scenario+AUX_ALLIED_UNITS , SEEK_SET);
	fread(&ala,sizeof(ala),1,inf);
/*
	printf("d=%d\n",d);
	printf("c=%d\n",c);
	printf("a=%d\n",a);
	printf("al=%d\n",al);
	printf("ala=%d\n",ala);
*/
	//set file start
	offset_set = offset_scenario+ d*4+DEPLOY_TO_UNITS + (c+a+al+ala)*14 + 7;
//	printf("offset_set=%d\n",offset_set);

	fseek(inf, offset_set-7 , SEEK_SET);
	fread(head,7,1,inf);
	//head[7]=0;
	//printf("HEAD: %s\n",head);

	fseek(inf, offset_set+MAP_X_ADDR, SEEK_SET);
	fread(&x, sizeof(x), 1, inf);
	fseek(inf, offset_set+MAP_Y_ADDR, SEEK_SET);
	fread(&y, sizeof(y), 1, inf);
	++x;
	++y; //PG sets this to one less than size i.e. last index
	//printf("x=%d\n",x);
	//printf("y=%d\n",y);
	//stm file start
	offset_stm = offset_set + MAP_LAYERS_START + 7 * x * y + 7;

	fseek(inf, offset_stm-7 , SEEK_SET);
	fread(head,7,1,inf);
	//head[7]=0;
	//printf("HEAD: %s\n",head);
//all offsets are ready
	parse_set_file(inf,offset_set);
	parse_stm_file(inf,offset_stm);
	//scenario, we do it manually
	//i=0;
	scn_size=0;
	fseek(inf, offset_scenario , SEEK_SET);
	while ( scn_size<(offset_set-offset_scenario-7) && scn_size<MAX_SCN_SIZE)
	{
	 ch=fgetc(inf);
	 scn_buffer[scn_size]=ch;
	 ++scn_size;
	 //i++;
	}
	//special case AUX_ALLIED_UNITS <> 0
	if (scn_buffer[AUX_ALLIED_UNITS]>0) {
		scn_buffer[ALLIED_UNITS]+=scn_buffer[AUX_ALLIED_UNITS];
		scn_buffer[AUX_ALLIED_UNITS]=0;

	}

	parse_scn_buffer(show_info);
	//printf("%d, %d\n",offset_set-offset_scenario-7,scn_size);
	fclose(inf);
	return 0;
}

//not used
int scan_ag_file(int scenario_number, int show_info){
	FILE * inf;
	char path[MAX_PATH];
	char head[16];
	unsigned char d,c,a,al,ala=0;
	unsigned short x,y;

	int offset_scenario=12;
	int offset_set;
	int offset_stm;

	sprintf(path,"game%03d.scn",scenario_number);
	canonicalize_filename(path,path,MAX_PATH);
	if (show_info) printf("Opening file %s\n",path);
	inf=fopen(path,"rb");
	if (inf == NULL) return ERROR_AG_SCN_BASE+ERROR_FPGE_FILE_NOT_FOUND;

	fread(head,12,1,inf);
	//head[12]=0;

	//printf("%s\n",head);

	fseek(inf, offset_scenario+DEPLOY_TABLE , SEEK_SET);
	fread(&d,sizeof(d),1,inf);
	fseek(inf, offset_scenario+CORE_UNITS , SEEK_SET);
	fread(&c,sizeof(c),1,inf);
	fseek(inf, offset_scenario+ALLIED_UNITS , SEEK_SET);
	fread(&a,sizeof(a),1,inf);
	fseek(inf, offset_scenario+AUX_UNITS , SEEK_SET);
	fread(&al,sizeof(al),1,inf);
	//fseek(inf, offset_scenario+AUX_ALLIED_UNITS , SEEK_SET);
	//fread(&al,sizeof(ala),1,inf);

	//printf("d=%d\n",d);
	//printf("c=%d\n",c);
	//printf("a=%d\n",a);
	//printf("al=%d\n",al);

	//set file start
	offset_set = offset_scenario+ d*4+DEPLOY_TO_UNITS + (c+a+al+ala)*14 + 7;

	fseek(inf, offset_set-7 , SEEK_SET);
	fread(head,7,1,inf);
	//head[7]=0;
	//printf("%s\n",head);

	fseek(inf, offset_set+MAP_X_ADDR, SEEK_SET);
	fread(&x, sizeof(x), 1, inf);
	fseek(inf, offset_set+MAP_Y_ADDR, SEEK_SET);
	fread(&y, sizeof(y), 1, inf);
	++x;
	++y; //PG sets this to one less than size i.e. last index
	//printf("x=%d\n",x);
	//printf("y=%d\n",y);
	//stm file start
	offset_stm = offset_set + MAP_LAYERS_START + 7 * x * y + 7;

	fseek(inf, offset_stm-7 , SEEK_SET);
	fread(head,7,1,inf);
	//head[7]=0;
	//printf("%s\n",head);

	fclose(inf);
	return ERROR_AG_SCN_BASE+ERROR_FPGE_AG_SCANED;
}
