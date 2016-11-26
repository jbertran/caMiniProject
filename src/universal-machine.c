#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "tools.h"

//#define DEBUG 1
#define DEFAULT_PLATTERS 64000

#ifdef DEBUG
#define debug(str) fprintf(stderr, "%s regA = %u; regB = %u; regC = %u\n", str, registers[WORDA], registers[WORDB], registers[WORDC]);
#else
#define debug(str) ;
#endif

#define Opcode(word) (word >> 28)
#define WORDA ((current_word >> 6) & 0x07)
#define WORDB ((current_word >> 3) & 0x07)
#define WORDC (current_word & 0x07)

#define Next_instr current_word = platters[0]->code[pc++]; goto *labels[Opcode(current_word)];

void um_exec(Um_code * code, FILE * out) {
  Um_code ** platters = malloc(sizeof(*platters) * DEFAULT_PLATTERS);
  unsigned int pc = 0;
  platters[0] = code;
  unsigned int current_word = platters[0]->code[pc];
  unsigned int registers[8] = { 0 };
  unsigned int next_allocate = 1;
  unsigned int allocate_max = DEFAULT_PLATTERS;
  Free_Queue * spaces = NULL;

  static void * labels [] = { &&CMV, &&AIND, &&AAMD, &&ADD, &&MUL, &&DIV, &&NAND,
			      &&HALT, &&ALLOC, &&ABDN, &&OUT, &&IN, &&LPRO, &&ORTH };

  goto *labels[Opcode(current_word)];

 CMV:
  debug("Conditional move");
  if (registers[WORDC] != 0) {
    registers[WORDA] = registers[WORDB];
  }
  Next_instr;
 AIND:
  debug("Array Index");
  registers[WORDA] = platters[registers[WORDB]]->code[registers[WORDC]];
  Next_instr;
 AAMD:
  debug("Array amendment");
  if (platters[registers[WORDA]] == NULL) {
    fprintf(stderr, "null\n");
  }
  platters[registers[WORDA]]->code[registers[WORDB]] = registers[WORDC];
  Next_instr;
 ADD:
  debug("Addition");
  registers[WORDA] = registers[WORDB] + registers[WORDC];
  Next_instr;
 MUL:
  debug("Multiplication");
  registers[WORDA] = registers[WORDB] * registers[WORDC];
  Next_instr;
 DIV:
  debug("Division");
  if (registers[WORDC])
    registers[WORDA] = registers[WORDB] / registers[WORDC];
  Next_instr;
 NAND:
  debug("Not-And");
  registers[WORDA] = ~(registers[WORDB] & registers[WORDC]);
  Next_instr;
 HALT:
  debug("Halt");
  destroyFreeQueue(spaces);
  for (unsigned int i = 0; i < allocate_max; i++)
    destroyCode(platters[i]);
  free(platters);
  exit(1);
 ALLOC:
  debug("Allocation");
  unsigned int size = registers[WORDC];
  unsigned int position = 0;
  Um_code * new_platter = malloc(sizeof *new_platter);
  new_platter->code = calloc(size, 4);
  new_platter->int_size = size;
  if (spaces != NULL) {
    position = popEltValue(&spaces);
  } else {
    if (next_allocate >= allocate_max) {
      position = allocate_max;
      allocate_max *= 2;
      platters = realloc(platters, allocate_max * sizeof(Um_code));
      next_allocate++;
    } else {
      position = next_allocate++;
    }
  }
  platters[position] = new_platter;
  registers[WORDB] = position;
  Next_instr;
 ABDN:
  debug("Abandonment");
  free(platters[registers[WORDC]]);
  platters[registers[WORDC]] = NULL;
  pushValue(&spaces, registers[WORDC]);
  Next_instr;
 OUT:
  debug("Output");
  unsigned char cout = registers[WORDC];
  fprintf(out, "%c", cout);
  Next_instr;
 IN:
  debug("Input");
  unsigned char cin;
  int dummy = scanf("%c", &cin);
  if (cin == EOF)
    registers[WORDC] = 0xFFFFFFFF;
  else
    registers[WORDC] = cin;
  Next_instr;
 LPRO:
  debug("Load program");
  if (registers[WORDB] != 0) {
    Um_code * src = platters[registers[WORDB]];
    Um_code * dest = malloc(sizeof(*dest));
    dest->code = calloc(src->int_size, 4);
    memcpy(dest->code, src->code, src->int_size * 4);
    dest->int_size = src->int_size;
    destroyCode(platters[0]);
    platters[0] = dest;
  }
  pc = registers[WORDC];
  Next_instr;
 ORTH:
  debug("Orthography");
  unsigned int imd = current_word & 0x01FFFFFF;
  unsigned int a = (current_word >> 25) & 0x07;
  registers[a] = imd;
  Next_instr;
}

int main (int argc, char ** argv) {
  FILE * fp;
  FILE * out = stdout;
  if (argc < 2) {
    fprintf(stderr, "\x1b[31mNo scroll specified\x1b[0m\n");
    exit(1);
  }
  else {
    if (!(fp = fopen(argv[1], "rb"))) {
      fprintf(stderr, "\x1b[31mUnable to open scroll\x1b[0m");
      exit(1);
    }
    else {
      if (argc > 2) {
        if (!(out = fopen(argv[2], "w"))) {
          out = stdout;
          fprintf(stdout, "djslajkdls");
        }
      }
      Um_code * code = readscroll(fp);
      um_exec(code, out);
    }
  }
  return 0;
}
