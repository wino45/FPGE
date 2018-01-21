/*
 * unitlist.c
 *
 *  Created on: 2010-05-03
 *      Author: wino
 */

#include <stdio.h>
#include <string.h>
#include <allegro.h>

#include "fpge.h"
#include "tables.h"
#include "unitlist.h"
#include "unitutil.h"
#include "unit.h"
#include "maingui.h"
#include "fpge_colors.h"
#include "drawmap.h"

/*
 //old color scheme
 #define UNIT_SCREEN_COLOR 33
 #define UNIT_FG_COLOR 15
 #define UNIT_BG_COLOR 38
 #define EDIT_BG_COLOR 14
 #define EDIT_FG_COLOR 4
 */
#define UNIT_SCREEN_COLOR GUI_BG_COLOR
#define UNIT_FG_COLOR GUI_FG_COLOR
#define UNIT_BG_COLOR GUI_BG_COLOR
#define EDIT_BG_COLOR GUI_EDIT_COLOR
#define EDIT_FG_COLOR GUI_FG_COLOR

#define UNIT_LIST_W 640
#define UNIT_LIST_H 480

#define UNIT_TYPE_RB_X 5
#define UNIT_TYPE_RB_Y 10
#define UNIT_TYPE_RB_SPACE 20
#define UNIT_TYPE_Y_SPACE 135
#define UNIT_TYPE_X_SHIFT (-19)

//#define SPACES20 "                    "
#define SEPARATOR " "
#define uldDoneBtnIdx 1
#define uldUpBtnIdx uldDoneBtnIdx+1
#define uldDownBtnIdx uldUpBtnIdx+1
#define uldDeleteBtnIdx uldDownBtnIdx+1
#define uldAddBtnIdx uldDeleteBtnIdx+1
#define uldCopyBtnIdx uldAddBtnIdx+1
#define uldGoToBtnIdx uldCopyBtnIdx+1
#define uldCoreRBIdx uldGoToBtnIdx+1
#define uldAuxRBIdx uldCoreRBIdx+1
#define uldAlliedRBIdx uldAuxRBIdx+1
#define uldAlliedAuxRBIdx uldAlliedRBIdx+1
#define uldUnitListIdx uldAlliedAuxRBIdx+1
#define uldPannelCheckIdx uldUnitListIdx+1

enum UTYPES unit_type = 0;

#define ULINE_SIZE (60 + 2 + 16 + 3 + 2 + 2 + 1)

char uline[ULINE_SIZE][MAX_ALLIED_UNITS];
int show = 1;

int findOffset_ex(enum UTYPES unit_type_ex){
	int offset;
	switch (unit_type_ex) {
	case axis_core:
		offset = 0;
		break;
	case axis_aux:
		offset = total_axis_core;
		break;
	case allied_core:
		offset = total_axis_core + total_axis_aux;
		break;
	case allied_aux:
		offset = total_axis_core + total_axis_aux + total_allied_core;
		break;
	default:
		offset = 0;
		break;
	}
	return offset;
}

int findOffset() {
	return findOffset_ex(unit_type);
}

int findSize() {
	int max_unit_list_size;
	switch (unit_type) {
	case axis_core:
		max_unit_list_size = total_axis_core;
		break;
	case axis_aux:
		max_unit_list_size = total_axis_aux;
		break;
	case allied_core:
		max_unit_list_size = total_allied_core;
		break;
	case allied_aux:
		max_unit_list_size = total_allied_aux;
		break;
	default:
		max_unit_list_size = 0;
		break;
	}
	return max_unit_list_size;
}

int findMaxIdx() {
	int max_unit_list_size;
	switch (unit_type) {
	case axis_core:
		max_unit_list_size = total_axis_core;
		break;
	case axis_aux:
		max_unit_list_size = total_axis_core + total_axis_aux;
		break;
	case allied_core:
		max_unit_list_size = total_axis_core + total_axis_aux
				+ total_allied_core;
		break;
	case allied_aux:
		max_unit_list_size = total_axis_core + total_axis_aux
				+ total_allied_core + total_allied_aux;
		break;
	default:
		max_unit_list_size = 0;
		break;
	}
	return max_unit_list_size;
}

