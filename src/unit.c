/* Unit.c the unit selection dialog called when the Unit */
/* button of FPGE is presses                             */
#include <stdio.h>
#include <string.h>
#include <allegro.h>

#include "fpge.h"
#include "unit.h"
#include "tables.h"
#include "fpge_colors.h"
#include "load.h"
#include "app6.h"

#define UNIT_SCREEN_COLOR GUI_BG_COLOR
#define UNIT_FG_COLOR GUI_FG_COLOR
#define UNIT_BG_COLOR GUI_BG_COLOR
#define EDIT_BG_COLOR GUI_EDIT_COLOR
#define EDIT_FG_COLOR GUI_FG_COLOR

#define UNIT_TYPE_RB_X 5
#define UNIT_TYPE_RB_Y 10
#define UNIT_TYPE_RB_SPACE 20

#define SBOX_X (420+8*3)
#define SBOX_Y 15
#define SBOX_H 150
#define SBOX_W (140+8*5)

#define BUTTON_LINE (480-BUTTON_H*3-5)

#define RADIO_W 8*18
#define RADIO_H 16

#define AIR_CARRIER_TYPE 16
#define SEA_CARRIER_TYPE 17
#define TRUCK_TYPE 15

/* variables for unit.c */
char *unames[MAX_UNITS/2];  //names for list box
short unumbers[MAX_UNITS/2]; //unit numbers for items in listbox

/* declaration of our functions */
int d_setbtn_proc(int,DIALOG *,int);
int d_ulist_proc(int,DIALOG *,int);
int d_uradio_proc(int,DIALOG *,int);
int d_sbox_proc(int,DIALOG *,int);
int d_dradio_proc(int,DIALOG *,int);
char *add_units(int idx,int *list_size);
char *add_countries(int idx,int *list_size);
//int d_unumedit_proc(int,DIALOG *,int);
//int d_tnumedit_proc(int,DIALOG *,int);
//int d_cnumedit_proc(int,DIALOG *,int);
int d_clist_proc(int,DIALOG *,int);
//int d_atransedit_proc(int,DIALOG *,int);
int d_ucheck_proc(int,DIALOG *,int);
int d_er_check_proc(int,DIALOG *,int);

int first_display=0;

unsigned char scndlg_countries[MAX_COUNTRY];
unsigned char scndlg_countries_no;

