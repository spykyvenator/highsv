CC ?= gcc
PKGCONFIG ?= $(shell which pkg-config)
CFLAGS = -DCLI -DDEBUG -DHIGHS_BACKEND -Wall -Wextra $(shell $(PKGCONFIG) --cflags highs) $(shell $(PKGCONFIG) --cflags glib-2.0) $(shell $(PKGCONFIG) --cflags gio-2.0) $(shell $(PKGCONFIG) --cflags gio-unix-2.0)
LIBS = -lm $(shell $(PKGCONFIG) --libs highs) $(shell $(PKGCONFIG) --libs glib-2.0) $(shell $(PKGCONFIG) --libs gio-2.0)
LDFLAGS = -Wl,--allow-multiple-definition
SRC =  ./main.c ../parse/bison/scanner.c ../parse/bison/parser.c ./sol.c ../pOstream.c ../tprint/tprint.c ../util.c ../parse/bison/pt.c
OBJS = $(SRC:.c=.o)
DEPENDS := $(patsubst %.c,%.d,$(SRC))

.PHONY: all clean options debug

all: options cli

debug: CFLAGS += -DDEBUG -g
debug: cli

../parse/bison/parser.c: ../parse/bison/parser.y
	bison -Wcounterexamples -o $@ -d $<
../parse/bison/scanner.c: ../parse/bison/scanner.l
	flex -o $@ --header-file=$(patsubst %.l,%.h,$<) $<

options: 
	@echo highsv build options:
	@echo "CFLAGS = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC = $(CC)"

clean:
	rm -f $(BUILT_SRC)
	rm -f $(OBJS)
	rm -f $(DEPENDS) ../parse/parse.o
	rm -f ../parse/bison/parser.c ../parse/bison/scanner.c
	rm -f cli

cli: $(OBJS)
	$(CC) $^ -g -o $@ $(LIBS) $(LDFLAGS)

-include $(DEPENDS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<