char *add_lunits(int idx, int *list_size) {
	char temp[256];
	int idx_i,c;

	if (idx < 0) {
		switch (unit_type) {
		case axis_core:
			*list_size = total_axis_core;
			break;
		case axis_aux:
			*list_size = total_axis_aux;
			break;
		case allied_core:
			*list_size = total_allied_core;
			break;
		case allied_aux:
			*list_size = total_allied_aux;
			break;
		default:
			*list_size = 0;
			break;
		}
	} else {
		idx_i = findOffset() + idx;
		//idx_i+=idx;
		//clear string
		strncpy(uline[idx], "",ULINE_SIZE);
		//number
		sprintf(temp, "%3d%s", idx + 1, SEPARATOR);
		strncat(uline[idx], temp,ULINE_SIZE);
		//name
		sprintf(temp, "%-20s%s", equip_name_utf8[all_units[idx_i].unum],
				SEPARATOR);
		strncat(uline[idx], temp,ULINE_SIZE);
		//country
		//printf("%d %d %3s\n",idx_i,all_units[idx_i].country,country_names_short[all_units[idx_i].country]);
		c=all_units[idx_i].country;
		if (c>0 && c < MAX_COUNTRY)
			sprintf(temp, "%3s%s", country_active[c]?country_names_short[c]:"   ", SEPARATOR);
		else
			sprintf(temp, "%3s%s", "   ", SEPARATOR);
		strncat(uline[idx], temp,ULINE_SIZE);
		//str
		sprintf(temp, "%2d%s", all_units[idx_i].str, SEPARATOR);
		strncat(uline[idx], temp,ULINE_SIZE);
		//entrench
		sprintf(temp, "%1d%s", all_units[idx_i].entrench, SEPARATOR);
		strncat(uline[idx], temp,ULINE_SIZE);
		//exp
		sprintf(temp, "%1d%s", all_units[idx_i].exp, SEPARATOR);
		strncat(uline[idx], temp,ULINE_SIZE);
		if (!show) {
			// organic transport
			if (all_units[idx_i].orgtnum > 0)
				sprintf(temp, "%-20s%s",
						equip_name_utf8[all_units[idx_i].orgtnum], SEPARATOR);
			else
				sprintf(temp, "%-20s%s", "", SEPARATOR);
			strncat(uline[idx], temp,ULINE_SIZE);
			// sea/air transport
			if (all_units[idx_i].auxtnum > 0)
				sprintf(temp, "%-20s%s:",
						equip_name_utf8[all_units[idx_i].auxtnum], SEPARATOR);
			else
				sprintf(temp, "%-20s%s:", "", SEPARATOR);
			strncat(uline[idx], temp,ULINE_SIZE);
		}
		return uline[idx];
	}
	return 0;
}

int d_coreUL_proc(int msg, DIALOG *d, int c) {
	enum UTYPES unit_type_tmp = unit_type;
	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) {
		if (&(unitslist_dlg[uldCoreRBIdx]) == d)
			unit_type = axis_core;
		if (&(unitslist_dlg[uldAuxRBIdx]) == d)
			unit_type = axis_aux;
		if (&(unitslist_dlg[uldAlliedRBIdx]) == d)
			unit_type = allied_core;
		if (&(unitslist_dlg[uldAlliedAuxRBIdx]) == d)
			unit_type = allied_aux;

		if (unit_type_tmp != unit_type) {
			unitslist_dlg[uldUnitListIdx].d1 = -1;
			unitslist_dlg[uldUnitListIdx].d2 = 0;
			unitslist_dlg[uldUnitListIdx].flags |= D_DIRTY;
			//broadcast_dialog_message(MSG_DRAW, c);
		}
	}
	return d_radio_proc(msg, d, c);
}

