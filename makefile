CC=gcc
CFLAGS=-I.
DEPS=proto.h
OBJ=msh.o arg_parse.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

msh: $(OBJ)
	$(CC) -g -Wall -o $@ $^ $(CFLAGS)
