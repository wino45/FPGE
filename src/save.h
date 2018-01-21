/*
 * save.h
 *
 *  Created on: 2010-03-09
 *      Author: wino
 */

#ifndef SAVE_H_
#define SAVE_H_

#define CONVERSION_NOT_FOUND 0
#define CONVERSION_FOUND 1
#define CONVERSION_FOUND_NEW_COUNTRY 2
#define CONVERSION_GENERIC 3
#define CONVERSION_FOUND_NEW_NAME 4


int convert_equipment_hard(int *result, int *new_country, int equipment_ID, int country,int);
int convert_equipment(int equipment_ID, int country,int display_ids, int );
void write_set(FILE *outf);
int save_set(int n, int);
void write_stm(FILE *outf);
int save_stm(int n, int);
void write_scenario(FILE *outf, int save_type);
int save_scenario(int,int);
int save_names();
int save_equip(char *file_name);

#endif /* SAVE_H_ */
