#include <allegro.h>

#include <stdio.h>

#include "fpge.h"
#include "load.h"
#include "callback.h"
#include "fpge_colors.h"
#include "minimap.h"
#include "filename.h"
#include "tables.h"

//cross
unsigned char mm_cross[][TILE_FULL_WIDTH_]={
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x27,0x59,0xFF,0x27,0x00,0x00,0x00,0x00},
{0x00,0x00,0x27,0x27,0x59,0xFF,0x27,0x27,0x00,0x00,0x00},
{0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00},
{0x00,0x00,0x59,0x59,0x59,0xFF,0x59,0x59,0x00,0x00,0x00},
{0x00,0x00,0x27,0x27,0x59,0xFF,0x27,0x27,0x00,0x00,0x00},
{0x00,0x00,0x00,0x27,0x59,0xFF,0x27,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};
//star
unsigned char mm_star[][TILE_FULL_WIDTH_]={
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0xBC,0x1C,0xFF,0x00,0x12,0x1C,0xFF,0x00,0x00},
{0x00,0x00,0x89,0x1C,0x1C,0x12,0x1C,0x19,0x88,0x00,0x00},
{0x00,0x00,0x00,0x19,0x1C,0x1C,0x1C,0x12,0x00,0x00,0x00},
{0x00,0x00,0x4E,0x1C,0x37,0x1C,0x77,0x1C,0x12,0x00,0x00},
{0x00,0xBC,0x1C,0x37,0x37,0x37,0x1C,0x1C,0x1C,0x12,0x00},
{0x00,0x00,0x00,0x12,0x19,0x1C,0x19,0x12,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x12,0x1C,0x12,0x00,0x00,0x00,0x00}
};
//white spot
unsigned char mm_white_city[][TILE_FULL_WIDTH_]={
{0x00,0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x00,0x00,0x00},
{0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x00,0x00},
{0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x00},
{0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F},
{0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F},
{0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x00},
{0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x00,0x00},
{0x00,0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x00,0x00,0x00}
};

//green spot
unsigned char mm_green_city[][TILE_FULL_WIDTH_]={
{0x00,0x00,0x00,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x00,0x00},
{0x00,0x00,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x00},
{0x00,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00},
{0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A},
{0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A},
{0x00,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00},
{0x00,0x00,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x00},
{0x00,0x00,0x00,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x00,0x00}
};

