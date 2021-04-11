UNAME := $(shell uname)

BASEFLAGS= -Wall -Wextra -Wpedantic -Werror -pthread -g
MACFLAGS= -lclipboard -framework cocoa -DMAC_OS
LINFLAGS= -l:libclipboard.a -lxcb

ifeq ($(UNAME), Darwin)
CFLAGS=$(BASEFLAGS) $(MACFLAGS)
else
#CFLAGS=$(BASEFLAGS) $(LINFLAGS)
CFLAGS= $(LINFLAGS) $(BASEFLAGS)
endif

CC=gcc

all: sc

pooler/pool.o: pooler/pool.c pooler/pool.h
	$(CC) $(BASEFLAGS) pooler/pool.c -c -o pooler/pool.o

# implicit rule not working for some reason
sc: sc.c pooler/pool.o
	$(CC) sc.c pooler/pool.o $(CFLAGS) -o sc

.PHONY:
clean:
	rm -f sc *.o
