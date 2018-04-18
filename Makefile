IDIR =.
CC=gcc
CFLAGS=-g -I$(IDIR) -I/usr/local/include/SDL2 -I/usr/include/SDL2 -D_GNU_SOURCE=1 -D_THREAD_SAFE

ODIR=obj
TEST_ODIR=$(ODIR)_test

LIBS=-L/opt/local/lib -L/usr/lib/x86_64-linux-gnu -lSDL2
TEST_LIBS=-lCppUTest -lCppUTestExt

_DEPS = main.h display.h charset.h object.h text.h llist.h sm.h sm_defs.h game.sm intro.h menu.h utils.h sprite.h game.h line.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o display.o charset.o object.o text.o llist.o sm.o intro.o menu.o sprite.o game.o line.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_UT_OBJ = llist.o
UT_OBJ = $(patsubst %,$(ODIR)/%,$(_UT_OBJ))

_TEST_OBJ = llist_test.o AllTests.o
TEST_OBJ = $(patsubst %,$(TEST_ODIR)/%,$(_TEST_OBJ))

PROGNAME=artillery_range
TESTNAME=$(PROGNAME)_test

# Main program rules
$(PROGNAME): $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR):
	mkdir $(ODIR)

$(ODIR)/%.o: %.c $(DEPS) $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# Unit testing rules
test: $(TESTNAME)
	./$(TESTNAME)

$(TESTNAME): $(TEST_OBJ) $(UT_OBJ)
	g++ -o $@ $^ $(CFLAGS) $(TEST_LIBS)

$(TEST_ODIR):
	mkdir $(TEST_ODIR)

$(TEST_ODIR)/%.o: test/%.cpp $(DEPS) $(TEST_ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# Documentation rules
doxygen:
	doxygen

.PHONY: clean doxygen

clean:
	rm -f $(ODIR)/*.o $(TEST_ODIR).*.o *~ $(PROGNAME) $(INCDIR)/*~ $(TESTNAME)
	rm -r doxygen
