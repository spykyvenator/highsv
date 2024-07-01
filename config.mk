PKGCONFIG = $(shell which pkg-config)
GCR = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

CFLAGS = -march=native -mtune=native -pipe -flto -Wall -Wextra
CFLAGS = $(shell $(PKGCONFIG) --cflags highs)
#CFLAGS += $(shell $(PKGCONFIG) --cflags gtk4)

LDFLAGS = -lfl
LDFLAGS += $(shell $(PKGCONFIG) --libs highs)
#LDFLAGS = $(shell $(PKGCONFIG) --libs gtk4)

SRC := $(wildcard ./src/*.c)
OBJ := $(patsubst %c,%o,$(SRC)) ./src/parse/parse.o

LEX := $(wildcard ./src/parse/*.l)

XML := $(wildcard ./data/**/*.xml)
XOBJ := $(patsubst %xlm,%c,$(XML))
XOBJ2 := $(patsubst data%,src%,$(XOBJ))