/* here we define the unit dialog components */
DIALOG unit_dlg[UNIT_DLG_SIZE] = {
//item 0 the clear routine
		{ d_my_textbox_proc, 0, 0, 640, 480, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 0, 0, 0, 0, 0 },
		//item 1 the done button
		{ d_button_proc, SBOX_X+SBOX_W-U_BUTTON_W, BUTTON_LINE, U_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 'd', D_EXIT, 0, 0, (void *) "&Done", 0, 0 },
		//item 2 the set button
		{ d_setbtn_proc, SBOX_X+SBOX_W-U_BUTTON_W, BUTTON_LINE-25, U_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 's', 0, 0, 0, (void *) "&Set", 0, 0 },
		//item 3 the 1st radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 1 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'i', D_SELECTED, 1, 0, (void *) "&Infantry", 0, 0 },
		//item 4 the 2nd radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 2 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 't', 0, 1, 0, (void *) "&Tank", 0, 0 },
		//item 5 the 3rd radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 3 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'r', 0, 1, 0, (void *) "&Recon", 0, 0 },
		//item 6 the 4th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 4 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Anti-Tank", 0, 0 },
		//item 7 the 5th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 5 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'y', 0, 1, 0, (void *) "Artillery", 0, 0 },
		//item 8 the 6th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 6 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Anti-Aircraft", 0, 0 },
		//item 9 the 7th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 7 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Air Defense", 0, 0 },
		//item 10 the 8th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 8 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Fortification", 0, 0 },
		//item 11 the 9th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 9 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'f', 0, 1, 0, (void *) "&Fighter", 0, 0 },
		//item 12 the 10th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 10 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'b', 0, 1, 0, (void *) "Tactical &Bomber", 0, 0 },
		//item 13 the 11th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 11 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'l', 0, 1, 0, (void *) "&Level Bomber", 0, 0 },
		//item 14 the 12th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 12 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'u', 0, 1, 0, (void *) "S&ubmarine", 0, 0 },
		//item 15 the 13th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 13 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'e', 0, 1, 0, (void *) "D&estroyer", 0, 0 },
		//item 16 the 14th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 14 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'c', 0, 1, 0, (void *) "&Capital Ship", 0, 0 },
		//item 17 the 15th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 15 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Aircraft Carrier", 0, 0 },
		//item 18 the 16th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 16 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Land Transport", 0, 0 },
		//item 19 the 17th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 17 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Air Transport", 0, 0 },
		//item 20 the 18th radio button of group 1
		{ d_uradio_proc, UNIT_TYPE_RB_X, 18 * UNIT_TYPE_RB_SPACE, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Sea Transport", 0, 0 },
		//item 21 the list of appropriate units
		{ d_ulist_proc, UNIT_TYPE_RB_X + 200, SBOX_Y, 180, 302, UNIT_FG_COLOR, UNIT_BG_COLOR, 0, 0, 0, 0, (void *) add_units, 0, 0 },
		//item 22 the box for selections
		{ d_sbox_proc, SBOX_X, SBOX_Y, SBOX_W, SBOX_H, UNIT_FG_COLOR, UNIT_BG_COLOR, 0, 0, 0, 0, 0, 0, 0 },
		//item 23 an edit box  for the unit number
		{ d_edit_proc, SBOX_X + 10, SBOX_Y + 28, 36 + 8, 15, EDIT_FG_COLOR, EDIT_BG_COLOR,0, 0, 4, 0, 0, 0, 0 }, //initialize on starting
		//item 24 an edit box  for the transport number
		{ d_edit_proc, SBOX_X + 10, SBOX_Y + 64, 36 + 8, 15, EDIT_FG_COLOR, EDIT_BG_COLOR,0, 0, 4, 0, 0, 0, 0 }, //initialize on starting
		//item 25 an edit box  for the country number
		{ d_edit_proc, SBOX_X + 10, SBOX_Y + 100, 36 + 8, 15, EDIT_FG_COLOR, EDIT_BG_COLOR,0, 0, 4, 0, 0, 0, 0 }, //initialize on starting
		//item 26 the country list box
		{ d_clist_proc, SBOX_X, SBOX_Y + 160, SBOX_W, 220, UNIT_FG_COLOR, UNIT_BG_COLOR,3, 0, 0, 0, (void *) add_countries, 0, 0 }, //initialize on starting
		//item 27 the auxtransport edit
		{ d_edit_proc, SBOX_X + 10, SBOX_Y + 125 + 12, 36 + 8, 15, EDIT_FG_COLOR, EDIT_BG_COLOR,0, 0, 4, 0, 0, 0, 0 }, //initialize on starting
		// item 28 a RB to select all units display the default udAllUnitIdx
		{ d_dradio_proc, UNIT_TYPE_RB_X + 200, 340, 8 * 10 + 4, 16, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'a', D_SELECTED, 100, 0, (void *) "&All Units", 0, 0 },
		// item 29 a RB to display date sensitive
		{ d_dradio_proc, UNIT_TYPE_RB_X + 295, 340, 8 * 8 + 4, 16, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 100, 0, (void *) "By Date", 0, 0 },
		//item 30
		{ d_ucheck_proc, UNIT_TYPE_RB_X + 200, 340 - 16, 8 * 24 + 4, 16, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "Scenario countries only", 0, 0 },
		//item 31
		{ d_er_check_proc, UNIT_TYPE_RB_X, 480 - 8 - RADIO_H, RADIO_W, RADIO_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 1, 0, (void *) "exclude RESERVED", 0, 0 },
		//32
		{ d_button_proc, SBOX_X+SBOX_W-U_BUTTON_W, BUTTON_LINE + 25, U_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 'c', D_EXIT, 0, 0, (void *) "&Cancel", 0, 0 },
		{ d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		// last item
		{ NULL } };

int scenario_units_only_filter=0;
int exclude_reserved_units_filter=0;

void display_unit_with_flag(int idx){
	int c;

	display_unit_info(idx,unit_dlg[0].x+INFO_X,unit_dlg[0].y+INFO_Y,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_SCREEN_COLOR));
//return;
	//flag
	c=equip_country[idx]-1;

	//printf("c=%d idx=%d\n",c,idx);
	if (pgf_mode || pacgen_mode){
		if (  c>=0 && c<total_flags && flag_bmp[c]!=NULL){
			masked_blit(flag_bmp[c],
					screen,
					0, 0,
					unit_dlg[0].x+INFO_X-70,
					unit_dlg[0].y+INFO_Y+2*INFO_SPACE+5,
					60,
					50);
		}
		else{
			rectfill(screen,
					unit_dlg[0].x+INFO_X+ 20-70,
					unit_dlg[0].y+INFO_Y+ 36+2*INFO_SPACE+5,
					unit_dlg[0].x+INFO_X+ 20+20-70,
					unit_dlg[0].y+INFO_Y+ 36+2*INFO_SPACE+5+15,
					colors_to24bits(UNIT_SCREEN_COLOR));

			if (c==-2){
				textout_ex(screen,font,"ALL",
						unit_dlg[0].x+INFO_X+ 20-70,
						unit_dlg[0].y+INFO_Y+ 36+2*INFO_SPACE+5,
						colors_to24bits(UNIT_FG_COLOR),
						colors_to24bits(UNIT_SCREEN_COLOR));
			}
		}
	}
	if (draw_app6 && pgf_mode && draw_app6_units)
		draw_app6_unit_symbol(screen,
			unit_dlg[0].x+INFO_X+ 20-70-20,
			unit_dlg[0].y+INFO_Y+ 36+2*INFO_SPACE+8,
			idx,0);

}


int d_my_textbox_proc(int msg,DIALOG *d,int c){
	if (msg==MSG_START){

		//set strings
		unit_dlg[23].dp=uselected;
		unit_dlg[24].dp=tselected;
		unit_dlg[25].dp=cselected;
		unit_dlg[27].dp=aselected;

		if (nupl_present)
			unit_dlg[udScenarioCountryUnitsOnlyIdx].flags&=~D_HIDDEN;
		else
			unit_dlg[udScenarioCountryUnitsOnlyIdx].flags|=D_HIDDEN;
		//copy to controls
		sprintf(uselected,"%d",unum_selected);
	    sprintf(tselected,"%d",tnum_selected);
	    sprintf(cselected,"%d",cnum_selected);
	    sprintf(aselected,"%d",anum_selected);
	    //
	    first_display=0;
	}
	if (first_display==0 && msg==MSG_IDLE ){
		first_display=1;
		display_unit_with_flag(unumbers[0]);
	}
	return d_textbox_proc(msg,d,c);
}


int d_setbtn_proc(int msg,DIALOG *d,int c)
{
	if ((msg==MSG_CLICK)||(msg==MSG_KEY))
    {
       d_button_proc(msg,d,c);
       sprintf(uselected,"%d",unum_selected);
       sprintf(tselected,"%d",tnum_selected);
       sprintf(cselected,"%d",cnum_selected);
       sprintf(aselected,"%d",anum_selected);
       d->flags^=D_SELECTED;

       ustr_selected=10;
       uexp_selected=0;
       uent_selected=0;

       unit_dlg[22].flags |= D_DIRTY;
       unit_dlg[23].flags |= D_DIRTY;
       unit_dlg[24].flags |= D_DIRTY;
       unit_dlg[25].flags |= D_DIRTY;
       unit_dlg[27].flags |= D_DIRTY;

       //broadcast_dialog_message(MSG_DRAW,0);
       return D_O_K;
    }
    return d_button_proc(msg,d,c);
}

void display_unit_info(int idx, int x0, int y0/*, int dx, int dy*/, int fg, int bg)
{
  char str[64];
  int bmp_num,x,y;

  //clear the info area
  x = x0;
  y = y0;
  rectfill(screen,x,y,x+INFO_W,y+INFO_H,bg);
  //text_mode(-1);
  //sprintf(str,"Unit: #%3d %-16s",idx,&(equip[idx][NAME]) );
  sprintf(str,"Unit: #%3d %-16s",idx,equip_name_utf8[idx] );
  textout_ex(screen,font,str,x,y+0*INFO_SPACE,fg,bg);
  sprintf(str,"HA:%2d  SA:%2d  AA:%2d NA:%2d",equip[idx][HA],equip[idx][SA],equip[idx][AA],equip[idx][NA]);
  textout_ex(screen,font,str,x,y+1*INFO_SPACE,fg,bg);
  sprintf(str,"GD:%2d  CD:%2d  AD:%2d",equip[idx][GD],equip[idx][CD],equip[idx][AD]);
  textout_ex(screen,font,str,x,y+2*INFO_SPACE,fg,bg);
  sprintf(str,"INI:%2d  RNG:%2d  SPT:%2d", equip[idx][INITIATIVE],equip[idx][RANGE],equip[idx][SPOTTING]);
  textout_ex(screen,font,str,x,y+3*INFO_SPACE,fg,bg);
  sprintf(str,"FUEL:%3d AMMO:%2d COST:%3d",equip[idx][FUEL],equip[idx][AMMO],equip[idx][COST]*COST_DIVISOR);
  textout_ex(screen,font,str,x,y+4*INFO_SPACE,fg,bg);

  sprintf(str,"MOV:%2d %s ",
    equip[idx][MOV],movement_type[equip[idx][MOV_TYPE]]);
  if (equip[idx][AAF]==0 && equip[idx][CLASS]==CLASS_TACTICAL_BOMBER) //bomber that can open fire
    strncat(str,"Fighter/Bomber",64);
  textout_ex(screen,font,str,x,y+5*INFO_SPACE,fg,bg);
  sprintf(str,"Avail:%02d/%02d -> %d",equip[idx][MON],equip[idx][YR],1900+equip[idx][LAST_YEAR]);
  textout_ex(screen,font,str,x,y+6*INFO_SPACE,fg,bg);
  sprintf(str,"Type:%s",target_type_name[equip[idx][TARGET_TYPE]]);
  textout_ex(screen,font,str,x+20*8,y+6*INFO_SPACE,fg,bg);


  //if (equip_flags[idx]&EQIPMENT_HARDCODED){
	  strncpy(str,"",64);
	  //sprintf(str,"%d",equip[idx][BMP]);
	  if (equip_flags[idx]&EQUIPMENT_CAN_BRIDGE_RIVERS) strncat(str,"River ",64);
	  if (equip_flags[idx]&EQUIPMENT_IGNORES_ENTRENCHMENT) strncat(str,"Rugged ",64);
	  if (equip_flags[idx]&EQUIPMENT_JET) strncat(str,"Jet ",64);
	  textout_ex(screen,font,str,x,y+7*INFO_SPACE,fg,bg);
  //}
  //text_mode(0);

  rectfill(screen,x-70,y+2*INFO_SPACE,x-70+60,y+2*INFO_SPACE+50,bg);
  bmp_num=equip[idx][BMP]+256*equip[idx][BMP+1];
  masked_blit(unit_bmp[bmp_num],screen,0,0,x-70,y+2*INFO_SPACE,60,50);
}

int d_ulist_proc(int msg, DIALOG *d, int c)
{
	int d1;

    if ((msg==MSG_CLICK)||(msg==MSG_CHAR))
    {
       d1=d->d1;
       d_list_proc(msg,d,c);
       //display_unit_info(unumbers[d->d1],ud[0].x+INFO_X,ud[0].y+INFO_Y,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_SCREEN_COLOR));
       display_unit_with_flag(unumbers[d->d1]);

       if (unit_type_selected==TRUCK_TYPE)
         tnum_selected=unumbers[d->d1];
       else if (unit_type_selected==SEA_CARRIER_TYPE || unit_type_selected==AIR_CARRIER_TYPE)
         anum_selected=unumbers[d->d1];
       else
         unum_selected=unumbers[d->d1];

       if (msg==MSG_CLICK) return D_O_K;
	   if (msg == MSG_CHAR){
			if (d1 !=d->d1)
				return D_USED_CHAR;
			else
				return D_O_K;
		}
	   
    }
    return d_list_proc(msg,d,c);
}

int can_display(int idx, int class){
	int i,k,cursor,nupl_country_idx,nupl_class_idx;

	if (pgf_mode || pacgen_mode){
		if (equip_country[idx]==-1) return 1; //hmm, is this ok
		for(i=0;i<12;i++) //all axis and allies countries
			if (scn_buffer[i]>0 && equip_country[idx]==scn_buffer[i]) return 1;
		//not found
		return 0;
	}else{
	if (nupl_present){
		if (class>10) return 1; //hmm, is this ok ?
		for(i=0;i<12;i++) //all axis and allies countries in scenario
			if (scn_buffer[i]>0){ //scenario buffer first 12 bytes
				nupl_class_idx=-1;
				for(k=0;k<NUPL_CLASS_IDX;k++)
					if (nupl_class[k]==class) { nupl_class_idx=k; break; }

				nupl_country_idx=-1;
				for(k=0;k<NUPL_COUNTRY_IDX;k++)
					if (nupl_countries[k]==scn_buffer[i]) { nupl_country_idx=k; break; }

				if (nupl_country_idx ==-1 || nupl_class_idx==-1 ) continue;

				cursor=nupl_cc[nupl_country_idx][nupl_class_idx];
				while (nupl[cursor]!= -1){
					if (nupl[cursor]==idx) return 1;
					cursor++;
				}
			}
		return 0;
	}
	}
	return 1;
}

char *add_units(int idx,int *list_size)   //function to add units to listbox
{
   int i,number,first_date,last_date,this_date;
   //static int next_index; //this is where to start search next time
   static char nstr[]="None";
   char lower_case_name[256];

   if (idx<0) //wants the size)
   {
      for (i=3; i<21; ++i)
        if (unit_dlg[i].flags&D_SELECTED) unit_type_selected=i-3;

      //get the date of the scenario
      this_date=12*scn_buffer[YEAR]+scn_buffer[MONTH]-1;
      //search the equipment data base for entries of this type
      number=0;

      for (i=0; i<total_equip; ++i)
      {
         if ((unit_type_selected==TRUCK_TYPE||
              unit_type_selected==AIR_CARRIER_TYPE||
              unit_type_selected==SEA_CARRIER_TYPE) && i==0)
         {
            unames[0]=nstr;
            unumbers[number]=0;
            ++number;
         }

         if (equip[i][CLASS]==unit_type_selected )
         {
        	if (exclude_reserved_units_filter) {
        		strncpy(lower_case_name,equip_name_utf8[i],256);
        		strlwr(lower_case_name);
        		if (strcmp(lower_case_name,"reserved")==0){
        			continue;
        		}
        	}

            //is correct type...is it in date
            first_date=12*equip[i][YR]+equip[i][MON]-1;
            last_date=12*equip[i][LAST_YEAR]+12;   //end of obsolete year
            if ((this_date>=first_date&&this_date<=last_date)
                || (unit_dlg[udAllUnitIdx].flags&D_SELECTED))
            {
              //unames[number]=(char*)&(equip[i][NAME]);
              //if ((ud[udScenarioCountryUnitsOnlyIdx].flags&D_SELECTED && can_display(i, unit_type_selected))
			///	  || ~ud[udScenarioCountryUnitsOnlyIdx].flags&D_SELECTED)
                if ((scenario_units_only_filter && can_display(i, unit_type_selected)) || scenario_units_only_filter==0)
              {
						unames[number] = equip_name_utf8[i];
						unumbers[number] = i;
						++number;
					}
            }
         }
      }
      *list_size=number;
      return 0;
   }
   return unames[idx];
}

int d_uradio_proc(int msg,DIALOG *d,int c)
{
   if (msg==MSG_CLICK || msg==MSG_KEY)
   {
      d_radio_proc(msg,d,c);
      unit_dlg[udListBoxIdx].d1=-1;
      unit_dlg[udListBoxIdx].d2=0;
      unit_dlg[udListBoxIdx].flags |= D_DIRTY;
      //broadcast_dialog_message(MSG_DRAW,c);
      return D_O_K;
   }
   return d_radio_proc(msg,d,c);
}

int d_dradio_proc(int msg,DIALOG *d,int c)
{
   if (msg==MSG_CLICK || msg==MSG_KEY)
   {
      d_radio_proc(msg,d,c);
      unit_dlg[udListBoxIdx].d1=-1;
      unit_dlg[udListBoxIdx].d2=0;
      unit_dlg[udListBoxIdx].flags |= D_DIRTY;
      //broadcast_dialog_message(MSG_DRAW,c);
      return D_O_K;
   }
   return d_radio_proc(msg,d,c);
}

int d_sbox_proc(int msg,DIALOG *d,int c)
{
   int temp,x,y;
   char tmp[64];

   if (msg==MSG_DRAW)
   {
      d_box_proc(msg,d,c);
	  x = unit_dlg[0].x + SBOX_X;
	  y = unit_dlg[0].y + SBOX_Y;
	  //text_mode(-1);
      textout_ex(screen,font,"Selected Unit",x+4,y+4,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_BG_COLOR));
     // textout(screen,font,(const char*)&(equip[unum_selected][NAME]),
      if (unum_selected>0)
    	  strncpy(tmp,equip_name_utf8[unum_selected],64);
      else
    	  strncpy(tmp,"          ",64);
      textout_ex(screen,font,tmp,x+4,y+16,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_BG_COLOR));
      textout_ex(screen,font,"Transport",x+4,y+40,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_BG_COLOR));

     // textout(screen,font,(const char*)&(equip[tnum_selected][NAME]),
      if (tnum_selected>0)
    	  strncpy(tmp,equip_name_utf8[tnum_selected],64);
      else
    	  strncpy(tmp,"          ",64);

      textout_ex(screen,font,tmp,x+4,y+52,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_BG_COLOR));

      textout_ex(screen,font,"Country",x+4,y+76,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_BG_COLOR));
      temp=atoi(cselected);
      textout_ex(screen,font,country_names[temp],x+4,y+88,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_BG_COLOR));
      textout_ex(screen,font,"Aux. Transport",x+4,y+112,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_BG_COLOR));
      if (anum_selected>0)
    	  strncpy(tmp,equip_name_utf8[anum_selected],64);
      else
    	  strncpy(tmp,"          ",64);
      textout_ex(screen,font,tmp,x+4,y+112+12,colors_to24bits(UNIT_FG_COLOR),colors_to24bits(UNIT_BG_COLOR));

      // text_mode(0);
      return D_O_K;
   }
   return d_box_proc(msg,d,c);
}

