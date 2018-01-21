/*
 * class.c
 * PGF Purchasable classes editor dialog
 *  Created on: 2011-07-15
 *      Author: wino
 */

#include <allegro.h>
#include "class.h"
#include "fpge.h"
#include "pgf.h"

#define CLASS_COL 32
#define CLASS_ROW 16
#define CLASS_W (8*18)
#define CLASS_H 16

int class_save_data;

DIALOG class_dlg[CLASS_DLG] =
{
   /* (proc)           (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                         (dp2) (dp3) */
   { d_class_box_proc,     0,  0,  CLASS_COL*2+CLASS_W, CLASS_ROW*22, GUI_FG_COLOR,   GUI_BG_COLOR,   0,    0,      0,   0,   NULL,                        NULL, NULL },
   { d_button_proc,    CLASS_COL*2+CLASS_W-86-16, CLASS_ROW*20, 86,  16,  GUI_FG_COLOR,   GUI_BG_COLOR,   'd',    D_EXIT,      0,   0,   "&Done",                        NULL, NULL },
   { d_class_cancelbtn_proc,    CLASS_COL-20, CLASS_ROW*20, 86,  16,  GUI_FG_COLOR,   GUI_BG_COLOR, 'c',    D_EXIT,      0,   0,   "&Cancel",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Infantry",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*2, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Tank",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*3, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Recon",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*4, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Anti-Tank",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*5, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Artillery",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*6, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Anti-Aircraft",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*7, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Air Defense",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*8, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Fortification",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*9, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Fighter",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*10, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Tactical Bomber",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*11, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Level Bomber",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*12, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Submarine",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*13, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Destroyer",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*14, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Capital Ship",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*15, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Aircraft Carrier",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*16, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Land Transport",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*17, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Air Transport",                    NULL, NULL },
   { d_check_proc,    CLASS_COL, CLASS_ROW*18, CLASS_W,  CLASS_H,  GUI_FG_COLOR,   GUI_BG_COLOR, 0,    0,      1,   0,   "Sea Transport",                    NULL, NULL },


   { d_yield_proc,0,0,0,0,0,0,0,0,0,0,0,0,0},
   { NULL,             0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                        NULL, NULL }
};

int d_class_box_proc(int msg,DIALOG *d,int c)
{
	int i,v,j;
	//char buff[256];

    if (msg==MSG_START){
    	class_save_data=1; //default is to save

    	for (i = 0; i < block9_lines; i++){
    		sscanf(block9[i],"%10d\t%10d",&j,&v);
    		if(j>=0 && j<CLASS_NUMBER && v) class_dlg[ClassInfantryChkBoxIdx+j].flags |= D_SELECTED;
    	}
    }

    if (msg==MSG_END && class_save_data){
		block9_lines=0;
    	for (i = 0; i < CLASS_NUMBER;i++){
    		if (class_dlg[ClassInfantryChkBoxIdx+i].flags & D_SELECTED){
    			//printf("%d\t%d\n",i,1);
    			sprintf(block9[block9_lines],"%d\t%d\n",i,1);
    			block9_lines++;
    		}
    	}
		if (0==block9_lines){
			//we must fill at least one line in this scenario section
			sprintf(block9[block9_lines],"%d\t%d\n",0,0);
			block9_lines++;
		}
    }

    return d_box_proc(msg,d,c);
}

int d_class_cancelbtn_proc(int msg,DIALOG *d,int c)
{
    if ((msg==MSG_CLICK)|(msg==MSG_KEY))
      class_save_data=0;
    return d_button_proc(msg,d,c);
}