int d_pannel_proc(int msg, DIALOG *d, int c) {
	int out;

	out = d_check_proc(msg, d, c);

	if (msg == MSG_START)
		if (show)
			d->flags |= D_SELECTED;

	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) {
		show = (unitslist_dlg[uldPannelCheckIdx].flags & D_SELECTED);
		if (show)
			unitslist_dlg[uldUnitListIdx].w = UNIT_LIST_W / 2 - 2 * UNIT_TYPE_RB_X;
		else
			unitslist_dlg[uldUnitListIdx].w = UNIT_LIST_W - 2 * UNIT_TYPE_RB_X;

		unitslist_dlg[uldUnitListIdx].d1 = -1;
		unitslist_dlg[uldUnitListIdx].d2 = 0;

		//d_ullist_proc(MSG_DRAW,&(uld[uldUnitListIdx]),c);
		broadcast_dialog_message(MSG_DRAW, c);
	}
	return out;
}

void draw_unit(DIALOG *d) {
	int bmp_off;
	int c;

	if (show) {
		display_unit_info(all_units[findOffset() + d->d1].unum,
				unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2 + 80
						+UNIT_TYPE_X_SHIFT, unitslist_dlg[0].y + LINE_1_Y,
				colors_to24bits(UNIT_FG_COLOR), colors_to24bits(
						UNIT_SCREEN_COLOR));
		//flag
		c=all_units[findOffset() + d->d1].country - 1;
		if (c>=0 && c<total_flags)
		masked_blit(flag_bmp[all_units[findOffset() + d->d1].country - 1],
				screen, 0, 0, unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2
						+ 8 + UNIT_TYPE_X_SHIFT, unitslist_dlg[0].y + LINE_1_Y + 35, 60,
				50);
		else
			rectfill(screen,
					unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2 + 8 + UNIT_TYPE_X_SHIFT+ 20,
					unitslist_dlg[0].y + LINE_1_Y + 35+ 36,
					unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2	+ 8 + UNIT_TYPE_X_SHIFT+ 20+20,
					unitslist_dlg[0].y + LINE_1_Y + 35+ 36+50,
					colors_to24bits(UNIT_SCREEN_COLOR));
		//str
		bmp_off = str_bmp_offset(findOffset() + d->d1);
		masked_blit(
				strength_bmp[all_units[findOffset() + d->d1].str + bmp_off],
				screen, 0, 0, unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2
						+ 8 - 20 + UNIT_TYPE_X_SHIFT, unitslist_dlg[0].y + LINE_1_Y + 35
						+ 17, 60, 50);
		//ent
		masked_blit(strength_bmp[all_units[findOffset() + d->d1].entrench
				+ bmp_off], screen, 0, 0, unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2
				+ UNIT_LIST_W / 2 + 8 - 0 + UNIT_TYPE_X_SHIFT, unitslist_dlg[0].y
				+ LINE_1_Y + 35 + 17, 60, 50);
		//exp
		masked_blit(
				strength_bmp[all_units[findOffset() + d->d1].exp + bmp_off],
				screen, 0, 0, unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2
						+ 8 + 20 + UNIT_TYPE_X_SHIFT, unitslist_dlg[0].y + LINE_1_Y + 35
						+ 17, 60, 50);
		//organic
		if (all_units[findOffset() + d->d1].orgtnum > 0)
			display_unit_info(all_units[findOffset() + d->d1].orgtnum, unitslist_dlg[0].x
					+ UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2 + 80
					+UNIT_TYPE_X_SHIFT,
					unitslist_dlg[0].y + LINE_1_Y + UNIT_TYPE_Y_SPACE, colors_to24bits(
							UNIT_FG_COLOR), colors_to24bits(UNIT_SCREEN_COLOR));
		else
			rectfill(screen, unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2
					+UNIT_TYPE_X_SHIFT + 8, unitslist_dlg[0].y + LINE_1_Y
					+ UNIT_TYPE_Y_SPACE, unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2
					+ UNIT_LIST_W / 2 + 80 + INFO_W + UNIT_TYPE_X_SHIFT,
					unitslist_dlg[0].y + LINE_1_Y + UNIT_TYPE_Y_SPACE + INFO_H,
					colors_to24bits(UNIT_SCREEN_COLOR));
		//transport
		if (all_units[findOffset() + d->d1].auxtnum > 0)
			display_unit_info(all_units[findOffset() + d->d1].auxtnum, unitslist_dlg[0].x
					+ UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2 + 80
					+UNIT_TYPE_X_SHIFT, unitslist_dlg[0].y + LINE_1_Y + UNIT_TYPE_Y_SPACE
					* 2, colors_to24bits(UNIT_FG_COLOR), colors_to24bits(
					UNIT_SCREEN_COLOR));
		else
			rectfill(screen, unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2 + UNIT_LIST_W / 2
					+UNIT_TYPE_X_SHIFT + 8, unitslist_dlg[0].y + LINE_1_Y
					+ UNIT_TYPE_Y_SPACE * 2, unitslist_dlg[0].x + UNIT_TYPE_RB_X * 2
					+ UNIT_LIST_W / 2 + 80 + INFO_W + UNIT_TYPE_X_SHIFT,
					unitslist_dlg[0].y + LINE_1_Y + UNIT_TYPE_Y_SPACE * 2 + INFO_H,
					colors_to24bits(UNIT_SCREEN_COLOR));

	}
}