int d_clist_proc(int msg, DIALOG *d, int c)
{
   if (msg==MSG_CLICK || msg==MSG_KEY)
   {
      d_list_proc(msg,d,c);
      cnum_selected=get_country_from_current_list(d->d1);

      return D_O_K;
   }
   return d_list_proc(msg,d,c);
}

int get_country_from_current_list(int idx){
	return scndlg_countries[idx];
}

void make_countries_table(int filter_type){
	int i,j;
	scndlg_countries_no=0;
	for(i=0;i<MAX_COUNTRY;i++)
		if (country_active[i] || i==0){
			if (filter_type==FILTER_COUNTRIES_ALL){
				scndlg_countries[scndlg_countries_no]=i;
				scndlg_countries_no++;
			}
			if (filter_type==FILTER_COUNTRIES_SCENARIO_ONLY){
				for(j=0;j<12;j++)
					if (scn_buffer[j]==i && i!=0){
						scndlg_countries[scndlg_countries_no]=i;
						scndlg_countries_no++;
						break;
					}
			}
		}
}

int active_countries_number(){
	//I do not want to change scenario dialog and add new countries
	return 25;
	//return MAX_COUNTRY;
}

char *add_countries(int idx, int *list_size)
{
	/*
   if (idx<0) *list_size=active_countries_number();
   else return &(country_names[idx][0]);
   return 0;
   */
	   if (idx<0) *list_size=scndlg_countries_no;
	   else return &(country_names[scndlg_countries[idx]][0]);
	   return 0;


}

