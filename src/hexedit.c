/*
 * hexedit.c
 *
 *  Created on: 22-04-2012
 *      Author: wino
 */

#include <allegro.h>
#include "fpge.h"
#include "fpge_colors.h"
#include "tables.h"
#include "move.h"

#define MAX_DYN_DLG_SIZE 100
#define RADIO_COL_WIDTH (12+8*(11+11))
#define RADIO_COL_X 8
#define RADIO_COL_Y 8

#define TEXT_COL_WIDTH (13*8)
#define TEXT_COL_X 8+RADIO_COL_WIDTH+8
#define TEXT_COL_Y 16

#define EDIT_COL_WIDTH (8*4)
#define EDIT_COL_X 8+RADIO_COL_WIDTH+8+TEXT_COL_WIDTH+8
#define EDIT_COL_Y 16+16

DIALOG hexedit_dlg[MAX_DYN_DLG_SIZE];

char edit_strs[MAX_TERRAIN_MOV_TYPES][MAX_COLUMNS_DYNDLG][6];

int fpge_edit_values[MAX_MOV_TYPE][MAX_TERRAIN_MOV_TYPES][MAX_COLUMNS_DYNDLG];

int s_radios;
int s_rows;
int s_cols;

void fill_edit_values_table_uchar(unsigned char *from, int radios, int rows, int cols){
	int i,j,k;
	unsigned char *ptr;

	memset(fpge_edit_values,0,sizeof(fpge_edit_values));

	for(i=0;i<radios;i++)
		for(j=0;j<rows;j++)
			for(k=0;k<cols;k++){
				ptr = from+i*(rows*cols)+j*cols+k;
				fpge_edit_values[i][j][k]=*ptr;
			}
}

void fill_edit_strs(int radio_value, int row_value, int col_value){
	char buf[100];
	int val;

	val =fpge_edit_values[radio_value][row_value][col_value];
	if (val <254){
		snprintf(buf,100,"%d",val);
	}else{
		if (val==255) snprintf(buf,100,"N/A");
		else snprintf(buf,100,"All");
	}
	strncpy(edit_strs[row_value][col_value],buf,5);

}

void fill_all(int radio_value){
	int i,j;

	for(i=0;i<s_rows;i++)
		for(j=0;j<s_cols;j++)
	{
			fill_edit_strs(radio_value,i,j);
	}
}

int my_d_radio_proc(int msg, DIALOG *d, int c) {
	int i, j, k, dlg_rows;

	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) {
		for (i = 0; i < s_radios; i++) {
			//radios are from index 1
			if (&(hexedit_dlg[i + 1]) == d) {
				fill_all(i);

				dlg_rows = 1 + s_radios + s_rows + s_cols;

				for (k = 0; k < s_rows; k++)
					for (j = 0; j < s_cols; j++) {
						hexedit_dlg[dlg_rows].flags |= D_DIRTY;
						dlg_rows++;
					}
				break;
			}
		}
	}
	//if (found) broadcast_dialog_message(MSG_DRAW, 0);
	return d_radio_proc(msg, d, c);
}

