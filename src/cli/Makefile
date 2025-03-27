CC ?= gcc
PKGCONFIG ?= $(shell which pkg-config)
CFLAGS = -DCLI -Wall -Wextra $(shell $(PKGCONFIG) --cflags highs) $(shell $(PKGCONFIG) --cflags glib-2.0) $(shell $(PKGCONFIG) --cflags gio-2.0) $(shell $(PKGCONFIG) --cflags gio-unix-2.0)
LIBS = $(shell $(PKGCONFIG) --libs highs) $(shell $(PKGCONFIG) --libs glib-2.0) $(shell $(PKGCONFIG) --libs gio-2.0)
LDFLAGS = -Wl,--allow-multiple-definition
SRC =  ./main.c ../parse/parse.c ../sol.c ../pOstream.c ../tprint/tprint.c
OBJS = $(SRC:.c=.o)
DEPENDS := $(patsubst %.c,%.d,$(SRC))

.PHONY: all clean options debug

all: options cli

debug: CFLAGS += -DDEBUG -g
debug: cli

options: 
	@echo highsv build options:
	@echo "CFLAGS = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC = $(CC)"

clean:
	rm -f $(BUILT_SRC)
	rm -f $(OBJS)
	rm -f $(DEPENDS) ../parse/parse.o
	rm -f cli

cli: $(OBJS)
	$(CC) $^ -g -o $@ $(LIBS) $(LDFLAGS)

-include $(DEPENDS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<
