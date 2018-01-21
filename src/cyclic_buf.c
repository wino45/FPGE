/*
 * cyclic_buf.c
 *
 *	Implementation of cyclic stack -stack with limited size
 *
 *  Created on: 2011-08-21
 *      Author: wino
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cyclic_buf.h"

int cbuf_size;
int cbuf_filled;
int cbuf_bottom;
int cbuf_top;
void *cbuf;

int init_cbuf(int size){
	cbuf_size=size;
	cbuf_bottom=0;
	cbuf_top=-1;
	cbuf_filled=0;
	cbuf=malloc(cbuf_size * sizeof(void *));
	return cbuf_size;
}

int free_cbuf(){
	free(cbuf);
	return 0;
}

int get_cbuf_fill(){
	return cbuf_filled;
}

int push_cbuf(void *ptr_to_store){
	mem_ptr *address;

	if (cbuf_top+1<cbuf_size){
		if (cbuf_filled==cbuf_size){
			cbuf_top++;
			cbuf_bottom=cbuf_top+1;
		}else{
			cbuf_top++;
			cbuf_filled++;
		}
	}else{
		//overflow
		cbuf_top=0;
		cbuf_bottom=cbuf_top+1;
	}

	address=cbuf+cbuf_top * sizeof(void *);
	*address=ptr_to_store;

	//int *p = (int *)address;
#ifdef CBUF_DEBUG
	printf("PUSH: size=%d bottom=%d top=%d filled=%d address=%p avalue=%p\n", cbuf_size, cbuf_bottom, cbuf_top, cbuf_filled, address, *address);
#endif
	return 0;
}

void *pull_cbuf(){
	mem_ptr *address=NULL;

	if (cbuf_filled==0) {
		//printf("PULL: size=%d bottom=%d top=%d filled=%d address=%p avalue=%d\n", cbuf_size, cbuf_bottom, cbuf_top, cbuf_filled, address, *(int *)address);
#ifdef CBUF_DEBUG
		printf("PULL: empty\n");
#endif
		return NULL; //empty
	}

	address=cbuf+cbuf_top * sizeof(void *);
	//now adjust
	cbuf_filled--;
	cbuf_top--;
	if (cbuf_top<0 && cbuf_filled>0) cbuf_top=cbuf_size-1;
#ifdef CBUF_DEBUG
	printf("PULL: size=%d bottom=%d top=%d filled=%d address=%p avalue=%p\n", cbuf_size, cbuf_bottom, cbuf_top, cbuf_filled, address, *address);
#endif
	return *address;
}

void *peek_cbuf(){
	mem_ptr *address=NULL;

	if (cbuf_filled==0) {
		//printf("PULL: size=%d bottom=%d top=%d filled=%d address=%p avalue=%d\n", cbuf_size, cbuf_bottom, cbuf_top, cbuf_filled, address, *(int *)address);
#ifdef CBUF_DEBUG
		printf("PULL: empty\n");
#endif
		return NULL; //empty
	}

	address=cbuf+cbuf_top * sizeof(void *);
	// just return current element
#ifdef CBUF_DEBUG
	printf("PEEK: size=%d bottom=%d top=%d filled=%d address=%p avalue=%p\n", cbuf_size, cbuf_bottom, cbuf_top, cbuf_filled, address, *address);
#endif
	return *address;
}


void test_cbuf(){

//	printf("sizeof(void *)=%d\n",sizeof(void *));
	init_cbuf(3);

	push_cbuf((void *)1);
	push_cbuf(NULL);
	push_cbuf(NULL);
	push_cbuf(NULL);
	push_cbuf((void *)1);
	push_cbuf((void *)2);
	push_cbuf((void *)3);

	pull_cbuf();
	pull_cbuf();
	pull_cbuf();
	pull_cbuf();

	push_cbuf((void *)4);
	push_cbuf((void *)5);
	push_cbuf((void *)6);
	push_cbuf((void *)7);
	push_cbuf((void *)8);

	pull_cbuf();
	pull_cbuf();
	pull_cbuf();
	pull_cbuf();

	free_cbuf();
}
