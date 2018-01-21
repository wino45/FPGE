/*
 * randomize.h
 *
 *  Created on: 2010-08-24
 *      Author: wino
 */

#ifndef RANDOMIZE_H_
#define RANDOMIZE_H_

#define RANDOMIZE_DLG_SIZE 15

extern DIALOG randomize_dlg[RANDOMIZE_DLG_SIZE];

void forest_random();
void randomize_tiles(int);
int randomize_dialog();
void randomize_patterns(int type);

#endif /* RANDOMIZE_H_ */
