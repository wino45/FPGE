/*
 * cyclic_buf.h
 *
 *  Created on: 2011-08-21
 *      Author: wino
 */

#ifndef CYCLIC_BUF_H_
#define CYCLIC_BUF_H_

//#define CBUF_DEBUG

typedef void * mem_ptr;

int init_cbuf(int size);
int free_cbuf();
int get_cbuf_fill();
int push_cbuf(void *ptr_to_store);
void *pull_cbuf();
void *peek_cbuf();

void test_cbuf();

#endif /* CYCLIC_BUF_H_ */