int d_ullist_proc(int msg, DIALOG *d, int c) {
	int d1;
	if ((msg == MSG_CLICK) || (msg == MSG_CHAR)) {
		//if (c==KEY_UP || c==KEY_DOWN || c==KEY_PGDN || c==KEY_PGUP || msg == MSG_CLICK)
		d1 = d->d1;
		d_list_proc(msg, d, c);
		//d1 = d->d1;
		if (d1 > -1 || msg == MSG_CLICK)
			draw_unit(d);

		if (msg == MSG_CLICK)
			return D_O_K;
		if (msg == MSG_CHAR) {
			//if (c==KEY_UP || c==KEY_DOWN || c==KEY_PGDN || c==KEY_PGUP)
			if (d1 != d->d1)
				return D_USED_CHAR;
			else
				return D_O_K;
		}
	}
	return d_list_proc(msg, d, c);
}

void find_xy(int idx, unsigned char isAir, int *x, int *y) {

	int x0, y0;
	int i, j, r;
	if (idx >= 0) {
		*x = all_units[idx].x;
		*y = all_units[idx].y;
	} else {
		*x = Min((map_x0+tiles_wide/2),mapx);
		*y = Min((map_y0+tiles_high/2),mapy);
	}
	*x = Max(*x ,0);
	*y = Max(*y ,0);
	//check exactly center
	if (isAir && map[*x][*y].auidx < 0)
		return;
	if (isAir == 0 && map[*x][*y].guidx < 0)
		return;

	for (r = 1; r <= 6; r++) {
		for (i = 0; i <= r; i++)
			for (j = -r + i / 2; j <= r - (i + 1) / 2; j++) {
				x0 = *x + i;
				y0 = *y + j + *x % 2 * (*x + i + 1) % 2;
				if (x0 >= 0 && x0 <= mapx && y0 >= 0 && y0 <= mapy) {
					if (isAir && map[x0][y0].auidx < 0) {
						*x = x0;
						*y = y0;
						return;
					}
					if (isAir == 0 && map[x0][y0].guidx < 0) {
						*x = x0;
						*y = y0;
						return;
					}
				}
				x0 = *x - i;
				y0 = *y + j + *x % 2 * (*x + i + 1) % 2;
				if (x0 >= 0 && x0 <= mapx && y0 >= 0 && y0 <= mapy) {
					if (isAir && map[x0][y0].auidx < 0) {
						*x = x0;
						*y = y0;
						return;
					}
					if (isAir == 0 && map[x0][y0].guidx < 0) {
						*x = x0;
						*y = y0;
						return;
					}
				}
			}
	}
	return;
}

