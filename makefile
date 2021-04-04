UNAME := $(shell uname)

BASEFLAGS= -Wall -Wextra -Wpedantic -Werror -pthread
MACFLAGS= -lclipboard -framework cocoa -DMAC_OS

ifeq ($(UNAME), Darwin)
#ifeq ("Darwin", "Darwin")
CFLAGS=$(BASEFLAGS) $(MACFLAGS)
else
CFLAGS=$(BASEFLAGS)
endif
#CFLAGS=" -Wall -Wextra -Wpedantic -Werror -pthread -lclipboard -framework cocoa -DMAC_OS"
#endif

CC=gcc

all: sc

sc: sc.c

.PHONY:
clean:
	rm -f sc *.o
