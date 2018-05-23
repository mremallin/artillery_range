IDIR =.
CC=gcc
CFLAGS=-g -I$(IDIR) -I/usr/local/include/SDL2 -I/usr/include/SDL2 -D_GNU_SOURCE=1 -D_THREAD_SAFE -Wall -Wno-missing-braces
TEST_CFLAGS=-fprofile-arcs -ftest-coverage
PROD_CFLAGS=

BIN_DIR=bin

ODIR=obj
TEST_ODIR=$(ODIR)_test

LIBS=-L/opt/local/lib -L/usr/lib/x86_64-linux-gnu -lSDL2 -lpthread
TEST_RUNNER_LIBS=-lCppUTest -lCppUTestExt

_DEPS = main.h display.h charset.h object.h text.h llist.h sm.h sm_defs.h game.sm intro.h menu.h utils.h sprite.h game.h line.h game_ui.h box.h game_server.h game_client.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o display.o charset.o object.o text.o llist.o sm.o intro.o menu.o sprite.o game.o line.o game_ui.o box.o game_server.o game_client.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_UT_OBJ = llist.o
UT_OBJ = $(patsubst %,$(ODIR)/%,$(_UT_OBJ))

_TEST_OBJ = llist_test.o AllTests.o
TEST_OBJ = $(patsubst %,$(TEST_ODIR)/%,$(_TEST_OBJ))

PROGNAME=artillery_range
TESTNAME=$(PROGNAME)_test

# Main program rules
$(PROGNAME): $(eval CFLAGS += $(PROD_CFLAGS))
$(PROGNAME): $(OBJ)
	gcc -o $(BIN_DIR)/$@ $^ $(CFLAGS) $(LIBS)

# Is there a better way to create the output bin dir automatically?
$(ODIR): $(BIN_DIR)
	@-mkdir $(ODIR)

$(BIN_DIR):
	@-mkdir $(BIN_DIR)

$(ODIR)/%.o: %.c $(DEPS) $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# Unit testing rules
test: $(eval CFLAGS += $(TEST_CFLAGS))
test: $(TESTNAME)
	bin/$(TESTNAME)

$(TESTNAME): $(TEST_OBJ) $(UT_OBJ)
	g++ -o $(BIN_DIR)/$@ $^ $(CFLAGS) $(TEST_RUNNER_LIBS)

$(TEST_ODIR):
	mkdir $(TEST_ODIR)

$(TEST_ODIR)/%.o: test/%.cpp $(DEPS) $(TEST_ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# Documentation rules
doxygen:
	doxygen

# Code Coverage
lcov: test
	mkdir lcov && \
	cd lcov && \
	lcov --directory ../obj --directory ../obj_test --capture --rc lcov_branch_coverage=1 --output-file $(TESTNAME).info && \
	genhtml --rc lcov_branch_coverage=1 $(TESTNAME).info



.PHONY: clean doxygen

clean:
	-@rm -r $(ODIR) $(TEST_ODIR) $(PROGNAME) $(TESTNAME) $(BIN_DIR) || true
	-@rm -r doxygen || true
	-@rm -r lcov || true
	-@rm -r $(PROGNAME) || true
