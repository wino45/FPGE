/*
 * unitsum.c
 * Dialog with units statistics
 *  Created on: 2011-07-16
 *      Author: wino
 */

#include <allegro.h>
#include <stdio.h>
#include "fpge.h"
#include "unitsum.h"
#include "tables.h"

#define UNITSUM_COL 32
#define UNITSUM_ROW 16
#define UNITSUM_W (8*(16+4+4+3+12))
#define UNITSUM_H 16
#define UNITSUM_ROWS_NO 27

#define TOTAL_ROW (CLASS_NUMBER+3)
#define ARMY_ROW CLASS_NUMBER
#define AIRFORCE_ROW (CLASS_NUMBER+1)
#define NAVY_ROW (CLASS_NUMBER+2)

char TextBox[1024*8];

DIALOG unitsum_dlg[UNITSUM_DLG] =
{
   /* (proc)           (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                         (dp2) (dp3) */
   { d_unitsum_box_proc,     0,  0,  UNITSUM_COL*2+UNITSUM_W, UNITSUM_ROW*UNITSUM_ROWS_NO, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   NULL,                        NULL, NULL },
   { d_textbox_proc,     0,  0,  UNITSUM_COL*2+UNITSUM_W, UNITSUM_ROW*UNITSUM_ROWS_NO, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   TextBox,                        NULL, NULL },
   { d_button_proc,    (UNITSUM_COL*2+UNITSUM_W-56)/2, UNITSUM_ROW*(UNITSUM_ROWS_NO-2), 56,  16,  GUI_FG_COLOR,   GUI_BG_COLOR,   'd',    D_EXIT,      0,   0,   "&Done",                        NULL, NULL },
   { d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
   { NULL,             0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                        NULL, NULL }
};

int d_unitsum_box_proc(int msg,DIALOG *d,int c)
{
    if (msg==MSG_START){

    	int i,is_allied,c;
    	int count[CLASS_NUMBER+1+3][2];
    	int cost[CLASS_NUMBER+1+3][2];
    	char buff[256];

    	char extra_desc[4][64]={"ARMY","AIRFORCE","NAVY","TOTAL"};

    	int add_exp_str=0;
    	int es;

    	for(i=0;i<CLASS_NUMBER+1+3;i++){
    		count[i][0]=0;
    		count[i][1]=0;
    		cost[i][0]=0;
    		cost[i][1]=0;
    	}

    	for(i=0;i<total_units;i++){
    		is_allied=-1;
    		if (all_units[i].uflag==allied_core || all_units[i].uflag==allied_aux)
    			is_allied=1;
    		if (all_units[i].uflag==axis_core || all_units[i].uflag==axis_aux)
    			is_allied=0;

    		if (is_allied>-1){
				count[ equip[all_units[i].unum][CLASS] ][is_allied]++;
				count[TOTAL_ROW][is_allied]++;

				c=equip[all_units[i].unum][COST]*COST_DIVISOR;

				if (add_exp_str){
					es=all_units[i].str+all_units[i].exp;
					c *= es;
					c /= 10;
				}
				cost[ equip[all_units[i].unum][CLASS] ][is_allied]+=c;
				cost[ TOTAL_ROW][is_allied]+=c;
    		}
    	}
    	//Army total
    	for(i=0;i<7;i++){
    		count[ARMY_ROW][0]+=count[i][0];
    		count[ARMY_ROW][1]+=count[i][1];
    		cost[ARMY_ROW][0]+=cost[i][0];
    		cost[ARMY_ROW][1]+=cost[i][1];
    	}
    	//airforce
    	for(i=8;i<8+3;i++){
    		count[AIRFORCE_ROW][0]+=count[i][0];
    		count[AIRFORCE_ROW][1]+=count[i][1];
    		cost[AIRFORCE_ROW][0]+=cost[i][0];
    		cost[AIRFORCE_ROW][1]+=cost[i][1];
    	}
    	//navy
    	for(i=8+3;i<8+3+4;i++){
    		count[NAVY_ROW][0]+=count[i][0];
    		count[NAVY_ROW][1]+=count[i][1];
    		cost[NAVY_ROW][0]+=cost[i][0];
    		cost[NAVY_ROW][1]+=cost[i][1];
    	}

       	strncpy(TextBox,"\n     Count                        Cost     \n",1024*8);
    	strncat(TextBox,  "   Axis Allied                  Axis Allied\n\n",1024*8);
    	//-3 =no transports are shown
    	for(i=0;i<CLASS_NUMBER;i++){
    		sprintf(buff,"   %4d%4d   %16s%6d%6d\n\n",count[i][0],count[i][1],pg_class_names[i],cost[i][0],cost[i][1]);
    		strncat(TextBox,buff,256);
    	}
    	for(i=0;i<4;i++){
			sprintf(buff,"   %4d%4d   %16s%6d%6d\n\n",count[ARMY_ROW+i][0],count[ARMY_ROW+i][1],extra_desc[i],cost[ARMY_ROW+i][0],cost[ARMY_ROW+i][1]);
			strncat(TextBox,buff,256);
    	}
    }

    return d_box_proc(msg,d,c);
}
