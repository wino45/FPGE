/*
 * scen.h
 *
 *  Created on: 2010-07-10
 *      Author: wino
 */

#ifndef SCEN_H_
#define SCEN_H_

#define PG_SCENARIO_STR_SIZE 14
#define PG_SCENARIO_DES_STR_SIZE 256

#define SCENARIO_STR_SIZE 32
#define SCENARIO_DES_STR_SIZE 1024

/*
#define SD_FG_COLOR 74
#define SD_BG_COLOR 69 //16
#define EDIT_FG_COLOR 14
#define EDIT_BG_COLOR 69 //16
*/
#define SD_FG_COLOR GUI_FG_COLOR
#define SD_BG_COLOR GUI_BG_COLOR
#define EDIT_FG_COLOR GUI_FG_COLOR
#define EDIT_BG_COLOR GUI_EDIT_COLOR


#define CWIDTH  68
#define CEDITWIDTH  8*4
#define RHEIGHT 15

#define WIDTH4  8*4
#define WIDTH5  8*5
#define WIDTH12  8*12

#define ROW1 5
#define ROW2 ROW1+RHEIGHT+RHEIGHT*4
#define ROW3 ROW1+2*RHEIGHT+RHEIGHT*4
#define ROW4 ROW1+3*RHEIGHT+RHEIGHT*4
#define ROW5 ROW1+4*RHEIGHT+RHEIGHT*4

#define ROW2_OLD ROW1+1*RHEIGHT
#define ROW3_OLD ROW1+2*RHEIGHT


#define COL1 5
#define COL2 COL1+CWIDTH
#define COL3 COL1+2*CWIDTH
#define COL5 COL1+4*CWIDTH+8
#define COL6 COL1+5*CWIDTH+8
#define COL7 COL1+6*CWIDTH+8
#define COL8 COL1+7*CWIDTH+8
#define COL9 COL1+8*CWIDTH

#define SCEN_BUTTON_LINE (480-RHEIGHT*2)

#define sdScnNumEditIdx 3
#define sdCtyNumEdit1Idx 7
#define sdCtyNumEdit6Idx 17
#define sdCtyListIdx 19
#define sdOrtRtRBIdx sdCtyListIdx+1
#define sdOrtLfRBIdx sdOrtRtRBIdx+1
#define sdFrtYRBIdx  sdOrtLfRBIdx+1
#define sdFrtNRBIdx  sdFrtYRBIdx+1

#define sdNALocRBIdx 25
#define sdSELocRBIdx sdNALocRBIdx+1
#define sdNELocRBIdx sdSELocRBIdx+1
#define sdEELocRBIdx sdNELocRBIdx+1

#define sdAxisOffRBIdx 30
#define sdAxisDefRBIdx sdAxisOffRBIdx+1
#define sdAlldOffRBIdx sdAxisDefRBIdx+2
#define sdAlldDefRBIdx sdAlldOffRBIdx+1

#define sdCurCoreIdx 41
#define sdCurAuxIdx  sdCurCoreIdx+1
#define sdCurAlldIdx sdCurAuxIdx+1

#define sdMaxCoreIdx 44
#define sdMaxAuxIdx  sdMaxCoreIdx+1
#define sdMaxAlldIdx sdMaxAuxIdx+1

#define sdMonthIdx  4
#define sdDayIdx sdMonthIdx+1
#define sdYearIdx sdDayIdx+1

#define sdTurnsIdx 57
#define sdTurnsPerDayIdx sdTurnsIdx+1
#define sdDaysPerTurnIdx sdTurnsPerDayIdx+1

#define sdAxisPrestigeAmtIdx 65
#define sdAlldPrestigeAmtIdx sdAxisPrestigeAmtIdx+1
#define sdAxisPrestigeTurnsIdx sdAlldPrestigeAmtIdx+1
#define sdAlldPrestigeTurnsIdx sdAxisPrestigeTurnsIdx+1

#define sdAxisPrestigeIdx 73
#define sdAlliedPrestigeIdx sdAxisPrestigeIdx+1

#define sdAxisAirTransNumIdx 82
#define sdAlldAirTransNumIdx sdAxisAirTransNumIdx+1
#define sdAxisAirTransTypeIdx sdAlldAirTransNumIdx+1
#define sdAlldAirTransTypeIdx sdAxisAirTransTypeIdx+1

#define sdAxisSeaTransNumIdx   sdAlldAirTransTypeIdx+1
#define sdAlldSeaTransNumIdx   sdAxisSeaTransNumIdx+1
#define sdAxisSeaTransTypeIdx  sdAlldSeaTransNumIdx+1
#define sdAlldSeaTransTypeIdx  sdAxisSeaTransTypeIdx+1

#define sdVicTxtBoxIdx         sdAlldSeaTransTypeIdx+1
#define tempsdVicTxtBoxIdx     sdVicTxtBoxIdx+1
#define sdCtyNumNeutralEdit1Idx tempsdVicTxtBoxIdx+1


#define sdShortDescIdx 100
#define sdLongDescIdx sdShortDescIdx+1
#define sdRotAxisBtnIdx sdLongDescIdx+1
#define sdRotAlliedBtnIdx sdRotAxisBtnIdx+1
#define sdGenPfCBtnIdx sdRotAlliedBtnIdx+1
#define sdGenAIPBtnIdx sdGenPfCBtnIdx+1

#define sdCurAlldAuxIdx sdGenAIPBtnIdx+1
#define sdMaxAlldAuxIdx sdCurAlldAuxIdx+1
#define sdMaxAlldAuxLabelIdx sdMaxAlldAuxIdx+1
#define sdAxisVCButtonIdx sdMaxAlldAuxLabelIdx+1
#define sdAlliedVCButtonIdx sdAxisVCButtonIdx+1
#define sdClassButtonIdx sdAlliedVCButtonIdx+1
#define sdScenNameEdit sdClassButtonIdx+1

#define S_BUTTON_W 96

#define SCENARIO_DLG_SIZE (108+6+2)

extern DIALOG senario_dlg[SCENARIO_DLG_SIZE];

extern int d_clr_proc(int,DIALOG *,int);
extern int d_ctyedit_proc(int,DIALOG *,int);
extern int d_cancelbtn_proc(int,DIALOG*,int);
extern int d_rotate_btn_proc(int,DIALOG*,int);
int d_genpfc_btn_proc(int msg,DIALOG *d,int c);
int d_genaip_btn_proc(int msg,DIALOG *d,int c);
int d_vc_btn_proc(int msg,DIALOG *d,int c);
int d_class_btn_proc(int msg,DIALOG *d,int c);

#endif /* SCEN_H_ */
