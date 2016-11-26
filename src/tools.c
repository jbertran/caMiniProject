#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tools.h"

Um_code * newPlatter(unsigned int size) {
  Um_code * result = malloc(sizeof *result);
  result->code = calloc(size, 4);
  result->int_size = size;
  return result;
}

void destroyCode(Um_code * code) {
  if (code) {
    free(code->code);
    free(code);
  }
}

Um_code * readscroll (FILE * fp) {
  int size;
  unsigned int * code;

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  code = calloc(size/4, 4);

  if (fread(code, 4, size/4, fp) != size/4) {
    fprintf(stderr, "\x1b[31mFile error\x1b[0m\n");
    exit(1);
  }
  for (unsigned int i = 0; i < size / 4; i++) {
    code[i] =
      (code[i] >> 24) |
      ((code[i] & 0x00FF0000) >> 8) |
      ((code[i] & 0x0000FF00) << 8) |
      (code[i] << 24);
  }
  Um_code * result = malloc(sizeof *result);
  result->code = code;
  result->int_size = size;
  return result;
}


void destroyFreeQueue(Free_Queue * fq) {
  Free_Queue * tmp;
  while (fq) {
    tmp = fq->next;
    free(fq);
    fq = tmp;
  }
}

unsigned int popEltValue(Free_Queue ** fq) {
  Free_Queue * tmp = *fq;
  Free_Queue * newH = tmp->next;
  unsigned int value = tmp->position;
  *fq = newH;
  free(tmp);
  return value;
}

void pushValue(Free_Queue ** fq, unsigned int value) {
  Free_Queue * newH = malloc(sizeof(*newH));
  newH->next = *fq;
  newH->position = value;
  *fq = newH;
}
