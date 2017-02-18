IDIR =.
CC=gcc
CFLAGS=-g -I$(IDIR) -I/opt/local/include/SDL2 -I/usr/include/SDL2 -D_GNU_SOURCE=1 -D_THREAD_SAFE

ODIR=obj

LIBS=-L/opt/local/lib -L/usr/lib/x86_64-linux-gnu -lSDL2

_DEPS = main.h display.h charset.h object.h text.h llist.h sm.h sm_defs.h game.sm intro.h menu.h utils.h sprite.h game.h line.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o display.o charset.o object.o text.o llist.o sm.o intro.o menu.o sprite.o game.o line.o
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
