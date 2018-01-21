/*
 * unitutil.h
 *
 *  Created on: 2010-05-06
 *      Author: wino
 */

#ifndef UNITUTIL_H_
#define UNITUTIL_H_

void correct_map_indexes(int idx, int step);
void copy_unit(struct UNIT *dest, struct UNIT *src);
void swap_two_units_with_map(int idx, int idx1);
void remove_unit_idx(int idx);
void remove_gunit(int x,int y);
void remove_aunit(int x,int y);
void add_unit(int idx);
int sort_units();
void clear_all_units();
int add_unit_type(int is_allied, int is_aux);

#endif /* UNITUTIL_H_ */