int d_bb_proc(int msg, DIALOG *d, int c) {
	int d1, idx, x, y;
	int listsize;

	/* typedef for the listbox callback functions */
	typedef char *(*getfuncptr)(int, int *);

	if ((msg == MSG_CLICK) || (msg == MSG_KEY)) {
		d1 = d_button_proc(msg, d, c);
		if (&(unitslist_dlg[uldDeleteBtnIdx]) == d) {
			idx = unitslist_dlg[uldUnitListIdx].d1;
			if (idx >= 0) {
				idx += findOffset();

				x = all_units[idx].x;
				y = all_units[idx].y;
				if (map[x][y].guidx == idx)
					map[x][y].guidx = -1;
				if (map[x][y].auidx == idx)
					map[x][y].auidx = -1;
				remove_unit_idx(idx);
				object_message(&unitslist_dlg[uldUnitListIdx], MSG_START, 0);
				(*(getfuncptr) unitslist_dlg[uldUnitListIdx].dp)(-1, &listsize);
				if (listsize == 0)
					unitslist_dlg[uldUnitListIdx].d1 = -1;
			}
			//do not allow button to be pressed
			d->flags &= ~D_SELECTED;
			unitslist_dlg[uldUnitListIdx].flags |= D_DIRTY;
			//broadcast_dialog_message(MSG_DRAW, c);
		}

		if (&(unitslist_dlg[uldUpBtnIdx]) == d) {
			idx = unitslist_dlg[uldUnitListIdx].d1;
			if (idx > 0) {
				idx += findOffset();
				// change map
				swap_two_units_with_map(idx, idx - 1);
				unitslist_dlg[uldUnitListIdx].d1--;
				object_message(&unitslist_dlg[uldUnitListIdx], MSG_START, 0);
			}
			//do not allow button to be pressed
			d->flags &= ~D_SELECTED;
			unitslist_dlg[uldUnitListIdx].flags |= D_DIRTY;
			//broadcast_dialog_message(MSG_DRAW, c);
		}

		if (&(unitslist_dlg[uldDownBtnIdx]) == d) {
			idx = unitslist_dlg[uldUnitListIdx].d1;
			if (idx < findSize() - 1) {
				idx += findOffset();
				// change map
				swap_two_units_with_map(idx, idx + 1);
				unitslist_dlg[uldUnitListIdx].d1++;

				object_message(&unitslist_dlg[uldUnitListIdx], MSG_START, 0);
			}
			//do not allow button to be pressed
			d->flags &= ~D_SELECTED;
			unitslist_dlg[uldUnitListIdx].flags |= D_DIRTY;
			//broadcast_dialog_message(MSG_DRAW, c);
		}

		if (&(unitslist_dlg[uldAddBtnIdx]) == d) {

			centre_dialog(unit_dlg);
			if (do_dialog(unit_dlg, -1) == 1) /* DONE */{
				//done was pressed
				unum_selected = atoi(uselected);
				tnum_selected = atoi(tselected);
				cnum_selected = atoi(cselected);
				anum_selected = atoi(aselected);

				ustr_selected = 10;
				uexp_selected = 0;
				uent_selected = 0;

				if (unitslist_dlg[uldUnitListIdx].d1 != -1) {
					//add at cursor
					idx = unitslist_dlg[uldUnitListIdx].d1;
					idx += findOffset();
				} else {
					//add as last of current list
					idx = findMaxIdx();
				}
				//TODO
				find_xy(-1, equip[unum_selected][GAF], &x, &y);

				add_unit(idx);
				all_units[idx].unum = unum_selected;
				all_units[idx].orgtnum = tnum_selected;
				all_units[idx].country = cnum_selected;
				all_units[idx].auxtnum = anum_selected;
				all_units[idx].x = x;
				all_units[idx].y = y;
				all_units[idx].str = ustr_selected;
				all_units[idx].entrench = uent_selected;
				all_units[idx].exp = uexp_selected;
				all_units[idx].uflag = unit_type;
				all_units[idx].fuel = -1;
				all_units[idx].ammo = -1;

				if (equip[unum_selected][GAF] == 1)
					map[x][y].auidx = idx;
				else
					map[x][y].guidx = idx;

				++total_units;
				switch (unit_type) {
				case axis_core:
					total_axis_core++;
					break;
				case axis_aux:
					total_axis_aux++;
					break;
				case allied_core:
					total_allied_core++;
					break;
				case allied_aux:
					total_allied_aux++;
					break;
				}
			}

			//do not allow button to be pressed
			d->flags &= ~D_SELECTED;
			broadcast_dialog_message(MSG_DRAW, c);
		}

		if (&(unitslist_dlg[uldGoToBtnIdx]) == d) {
			if (unitslist_dlg[uldUnitListIdx].d1 != -1) {
				idx = unitslist_dlg[uldUnitListIdx].d1;
				idx += findOffset();

				map_x0
						=Min((all_units[idx].x-tiles_wide/2),get_h_slide_max());
				map_x0 = Max(map_x0,0);
				map_y0
						=Min((all_units[idx].y-tiles_high/2),get_v_slide_max());
				map_y0 = Max(map_y0,0);

				draw_map(map_bmp, map_x0, map_y0, tiles_high, tiles_wide);
				main_dlg[dmHSlideIdx].d2 = map_x0;
				main_dlg[dmVSlideIdx].d2 = vslide_max - map_y0;

				main_dlg[dmHSlideIdx].flags |= D_DIRTY;
				main_dlg[dmVSlideIdx].flags |= D_DIRTY;
				//d_slider_proc(MSG_DRAW, &main_dlg[dmHSlideIdx], 0);
				//d_slider_proc(MSG_DRAW, &main_dlg[dmVSlideIdx], 0);
				main_dlg[dmMapBmpIdx].flags |= D_DIRTY;
				//d_bitmap_proc(MSG_DRAW, &main_dlg[dmMapBmpIdx], c);
			}
		}

		if (&(unitslist_dlg[uldCopyBtnIdx]) == d) {
			if (unitslist_dlg[uldUnitListIdx].d1 != -1) {
				//add at cursor
				idx = unitslist_dlg[uldUnitListIdx].d1;
				idx += findOffset();

				//TODO
				find_xy(idx, equip[all_units[idx].unum][GAF], &x, &y);

				add_unit(idx + 1); //add as next unit
				all_units[idx + 1].unum = all_units[idx].unum;
				all_units[idx + 1].orgtnum = all_units[idx].orgtnum;
				all_units[idx + 1].country = all_units[idx].country;
				all_units[idx + 1].auxtnum = all_units[idx].auxtnum;
				all_units[idx + 1].x = x;
				all_units[idx + 1].y = y;
				all_units[idx + 1].str = all_units[idx].str;
				all_units[idx + 1].entrench = all_units[idx].entrench;
				all_units[idx + 1].exp = all_units[idx].exp;
				all_units[idx + 1].uflag = all_units[idx].uflag;
				all_units[idx + 1].fuel = all_units[idx].fuel;
				all_units[idx + 1].ammo = all_units[idx].ammo;

				if (equip[all_units[idx].unum][GAF] == 1)
					map[x][y].auidx = idx;
				else
					map[x][y].guidx = idx;

				++total_units;
				switch (unit_type) {
				case axis_core:
					total_axis_core++;
					break;
				case axis_aux:
					total_axis_aux++;
					break;
				case allied_core:
					total_allied_core++;
					break;
				case allied_aux:
					total_allied_aux++;
					break;
				}
			}

			//do not allow button to be pressed
			d->flags &= ~D_SELECTED;
			unitslist_dlg[uldUnitListIdx].flags |= D_DIRTY;
			//broadcast_dialog_message(MSG_DRAW, c);
		}
		return d1;
	}
	return d_button_proc(msg, d, c);
}

