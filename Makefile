
all: libfbgl.a

# Set export PROFILE=yes to turn on profiling flags.

CC=gcc
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

LDFLAGS=-L/usr/local/lib -L../utils/libs -lstrutils -lfreetype -lrt -lpthread
CFLAGS=-std=gnu99 -g -Wall -I../utils/incs -I/usr/local/include -I/usr/include/freetype2
ifeq ($(PROFILE),yes)
	CFLAGS += -pg
	LDFLAGS += -pg
endif

ARC=libfbgl.a

all: $(ARC)

$(ARC): $(OBJS)
	$(AR) -r $(ARC) $(OBJS)

$(OBJS): fbgl.h

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(ARC)

