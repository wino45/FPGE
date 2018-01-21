/*
 * vc.c
 * PGF victory conditions dialog
 *  Created on: 2011-07-11
 *      Author: wino
 */
#include <allegro.h>
#include <stdio.h>
#include "vc.h"
#include "fpge.h"

char TurnsBeforeEndStr[128];
char CitesConqueredStr[128];
//char VP_non_active_Str[MAX_VICTORY_HEXES][64];
//char VP_active_Str[MAX_VICTORY_HEXES][64];
int VP_non_active_no;
int VP_active_no;
int VP_total_no;

int vc_save_data;

char ax_vc_str[] = "Axis Victory Conditions";
char al_vc_str[] = "Allied Victory Conditions";

char ParseVC[1024];

struct VP_points{
  char name[64];
  int active;
  int vic_no;
};

struct VP_points VP_non_active[MAX_VICTORY_HEXES];
//struct VP_points VP_active[MAX_VICTORY_HEXES];

#define LISTW (8*10)

DIALOG vc_dlg[VC_DLG] =
{
   /* (proc)           (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                         (dp2) (dp3) */
   { d_vc_clr_proc,     0,  0,  320+LISTW*2, 320-16, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   NULL,                        NULL, NULL },
   { d_button_proc,    336+LISTW-32-96, 336-64, 96,  16,  GUI_FG_COLOR,   GUI_BG_COLOR,   'd',    D_EXIT,      0,   0,   "&Done",                        NULL, NULL },
   { d_vc_cancelbtn_proc,    176-32-96, 336-64, 96,  16,  GUI_FG_COLOR,   GUI_BG_COLOR, 'c',    D_EXIT,      0,   0,   "&Cancel",                    NULL, NULL },
   { d_radio_proc,     112-96, 112-16-64, 256, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "Opposite side fails to win",    NULL, NULL },

   { d_radio_proc,     112-96, 112-64, 256, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "All VP",                    NULL, NULL },
   { d_edit_proc,      136-96, 136-64, 24,  16,  GUI_FG_COLOR, GUI_EDIT_COLOR,   0,    0,      2,   0,   "0",                        NULL, NULL },
   { d_text_proc,      168-96, 136-64, 200, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "Turns before scenario end", NULL, NULL },

   { d_radio_proc,     112-96, 152-64, 256, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "Some VP at scenario end",   NULL, NULL },
   { d_edit_proc,      136-96, 176-64, 24,  16,  GUI_FG_COLOR, GUI_EDIT_COLOR,   0,    0,      2,   0,   "0",                        NULL, NULL },
   { d_text_proc,      168-96, 176-64, 200, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "at least VP",               NULL, NULL },

   { d_text_proc,      272+LISTW-96, 192-64, 104, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "Mandatory VP",              NULL, NULL },
   { d_text_proc,      112-96, 192-64,       104, 16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "All VP",                    NULL, NULL },
   { d_list_proc,      112-96, 208-64, 120+LISTW, 112, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)vc_add_non_active,                        NULL, NULL },
   { d_move_to_active_proc,     232+LISTW-96, 232-64, 40,  16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "-->",                       NULL, NULL },
   { d_move_to_non_active_proc, 232+LISTW-96, 264-64, 40,  16,  GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "<--",                       NULL, NULL },
   { d_list_proc,      272+LISTW-96, 208-64, 120+LISTW, 112, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   (void *)vc_add_active,                        NULL, NULL },
   { d_text_proc,      120+LISTW-96, 80-8-64,  232, 8,   GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   "PGF Victory Conditions",    NULL, NULL },

   { d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
   { NULL,             0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                        NULL, NULL }
};

void update_active(){
	int i;

	 VP_non_active_no=0;
	 VP_active_no=0;
	 for(i=0;i<VP_total_no;i++)
		 if (VP_non_active[i].active==1)
			 VP_active_no++;
		 else
			 VP_non_active_no++;
}


