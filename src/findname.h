#ifndef _FINDNAME_H_
#define _FINDNAME_H_

#define _FINDNAME_LIST_NO0 0+1
#define _FINDNAME__FIND 1+1
#define _FINDNAME_FIND__NEXT 2+1
#define _FINDNAME__CANCEL 3+1
#define _FINDNAME__DONE 4+1

#define _FINDNAME__SORT 13
#define _FINDNAME__FILTER 14

#define _FINDNAME__NAME_TO_EDIT (10+1)

int d_myclear_proc(int msg, DIALOG *d, int c);
char *LIST_no0(int , int *);
int d_find_proc(int , DIALOG *, int );
int d_find_next_proc(int , DIALOG *, int );
int d_done_proc(int , DIALOG *, int );
int d_my_list_proc(int msg, DIALOG *d, int c);
int d_update_proc(int msg, DIALOG *d, int c);
int d_add_new_proc(int msg, DIALOG *d, int c);
int d_save_proc(int msg, DIALOG *d, int c);

int d_filter_proc(int msg, DIALOG *d, int c);
int d_sort_proc(int msg, DIALOG *d, int c);

extern char name_to_find[];
extern char name_to_edit[];

extern int findname_sort;
extern int findname_filter;

#define FINDNAME_DLG_SIZE (15+2)

extern DIALOG findname_dlg[FINDNAME_DLG_SIZE];
#endif