/* here we define the unit dialog components */
DIALOG unitslist_dlg[ULD_DLG] = {
//item 0 the clear routine
		{ d_textbox_proc, 0, 0, UNIT_LIST_W, UNIT_LIST_H, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 0, 0, 0, 0, 0 },

		//item 1 the done button
		{ d_button_proc, 640 - BUTTON_W * 2, UNIT_LIST_H - 2 * BUTTON_H, UL_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 'd', D_EXIT, 0, 0, (void *) "&Done", 0, 0 },
		{ d_bb_proc, 640 - BUTTON_W * 4, UNIT_LIST_H - 2 * BUTTON_H, UL_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 'p', 0, 0, 0, (void *) "U&p", 0, 0 },
		{ d_bb_proc, 640 - BUTTON_W * 6, UNIT_LIST_H - 2 * BUTTON_H, UL_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 'n', 0, 0, 0, (void *) "Dow&n", 0, 0 },
		{ d_bb_proc, 640 - BUTTON_W * 8, UNIT_LIST_H - 2 * BUTTON_H, UL_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 'e', 0, 0, 0, (void *) "D&el", 0, 0 },
		{ d_bb_proc, 640 - BUTTON_W * 10, UNIT_LIST_H - 2 * BUTTON_H, UL_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, '+', 0, 0, 0, (void *) "Add&+", 0, 0 },
		{ d_bb_proc, 640 - BUTTON_W * 12, UNIT_LIST_H - 2 * BUTTON_H, UL_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 'c', 0, 0, 0, (void *) "&Copy", 0, 0 },
		{ d_bb_proc, 640 - BUTTON_W * 14, UNIT_LIST_H - 2 * BUTTON_H, UL_BUTTON_W, BUTTON_H,UNIT_FG_COLOR, UNIT_BG_COLOR, 'g', D_EXIT, 0, 0, (void *) "&GoTo", 0, 0 },
		{ d_coreUL_proc, UNIT_TYPE_RB_X + 16, 8, 12 + 4 * 8, 10, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'r', D_SELECTED, 99, 0, (void *) "Co&re", 0, 0 },
		{ d_coreUL_proc, UNIT_TYPE_RB_X + 50 + 16, 8, 12 + 3 * 8, 10, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'u', 0, 99, 0, (void *) "A&ux", 0, 0 },
		{ d_coreUL_proc, UNIT_TYPE_RB_X + 100 + 16, 8, 12 + 6 * 8, 10, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'l', 0, 99, 0, (void *) "A&llied", 0, 0 },
		{ d_coreUL_proc, UNIT_TYPE_RB_X + 100 + 8 * (6 + 3) + 16, 8, 12 + 10 * 8, 10, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 0, 0, 99, 0, (void *) "Allied aux", 0, 0 },
		//item the list of appropriate units
		{ d_ullist_proc, UNIT_TYPE_RB_X, LINE_1_Y, UNIT_LIST_W / 2 - 2 * UNIT_TYPE_RB_X, UNIT_LIST_H - LINE_1_Y - 3 * BUTTON_H, UNIT_FG_COLOR, UNIT_BG_COLOR, 0, 0, 0, 0, (void *) add_lunits, 0, 0 },
		//pannel check box
		{ d_pannel_proc, UNIT_TYPE_RB_X + 200 + 16 + 8 * 12, 8, 12 + 6 * 8, 10, UNIT_FG_COLOR, UNIT_SCREEN_COLOR, 'a', D_SELECTED, 1, 0, (void *) "P&anel", 0, 0 },

		{ d_yield_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		// last item
		{ NULL } };
