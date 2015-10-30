CC=cc
CFLAGS=-std=c99 -Wall -ledit -lm

all: lisp

lisp: lisp.c mpc.c
	$(CC) $(CFLAGS) lisp.c mpc.c -o lisp

clean:
	rm lisp
