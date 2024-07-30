CC ?= gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = -g $(shell $(PKGCONFIG) --cflags gtk4) $(shell $(PKGCONFIG) --cflags highs)
LIBS = $(shell $(PKGCONFIG) --libs gtk4) $(shell $(PKGCONFIG) --libs highs) -lfl
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

SRC = highsv.c ./highsvWin.c ./main.c ../lp.c ../parse/parse.c ./highsvFile.c ./highsvSol.c ./highsvActions.c
BUILT_SRC = resources.c

OBJS = $(BUILT_SRC:.c=.o) $(SRC:.c=.o)

all: highsv

resources.c: highsv.gresource.xml window.ui
	$(GLIB_COMPILE_RESOURCES) highsv.gresource.xml --target=$@ --sourcedir=. --generate-source

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

highsv: $(OBJS)
	$(CC) -g -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(BUILT_SRC)
	rm -f $(OBJS)
	rm -f highsv
