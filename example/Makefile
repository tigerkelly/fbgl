
CC=gcc
DEPS = glt.h
CFLAGS=-I. -I../ -I../../utils/incs -I/usr/include/freetype2 -I/usr/include/libxml2
LDFLAGS=-L. -L../ -L../../utils/libs

LIBS=-lfbgl -lpng -lfreetype -lini -lstrutils -lxml2

PRG=fbgl_ex

$(PRG): $(PRG).o ../libfbgl.a
	$(CC) -o $(PRG) $(PRG).o $(LDFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f fbgl_ex fbgl_ex.o
