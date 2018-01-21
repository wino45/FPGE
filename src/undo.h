/*
 * undo.h
 *
 *  Created on: 2011-08-21
 *      Author: wino
 */

#ifndef UNDO_H_
#define UNDO_H_

#include "fpge.h"

//#define UNDO_DEBUG

#define UNDO_STACK_SIZE 10000000

struct MAP_undo {
	struct MAP element;
	int sequence_no;
	int x;
	int y;
};

void init_undo();
void free_undo();
void open_push_undo_sequenece();
void close_push_undo_sequenece();

void push_undo_tile(int x, int y);
void push_undo_one_tile(int x, int y);
int pull_undo();

void test_undo();

#endif /* UNDO_H_ */