int d_ucheck_proc(int msg, DIALOG *d, int c)
{
	int result;

	if (msg==MSG_START && nupl_present){
		//printf("MSG_START %d\n",filter_units);
		if (scenario_units_only_filter)
			unit_dlg[udScenarioCountryUnitsOnlyIdx].flags|=D_SELECTED;
		else
			unit_dlg[udScenarioCountryUnitsOnlyIdx].flags&=~D_SELECTED;

		make_countries_table(scenario_units_only_filter?FILTER_COUNTRIES_SCENARIO_ONLY:FILTER_COUNTRIES_ALL);

	}

   if (msg==MSG_CLICK || msg==MSG_KEY)
   {
	   if (nupl_present){
		   result=d_check_proc(msg,d,c);
		   scenario_units_only_filter=unit_dlg[udScenarioCountryUnitsOnlyIdx].flags&D_SELECTED?1:0;
		   make_countries_table(scenario_units_only_filter?FILTER_COUNTRIES_SCENARIO_ONLY:FILTER_COUNTRIES_ALL);
		   //printf("MSG_CLICK %d\n",filter_units);
		   unit_dlg[udListBoxIdx].flags |= D_DIRTY;
		   //broadcast_dialog_message(MSG_DRAW,c);
		   return result;
	   }
   }
   return d_check_proc(msg,d,c);
}

int d_er_check_proc(int msg, DIALOG *d, int c)
{
	int result;

	if (msg==MSG_START){
		//printf("MSG_START %d\n",filter_units);
		if (exclude_reserved_units_filter)
			unit_dlg[udExcludeReservedIdx].flags|=D_SELECTED;
		else
			unit_dlg[udExcludeReservedIdx].flags&=~D_SELECTED;
	}

   if (msg==MSG_CLICK || msg==MSG_KEY)
   {
		   result=d_check_proc(msg,d,c);
		   exclude_reserved_units_filter=unit_dlg[udExcludeReservedIdx].flags&D_SELECTED?1:0;
		   //printf("MSG_CLICK %d\n",filter_units);
		   unit_dlg[udListBoxIdx].flags |= D_DIRTY;
		   //broadcast_dialog_message(MSG_DRAW,c);
		   return result;
   }
   return d_check_proc(msg,d,c);
}
