#ifndef __tools_h_
#define __tools_h_

/**
 **    Um_code
 **/
typedef struct _um_code {
  unsigned int * code;
  unsigned int int_size;
} Um_code;

void destroyCode(Um_code * code);
Um_code ** realloc_and_copy(Um_code ** source, 
			    unsigned int oldSize, unsigned int newSize);
Um_code * readscroll(FILE * fp);

/**
 **    Free_Queue
 **/
typedef struct _free_queue {
  struct _free_queue * next;
  unsigned int position;
} Free_Queue;


unsigned int popEltValue(Free_Queue ** fq);
void pushValue(Free_Queue ** fq, unsigned int value);
void destroyFreeQueue(Free_Queue * fq);

#endif
