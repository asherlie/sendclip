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

spool/sp.o: spool/sp.c spool/sp.h
	$(CC) $(BASEFLAGS) spool/sp.c -c -o spool/sp.o

# implicit rule not working for some reason
sc: sc.c spool/sp.o
	$(CC) sc.c spool/sp.o $(CFLAGS) -o sc

.PHONY:
clean:
	rm -f sc *.o
