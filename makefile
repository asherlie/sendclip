UNAME := $(shell uname)

BASEFLAGS= -Wall -Wextra -Wpedantic -Werror -pthread
MACFLAGS= -lclipboard -framework cocoa -DMAC_OS

ifeq ($(UNAME), Darwin)
CFLAGS=$(BASEFLAGS) $(MACFLAGS)
else
CFLAGS=$(BASEFLAGS)
endif

CC=gcc

all: sc

sc: sc.c

.PHONY:
clean:
	rm -f sc *.o