//here we load strategy map small (11x8) tiles and create the bitmaps
//referenced by mtil_bmp[]. Return 0 on success
//an error code on failure
int load_mini_tiles()
{
  FILE *inf;
  int i,x,y,c;
	char path[MAX_PATH];

	strncpy(path,mtil_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
  inf=fopen(path,"rb");
  if (!inf)
  {
     allegro_exit();
     printf("\nCould not open strategic map tile file %s.\n",mtil_file);
     return ERROR_MINITILES_BASE+ERROR_FPGE_FILE_NOT_FOUND;
  }
  fseek(inf,4,SEEK_SET);
  fread(&total_mtiles,4,1,inf);
  if (total_mtiles>MAX_TILES)
  {
	 fclose(inf);
     allegro_exit();
     printf("Maximum strategic map tiles limit (%d) is exceeded!!!\n",MAX_TILES);
     return ERROR_MINITILES_BASE+ERROR_FPGE_MAXIMUM_REACHED;
  }
  // read addresses
  for (i=0; i<total_mtiles; ++i)
  {
    fseek(inf,8+8*i,SEEK_SET);
    fread(&(icon_addr[i]),4,1,inf);
  }
  //create the bitmap
  for (i=0; i<total_mtiles; ++i)
  {
   mtil_bmp[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
   dark_mtil_bmp[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
   mtil_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
   dark_mtil_bmp_mud[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
   mtil_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
   dark_mtil_bmp_snow[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
   if (!mtil_bmp[i] && !dark_mtil_bmp[i])
   {
     //allegro_exit();
     printf("\nCould not allocate strategic map tile bitmap %d\n",i);
     return ERROR_MINITILES_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
   }

   // zero out the bitmap with the transparent color
   for (y=0; y<TILE_HEIGHT_; ++y)
     for (x=0; x<TILE_FULL_WIDTH_; ++x){
       putpixel(mtil_bmp[i],x,y,fpge_mask_color);
	   putpixel(dark_mtil_bmp[i],x,y,fpge_mask_color);
	   }
    //read in the data from the shp file
   read_header(inf,icon_addr[i]);
   read_shp(mtil_bmp[i],inf,icon_addr[i]);
   
   for (y=0; y<TILE_HEIGHT_; ++y)
    for (x=0; x<TILE_FULL_WIDTH_; ++x)
	if (getpixel(mtil_bmp[i],x,y)!=fpge_mask_color){
		if (fpge_colors_bits == 8)
			putpixel(dark_mtil_bmp[i],x,y, getpixel(mtil_bmp[i],x,y)+0x70);
		else {
			c = find_pal_element(getpixel(mtil_bmp[i], x, y));
			if (c > -1)
				putpixel(dark_mtil_bmp[i], x, y, colors_to24bits(c
					+ 0x70));
			}
	}
	else
	   putpixel(dark_mtil_bmp[i],x,y,fpge_mask_color);

   // make mud & snow tiles
   //if (show_info) printf("Generating mud and snow tiles.\n");
   for (y = 0; y < TILE_HEIGHT; ++y)
			for (x = 0; x < TILE_FULL_WIDTH; ++x)
				if (fpge_colors_bits == 8){
					putpixel(mtil_bmp_mud[i], x, y, getpixel(mtil_bmp[i], x,y));
					putpixel(dark_mtil_bmp_mud[i], x, y, getpixel(dark_mtil_bmp[i], x,y));

					putpixel(mtil_bmp_snow[i], x, y, getpixel(mtil_bmp[i], x,y));
					putpixel(dark_mtil_bmp_snow[i], x, y, getpixel(dark_mtil_bmp[i], x,y));
				}
				else{
					c = find_pal_element92(getpixel(mtil_bmp[i], x, y));
					if (c==92){
						putpixel(mtil_bmp_mud[i], x, y, makecol(211,178,105)/*((int)211<<16)+((int)178<<8)+(int)105*/);
						putpixel(dark_mtil_bmp_mud[i], x, y, makecol(162,134,85)/*((int)162<<16) + ((int)134<<8) + 85*/);

						putpixel(mtil_bmp_snow[i], x, y, makecol(231,239,239)/*((int)231<<16)+((int)239<<8)+239*/);
						putpixel(dark_mtil_bmp_snow[i], x, y,  makecol(174,178,178)/*((int)174<<16) + ((int)178<<8) + 178*/);
					}else{
						putpixel(mtil_bmp_mud[i], x, y, getpixel(mtil_bmp[i], x,y));
						putpixel(dark_mtil_bmp_mud[i], x, y, getpixel(dark_mtil_bmp[i], x,y));

						putpixel(mtil_bmp_snow[i], x, y, getpixel(mtil_bmp[i], x,y));
						putpixel(dark_mtil_bmp_snow[i], x, y, getpixel(dark_mtil_bmp[i], x,y));
					}
				}
  }
  fclose(inf);
  return 0;
}

int build_mini_uicons(){
	int x,y;

	total_muicons=2;
	munit_bmp[0]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	munit_bmp[1]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	for (y=0; y<TILE_HEIGHT_; ++y)
	     for (x=0; x<TILE_FULL_WIDTH_; ++x){
	    	 // [8-y] - pictures are filpped ..
	       putpixel(munit_bmp[0],x,y,colors_to24bits(mm_cross[8-y][x]));
	       putpixel(munit_bmp[1],x,y,colors_to24bits(mm_star[8-y][x]));
	     }

	return 0;
}

int load_mini_uicons()
{
  FILE *inf;
  int i,x,y;
	char path[MAX_PATH];

	strncpy(path,muicons_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
  inf=fopen(path,"rb");
  if (!inf)
  {
     //allegro_exit();
     printf("\nCould not open strategic map icons file %s.\n",muicons_file);
     return ERROR_MINIUNIT_BASE+ERROR_FPGE_FILE_NOT_FOUND;
  }
  fseek(inf,4,SEEK_SET);
  fread(&total_muicons,4,1,inf);

  //use only first two icons
  total_muicons=2;
  // read addresses
  for (i=0; i<total_muicons; ++i)
  {
    fseek(inf,8+8*i,SEEK_SET);
    fread(&(icon_addr[i]),4,1,inf);
  }
  //create the bitmap
  for (i=0; i<total_muicons; ++i)
  {
   munit_bmp[i]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
   if (!munit_bmp[i])
   {
     printf("\nCould not allocate strategic map icon bitmap %d\n",i);
     return ERROR_MINIUNIT_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
   }

   // zero out the bitmap with the transparent color
   for (y=0; y<TILE_HEIGHT_; ++y)
     for (x=0; x<TILE_FULL_WIDTH_; ++x)
       putpixel(munit_bmp[i],x,y,fpge_mask_color);
    //read in the data from the shp file
   read_header(inf,icon_addr[i]);
   read_shp(munit_bmp[i],inf,icon_addr[i]);
  }
  fclose(inf);
  return 0;
}

int build_mini_vichex(){
	int x,y;
	total_mflags=2;
	//cross
	mini_victory_hex_bmp[0]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	mini_victory_hex_bmp[1]=create_bitmap(TILE_FULL_WIDTH_,TILE_HEIGHT_);
	for (y=0; y<TILE_HEIGHT_; ++y)
	     for (x=0; x<TILE_FULL_WIDTH_; ++x){
	       putpixel(mini_victory_hex_bmp[0],x,y,colors_to24bits(mm_white_city[8-y][x]));
	       putpixel(mini_victory_hex_bmp[1],x,y,colors_to24bits(mm_green_city[8-y][x]));
	     }
	return 0;
}

int load_mini_vichex() {
	FILE *inf;
	int i, x, y;
	char path[MAX_PATH];

	strncpy(path,mflag_file,MAX_PATH);
	canonicalize_filename(path,path,MAX_PATH);
	inf = fopen(path, "rb");
	if (!inf) {
		//allegro_exit();
		printf("\nCould not open victory hexes indicators file %s.\n",mflag_file);
		return ERROR_MINIVICTORY_BASE+ERROR_FPGE_FILE_NOT_FOUND;
	}
	fseek(inf, 4, SEEK_SET);
	fread(&total_mflags, 4, 1, inf);

	//use only first two indcators
	total_mflags = 2;
	// read addresses
	for (i = 0; i < total_mflags; ++i) {
		fseek(inf, 8 + 8 * i, SEEK_SET);
		fread(&(icon_addr[i]), 4, 1, inf);
	}
	//create the bitmap
	for (i = 0; i < total_mflags; ++i) {
		mini_victory_hex_bmp[i] = create_bitmap(TILE_FULL_WIDTH_, TILE_HEIGHT_);
		if (!mini_victory_hex_bmp[i]) {
			allegro_exit();
			printf("\nCould not allocate victory hexes indicators bitmap %d\n",
					i);
			return ERROR_MINIVICTORY_BASE+ERROR_FPGE_CANNOT_ALLOCATE_MEM;
		}

		// zero out the bitmap with the transparent color
		for (y = 0; y < TILE_HEIGHT_; ++y)
			for (x = 0; x < TILE_FULL_WIDTH_; ++x)
				putpixel(mini_victory_hex_bmp[i], x, y, fpge_mask_color);
		//read in the data from the shp file
		read_header(inf, icon_addr[i]);
		read_shp(mini_victory_hex_bmp[i], inf, icon_addr[i]);

	}
	fclose(inf);
	return 0;
}

int is_ally(unsigned char own) {
	int i, found = 0;
	for (i = 0; i < 6; i++)
		if ((scn_buffer[i * 2 + 1] & 255) == own)
			found = 1;
	return found;
}

void draw_mmap(BITMAP *map_to_draw,int x0, int y0, int tiles_high, int tiles_wide, int show_air, int show_vic, int show_shade, int show_units)
{
	int x,y,xs,ys,idx;
	BITMAP *tile_to_draw;

    //top row
    rectfill(map_to_draw,0,0,map_to_draw->w,TILE_HEIGHT_/2,MAP_COLOR);
    //bottom row
    rectfill(map_to_draw,0,map_to_draw->h-TILE_HEIGHT_/2,map_to_draw->w,map_to_draw->h,MAP_COLOR);
    //left column
    rectfill(map_to_draw,0,0,map_to_draw->w+TILE_WIDTH_/2,map_to_draw->h,MAP_COLOR);
    //right column
    rectfill(map_to_draw,map_to_draw->w-TILE_WIDTH_/2,map_to_draw->w,map_to_draw->w,map_to_draw->h,MAP_COLOR);

	for (y=y0-1; y<y0+tiles_high+1; ++y)
    for (x=x0-1; x<x0+tiles_wide+1; ++x)
    {
      if (x>=0 && x<mapx && y>=0 && y<mapy)
      {
        xs=(x-x0)*TILE_WIDTH_-3;
        ys=(y-y0)*TILE_HEIGHT_-4;
        if ((x)%2) ys+=TILE_HEIGHT_/2;
        if ((map[x][y].shade&SHADE_MASK) && show_shade ){
        				switch(showWeather){
        					case 0:tile_to_draw = dark_mtil_bmp[map[x][y].tile]; break;
        					case 1:tile_to_draw = dark_mtil_bmp_mud[map[x][y].tile];break;
        					case 2:tile_to_draw = dark_mtil_bmp_snow[map[x][y].tile];break;
        				}
        			}else{
        				switch(showWeather){
        					case 0:tile_to_draw = mtil_bmp[map[x][y].tile];break;
        					case 1:tile_to_draw = mtil_bmp_mud[map[x][y].tile];break;
        					case 2:tile_to_draw = mtil_bmp_snow[map[x][y].tile];break;
        				}
        			}

        //if ((x+y)%4==0)
        //if (x==4 && y==4)
        	//masked_blit(tile_to_draw,map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH_,TILE_HEIGHT_);
        draw_sprite(map_to_draw,tile_to_draw,xs,ys);

        //if (x==10 && y==8) draw_sprite(map_to_draw,mtil_bmp[0],xs,ys);
		//if (show_shade && map[x][y].shade&SHADE_MASK)
		//	masked_blit(dark_mtil_bmp[map[x][y].tile],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH_,TILE_HEIGHT_);
		
		if (map[x][y].side==3 && show_vic)
			masked_blit(dark_mtil_bmp[map[x][y].tile],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH_,TILE_HEIGHT_);
		
		if (map[x][y].vic && show_vic)
			draw_sprite(map_to_draw,mini_victory_hex_bmp[is_ally(map[x][y].own)?1:0],xs,ys);
		
		if (map[x][y].guidx>=0 && (show_air==0 || displayAllUnits))
			if (show_units==0 || (show_units==1 && isNotAllied(all_units[map[x][y].guidx].uflag)) ||
			   (show_units==2 && isAllied(all_units[map[x][y].guidx].uflag))|| displayAllUnits ){
					idx=all_units[map[x][y].guidx].uflag==allied_core?1:0;
					if (swap_indicators) idx=(idx+1)%2;
					draw_sprite(map_to_draw,munit_bmp[idx],xs,ys);
				}
		if (map[x][y].auidx>=0 && (show_air==1 || displayAllUnits))
			if (show_units==0 || (show_units==1 && isNotAllied(all_units[map[x][y].auidx].uflag)) ||
			   (show_units==2 && isAllied(all_units[map[x][y].auidx].uflag))|| displayAllUnits ){
					idx=all_units[map[x][y].auidx].uflag==allied_core?1:0;
					if (swap_indicators) idx=(idx+1)%2;
					draw_sprite(map_to_draw,munit_bmp[idx],xs,ys);
				}
		}
	}	
}

void draw_medmap(BITMAP *map_to_draw,int x0, int y0, int tiles_high, int tiles_wide, int show_air, int show_vic, int show_shade, int show_units)
{
	int x,y,xs,ys;
	BITMAP *tile_to_draw;

	int bmp_tile_x=TILE_WIDTH_*2;
	int bmp_tile_full_x=TILE_FULL_WIDTH_*2;
	int bmp_tile_y=TILE_HEIGHT_*2;

    //top row
    rectfill(map_to_draw,0,0,map_to_draw->w,bmp_tile_x/2,MAP_COLOR);
    //bottom row
    rectfill(map_to_draw,0,map_to_draw->h-bmp_tile_x/2,map_to_draw->w,map_to_draw->h,MAP_COLOR);
    //left column
    rectfill(map_to_draw,0,0,map_to_draw->w+bmp_tile_y/2,map_to_draw->h,MAP_COLOR);
    //right column
    rectfill(map_to_draw,map_to_draw->w-bmp_tile_y/2,map_to_draw->w,map_to_draw->w,map_to_draw->h,MAP_COLOR);

	for (y=y0-1; y<y0+tiles_high+1; ++y)
    for (x=x0-1; x<x0+tiles_wide+1; ++x)
    {
      if (x>=0 && x<mapx && y>=0 && y<mapy)
      {
        xs=(x-x0)*bmp_tile_x-3*2;
        ys=(y-y0)*bmp_tile_y-4*2;
        if ((x)%2) ys+=bmp_tile_y/2;

        if ((map[x][y].shade&SHADE_MASK) && show_shade ){
        				switch(showWeather){
        					case 0:tile_to_draw = dark_til_bmp[map[x][y].tile]; break;
        					case 1:tile_to_draw = dark_til_bmp_mud[map[x][y].tile];break;
        					case 2:tile_to_draw = dark_til_bmp_snow[map[x][y].tile];break;
        					default:tile_to_draw =0;break;
        				}
        			}else{
        				switch(showWeather){
        					case 0:tile_to_draw = til_bmp[map[x][y].tile];break;
        					case 1:tile_to_draw = til_bmp_mud[map[x][y].tile];break;
        					case 2:tile_to_draw = til_bmp_snow[map[x][y].tile];break;
        					//should not happen
        					default:tile_to_draw = 0;break;
        				}
        			}
        masked_stretch_blit( tile_to_draw,  map_to_draw,
        		0, 0,
        		tile_to_draw->w, tile_to_draw->h,
        		xs, ys,
        		bmp_tile_full_x, bmp_tile_y);

        //draw_sprite(map_to_draw,tile_to_draw,xs,ys);
      }
    }
//		if (map[x][y].side==3 && show_vic)
//			draw_sprite(map_to_draw,dark_mtil_bmp[map[x][y].tile],xs,ys);
		//masked_blit(dark_mtil_bmp[map[x][y].tile],map_to_draw,0,0,xs,ys,TILE_FULL_WIDTH_,TILE_HEIGHT_);

//		if (map[x][y].vic && show_vic)
//			draw_sprite(map_to_draw,mflag_bmp[is_ally(map[x][y].own)?1:0],xs,ys);

  	for (y=y0-1; y<y0+tiles_high+1; ++y)
      for (x=x0-1; x<x0+tiles_wide+1; ++x)
      {
        if (x>=0 && x<mapx && y>=0 && y<mapy)
        {
          xs=(x-x0)*bmp_tile_x-3*2;
          ys=(y-y0)*bmp_tile_y-4*2;
          if ((x)%2) ys+=bmp_tile_y/2;

		if (map[x][y].guidx>=0 && (show_air==0 || displayAllUnits))
			if (show_units==0 || (show_units==1 && isNotAllied(all_units[map[x][y].guidx].uflag)) || (show_units==2 && isAllied(all_units[map[x][y].guidx].uflag)) )
				//draw_sprite(map_to_draw,flag_bmp[all_units[map[x][y].guidx].country],xs,ys);
        masked_blit( flag_bmp[all_units[map[x][y].guidx].country-1],  map_to_draw,
        		20, 36, xs, ys, 19, 12);

		if (map[x][y].auidx>=0 && (show_air==1 || displayAllUnits))
			if (show_units==0 || (show_units==1 && isNotAllied(all_units[map[x][y].auidx].uflag)) || (show_units==2 && isAllied(all_units[map[x][y].auidx].uflag)) )
				//draw_sprite(map_to_draw,flag_bmp[all_units[map[x][y].auidx].country],xs,ys);
		        masked_blit( flag_bmp[all_units[map[x][y].auidx].country-1],  map_to_draw,
		        		20, 36, xs, ys, 19, 12);
		}
	}

}

void set_shade(int x, int y, int value){
	if (x<0 || x>mapx || y<0 || y> mapy) return;
	if (value)	map[x][y].shade |= SHADE_MASK;
	else map[x][y].shade &= ~SHADE_MASK;
	return;
}

void set_in_radius(int x, int y, int r, int value)
{	
	int i,j;

	if (r<1) return;
	for (i=0;i<=r;i++)
		for (j=-r+i/2;j<=r-(i+1)/2;j++){
			set_shade(x+i,y+j+x%2*(x+i+1)%2,value);
			set_shade(x-i,y+j+x%2*(x+i+1)%2,value);
		}
	
}

int isOwnerRight(enum UTYPES checkMe, int isAlliedExpected){
	/*
	if (isAlliedExpected == 0) {
		if (checkMe != allied_core)
			return 1;
		else
			return 0;
	}
	if (isAlliedExpected == 1) {
		if (checkMe == allied_core)
			return 1;
		else
			return 0;
	}
	*/
	if (isAlliedExpected && (checkMe==allied_core || checkMe==allied_aux) ) return 1;
	if (!isAlliedExpected && (checkMe==axis_core || checkMe==axis_aux) ) return 1;
	return 0;
}

void compute_shade(int side){
	int x,y,guidx,auidx;
	unsigned char r=0;
	//int side_enum;

	//enum UTYPES uflag=core;
	for (x=0;x<mapx;x++)
		for (y=0;y<mapy;y++)
			set_shade(x,y,SHADE_MASK); //all not visible
	
	/*switch(side){
		case 0: uflag=aux; break;
		case 1: uflag=allied; break;
	}*/
	//check cites and units
	for (x=0;x<mapx;x++)
	for (y=0;y<mapy;y++){
		//side = city
		if (map[x][y].side==side && map[x][y].own>0) set_in_radius(x,y,1,0);

		guidx=map[x][y].guidx;
		auidx=map[x][y].auidx;

		if ( guidx>=0 || auidx>=0 ){
		//ground
		r=0;
		if (guidx>=0 && isOwnerRight(all_units[guidx].uflag,side) ){
			if (all_units[guidx].auxtnum>0)
				r=equip[all_units[guidx].auxtnum][SPOTTING];
			else
				r=equip[all_units[guidx].unum][SPOTTING];
		}
		//air
		if (auidx>=0 &&  isOwnerRight(all_units[auidx].uflag,side) )
			r=equip[all_units[auidx].unum][SPOTTING];

		set_in_radius(x,y,r,0);
		}
		}
		
}

void make_mmap(int show_air, int show_vic, int show_shade, int show_units, int map_size) {

	char path[MAX_PATH];

	//set_color_mode();
	set_pal(); //we need this for 256 mode
	/*
	set_weather_palette();
	set_color_depth(8);
	set_color_conversion(COLORCONV_REDUCE_TO_256);

	//WARNING set_pallete(pgpal) in COMMENTS
	*/
	// set the color pallete 
	//set_pallete(pgpal);
	if (show_shade != 3)
		compute_shade(show_shade - 1);

	if (map_size==0){
		sprintf(path, ".\\mmap%03d.bmp", getScenarioNumber());
		canonicalize_filename(path,path,MAX_PATH);
		save_map_bmp = create_bitmap( mapx * TILE_WIDTH_ + 4 - 6, mapy * TILE_HEIGHT_ + TILE_HEIGHT_ / 2 - 8);
		draw_mmap(save_map_bmp, 0, 0, mapy, mapx, show_air, show_vic, show_shade!= 3 ? 1 : 0, show_units);
	}else{
		sprintf(path, ".\\midmap%03d.bmp", getScenarioNumber());
		canonicalize_filename(path,path,MAX_PATH);
		save_map_bmp = create_bitmap( mapx * TILE_WIDTH_*2 + (4-6)*2 , mapy * TILE_HEIGHT_*2 + TILE_HEIGHT_*2 / 2 - 8*2);
		draw_medmap(save_map_bmp, 0, 0, mapy, mapx, show_air, show_vic, show_shade!= 3 ? 1 : 0, show_units);
	}
	if (save_bmp(path, save_map_bmp, pgpal)) {
		printf("ERROR: Cannot save BMP file. File cannot be saved.\n");
	}
	destroy_bitmap(save_map_bmp);
}




