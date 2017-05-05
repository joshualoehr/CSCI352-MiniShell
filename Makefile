#    $Id: Makefile,v 1.3 2017/04/24 04:39:24 loehrj Exp $

CC=gcc
CFLAGS= -g -Wall
OBJS= msh.o arg_parse.o builtin.o expand.o

msh: ${OBJS:.c=.o}
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): proto.h

clean:
	-@rm -r msh $(OBJS) 2> /dev/null