int dynamic_dlg(int dlg_type, int radios, int rows, int cols){
	int i,j;
	int dlg_rows=0;

	s_radios = radios;
	s_rows = rows;
	s_cols = cols;

	if (dlg_type)
		fill_edit_values_table_uchar(PgStaticWeatherTable,radios,rows,cols);
	else
		fill_edit_values_table_uchar(PgStaticMoveTable,radios,rows,cols);


	memset(hexedit_dlg,0,sizeof(hexedit_dlg));

	//window
	hexedit_dlg[dlg_rows].proc=d_textbox_proc;
	hexedit_dlg[dlg_rows].w=8+TEXT_COL_WIDTH+8+RADIO_COL_WIDTH+8+(EDIT_COL_WIDTH+8)*cols+8+16;
	hexedit_dlg[dlg_rows].h=16+13*16;
	hexedit_dlg[dlg_rows].fg=GUI_FG_COLOR;
	hexedit_dlg[dlg_rows].bg=GUI_BG_COLOR;
	dlg_rows++;
	//1

	//ratio buttons
	for(i=0;i<radios;i++){
		hexedit_dlg[dlg_rows].proc=my_d_radio_proc;
		hexedit_dlg[dlg_rows].x=RADIO_COL_X;
		hexedit_dlg[dlg_rows].y=RADIO_COL_Y+16*i;
		hexedit_dlg[dlg_rows].w=RADIO_COL_WIDTH;
		hexedit_dlg[dlg_rows].h=16;
		hexedit_dlg[dlg_rows].fg=GUI_FG_COLOR;
		hexedit_dlg[dlg_rows].bg=GUI_BG_COLOR;
		if (dlg_type)
			hexedit_dlg[dlg_rows].dp=(void *)weather_zones[i+1];
		else
			hexedit_dlg[dlg_rows].dp=(void *)movement_type[i];

		if (i==0) hexedit_dlg[dlg_rows].flags |= D_SELECTED;

		dlg_rows++;
	}
	//1+PG_MOV_TYPES

	for(i=0;i<rows;i++){
		hexedit_dlg[dlg_rows].proc=d_rtext_proc;
		hexedit_dlg[dlg_rows].x=TEXT_COL_X;
		hexedit_dlg[dlg_rows].y=TEXT_COL_Y+16*i+16;
		hexedit_dlg[dlg_rows].w=TEXT_COL_WIDTH;
		hexedit_dlg[dlg_rows].h=16;
		hexedit_dlg[dlg_rows].fg=GUI_FG_COLOR;
		hexedit_dlg[dlg_rows].bg=GUI_BG_COLOR;
		if (dlg_type)
			hexedit_dlg[dlg_rows].dp=(void *)months_names[i];
		else
			hexedit_dlg[dlg_rows].dp=(void *)movement_terrain_names[i];
		dlg_rows++;
	}
	//1+PG_MOV_TYPES+PG_TERRAIN_MOV_TYPES

	for(i=0;i<cols;i++)
		{
			hexedit_dlg[dlg_rows].proc=d_text_proc;
			hexedit_dlg[dlg_rows].x=EDIT_COL_X+(EDIT_COL_WIDTH+8)*i;
			hexedit_dlg[dlg_rows].y=EDIT_COL_Y-16;
			hexedit_dlg[dlg_rows].w=EDIT_COL_WIDTH;
			hexedit_dlg[dlg_rows].h=16;
			hexedit_dlg[dlg_rows].fg=GUI_FG_COLOR;
			hexedit_dlg[dlg_rows].bg=GUI_BG_COLOR;
			if (dlg_type)
				hexedit_dlg[dlg_rows].dp=(void *)weather_params[i];
			else
				hexedit_dlg[dlg_rows].dp=(void *)weather_str[i];
			dlg_rows++;
		}
	//1+PG_MOV_TYPES+PG_TERRAIN_MOV_TYPES+COL_PG_WEATHER_TYPES


	for(i=0;i<rows;i++)
		for(j=0;j<cols;j++)
	{
		hexedit_dlg[dlg_rows].proc=d_edit_proc;
		hexedit_dlg[dlg_rows].x=EDIT_COL_X+(EDIT_COL_WIDTH+8)*j;
		hexedit_dlg[dlg_rows].y=EDIT_COL_Y+16*i;
		hexedit_dlg[dlg_rows].w=EDIT_COL_WIDTH;
		hexedit_dlg[dlg_rows].h=16;
		hexedit_dlg[dlg_rows].fg=GUI_FG_COLOR;
		hexedit_dlg[dlg_rows].bg=GUI_EDIT_COLOR;
		hexedit_dlg[dlg_rows].dp=(void *)edit_strs[i][j];
		dlg_rows++;
	}

	fill_all(0);

	//yield
	hexedit_dlg[dlg_rows].proc=d_yield_proc;
	dlg_rows++;

	dialog_to24bits(hexedit_dlg,dlg_rows);

	centre_dialog(hexedit_dlg);
	do_dialog(hexedit_dlg,-1);

	return D_REDRAW;
}

int show_move_table_dlg(){
	if (pgf_mode && bin_tables_present){
		return dynamic_dlg(0,RADIO_PGF_MOV_TYPES,ROW_PG_TERRAIN_MOV_TYPES,COL_PG_WEATHER_TYPES);
	}else{
		return dynamic_dlg(0,RADIO_PG_MOV_TYPES,ROW_PG_TERRAIN_MOV_TYPES,COL_PG_WEATHER_TYPES);
	}
}

int show_weather_table_dlg(){
	//3 - three zones
	//12 rows = months
	//4 - parameters per row
	return dynamic_dlg(1,3,12,4);

}




