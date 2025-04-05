CC ?= gcc
PKGCONFIG ?= $(shell which pkg-config)
CFLAGS = -std=c23 -Wpedantic -Wall -Wextra $(shell $(PKGCONFIG) --cflags gtk4) \
	 $(shell $(PKGCONFIG) --cflags highs)
LIBS = $(shell $(PKGCONFIG) --libs gtk4) \
       $(shell $(PKGCONFIG) --libs highs)
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)
LDFLAGS = -Wl,--allow-multiple-definition

SRC = highsv.c ./highsvWin.c ./main.c ../parse/parse.c ./highsvFile.c ./highsvSol.c ./highsvActions.c ../sol.c ../pOstream.c ../tprint/tprint.c
BUILT_SRC = resources.c

OBJS = $(BUILT_SRC:.c=.o) $(SRC:.c=.o)
DEPENDS := $(patsubst %.c,%.d,$(SRC))

.PHONY: all clean options debug windows

all: options highsv

debug: CFLAGS += -DDEBUG_RINTER -g
debug: highsv

windows: LDFLAGS += -mwindows
windows: highsv

options: 
	@echo highsv build options:
	@echo "CFLAGS = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC = $(CC)"

resources.c: highsv.gresource.xml window.ui
	$(GLIB_COMPILE_RESOURCES) highsv.gresource.xml --target=$@ --sourcedir=. --generate-source

../parse/parse.c: ../parse/parse.l
	flex -o $@ --header-file=$(patsubst %.l,%.h,$<) $<

clean:
	rm -f $(BUILT_SRC)
	rm -f $(OBJS)
	rm -f $(DEPENDS) ../parse/parse.c ../parse/parse.h
	rm -f highsv

highsv: $(OBJS)
	$(CC) $^ -g -o $@ $(LIBS) $(LDFLAGS)

-include $(DEPENDS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<
