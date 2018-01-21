/*
 * load.c
 *
 *  Created on: 2010-03-09
 *      Author: wino
 */
#include <allegro.h>
#include <stdio.h>
#include "fpge.h"
#include "filename.h"
#include "tables.h"
#include "load.h"
#include "cli.h"
#include "pgf.h"
#include "minimap.h"
#include "mapfrg.h"
#include "fpge_colors.h"

char load_error_str[256]; // last error description

int nupl_cc[NUPL_COUNTRY_IDX][NUPL_CLASS_IDX];
WORD nupl[NUPL_SIZE];

char nupl_countries[NUPL_COUNTRY_IDX] = {15,23,13,16,8,2,7,20,10,24,18,11,3,9,6,1,4,5,12,17,19,21,22,14};
char nupl_class[NUPL_CLASS_IDX]= {0,1,2,3,4,5,6,7,8,9,10,15};

// place the color number in the bmp image and bump pix_pos
void put_pix(BITMAP *bmp, int y, int clr)
{
  if (pix_pos>=max_pix_x) max_pix_x=pix_pos+1;  //some SP info
  if (y>=max_pix_y) max_pix_y=y+1;
  putpixel(bmp,pix_pos,y,clr);
  ++pix_pos;
}

void read_shp(BITMAP *bmp, FILE *inf,long addr){
	read_shp_ex(bmp,inf,addr,SHP_SWAP_COLOR);
}

// read the shp data (for icon n) into the bitmap image (bmp)
void read_shp_ex(BITMAP *bmp, FILE *inf,long addr, int swap)
{
   int l; // first line the counter
   int lf; // last line
   int ch,b,r,i;

   fseek(inf,addr+HEADER_SIZE,0);
   if (header.ystart<0)
   {
    l=0; lf=header.yend+abs(header.ystart);   //SP...
   }
   else
   {
     l=header.ystart;
     lf=header.yend;
   }
   if (header.xstart<0)      //SP....
      pix_pos=0;
   else
    pix_pos=header.xstart;
   do
   {
     // read data  and decode
     ch=fgetc(inf);
     r=ch%2;
     b=ch/2;
     if (b==0 && r==1) // a skip over
     {
        ch=fgetc(inf);
        for (i=0; i<ch; ++i)
        	//put_pix(bmp,l,BK_COLOR);
        	put_pix(bmp,l,fpge_mask_color);
     }
     else if (b==0)   // end of line
     {
       ++l;
       if (header.xstart<0)
       {
         pix_pos=0;
       }
       else pix_pos=header.xstart;
     }
     else if (r==0) // a run of bytes
     {
       ch=fgetc(inf); // the color #
       // SHP files uses 255 as transparent
       //allego in 8bit uses 0 as transparent, so we need to convert
       if (ch==0 && swap==1) ch=255;  //allegro uses color 0 as transparent (cf BK_COLOR)
       ch = colors_to24bits_ex(ch,swap);
       //else if (fpge_colors_bits>8) { c=ch; ch=(pgpal[c].r<<16)+(pgpal[c].g<<8)+pgpal[c].b; }
       //ch=mask_color;
       for (i=0; i<b; ++i)
        put_pix(bmp,l,ch);
     }
     else // b!0 and r==1 ... read the next b bytes as color #'s
     {
       for (i=0; i<b; ++i)
       {
         ch=fgetc(inf);
         if (ch==0 && swap==1) ch=255;
         ch = colors_to24bits_ex(ch,swap);
         //if (ch==0) (ch=fpge_mask_color);  //allegro uses color 0 as transparent (cf BK_COLOR)
         //else if (fpge_colors_bits>8) { c=ch; ch=(pgpal[c].r<<16)+(pgpal[c].g<<8)+pgpal[c].b; }
         //ch=mask_color;
         put_pix(bmp,l,ch);
       }
     }
   } while (l<=lf);
}

void read_header(FILE *shp,long addr)
{
   fseek(shp,addr,SEEK_SET);
   fread(&(header.lines),2,1,shp);
   fread(&(header.width),2,1,shp);
   fread(&(header.centerx),2,1,shp);
   fread(&(header.centery),2,1,shp);
   fread(&(header.xstart),4,1,shp);
   fread(&(header.ystart),4,1,shp);
   fread(&(header.xend),4,1,shp);
   fread(&(header.yend),4,1,shp);
}

