IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -I/opt/local/include/SDL2 -D_GNU_SOURCE=1 -D_THREAD_SAFE

ODIR=obj

LDIR=/opt/local/lib
LIBS=-L$(LDIR) -lsdl2

_DEPS = main.h display.h charset.h object.h text.h llist.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o display.o charset.o object.o text.o llist.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

PROGNAME=artillery_range

$(PROGNAME): $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR):
	mkdir $(ODIR)

$(ODIR)/%.o: %.c $(DEPS) $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(PROGNAME) $(INCDIR)/*~ 
