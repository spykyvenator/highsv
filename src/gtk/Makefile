CC ?= gcc
PKGCONFIG ?= $(shell which pkg-config)
CFLAGS = -DHIGHS_BACKEND -std=c23 -Wpedantic -Wstrict-prototypes -Wmissing-declarations -Wall -Wextra $(shell $(PKGCONFIG) --cflags gtk4) \
	 $(shell $(PKGCONFIG) --cflags highs) \
	 $(shell $(PKGCONFIG) --cflags gtksourceview-5)
LIBS = $(shell $(PKGCONFIG) --libs gtk4) \
       $(shell $(PKGCONFIG) --libs highs) \
       $(shell $(PKGCONFIG) --libs gtksourceview-5) \
       -lm
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)
LDFLAGS = -Wl,--allow-multiple-definition

SRC = highsv.c ./highsvWin.c ./main.c ../parse/bison/pt.c ../parse/bison/scanner.c ../parse/bison/parser.c ./highsvFile.c ./highsvSol.c ./highsvActions.c ../sol.c ../pOstream.c ../tprint/tprint.c ../util.c
BUILT_SRC = resources.c

OBJS = $(BUILT_SRC:.c=.o) $(SRC:.c=.o)
DEPENDS := $(patsubst %.c,%.d,$(SRC))

.PHONY: all clean options debug windows

all: options highsv

debug: CFLAGS += -DDEBUG_RINTER -g
debug: highsv

egypt: clean
egypt: CFLAGS += -fdump-rtl-expand
egypt: highsv

windows: LDFLAGS += -mwindows
windows: highsv

options: 
	@echo highsv build options:
	@echo "CFLAGS = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC = $(CC)"

resources.c: highsv.gresource.xml window.ui
	$(GLIB_COMPILE_RESOURCES) highsv.gresource.xml --target=$@ --sourcedir=. --generate-source

../parse/bison/parser.c ../parse/bison/parser.h: ../parse/bison/parser.y
	bison --warnings -r all --report-file=./report -Wcounterexamples --debug -o ../parse/bison/parser.c -d $<
../parse/bison/scanner.h ../parse/bison/scanner.c: ../parse/bison/scanner.l ../parse/bison/parser.h
	flex -o ../parse/bison/scanner.c --header-file=$(patsubst %.l,%.h,$<) $<

clean:
	rm -f $(BUILT_SRC)
	rm -f $(OBJS)
	rm -f $(DEPENDS) ../parse/parse.c ../parse/parse.h
	rm -f highsv

highsv: $(OBJS)
	$(CC) $^ -o $@ $(LIBS) $(LDFLAGS)

-include $(DEPENDS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<
