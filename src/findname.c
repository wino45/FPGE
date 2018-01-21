/* FINDNAME.C handles input for Search of names */

#include <stdio.h>
#include <allegro.h>
#include "fpge.h"
#include "findname.h"
#include "save.h"
#include "tables.h"
#include "terr.h"

/*
#define FN_FG_COLOR 15
#define FN_BG_COLOR 88
#define FN_EDIT_COLOR 33
*/
#define FN_FG_COLOR GUI_FG_COLOR
#define FN_BG_COLOR GUI_BG_COLOR
#define FN_EDIT_COLOR GUI_EDIT_COLOR

#define FN_COL1 198+16
#define FN_COL2 198+16+96+16

#define LINES_IN_LISTBOX 27

DIALOG findname_dlg[FINDNAME_DLG_SIZE]= {
        { d_myclear_proc,0,0,198+96+16+96+16+16,222+16,FN_FG_COLOR,	FN_BG_COLOR,	0 ,	0 ,	0 ,	0 ,0},
/* 1 */	{ d_my_list_proc   ,	6   ,	6  ,	/*174+16*/ 7*LINES_IN_LISTBOX ,	222 ,	FN_FG_COLOR,	FN_BG_COLOR,	0 , 	0 ,	0 ,	0 ,	(void *)LIST_no0},
/* 2 */	{ d_find_proc   ,	FN_COL1 ,	120-80+8 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	'f' ,	0 ,	0 ,	0 ,	 (void *)"&Find" },
/* 3 */	{ d_find_next_proc,	FN_COL2 ,	120-80+8 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	'n' ,	0 ,	0 ,	0 ,	 (void *)"Find &next" },
/* 4 */	{ d_button_proc ,	FN_COL1 ,	204 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	'c' ,	D_EXIT ,0 ,	0 ,	 (void *)"&Cancel" },
/* 5 */	{ d_done_proc   ,	FN_COL2 ,	204 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	'd' ,	D_EXIT ,0 ,	0 ,	 (void *)"&Done" },
/* 6 */	{ d_edit_proc , 	FN_COL1 ,	102-80+8 ,	8*21 ,	15 ,	FN_FG_COLOR,	FN_EDIT_COLOR,	0 ,	    0 ,	20 ,1 ,	(void *)name_to_find},
/* 7 */	{ d_text_proc ,	    FN_COL1 ,	84-80+8 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Search name:" },

/* 8 */	{ d_update_proc   ,	FN_COL1 ,	120 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	'u' ,	0 ,	0 ,	0 ,	 (void *)"&Update" },
/* 9 */	{ d_add_new_proc,	FN_COL2 ,	120 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	'a' ,	0 ,	0 ,	0 ,	 (void *)"&Add new" },
/* 10 */{ d_save_proc,		FN_COL2 ,	144 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	's' ,	0 ,	0 ,	0 ,	 (void *)"&Save" },

/* 11 */{ d_edit_proc , 	FN_COL1 ,	102 ,	8*21 ,	15 ,	FN_FG_COLOR,	FN_EDIT_COLOR,	0 ,	    0 ,	20 ,1 ,	(void *)name_to_edit},
/* 12 */{ d_text_proc ,	    FN_COL1 ,	84 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	0 ,	    0 ,	0 ,	0 ,	 (void *)"Edit name:" },

/* 13 */{ d_sort_proc,		FN_COL1 ,	144+16 ,	96 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	0,	0 ,	1 ,	0 ,	 (void *)"Sort names" },
/* 14 */{ d_filter_proc,	FN_COL1 ,	144+32 ,	96*2-8*3 ,	15 ,	FN_FG_COLOR,	FN_BG_COLOR,	0,	0 ,	1 ,	0 ,	 (void *)"Only scenario names" },

{ d_yield_proc,        0,      0,     0,      0,     0,  0,  0,     0,   0,  0,    NULL, NULL, NULL  },
{ NULL,                0,      0,     0,      0,     0,  0,  0,     0,   0,  0,    NULL }
 };


int findname_sort=0;
int findname_filter=0;
int total_names_filter=0;

char name_to_find[20*4]="" ;
char name_to_edit[20*4]="" ;

unsigned char gln_filter[MAX_NAMES]; //0-filtered
int gln_idx[MAX_NAMES]; //ids of gln

int name_comp_f(const void *ii, const void *jj)
{
    int *i=(int *)ii;
    int *j=(int *)jj;

    if (strcmp(gln_utf8[*i],gln_utf8[*j])!=0)
    	return strcmp(gln_utf8[*i],gln_utf8[*j]);
    return *i-*j;
}

void filter(int filter, int sort){
	int i,y,x;

	total_names_filter=total_names;
	for(i=0;i<total_names;i++){
		gln_idx[i]=i;
		gln_filter[i]=(filter==0?1:0);
	}

	if (!filter) {
		if (sort)
			qsort(gln_idx,total_names_filter,sizeof(int),name_comp_f );
		return;
	}
	//for filter=1, mark only existing names

	for(y=0;y<mapy;y++)
		for(x=0;x<mapx;x++)
			gln_filter[map[x][y].gln]=1;

	total_names_filter=0;
	for(i=0;i<total_names;i++)
		if (gln_filter[i]) {
			gln_idx[total_names_filter]=i; //only filtered names are here
			total_names_filter++;
		}
	if (sort)
		qsort(gln_idx,total_names_filter,sizeof(int),name_comp_f );

	return;
}

char *LIST_no0(int index, int *list_size){
     if (index<0){
    	*list_size=total_names_filter;
        return 0;
     }
     return gln_utf8[gln_idx[index]];
}

int d_sort_proc(int msg, DIALOG *d, int c){
	if ((msg==MSG_CLICK)||(msg==MSG_KEY)){
		int result = d_check_proc(msg,d,c);
		findname_sort=0;
		if (findname_dlg[_FINDNAME__SORT].flags & D_SELECTED) findname_sort=1;
		filter(findname_filter,findname_sort);
		findname_dlg[_FINDNAME_LIST_NO0].flags |= D_DIRTY;
		findname_dlg[_FINDNAME_LIST_NO0].d1=0;
		findname_dlg[_FINDNAME_LIST_NO0].d2=0;
		sprintf(name_to_edit,"%s",gln_utf8[gln_idx[findname_dlg[_FINDNAME_LIST_NO0].d1]]);
		findname_dlg[_FINDNAME__NAME_TO_EDIT].flags |= D_DIRTY;
		return result;
	}
	return d_check_proc(msg,d,c);
}

int d_filter_proc(int msg, DIALOG *d, int c){
	if ((msg==MSG_CLICK)||(msg==MSG_KEY)){
		int result = d_check_proc(msg,d,c);
		findname_filter=0;
		if (findname_dlg[_FINDNAME__FILTER].flags & D_SELECTED) findname_filter=1;
		filter(findname_filter,findname_sort);
		findname_dlg[_FINDNAME_LIST_NO0].flags |= D_DIRTY;
		findname_dlg[_FINDNAME_LIST_NO0].d1=0;
		findname_dlg[_FINDNAME_LIST_NO0].d2=0;
		sprintf(name_to_edit,"%s",gln_utf8[gln_idx[findname_dlg[_FINDNAME_LIST_NO0].d1]]);
		findname_dlg[_FINDNAME__NAME_TO_EDIT].flags |= D_DIRTY;

		return result;
	}
	return d_check_proc(msg,d,c);
}

int d_myclear_proc(int msg, DIALOG *d, int c){
	if (msg==MSG_START){
		if (findname_sort)
			findname_dlg[_FINDNAME__SORT].flags |= D_SELECTED;
		else
			findname_dlg[_FINDNAME__SORT].flags &= ~D_SELECTED;

		if (findname_filter)
			findname_dlg[_FINDNAME__FILTER].flags |= D_SELECTED;
		else
			findname_dlg[_FINDNAME__FILTER].flags &= ~D_SELECTED;

		filter(findname_filter,findname_sort);
	}
	return d_textbox_proc(msg,d,c);
}


int d_find_proc(int msg, DIALOG *d, int c){
	int i,found=0;
	char temp[40], temp1[40];
	
	if ((msg==MSG_CLICK)||(msg==MSG_KEY)){
        strncpy(temp1,name_to_find,40);
        strlwr(temp1);
		for(i=0; i<total_names_filter; i++){
            strncpy(temp,gln_utf8[gln_idx[i]],40);
			if (strstr(strlwr(temp),temp1)!=NULL) {
				findname_dlg[_FINDNAME_LIST_NO0].d1=i;
				findname_dlg[_FINDNAME_LIST_NO0].d2=Min(Min(i,total_names_filter-LINES_IN_LISTBOX),Max(0,total_names_filter-LINES_IN_LISTBOX)); //total_names_filter-LINES_IN_LISTBOX
				found=1;
				break;
			}
        }
		if (found) {
			object_message(&findname_dlg[_FINDNAME_LIST_NO0], MSG_DRAW, 0);
			sprintf(name_to_edit,"%s",gln_utf8[gln_idx[findname_dlg[_FINDNAME_LIST_NO0].d1]]);
			findname_dlg[_FINDNAME__NAME_TO_EDIT].flags |= D_DIRTY;
		}
		d->flags&=~D_SELECTED;
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}


int d_find_next_proc(int msg, DIALOG *d, int c){
	int i,start,found=0;
	char temp[40], temp1[40];
	
	if ((msg==MSG_CLICK)||(msg==MSG_KEY)){
        strncpy(temp1,name_to_find,40);
        strlwr(temp1);
        start=Min(findname_dlg[_FINDNAME_LIST_NO0].d1+1,total_names_filter-1) ;
		for(i=start; i<total_names_filter; i++){
            strncpy(temp,gln_utf8[gln_idx[i]],40);
			if (strstr(strlwr(temp),temp1)!=NULL) {
				findname_dlg[_FINDNAME_LIST_NO0].d1=i;
				findname_dlg[_FINDNAME_LIST_NO0].d2=Min(Min(i,total_names_filter-LINES_IN_LISTBOX),Max(0,total_names_filter-LINES_IN_LISTBOX));
				found=1;
				break;
			}
        }
		if (found) {
			object_message(&findname_dlg[_FINDNAME_LIST_NO0], MSG_DRAW, 0);
			sprintf(name_to_edit,"%s",gln_utf8[gln_idx[findname_dlg[_FINDNAME_LIST_NO0].d1]]);
			findname_dlg[_FINDNAME__NAME_TO_EDIT].flags |= D_DIRTY;
		}
		d->flags&=~D_SELECTED;
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}

int d_done_proc(int msg, DIALOG *d, int c){	
	if ((msg==MSG_CLICK)||(msg==MSG_KEY)){
        sprintf(tdGLNStr,"%d",gln_idx[findname_dlg[_FINDNAME_LIST_NO0].d1]);
		return D_CLOSE|d_button_proc(msg,d,c);;
	}
	return d_button_proc(msg,d,c);
}

int d_my_list_proc(int msg, DIALOG *d, int c){
	int d1;

	if (msg==MSG_CLICK || msg == MSG_CHAR){
		d1 = d->d1;
		d_list_proc(msg,d,c);

		if (d1 > -1 || msg == MSG_CLICK)
		{
			sprintf(name_to_edit,"%s",gln_utf8[gln_idx[findname_dlg[_FINDNAME_LIST_NO0].d1]]);
			findname_dlg[_FINDNAME__NAME_TO_EDIT].flags |= D_DIRTY;

			if (msg == MSG_CLICK)
				return D_O_K;
			if (msg == MSG_CHAR) {
				if (d1 != d->d1)
					return D_USED_CHAR;
				else
					return D_O_K;
			}
		}
		return D_O_K;
	}

	return d_list_proc(msg,d,c);
}

void save_to_gln_with_CP1250_conversion(int idx, char *name_to_edit){
	int i,j,c1,cc,limit;

	memset(gln[idx],0,MAX_NAME_SIZE);
	limit=Min(ustrlen(name_to_edit),MAX_NAME_SIZE);
	for(i=0;i<limit;i++){
		c1=ugetat(name_to_edit,i);
		cc=32; //space in case of unknow char
		for(j=0;j<256;j++)
			if (cp1250_to_utf[j]==c1) cc=j;
		//printf("i=%d %x=%d\n",i, c1,cc);
		gln[idx][i]=(unsigned char)cc;
	}
}

int d_update_proc(int msg, DIALOG *d, int c){
	int idx,i,gln_tmp;

	if ((msg==MSG_CLICK)||(msg==MSG_KEY)){
        //sprintf(tdGLNStr,"%d",findname_dlg[_FINDNAME_LIST_NO0].d1);
		idx = findname_dlg[_FINDNAME_LIST_NO0].d1;
		if (idx>-1 && idx<MAX_NAMES){
			strncpy(gln_utf8[gln_idx[idx]],name_to_edit,MAX_NAME_UTF_SIZE);
			save_to_gln_with_CP1250_conversion(gln_idx[idx],name_to_edit);
			//strncpy(gln[idx],name_to_edit,MAX_NAME_SIZE);
			gln_tmp=gln_idx[idx];
			mapnames_changed = 1;
			filter(findname_filter,findname_sort); //sort
			//search for new place list
			if (findname_sort) {

				for (i = 0; i < total_names_filter; i++) {
					if (gln_idx[i] == gln_tmp) {
						findname_dlg[_FINDNAME_LIST_NO0].d1 = i;
						findname_dlg[_FINDNAME_LIST_NO0].d2 = Min(Min(i,total_names_filter-LINES_IN_LISTBOX),Max(0,total_names_filter-LINES_IN_LISTBOX)); //total_names_filter-LINES_IN_LISTBOX
						break;
					}
				}
			}

			//redraw list
			findname_dlg[_FINDNAME_LIST_NO0].flags |=D_DIRTY;
		}
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}

int d_add_new_proc(int msg, DIALOG *d, int c){
	int idx;
	if ((msg==MSG_CLICK)||(msg==MSG_KEY)){
		if (total_names<MAX_NAMES){
			idx = total_names;
			strncpy(gln_utf8[idx],name_to_edit,MAX_NAME_UTF_SIZE);
			// convert from UTF-8 to CP1250
			save_to_gln_with_CP1250_conversion(idx,name_to_edit);
			//strncpy(gln[idx],name_to_edit,MAX_NAME_SIZE);
			//redraw list
			total_names++;
			mapnames_changed = 1;
			findname_dlg[_FINDNAME_LIST_NO0].d1=Min(idx,total_names_filter);
			findname_dlg[_FINDNAME_LIST_NO0].d2=Min(Min(idx,total_names_filter-LINES_IN_LISTBOX),Max(0,total_names_filter-LINES_IN_LISTBOX));
			findname_dlg[_FINDNAME_LIST_NO0].flags |=D_DIRTY;

			filter(findname_filter,findname_sort);
			//special case, if filtered included also new name
			if (findname_filter){
				gln_filter[idx]=1;
				gln_idx[total_names_filter]=idx;
				findname_dlg[_FINDNAME_LIST_NO0].d2++;
				total_names_filter++;
			}
			sprintf(name_to_edit,"%s",gln_utf8[gln_idx[findname_dlg[_FINDNAME_LIST_NO0].d1]]);
			findname_dlg[_FINDNAME__NAME_TO_EDIT].flags |= D_DIRTY;
		}
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}

int d_save_proc(int msg, DIALOG *d, int c){
	if ((msg==MSG_CLICK)||(msg==MSG_KEY)){
		if (alert("Do you want to overwrite 'mapnames.str' file ?", NULL, NULL,"&Yes", "&No", 'y', 'n')==1){
			save_names();
		}
		return D_O_K;
	}
	return d_button_proc(msg,d,c);
}
