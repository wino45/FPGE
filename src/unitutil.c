/*
 * unitutil.c
 *
 *  Created on: 2010-05-06
 *      Author: wino
 */
#include "fpge.h"

//copy unit
void copy_unit(struct UNIT *dest, struct UNIT *src)
{
   dest->unum=src->unum;
   dest->orgtnum=src->orgtnum;
   dest->country=src->country;
   dest->auxtnum=src->auxtnum;
   dest->x=src->x;
   dest->y=src->y;
   dest->str=src->str;
   dest->entrench=src->entrench;
   dest->exp=src->exp;
   dest->uflag=src->uflag;
   dest->fuel=src->fuel;
   dest->ammo=src->ammo;
}

void swap_two_units_with_map(int idx, int idx1) {

	int x, y, a_or_g = 0;
	int x1, y1, a_or_g1 = 0;
	struct UNIT temp_unit;

	// we must do it like this, first read both, then set both. Think of case:
	// map[x][y].guidx==idx and map[x][y].auidx==idx-1

	//first unit
	x = all_units[idx].x;
	y = all_units[idx].y;
	if (map[x][y].guidx == idx)
		a_or_g = 2;
	if (map[x][y].auidx == idx)
		a_or_g = 1;

	//second unit
	x1 = all_units[idx1].x;
	y1 = all_units[idx1].y;
	if (map[x1][y1].guidx == idx1)
		a_or_g1 = 2;
	if (map[x1][y1].auidx == idx1)
		a_or_g1 = 1;

	if (a_or_g == 1)
		map[x][y].auidx = idx1;
	if (a_or_g == 2)
		map[x][y].guidx = idx1;
	if (a_or_g1 == 1)
		map[x1][y1].auidx = idx;
	if (a_or_g1 == 2)
		map[x1][y1].guidx = idx;

	// copy units
	copy_unit(&temp_unit, &(all_units[idx1]));
	copy_unit(&(all_units[idx1]), &(all_units[idx]));
	copy_unit(&(all_units[idx]), &temp_unit);
}

void correct_map_indexes(int idx, int step){
  int mx,my;

  for (mx=0; mx<mapx; ++mx)
   for (my=0; my<mapy; ++my)
   {
      if (map[mx][my].guidx>idx) map[mx][my].guidx+=step;
      if (map[mx][my].auidx>idx) map[mx][my].auidx+=step;
   }

}

void remove_unit_idx(int idx){
	int i;

	if (idx >= 0) {
		switch (all_units[idx].uflag) {
		case axis_core:
			total_axis_core--;
			break;
		case axis_aux:
			total_axis_aux--;
			break;
		case allied_core:
			total_allied_core--;
			break;
		case allied_aux:
			total_allied_aux--;
			break;
		}
	}

	all_units[idx].unum = -1;
	for (i = 0; i < total_units - 1; ++i) {
		if (all_units[i].unum <= 0) {
			copy_unit(&(all_units[i]), &(all_units[i + 1]));
			all_units[i + 1].unum = -1; //unit deleted
		}
	} //end for i
	//now need to correct the map indexes
	correct_map_indexes(idx, -1);

	--total_units;
}



//remove the ground unit at x,y on the map
//and the array
void remove_gunit(int x, int y) {
	int idx;

	idx = map[x][y].guidx;
	if (idx >= 0) //there is a unit
	{
		map[x][y].guidx = -1;
		remove_unit_idx(idx);
	}

}

//remove the air unit at x,y on the map
//and the array
void remove_aunit(int x, int y) {
	int idx;

	idx = map[x][y].auidx;
	if (idx >= 0) //there is a unit
	{
		map[x][y].auidx = -1;
		remove_unit_idx(idx);
	}
}

void add_unit(int idx)
{
  int i;
  //int mx,my;

  if (idx>=0)  //there is a unit
  {
    for (i=total_units; i>idx; i--)
         copy_unit(&(all_units[i]),&(all_units[i-1]));
    memset(&(all_units[i]),0,sizeof(struct UNIT));
  }
  //now need to correct the map indexes
  correct_map_indexes(idx-1,1);
}

int compare_units(const void *ii, const void *jj)
{
    struct UNIT *i=(struct UNIT *)ii;
    struct UNIT *j=(struct UNIT *)jj;

    if (i->uflag!=j->uflag) return i->uflag-j->uflag; // type
    if (equip[i->unum][CLASS]!=equip[j->unum][CLASS]) return equip[i->unum][CLASS]-equip[j->unum][CLASS]; // type
    return i->unum-j->unum;
    /*
    if (strcmp(i->name,j->name)!=0)
    	return strcmp(i->name,j->name);
    return i->country-j->country;
    */
}

int sort_units(){
	qsort(all_units,total_units,sizeof(struct UNIT),compare_units);
	return D_REDRAW;
}

void clear_all_units(){
int i;
//clear all units
for(i=0;i<MAX_AXIS_UNITS+MAX_ALLIED_UNITS+MAX_AXIS_AUX_UNITS;i++) all_units[i].unum=-1;
  //units
  total_axis_core=0;
  total_allied_core=0;
  total_axis_aux=0;
  total_allied_aux=0;
  total_units=0;
}

int add_unit_type(int is_allied, int is_aux) {

	int where_add_new=-1;
	if (is_allied == 1) { //allied
		if (is_aux == 1) { //aux
			where_add_new = total_axis_core + total_axis_aux + total_allied_core + total_allied_aux;
			add_unit(where_add_new);
			all_units[where_add_new].uflag = allied_aux;
			total_allied_aux++;
		} else {
			where_add_new = total_axis_core + total_axis_aux + total_allied_core;
			add_unit(where_add_new);
			all_units[where_add_new].uflag = allied_core;
			total_allied_core++;
		}
	}
	if (is_allied == 0) { //axis
		if (is_aux == 1) { //aux
			where_add_new = total_axis_core + total_axis_aux;
			add_unit(where_add_new);
			all_units[where_add_new].uflag = axis_aux;
			total_axis_aux++;
		} else {
			where_add_new = total_axis_core;
			add_unit(where_add_new);
			all_units[where_add_new].uflag = axis_core;
			total_axis_core++;
		}
	}
	return where_add_new;
}
