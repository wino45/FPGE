/*
 * undo.c
 *
 *  Created on: 2011-08-21
 *      Author: wino
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "undo.h"
#include "minimap.h"
#include "cyclic_buf.h"

int undo_sequence;

void init_undo(){
	init_cbuf(UNDO_STACK_SIZE);
	undo_sequence=0;
}

void free_undo(){
	free_cbuf();
}

void open_push_undo_sequenece(){

}
void close_push_undo_sequenece(){
	undo_sequence++;
}

void push_undo_tile(int x, int y){

	void * ptr= malloc(sizeof(struct MAP_undo));
	struct MAP_undo *undo_map_element = (struct MAP_undo*)ptr;
	undo_map_element->sequence_no=undo_sequence;
	undo_map_element->x=x;
	undo_map_element->y=y;
	memcpy(&undo_map_element->element, &map[x][y], sizeof(struct MAP));
#ifdef UNDO_DEBUG
	printf("UPUSH: x=%3d y=%3d sq=%3d tile=%3d ptr=%p\n",
			undo_map_element->x ,undo_map_element->y,
			undo_map_element->sequence_no, undo_map_element->element.tile,ptr );
#endif
	push_cbuf(ptr);
}

void push_undo_one_tile(int x, int y){
	open_push_undo_sequenece();
	push_undo_tile(x,y);
	close_push_undo_sequenece();
}

int pull_undo(){
	if (get_cbuf_fill()==0) {
#ifdef UNDO_DEBUG
		printf("UPULL: empty on entry\n");
#endif
		return 0; //no more undo elements
	}

	mem_ptr * ptr;
	struct MAP_undo *undo_map_element;
	int current_sequence, processed=0;

	ptr= peek_cbuf();
	undo_map_element = (struct MAP_undo *)ptr;
	current_sequence=undo_map_element->sequence_no;

	do {
		ptr= pull_cbuf();
		undo_map_element = (struct MAP_undo *)ptr;
#ifdef UNDO_DEBUG
		printf("UPULL: x=%3d y=%3d sq=%3d tile=%3d ptr=%p\n",
				undo_map_element->x ,
				undo_map_element->y,
				undo_map_element->sequence_no,
				undo_map_element->element.tile,
				ptr);
#endif
		//do stuff
		memcpy(&map[undo_map_element->x][undo_map_element->y], &undo_map_element->element, sizeof(struct MAP));

		processed++;
		free(ptr);

		if (get_cbuf_fill()==0) {
#ifdef UNDO_DEBUG
				printf("UPULL: empty on seq, break\n");
#endif
				break; //exit while
				//return processed; //no more undo elements
			}

		//process next ?
		ptr= peek_cbuf();
		undo_map_element = (struct MAP_undo *)ptr;
	}while (current_sequence==undo_map_element->sequence_no);
#ifdef UNDO_DEBUG
	printf("UPULL: END OF SEQ\n");
#endif

	// update scan ranges
	if (show_ranges != 2)
		compute_shade(show_ranges);

	return processed;
}

void test_undo(){

	init_undo();

	push_undo_one_tile(1,1);
	push_undo_one_tile(2,1);
	push_undo_one_tile(3,1);
	push_undo_one_tile(4,1);

	open_push_undo_sequenece();
	push_undo_tile(2,1);
	push_undo_tile(2,2);
	push_undo_tile(2,3);
	push_undo_tile(2,4);
	close_push_undo_sequenece();

	pull_undo();
	pull_undo();
	pull_undo();
	pull_undo();
	pull_undo();

	free_undo();
}
