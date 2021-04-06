UNAME := $(shell uname)

BASEFLAGS= -Wall -Wextra -Wpedantic -Werror -pthread -g
MACFLAGS= -lclipboard -framework cocoa -DMAC_OS

ifeq ($(UNAME), Darwin)
CFLAGS=$(BASEFLAGS) $(MACFLAGS)
else
CFLAGS=$(BASEFLAGS)
endif

CC=gcc

all: sc

pooler/pool.o: pooler/pool.c pooler/pool.h
	$(CC) $(BASEFLAGS) pooler/pool.c -c -o pooler/pool.o

sc: sc.c pooler/pool.o

.PHONY:
clean:
	rm -f sc *.o