//here we load tiles and create the bitmaps
//referenced by til_bmp[]. Return 0 on success
//an error code on failure
int load_tiles(int show_info)
{
  FILE *inf;
  int i,x,y,c,pix_color;
	char path[MAX_PATH];

	strncpy(path,til_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
  inf=fopen(path,"rb");
  if (!inf)
  {
     printf("\nCould not open tile file %s.\n",til_file);
     return ERROR_TILES_BASE+ERROR_FPGE_FILE_NOT_FOUND;
  }
  fseek(inf,4,SEEK_SET);
  fread(&total_tiles,4,1,inf);
  if (total_tiles>MAX_TILES)
  {
	  fclose(inf);
     printf("Maximum tiles limit (%d) is exceeded!!!\n",MAX_TILES);
     return ERROR_TILES_BASE+ERROR_FPGE_MAXIMUM_REACHED;
  }
  // read addresses
  for (i=0; i<total_tiles; ++i)
  {
    fseek(inf,8+8*i,SEEK_SET);
    fread(&(icon_addr[i]),4,1,inf);
  }
  //create the bitmap
  for (i=0; i<total_tiles; ++i)
  {
   til_bmp[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
   if (!til_bmp[i])
   {
     //allegro_exit();
     printf("\nCould not allocate tile bitmap %d\n",i);
     fclose(inf);
     return ERROR_TILES_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
   }
   dark_til_bmp[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
   if (!dark_til_bmp[i])
   {
     //allegro_exit();
     printf("\nCould not allocate dark tile bitmap %d\n",i);
     fclose(inf);
     return ERROR_TILES_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
   }

   til_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
   dark_til_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);

   til_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
   dark_til_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);

   // zero out the bitmap with the transparent color
   for (y=0; y<TILE_HEIGHT; ++y)
     for (x=0; x<TILE_FULL_WIDTH; ++x){
       putpixel(til_bmp[i],x,y,fpge_mask_color);
       putpixel(dark_til_bmp[i],x,y,fpge_mask_color);

       //putpixel(til_bmp_mud[i],x,y,fpge_mask_color);
       //putpixel(dark_til_bmp_mud[i],x,y,fpge_mask_color);

       //putpixel(til_bmp_snow[i],x,y,fpge_mask_color);
       //putpixel(dark_til_bmp_snow[i],x,y,fpge_mask_color);
     }
    //read in the data from the shp file
   read_header(inf,icon_addr[i]);
   read_shp(til_bmp[i],inf,icon_addr[i]);

   //make dark tile
   if (show_info && i>0 && i%500==0) printf("Loaded %d tiles so far...\n",i);
   for (y = 0; y < TILE_HEIGHT; ++y)
			for (x = 0; x < TILE_FULL_WIDTH; ++x){
				pix_color=getpixel(til_bmp[i], x, y);
				if ( pix_color!= fpge_mask_color) {
					//if (pix_color<16) printf("x:%d y:%d i:%d c:%d\n",x,y,i,pix_color); //debug for colorls <16
					if (fpge_colors_bits == 8)
						putpixel(dark_til_bmp[i], x, y, pix_color>16?(pix_color + 0x70):pix_color);
					else {
						c = find_pal_element(pix_color);
						if (c > -1)
							putpixel(dark_til_bmp[i], x, y, colors_to24bits(c>16?(c + 0x70):c));
					}
				} else
					putpixel(dark_til_bmp[i], x, y, fpge_mask_color);
			}
   // make mud & snow tiles
   //if (show_info) printf("Generating mud and snow tiles.\n");
   for (y = 0; y < TILE_HEIGHT; ++y)
			for (x = 0; x < TILE_FULL_WIDTH; ++x)
				if (fpge_colors_bits == 8){
					putpixel(til_bmp_mud[i], x, y, getpixel(til_bmp[i], x,y));
					putpixel(dark_til_bmp_mud[i], x, y, getpixel(dark_til_bmp[i], x,y));

					putpixel(til_bmp_snow[i], x, y, getpixel(til_bmp[i], x,y));
					putpixel(dark_til_bmp_snow[i], x, y, getpixel(dark_til_bmp[i], x,y));
				}
				else{
					c = find_pal_element92(getpixel(til_bmp[i], x, y));
					if (c==92){
						putpixel(til_bmp_mud[i], x, y, make_color_fpge(211,178,105)/*((int)211<<16)+((int)178<<8)+(int)105*/);
						putpixel(dark_til_bmp_mud[i], x, y, make_color_fpge(162,134,85)/*((int)162<<16) + ((int)134<<8) + 85*/);

						putpixel(til_bmp_snow[i], x, y, make_color_fpge(231,239,239)/*((int)231<<16)+((int)239<<8)+239*/);
						putpixel(dark_til_bmp_snow[i], x, y,  make_color_fpge(174,178,178)/*((int)174<<16) + ((int)178<<8) + 178*/);
					}else{
						putpixel(til_bmp_mud[i], x, y, getpixel(til_bmp[i], x,y));
						putpixel(dark_til_bmp_mud[i], x, y, getpixel(dark_til_bmp[i], x,y));

						putpixel(til_bmp_snow[i], x, y, getpixel(til_bmp[i], x,y));
						putpixel(dark_til_bmp_snow[i], x, y, getpixel(dark_til_bmp[i], x,y));
					}

				}

  }
  fclose(inf);
  //DEBUG, add more tiles
  /*
  x=10;
  for(i=0;i<total_tiles;i++){
	  for (y=0;y<x;y++){
	  til_bmp[i+total_tiles*y]=create_sub_bitmap(til_bmp[i],0,0,60,50);
	  dark_til_bmp[i+total_tiles*y]=create_sub_bitmap(dark_til_bmp[i],0,0,60,50);
	  }

  }
  total_tiles+=total_tiles*x;
  */
  //END OF DEBUG, add more tiles
  if (show_info) printf("#%d tiles loaded.\n",total_tiles);

   return 0;
}

//load the flags from the flag icon file
int load_flags()
{
  FILE *inf;
  int i,x,y;
  BITMAP *tmp_bmp;
	char path[MAX_PATH];

	strncpy(path,flag_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
  inf=fopen(path,"rb");
  if (!inf)
  {
     //allegro_exit();
     printf("\nCould not open flag file %s.\n",flag_file);
     return ERROR_FLAGS_BASE+ERROR_FPGE_FILE_NOT_FOUND;
  }
  fseek(inf,4,SEEK_SET);
  fread(&total_flags,4,1,inf);
  if (total_flags>MAX_FLAGS)
  {
     fclose(inf);
     printf("Maximum flags icons limit (%d) is exceeded!!!\n",MAX_FLAGS);
     return ERROR_FLAGS_BASE+ERROR_FPGE_MAXIMUM_REACHED;
  }
  // read addresses
  for (i=0; i<total_flags; ++i)
  {
    fseek(inf,8+8*i,SEEK_SET);
    fread(&(icon_addr[i]),4,1,inf);
  }
  //create the bitmap
  for (i=0; i<total_flags; ++i)
  {
   tmp_bmp=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
   //printf("tmp_bmp=%p\n",tmp_bmp);
   if (!tmp_bmp)
   {
	 fclose(inf);
     printf("\nCould not allocate flag bitmap %d\n",i);
     return ERROR_FLAGS_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
   }
   // zero out the bitmap with the transparent color
   for (y=0; y<TILE_HEIGHT; ++y)
     for (x=0; x<TILE_FULL_WIDTH; ++x)
       putpixel(tmp_bmp,x,y,fpge_mask_color);
    //read in the data from the shp file
   read_header(inf,icon_addr[i]);
   read_shp(tmp_bmp,inf,icon_addr[i]);

   flag_bmp[i]=tmp_bmp;
  }
  fclose(inf);
  return 0;
}

//load the strength from the strength icon file
// we also add "0" counter
int load_strength()
{
  FILE *inf;
  int i,x,y,color,h=0;
	char path[MAX_PATH];

  //printf("Opening strength file %s.\n",strength_file);
	strncpy(path,strength_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);

  inf=fopen(path,"rb");
  if (!inf)
  {
     //allegro_exit();
     printf("\nCould not open strength file %s.\n",strength_file);
     return ERROR_STRENGTH_BASE+ERROR_FPGE_FILE_NOT_FOUND;
  }
  fseek(inf,4,SEEK_SET);
  fread(&total_strength_counters,4,1,inf);
  //printf("Opening strength file %d.\n",total_strength_counters);
  if (total_strength_counters>MAX_STRENGTH)
  {
	  fclose(inf);
     printf("Maximum icon limit (%d) is exceeded!!!\n",MAX_STRENGTH);
     return ERROR_STRENGTH_BASE+ERROR_FPGE_MAXIMUM_REACHED;
  }
  // read addresses
  //printf("Read addresses\n");

  for (i=0; i<total_strength_counters; ++i)
  {
    fseek(inf,8+8*i,SEEK_SET);
    fread(&(icon_addr[i]),4,1,inf);
  }
  //create the bitmap
  for (i=0; i<MAX_STRENGTH_IN_ROW*3; ++i)
  {
  //printf("create the bitmap %d.\n",i);

   strength_bmp[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
   if (!strength_bmp[i])
   {
	   fclose(inf);
     printf("\nCould not allocate strength bitmap %d\n",i);
     return ERROR_STRENGTH_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
   }

   // zero out the bitmap with the transparent color
   for (y=0; y<TILE_HEIGHT; ++y)
     for (x=0; x<TILE_FULL_WIDTH; ++x)
       putpixel(strength_bmp[i],x,y,fpge_mask_color);
    //read in the data from the shp file
	//  printf("read in the data from the shp file.\n");
   if (i%MAX_STRENGTH_IN_ROW!=0 && i%MAX_STRENGTH_IN_ROW<16){
//	read_header(inf,icon_addr[i-i/MAX_STRENGTH_IN_ROW-1]);
//	read_shp(strength_bmp[i],inf,icon_addr[i-i/MAX_STRENGTH_IN_ROW-1]);
		read_header(inf,icon_addr[h]);
		read_shp(strength_bmp[i],inf,icon_addr[h]);
		h++;
   }
  }
//make 0 counters out of 10 counter

  if (!str0_bmp_loaded){
	  for(i=0;i<3;i++){
		  read_header(inf,icon_addr[9+i*15]);
		  read_shp(strength_bmp[0+i*MAX_STRENGTH_IN_ROW],inf,icon_addr[9+i*15]);
		  color=getpixel(strength_bmp[0+i*MAX_STRENGTH_IN_ROW],23,37);
		  rectfill(strength_bmp[0+i*MAX_STRENGTH_IN_ROW],24,38,29,46,color);
		  for(y=38;y<45;y++)
			  for(x=28;x<28+6;x++)
				  putpixel(strength_bmp[0+i*MAX_STRENGTH_IN_ROW],x,y,getpixel(strength_bmp[0+i*MAX_STRENGTH_IN_ROW],x+2,y));
	  }
  }
  /*
  read_header(inf,icon_addr[9+15]); //there are 15 counters per type in SHP
  read_shp(strength_bmp[0+MAX_STRENGTH_IN_ROW],inf,icon_addr[9+15]);
  color=getpixel(strength_bmp[0+MAX_STRENGTH_IN_ROW],23,37);
  rectfill(strength_bmp[0+MAX_STRENGTH_IN_ROW],24,38,29,46,color);

  read_header(inf,icon_addr[9+30]);
  read_shp(strength_bmp[0+MAX_STRENGTH_IN_ROW*2],inf,icon_addr[9+30]);
  color=getpixel(strength_bmp[0+MAX_STRENGTH_IN_ROW*2],23,37);
  rectfill(strength_bmp[0+MAX_STRENGTH_IN_ROW*2],24,38,29,46,color);
  */
  fclose(inf);
  return 0;
}

int convert_from_cp1250_to_utf8(unsigned char *save_to,
		unsigned char *read_from, int limit) {
	int c, j;
	unsigned short u1, u2;

	c = 0;
	for (j = 0; j < limit; j++) {
		if (read_from[j] > 0x7F) {
			//skip some
			if (cp1250_to_utf[read_from[j]] >= 0x80
					&& cp1250_to_utf[read_from[j]] <= 0x7ff) {

				u1 = (cp1250_to_utf[read_from[j]] & 0x03F) | 0x80;
				u2 = ((cp1250_to_utf[read_from[j]] & 0x007C0) | 0x3000)	>> 6;
				save_to[c] = (unsigned char) u2;
				c++;
				save_to[c] = (unsigned char) u1;
				c++;
			}
		} else {
			save_to[c] = read_from[j];
			c++;
		}
	}
	return c;
}

//here we load unit icons and create the bitmaps
//referenced by unit_bmp[]. Return 0 on success
//an error code on failure
int load_uicons()
{
  FILE *inf;
  int i,x,y;
	char path[MAX_PATH];

	strncpy(path,uicons_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
  inf=fopen(path,"rb");
  if (!inf)
  {
     printf("\nCould not open tile file %s.\n",uicons_file);
     return ERROR_UNIT_ICONS_BASE+ERROR_FPGE_FILE_NOT_FOUND;
  }
  fseek(inf,4,SEEK_SET);
  fread(&total_uicons,4,1,inf);
  if (total_uicons>MAX_UICONS)
  {
	 fclose(inf);
     printf("Maximum unit icon limit (%d) is exceeded!!!\n",MAX_UICONS);
     return ERROR_UNIT_ICONS_BASE+ERROR_FPGE_MAXIMUM_REACHED;
  }
  // read addresses
  for (i=0; i<total_uicons; ++i)
  {
    fseek(inf,8+8*i,SEEK_SET);
    fread(&(icon_addr[i]),4,1,inf);
  }
  //create the bitmap
  for (i=0; i<total_uicons; ++i)
  {
   unit_bmp[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
   if (!unit_bmp[i])
   {
	   fclose(inf);
     printf("\nCould not allocate unit bitmap %d\n",i);
     return ERROR_UNIT_ICONS_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
   }

   // zero out the bitmap with the transparent color
   for (y=0; y<TILE_HEIGHT; ++y)
     for (x=0; x<TILE_FULL_WIDTH; ++x)
       putpixel(unit_bmp[i],x,y,fpge_mask_color);
    //read in the data from the shp file
   read_header(inf,icon_addr[i]);
   if (header.xstart>=0 && header.lines<=120 && header.width<=120) //skip the problem icons
      read_shp(unit_bmp[i],inf,icon_addr[i]);
  }
  fclose(inf);
  return 0;
}

// load stack icons file
int load_sicons()
{
  FILE *inf;
  int i,x,y;
	char path[MAX_PATH];

	strncpy(path,sicons_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
  inf=fopen(path,"rb");
  if (!inf)
  {
     //allegro_exit();
     printf("\nCould not open file %s.\n",sicons_file);
     return ERROR_STACK_UNIT_ICONS_BASE+ERROR_FPGE_FILE_NOT_FOUND;
  }
  fseek(inf,4,SEEK_SET);
  fread(&total_sicons,4,1,inf);
  if (total_sicons>MAX_SICONS)
  {
	  fclose(inf);
     printf("Maximum stack units icon limit (%d) is exceeded!!!\n",MAX_SICONS);
     return ERROR_STACK_UNIT_ICONS_BASE+ERROR_FPGE_MAXIMUM_REACHED;
  }
  // read addresses
  for (i=0; i<total_sicons; ++i)
  {
    fseek(inf,8+8*i,SEEK_SET);
    fread(&(icon_addr[i]),4,1,inf);
  }
  //create the bitmap
  for (i=0; i<total_sicons; ++i)
  {
   stack_bmp[i]=create_bitmap(TILE_FULL_WIDTH,TILE_HEIGHT);
   if (!stack_bmp[i])
   {
	 fclose(inf);
     printf("\nCould not allocate stack unit bitmap %d\n",i);
     return ERROR_STACK_UNIT_ICONS_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
   }

   // zero out the bitmap with the transparent color
   for (y=0; y<TILE_HEIGHT; ++y)
     for (x=0; x<TILE_FULL_WIDTH; ++x)
       putpixel(stack_bmp[i],x,y,fpge_mask_color);
    //read in the data from the shp file
   read_header(inf,icon_addr[i]);
   if (header.xstart>=0) //skip the problem icons
      read_shp(stack_bmp[i],inf,icon_addr[i]);
  }
  fclose(inf);
  return 0;
}

int is_map2bmp_category(int cat){

	cat--; //adjust category to filter bit shifts

	if (cat==COAST_FILTER_INDEX || cat==ROAD_FILTER_INDEX || cat==RIVER_FILTER_INDEX)
		return 1;
	else
		return 0;
}

int validate_bit_string(char *str){
	int i;

	if (strlen(str)!=6) {
		return 0;
	}

	for(i=0;i<6;i++){
		if(str[i]!='X'&&str[i]!='O'){
			return 0;
		}
	}
	//printf("%6s\n",str);
	return 1;
}

int load_tiles_description(){
	FILE *inf;
	//char fname[16];
	char line[1024],tokens[50][256];
	//char road_connections[6];
	int id,i,j,cursor=0,token=0,lines;
	int m, main_category,found;
	char ms[10];
	//int gcursor=0;
	short t;
	unsigned int tt;
	//int dir_bit_mask[] = {0x01,0x02,0x08,0x10,0x20,0x80};
	char path[MAX_PATH];

	strncpy(path,fpge_tiles,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf == NULL)
		return ERROR_TILES_DESC_BASE+ERROR_FPGE_FILE_NOT_FOUND;

	lines=0;

	//count lines so error can be displayed with line number
	while(read_utf16_line_convert_to_utf8(inf,line)>=0){

		lines++;

		// What does those two line do ?
		//line[gcursor]=0;
		//gcursor=0;
		//strip comments
		for(i=0;i<strlen(line);i++)
			if (line[i]==0x23) { line[i]=0; break; }
		//tokenize
		token=0;
		cursor=0;
		memset(tokens,0,sizeof(tokens));

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

		id=atoi(tokens[0]);
		//print_dec(id);
		if (id>=MAX_TILES_IN_PG && id <total_tiles && token>1 ){
			//first token: default terrain type
			if (strlen(tokens[1]) != 0){
				t=atoi(tokens[1]);
				TTData_Max_Tiles[id]=t;
			}

			//default filter
			tt=0;
			if (strlen(tokens[2]) != 0){
				sscanf(tokens[2],"%x",&tt);
				FilterTiles_Max_Tiles[id]=tt;
			}

			t=0;
			if (strlen(tokens[8]) != 0){
				t=atoi(tokens[8]); //main group ID, in case when more then one bit is set in "default filter"
			}

			if (t<0 || t>12){
				printf("Error: Default category %d not >=0 and <=12, line %d\n",t,lines);
				break;
			}
			main_category=0;
			if (CountBits(tt)>1){
				if (t){
					//ok
					main_category=t;
				}else{
					//find lowest set bit
					for(i=0;i<32;i++){
						if (tt & (1 << i)){
							main_category=i+1;
							break;
						}
					}
					printf("Warning: filter got more then one bit set : 0x%x, default group set to %d, line %d\n",tt,main_category,lines);

				}
			}else{
				if (CountBits(tt)==1){
					//main category == first set bit, there is at least one bit set here
					m=1; //0x0001
					main_category=1;
					for(i=0;i<32;i++){
						if (tt & m) break;
						else {
							m = m << 1;
							main_category++;
						}
					}
				}
			}
			//main_category is valid
			m= 1<<(main_category-1);
			MainCategoryTiles_Max_Tiles[id]=m;

			for(i=0;i<total_tiles;i++){
				if (MainCategoryTiles_Max_Tiles[tiles_display_max_tiles[i]]>m) {
					for(j=total_tiles;j>i;j--)
						tiles_display_max_tiles[j]=tiles_display_max_tiles[j-1];
					tiles_display_max_tiles[i]=id;
					break;
				}
			}

			//default name
			if (strlen(tokens[3]) != 0){
				t=atoi(tokens[3]);
				NData_Max_Tiles[id]=t;
			}

			//map pattern
			//map2bmp pattern: coast: where ocean is, river where river is, road where road is.
			//XOOXOO - N, NE, SE, S, SW, NW - map pattern
			strupr(tokens[4]);
			if (strlen(tokens[4]) != 0) {
				if (is_map2bmp_category(main_category)) {
					if (validate_bit_string(tokens[4])) {
						m = 0;
						for (i = 0; i < 6; i++)
							if (tokens[4][i] == 'X') {
								m |= mask_conv_N_CW_to_SE_CCW[i];
							}
						//we have main_category set here
						switch (main_category - 1) {
						case COAST_FILTER_INDEX:
							//for coast we check if pattern is already in table
							found = 0;
							for (i = 0; i < coast_size; i++) {
								if (coast_pattern_SE_CCW[i] == m) {
									//some cases
									//AAA, ABA no other cases are handled
									found = 1;
									if (coast_pattern_tile[i][0] == coast_pattern_tile[i][1] && coast_pattern_tile[i][2] == coast_pattern_tile[i][1]) {
										//AAA -> ABA
										coast_pattern_tile[i][1] = id;
										break; //for
									}
									if (coast_pattern_tile[i][0] == coast_pattern_tile[i][2] && coast_pattern_tile[i][0] != coast_pattern_tile[i][1]) {
										//ABA -> ABC
										coast_pattern_tile[i][2] = id;
										break; //for
									}
									//warning, 4th case found
									printf("Warning: Pattern defined 4th time, ignoring. Line %d\n", lines);
									break;
								}
							}
							if (!found) {
								coast_pattern_SE_CCW[coast_size] = m;
								coast_pattern_tile[coast_size][0] = id;
								coast_pattern_tile[coast_size][1] = id;
								coast_pattern_tile[coast_size][2] = id;
								coast_size++;
							}
							break;
						case ROAD_FILTER_INDEX:
							road_pattern_SE_CCW[road_size] = m;
							;
							road_pattern_tile[road_size][0] = id;
							road_pattern_tile[road_size][1] = id;
							road_pattern_tile[road_size][2] = id;
							road_size++;
							break;
						case RIVER_FILTER_INDEX:
							river_pattern_SE_CCW[river_size] = m;
							river_pattern_tile[river_size][0] = id;
							river_pattern_tile[river_size][1] = id;
							river_pattern_tile[river_size][2] = id;
							river_size++;
							break;
						}
					} else {
						printf("Error: %s, line %d\n", tokens[4], lines);
						break;
					}
				}
			}
			//road connections
			//XOOXOO - NNE, S, SSE, SSW, W, NNW - road touches hex CORRNER
			strupr(tokens[5]);
			if (FilterTiles_Max_Tiles[id] & (1 << ROAD_FILTER_INDEX)) {
				if (strlen(tokens[5]) != 0) {
					if (validate_bit_string(tokens[5])) {
						for (i = 0; i < 6; i++) {
							if (tokens[5][i] == 'X') {
								ms[i] = '1';
							} else {
								ms[i] = '0';
							}
						}
						ms[6] = 0;
						road_connection_info[roads_tiles_size].tile=id;
						strncpy(road_connection_info[roads_tiles_size].pattern_NNW_CW,ms,7);
						roads_tiles_size++;

					} else {
						printf("Error: %s, line %d\n", tokens[5], lines);
						break;
					}
				} else {
					printf("Warning: Road tile should have connections defined, line %d\n", lines);
				}
			}

			//river connections, PzC output filter only
			//XOOXOO - NNE, S, SSE, SSW, W, NNW - river touches hex CORRNER
			strupr(tokens[6]);
			if (FilterTiles_Max_Tiles[id] & (1 << RIVER_FILTER_INDEX)) {
				if (strlen(tokens[6]) != 0) {
					if (validate_bit_string(tokens[6])) {
						//ms="";
						for (i = 0; i < 6; i++) {
							if (tokens[6][i] == 'X') {
								ms[i] = '1';
							} else {
								ms[i] = '0';
							}
						}
						ms[6] = 0;

						river_connection_info[max_river_conversions_patterns].tile=id;
						strncpy(river_connection_info[max_river_conversions_patterns].pattern_NNW_CW,ms,7);
						max_river_conversions_patterns++;
					} else {
						printf("Error: %s, line %d\n", tokens[4], lines);
						break;
					}
				} else {
					printf("Warning: River tile should have connections defined for PzC map export, line %d\n", lines);
				}
			}
			// Only one flag at a time is possible.
			// R- passive road tiles, probably city tile. Used for road connections
			// O- ocean tile - with more water then ground
			strupr(tokens[7]);
			if (strlen(tokens[7]) != 0) {
				switch (tokens[7][0]){
				case 'R':
					roads_passive_tiles[roads_passive_tiles_size]=id;
					roads_passive_tiles_size++;
					break;
				case 'O':
					ocean_tiles[ocean_tiles_size]=id;
					ocean_tiles_size++;
					break;
				}
			}

			if (token ==10){
				//10th column is random group
				t=atoi(tokens[9]);
				RandomGroupTiles_Max_Tiles[id]=t;
			}

		}else{
			if (token!=1 && id!=0) printf("Warning: Tile ID illegal or wrong number of columns. Ignoring line %d\n", lines);

		}
		//for(i=0;i<token;i++)
		//	printf("%s->",tokens[i]);
	//}

	}

	fclose(inf);
	return 0;
}


int parse_nupl() {
	int class = 0, country = 0, cursor = 0, saved_cursor = 0, i,j,k;

	for (i = 0; i < NUPL_SIZE; i++) {
		if (nupl[cursor] == -1) {
			nupl_cc[country][class] = saved_cursor;
			cursor++;
			saved_cursor = cursor;
			class++;
			if (class >= NUPL_CLASS_IDX) {
				class = 0;
				country++;
			}
			if (country>NUPL_COUNTRY_IDX) break;//end of processing
		} else {
			if (nupl[cursor] == -2) break; //end of processing
			cursor++;
		}
	}
	//some special cases:
	//AG
	// FIN got own NUPL
	//Norway-austria, czech, denmark, turkey, portugal, spain, sweden , Switzerland
	//Belgium-luxemburg
	//PG:
	//1 Norway ---------------------- Dutch(Netherlands), Finland, Sweden, Switzerland, Spain, Portugal, Austria, Turkey, Denmark
	//	6 Belgium --------------------- Luxembourg
	//printf("country=%d,class=%d,nupl_countries[country]=%d\n",country,class,nupl_countries[country]);
	if (country!=13 && country!=14 && country!=15) return 1; //we exit since there is a problem with nulp table
	// now we try to copy NUPL for rest of the countries
	if (class!=0) country++;
	j=9;
	//printf("country-1=%d,class=%d,nupl_countries[country-1]=%d\n",country-1,class,nupl_countries[country-1]);
	if (nupl_countries[country-1]==6) j--; //finland
	for(i=0;i<j;i++)
		for(k=0;k<NUPL_CLASS_IDX;k++)
			nupl_cc[country+i][k]=nupl_cc[0][k]; // 0-we use Norway
	country+=j;
	//Luxembourg
	//printf("country=%d,class=%d,nupl_countries[country]=%d\n",country,class,nupl_countries[country]);

	for(k=0;k<NUPL_CLASS_IDX;k++)
		nupl_cc[country][k]=nupl_cc[5][k]; // 0-we use Belgium

	//debug
	//for(k=0;k<NUPL_COUNTRY_IDX;k++)
	//	printf("k=%d nc=%d c=%d\n",k,nupl_cc[k][0],nupl_countries[k]);

	return 1;
}

int load_nupl(){

		char path[MAX_PATH];
		FILE *inf;
		//int error=0;
		long offset=-1;

		sprintf(path,"%s%s","..\\exe\\",exe_panzer);
		canonicalize_filename(path,path,MAX_PATH);
		inf = fopen(path, "rb");
		if (!inf){
			strncpy(path, exe_panzer,MAX_PATH);
			canonicalize_filename(path,path,MAX_PATH);
			inf = fopen(path, "rb");
			if (!inf)
				return ERROR_PANZER_EXE_BASE+ERROR_FPGE_FILE_NOT_FOUND;
		}
		fseek(inf, 0, SEEK_END);
		long file_size = ftell(inf);
		//printf("%d\n",file_size);
		//FPGE will understand only two PG.exe sizes:
		if (file_size==814305) offset=776008;
		if (file_size==811233) offset=772936;
		if (offset==-1) {
			fclose(inf);
			return ERROR_PANZER_EXE_BASE+ERROR_FPGE_UNKNOWN_EXE;
		}
		fseek(inf,offset,SEEK_SET);

		int blocks_read = fread(nupl,sizeof(WORD),PG_NUPL_SIZE,inf);
		if (blocks_read!=PG_NUPL_SIZE){
			fclose(inf);
			return ERROR_PANZER_EXE_BASE+ERROR_FPGE_WRONG_NULP;
		}
		//for(i=0;i<20;i++) printf("%d %d\n",i,nulp[i]);
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

int load_p_slots_txt() {
	FILE *inf;
	//char fname[16];
	char line[1024];//, tokens[50][256], road_connections[6];
	//int m, id, i, cursor = 0, gcursor = 0, token = 0, lines, eof = 0;
	int i,  gcursor = 0,  lines, eof = 0, nulp_cursor=0;
	short digit;
	//short t;
	char path[MAX_PATH];

	strncpy(path,nulp_pslots,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf = fopen(path, "rb");
	if (inf == NULL)
		return ERROR_P_SLOTS_TXT_BASE + ERROR_FPGE_FILE_NOT_FOUND;

	lines = 0;
	while (!eof) {
		//eof = fread(&digit, 1, 1, inf) <= 0; //we read only 1 byte
		digit=fgetc(inf);

		if (digit == EOF) eof=1; //end while
		//if (eof == 1)
		//	digit = EOF;
		//printf("%d\n",digit);
		//count lines so error can be displayed with line number
		lines++;
		if (digit == 0x0d)
			continue;
		if (digit == 0x0a || digit == EOF) {
			line[gcursor] = 0;
			gcursor = 0;
			digit = 256;  //do not append this digit to new line
			//strip comments
			for (i = 0; i < strlen(line); i++)
				if (line[i] == '"') {
					line[i] = 0;
					break;
				}

			if (strlen(line) > 1) {
				//id = ;
				nupl[nulp_cursor]=atoi(line);
				nulp_cursor++;
				//for(i=0;i<token;i++)
				//printf("%s->", line);
			}
		}

		if (digit > 0 && digit < 256) {
			//protect from longlines
			if (gcursor < 1024) {
				line[gcursor] = (unsigned char) digit;
				gcursor++;
			}
		}
	}
	//add end of table
	nupl[nulp_cursor]=-2;
	parse_nupl();

	fclose(inf);
	return 0;
}

int load_nulp_txt() {
	FILE *inf;
	//char fname[16];
	char line[1024];//, tokens[50][256], road_connections[6];
	//int m, id, i, cursor = 0, gcursor = 0, token = 0, lines, eof = 0;
	int  i, gcursor = 0,  lines, eof = 0, nulp_cursor=0;
	short digit;
	short value;
	char path[MAX_PATH];

	strncpy(path,nulp_nulp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf = fopen(path, "rb");
	if (inf == NULL)
		return ERROR_UNIT_TXT_BASE + ERROR_FPGE_FILE_NOT_FOUND;

	//printf("nulp\n");

	lines = 0;
	while (!eof) {
		//eof = fread(&digit, 1, 1, inf) <= 0; //we read only 1 byte
		digit=fgetc(inf);

		if (digit == EOF) eof=1; //end while
		//if (eof == 1)
		//	digit = EOF;
		//if (lines==0) printf("%d\n",digit);
		//count lines so error can be displayed with line number

		if (digit == 0x0d)
			continue;
		if (digit == 0x0a || digit == EOF) {
			line[gcursor] = 0;
			gcursor = 0;
			digit = 256; //do not append this digit to new line
			lines++;
			//printf("%s, %d\n", line,lines);
			//ignore comments from first line
			if (lines == 1) continue;

			if (strlen(line) > 5) {//we ignore lines with less then 5 chars
				if (line[0]==' ' && line[1]==' '){ //we ignore lines with not two spaces at the front

					if (line[2]==' '){
						sscanf(line,"%8x",&i);
						value=i;
					}else{
						value=-1;
					}
					//id = ;
					nupl[nulp_cursor]=value;
					nulp_cursor++;
					//for(i=0;i<token;i++)
					//printf("%d\n", value);
				}
			}
		}

		if (digit > 0 && digit < 256) {
			//protect from longlines
			if (gcursor < 1024) {
				line[gcursor] = (unsigned char) digit;
				gcursor++;
			}
		}
	}
	//add end of table
	nupl[nulp_cursor]=-2;
	parse_nupl();

	fclose(inf);
	return 0;
}

int load_unit_txt() {
	FILE *inf;
	//char fname[16];
	char line[1024];//, tokens[50][256], road_connections[6];
	//int m, id, i, cursor = 0, gcursor = 0, token = 0, lines, eof = 0;
	int  i,v, gcursor = 0,  lines, eof = 0, no,c=0;
	short digit;
	short value;
	char path[MAX_PATH];

	strncpy(path,nulp_unit,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf = fopen(path, "rb");
	if (inf == NULL)
		return ERROR_UNIT_TXT_BASE + ERROR_FPGE_FILE_NOT_FOUND;

	//printf("nulp\n");

	for (i=0;i<NUPL_SIZE;i++)
		nupl[i]=-1;

	lines = 0;
	while (!eof) {
		//eof = fread(&digit, 1, 1, inf) <= 0; //we read only 1 byte
		digit=fgetc(inf);

		if (digit == EOF) eof=1; //end while
		//if (eof == 1)
		//	digit = EOF;
		//if (lines==0) printf("%d\n",digit);
		//count lines so error can be displayed with line number

		if (digit == 0x0d)
			continue;
		if (digit == 0x0a || digit == EOF) {
			line[gcursor] = 0;
			gcursor = 0;
			digit = 256; //do not append this digit to new line
			lines++;
			//printf("%s, %d\n", line,lines);
			//ignore comments from first line
			//if (lines == 1) continue;

			no= sscanf(line,"%8d %8d",&i,&v);

			if (no == 2) {
				value = v;
				nupl[i] = value;
				c = 0;
			} else {
				//printf("%s\n",line);
				if (strstr(line, "......") != NULL)
					c++;
			}

		}

		if (digit > 0 && digit < 256) {
			//protect from longlines
			if (gcursor < 1024) {
				line[gcursor] = (unsigned char) digit;
				gcursor++;
			}
		}
	}
	//Add end of table. Let's hope it will work...
	nupl[i+c+1]=-2;
	//printf("NULP unit.txt i=%d, c=%d, sum=%d\n",i,c,i+c);
	parse_nupl();

	fclose(inf);
	return 0;
}

int load_map_fragments(){
	FILE *inf;
	//char fname[16];
	char line[1024],tokens[50][256];
	int i,cursor=0,token=0,lines;
	//short t;
	int expect_more_maps_frg=0,x=-1,y=-1,off=-1,type=-1,val;
	char path[MAX_PATH];

	strncpy(path,fpge_mapfrg,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf == NULL)
		return ERROR_MAP_FRAGMENTS_BASE+ERROR_FPGE_FILE_NOT_FOUND;

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

		if (token==4){
			if (expect_more_maps_frg){
				printf("Error. Line %d\n",lines);
				expect_more_maps_frg=0;
			}else{
				x=atoi(tokens[0]);
				y=atoi(tokens[1]);
				off=atoi(tokens[2]);
				type=atoi(tokens[3]);
				expect_more_maps_frg=y;
				//printf("%d, %d, %d, %d\n",x,y,off,type);
			}
		}
		if (expect_more_maps_frg && token>=y){

			for (i = 0; i < x; ++i) {
				val=atoi(tokens[i]);
				map_frg_big_table_new[map_frg_big_table_size].map_frg_big_table[y-expect_more_maps_frg][i]=val;
				//printf("%3d, ",val);
			}
			//printf("\n");
			expect_more_maps_frg--;
			if (expect_more_maps_frg==0){
				map_frg_big_table_new[map_frg_big_table_size].map_frg_big_table_offset=off;
				map_frg_big_table_new[map_frg_big_table_size].map_frg_big_table_type=type;
				map_frg_big_table_size++;
			}
		}
	}

	//dump
/*
	int j,k;
	printf("%d\n",map_frg_big_table_size);


	for(i=0;i<map_frg_big_table_size;i++){
		printf("{.map_frg_big_table_type=%d, .map_frg_big_table_offset=%d, .map_frg_big_table={\n",map_frg_big_table_new[i].map_frg_big_table_type,map_frg_big_table_new[i].map_frg_big_table_offset);
		for(j=0;j<9;j++){
			printf("{");

		for(k=0;k<9;k++)
			printf("%d%c", map_frg_big_table_new[i].map_frg_big_table[j][k],k<8?',':' ');
		printf("},\n");
		}
		printf("}},\n");
	}
*/
	fclose(inf);
	return 0;
}

int load_countries_description(){
	FILE *inf;

	char line[1024],tokens[50][256];
	int id,i,cursor=0,token=0,lines;
	char path[MAX_PATH];

	strncpy(path,fpge_countries,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf == NULL) return ERROR_COUNTRY_DESC_BASE+ERROR_FPGE_FILE_NOT_FOUND;

	lines=0;

	//count lines so error can be displayed with line number
	while(read_utf16_line_convert_to_utf8(inf,line)>=0){

		lines++;

		// What does those two line do ?
		//line[gcursor]=0;
		//gcursor=0;
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
		//for(id=0;id<MAX_COUNTRY; id++)
		//		printf("%s %s \n",country_names[id],country_names_short[id]);

		id=atoi(tokens[0]);
		if (id>=0 && id <=MAX_COUNTRY && (token==3 || token==4)){
			strncpy(country_names[id],tokens[1],MAX_COUNTRY_NAME_SIZE);
			country_names[id][MAX_COUNTRY_NAME_SIZE-1]=0;

			strncpy(country_names_short[id],tokens[2],MAX_COUNTRY_SHORT_NAME_SIZE);
			country_names_short[id][MAX_COUNTRY_SHORT_NAME_SIZE-1]=0;
			//printf(">%s< >%s< >%s<\n",tokens[0],tokens[1],tokens[2]);

			country_active[id]=1; //make active

			if (token==4)
				country_side[id]=atoi(tokens[3]);

			//for(id=0;id<MAX_COUNTRY; id++)
			//		printf("%s %s \n",country_names[id],country_names_short[id]);

		}else{
			if (id!=0)
				printf("Wrong line : %s\n",line);
		}
	}

	fclose(inf);

	//for(id=0;id<MAX_COUNTRY; id++)
	//	printf("%s %s \n",country_names[id],country_names_short[id]);
	return 0;
}

int load_icons_description(){
	FILE *inf;

	char line[1024],tokens[50][256];
	int id,i,cursor=0,token=0,lines;
	char path[MAX_PATH];

	strncpy(path,fpge_icons,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf == NULL) return ERROR_UNITS_DESC_BASE+ERROR_FPGE_FILE_NOT_FOUND;

	lines=0;

	//count lines so error can be displayed with line number
	while(read_utf16_line_convert_to_utf8(inf,line)>=0){

		lines++;

		// What does those two line do ?
		//line[gcursor]=0;
		//gcursor=0;
		//strip comments
		for(i=0;i<strlen(line);i++)
			if (line[i]==0x23)
			{
				line[i]=0;
				break;
			}
		//tokenize
		token=0;
		cursor=0;
		for(i=0;i<strlen(line);i++)
			if (line[i]==0x09)
			{
				tokens[token][cursor]=0;
				token++;
				cursor=0;
			}
			else
			{
				tokens[token][cursor]=line[i];
				cursor++;
			}
		tokens[token][cursor]=0;
		token++;
		//for(id=0;id<MAX_COUNTRY; id++)
		//		printf("%s %s \n",country_names[id],country_names_short[id]);

		id=atoi(tokens[0]);
		if (id>=0 && id <=MAX_UNITS && (token==3 || token==2)){

			if (token==3){
				strncpy(icon_name_aux1[id],tokens[1],MAX_UNIT_AUX_NAME_SIZE*2);
				icon_name_aux1[id][MAX_UNIT_AUX_NAME_SIZE*2-1]=0;

				strncpy(icon_name_aux2[id],tokens[2],MAX_UNIT_AUX_NAME_SIZE*2);
				icon_name_aux2[id][MAX_UNIT_AUX_NAME_SIZE*2-1]=0;
			}
			else{
				//token==2
				strncpy(icon_name_aux2[id],tokens[1],MAX_UNIT_AUX_NAME_SIZE*2);
				icon_name_aux2[id][MAX_UNIT_AUX_NAME_SIZE*2-1]=0;

				snprintf(icon_name_aux1[id],MAX_UNIT_AUX_NAME_SIZE*2,"%d",id);
			}
			//printf(">%s< >%s< >%s<\n",tokens[0],tokens[1],tokens[2]);


		}else{
			if (id!=0)
				printf("Wrong line : %s\n",line);
		}
	}

	fclose(inf);

	//for(id=0;id<MAX_COUNTRY; id++)
	//	printf("%s %s \n",country_names[id],country_names_short[id]);
	return 0;
}

int load_bmp2country_description(){
	FILE *inf;

	char line[1024],tokens[50][256];
	int id,i,cursor=0,token=0,lines;
	char path[MAX_PATH];

	strncpy(path,fpge_bmp2ctry,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (inf == NULL) return ERROR_BMP2CNTY_FILE_BASE+ERROR_FPGE_FILE_NOT_FOUND;

	lines=0;

	//count lines so error can be displayed with line number
	while(read_utf16_line_convert_to_utf8(inf,line)>=0){

		lines++;

		// What does those two line do ?
		//line[gcursor]=0;
		//gcursor=0;
		//strip comments
		for(i=0;i<strlen(line);i++)
			if (line[i]==0x23)
			{
				line[i]=0;
				break;
			}
		//tokenize
		token=0;
		cursor=0;
		for(i=0;i<strlen(line);i++)
			if (line[i]==0x09)
			{
				tokens[token][cursor]=0;
				token++;
				cursor=0;
			}
			else
			{
				tokens[token][cursor]=line[i];
				cursor++;
			}
		tokens[token][cursor]=0;
		token++;
		//for(id=0;id<MAX_COUNTRY; id++)
		//		printf("%s %s \n",country_names[id],country_names_short[id]);

		id=atoi(tokens[0]);
		if (id>=0 && id <=MAX_UICONS && token==2){

			bmp_to_country[id]=atoi(tokens[1]);

			//printf(">%s< >%s< >%s<\n",tokens[0],tokens[1]);

			//for(id=0;id<MAX_COUNTRY; id++)
			//		printf("%s %s \n",country_names[id],country_names_short[id]);

		}else{
			if (id!=0)
				printf("Wrong line : %s\n",line);
		}
	}

	fclose(inf);

	//for(id=0;id<MAX_COUNTRY; id++)
	//	printf("%s %s \n",country_names[id],country_names_short[id]);
	return 0;
}


int load_0str_bmp(){
	FILE *inf;
	char path[MAX_PATH];
	BITMAP *strength_bitmap;
	int x,y,dx,dy,i,j,c,h=0,pink;

	strncpy(path,fpge_0str_bmp,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf=fopen(path,"rb");
	if (!inf){
			return ERROR_0STR_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fclose(inf);
	if(fpge_colors_bits_shift) pink=0xe1e1ff+(fpge_colors_bits_shift==2?0xff000000:0);
	else pink=0xffe1e1;
	//all ok, load file
	//printf("01\n");
	strength_bitmap = load_bmp(path, NULL);
	//printf("%d %d %d\n",new_map_bitmap->w,new_map_bitmap->h, bitmap_color_depth(new_map_bitmap));
	dx=strength_bitmap->w/TILE_FULL_WIDTH;
	dy=strength_bitmap->h/TILE_HEIGHT;
	//printf("%d %d\n",dx,dy);
	if (strength_bitmap->w % TILE_FULL_WIDTH || dx!=1) {
		destroy_bitmap(strength_bitmap);
		return ERROR_0STR_BASE + ERROR_FPGE_WRONG_BMP_SIZE_X;
	}
	if (strength_bitmap->h%TILE_HEIGHT || dy!=4) {
		destroy_bitmap(strength_bitmap);
		return ERROR_0STR_BASE+ERROR_FPGE_WRONG_BMP_SIZE_Y;
	}
	//printf("001\n");

	//we will read only 4 rows
	dy=4;
	for(y=0;y<dy;y++)
		for(x=0;x<dx;x++){
			//proxy_bitmap=create_sub_bitmap(new_map_bitmap,x*dx,y*dy,60,50);
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
			h+=MAX_STRENGTH_IN_ROW;
		}
	destroy_bitmap(strength_bitmap);
	str0_bmp_loaded=1;
	return 0;
}