int d_move_to_active_proc(int msg,DIALOG *d,int c){
	int i,cc=0,idx;
	if ((msg==MSG_CLICK)|(msg==MSG_KEY)){
		idx = vc_dlg[vcNonActiveList].d1;
		//printf("vcNonActiveList=%d\n",idx);

		//idx = vc_dlg[vcActiveList].d1;
		//printf("vcActiveList=%d\n",idx);

		if (idx < VP_non_active_no){
			 for (i = 0; i < VP_total_no; i++){
				 //count non-active
				 if (VP_non_active[i].active==0)
					 cc++;
				 if (idx==(cc-1))
					 VP_non_active[i].active=1;
			 }
		}
		update_active();
		vc_dlg[vcActiveList].flags |=D_DIRTY;
		vc_dlg[vcNonActiveList].flags |=D_DIRTY;
		//broadcast_dialog_message(MSG_DRAW,0);
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}

int d_move_to_non_active_proc(int msg,DIALOG *d,int c){
	int i,cc=0,idx;
	if ((msg==MSG_CLICK)|(msg==MSG_KEY)){
		idx = vc_dlg[vcActiveList].d1;

		if (idx < VP_active_no){
			 for (i = 0; i < VP_total_no; i++){
				 //count non-active
				 if (VP_non_active[i].active==1)
					 cc++;
				 if (idx==(cc-1))
					 VP_non_active[i].active=0;
			 }
		}
		update_active();
		vc_dlg[vcActiveList].flags |=D_DIRTY;
		vc_dlg[vcNonActiveList].flags |=D_DIRTY;
		//broadcast_dialog_message(MSG_DRAW,0);
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}

int is_vic_hex_valid(int idx){
	if (victory_hexes[idx].x > -1 &&
		victory_hexes[idx].x < mapx &&
		victory_hexes[idx].y > -1 &&
		victory_hexes[idx].y < mapy &&
		victory_hexes[idx].own > 0)
		return 1;
	return 0;
}

int d_vc_clr_proc(int msg,DIALOG *d,int c)
{
  int i,j,x,y;
  char buff[64],buff2[256],tmp[64];
  int vc_type, aux_info;
  char TokensStr[5][128];
  int TokensNo;
  char TokensVCStr[MAX_VICTORY_HEXES][128];
  int TokensVCNo;
  //initialize everything that needs it
  if (msg==MSG_START)
  {
	  vc_save_data=1; //default is to save

	  vc_dlg[vcTurnsBeforeEnd].dp = TurnsBeforeEndStr;
	  vc_dlg[vcCitesConquered].dp = CitesConqueredStr;

	  sprintf(TurnsBeforeEndStr,"%d",0);
	  sprintf(CitesConqueredStr,"%d",0);

	  //prepare strings
	  VP_total_no=0;
	 for (i = 0; i < MAX_VICTORY_HEXES; i++){
				if (is_vic_hex_valid(i)) {
					int name_id=map[victory_hexes[i].x][victory_hexes[i].y].gln;
					if (name_id>=0 && name_id<total_names){
						strncpy(buff,gln_utf8[name_id],64);
					}else{
						strncpy(buff,"",64);
					}

					sprintf(VP_non_active[VP_total_no].name,"(%d,%d) %s",victory_hexes[i].x,victory_hexes[i].y,buff);
					VP_non_active[VP_total_no].vic_no=i;
					VP_non_active[VP_total_no].active=0;
					VP_total_no++;
				}
	 }


	 TokensNo=0;
	 int cursor=0,token=0;
	 for(i=0;i<=strlen(ParseVC);i++){
		 if (ParseVC[i] == '\t' || ParseVC[i] == 0) {
				//printf("%s %d %d\n",TokensStr[token],token,cursor);
				TokensStr[token][cursor]=0;
				token++;
				cursor = 0;
				continue;
			}
		 TokensStr[token][cursor]=ParseVC[i];
		 cursor++;
	 }
	 TokensNo=token;
	 //printf("TokensNo=%d\n",TokensNo);

	 //First token parse
	 if (TokensNo>0){
		if (strcmp(TokensStr[0],"AXIS VICTORY")==0)
			vc_dlg[vcTitleText].dp=ax_vc_str;
		else
			vc_dlg[vcTitleText].dp=al_vc_str;
	 }
	 if (TokensNo>2){
		 vc_type = atoi(TokensStr[1]);

		 //clear all radio buttons
		 vc_dlg[vcTypeOponentDoNotWin].flags&= ~D_SELECTED;
		 vc_dlg[vcTypeTakeAll].flags&= ~D_SELECTED;
		 vc_dlg[vcTypeTakeSome].flags&= ~D_SELECTED;

		 //select one
		 if (vc_type==-1) vc_dlg[vcTypeOponentDoNotWin].flags|=D_SELECTED;

		 if (vc_type==0) {
			 vc_dlg[vcTypeTakeSome].flags|=D_SELECTED;
			 sprintf(CitesConqueredStr,"%d",atoi(TokensStr[2]));
			 if (strlen(TokensStr[3])){

					 TokensVCNo=0;
				 	 int cursor=0;
				 	 TokensVCStr[TokensVCNo][cursor]=0; //init

				 	 //tokenizie
				 	 for(i=0;i<strlen(TokensStr[3]);i++){
				 		 if (TokensStr[3][i] == ')') {
								TokensVCStr[TokensVCNo][cursor]	= TokensStr[3][i];
								cursor++;
				 				//printf("%s %d %d\n",TokensVCStr[TokensVCNo],TokensVCNo,cursor);

				 				TokensVCStr[TokensVCNo][cursor]=0;
				 				TokensVCNo++;
				 				cursor = 0;
				 				continue;
				 			}
				 		 TokensVCStr[TokensVCNo][cursor]=TokensStr[3][i];
				 		 cursor++;
				 	 }
				 	 //find VP
				 	 for(i=0;i<TokensVCNo;i++){
				 		 sscanf(TokensVCStr[i],"(%8d,%8d)",&x,&y);
				 		 //printf("%d %d %d\n",i,x,y);

							for(j=0;j<VP_total_no;j++){
								if (victory_hexes[VP_non_active[j].vic_no].x==x && victory_hexes[VP_non_active[j].vic_no].y==y)
									VP_non_active[j].active=1;
							}
				 	 }
			 }
		 }

		 if (vc_type>0) {
			 vc_dlg[vcTypeTakeAll].flags|=D_SELECTED;
			 sprintf(TurnsBeforeEndStr,"%d",vc_type);
		 }
	 }
	 //count active/non active
	 update_active();
  } //MSG_START

  if (msg==MSG_END && vc_save_data){

		if (strcmp(TokensStr[0],"AXIS VICTORY")==0)
			strncpy(buff,"AXIS VICTORY",64);
		else
			strncpy(buff,"ALLIED VICTORY",64);

		if ( vc_dlg[vcTypeOponentDoNotWin].flags&D_SELECTED) {
			vc_type=-1;
			aux_info=-1;
		}
		if ( vc_dlg[vcTypeTakeSome].flags&D_SELECTED) {
			vc_type=0;
			aux_info=atoi(CitesConqueredStr);
		}
		if ( vc_dlg[vcTypeTakeAll].flags&D_SELECTED) {
			vc_type=atoi(TurnsBeforeEndStr);
			if (vc_type<1) vc_type=1;
			aux_info=0;
		}

		strncpy(buff2,"",256);

		if(vc_type==0)
			for(i=0;i<VP_total_no;i++){
				if (VP_non_active[i].active){
					sprintf(tmp,"(%d,%d)",victory_hexes[VP_non_active[i].vic_no].x,victory_hexes[VP_non_active[i].vic_no].y);
					//printf("%s\n",tmp);
					strncat(buff2,tmp,64);
				}
			}

		sprintf(ParseVC,"%s\t%d\t%d\t%s",buff,vc_type,aux_info,buff2);


  }//MSG_END


  return d_textbox_proc(msg,d,c);
}

char *vc_add_non_active(int idx, int *list_size)
{
	int i,c=-1;
   if (idx<0) *list_size=VP_non_active_no;
   else{

		 for (i = 0; i < VP_total_no; i++){
			 if (VP_non_active[i].active==0) c++;
			 if (idx==c)
				 return &(VP_non_active[i].name[0]);
		 }
   }
   return 0;
}

char *vc_add_active(int idx, int *list_size)
{
	int i,c=-1;
   if (idx<0) *list_size=VP_active_no;
   else{

		 for (i = 0; i < VP_total_no; i++){
			 if (VP_non_active[i].active==1) c++;
			 if (idx==c)
				 return &(VP_non_active[i].name[0]);
		 }
 }
   return 0;
}

int d_vc_cancelbtn_proc(int msg,DIALOG *d,int c)
{
    if ((msg==MSG_CLICK)|(msg==MSG_KEY))
      vc_save_data=0;
    return d_button_proc(msg,d,c);
}
