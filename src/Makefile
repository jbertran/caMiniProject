CC=gcc
CFLAGS=-Wall -std=c11
SRC=universal-machine.c tools.c
EXE=universal-machine

all: universal-machine

%.o: %.c
	${CC} -c $@ $<

universal-machine: universal-machine.c tools.c
	${CC} -o $@ -O3 -std=c11 $^

clean: 
	rm -f *~ *.o ${EXE}
