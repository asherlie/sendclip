CC=gcc
CFLAGS= -Wall -Wextra -Wpedantic -Werror -pthread -lclipboard -framework cocoa -DMAC_OS

all: sc

sc: sc.c

.PHONY:
clean:
	rm -f sc *.o
