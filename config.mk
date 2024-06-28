PKGCONFIG = $(shell which pkg-config)
GCR = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

CFLAGS = -march=native -mtune=native -pipe -flto -Wall -Wextra
CFLAGS += $(shell $(PKGCONFIG) --cflags gtk4)
LDFLAGS = $(shell $(PKGCONFIG) --libs gtk4)

SRC := $(wildcard ./src/*.c)
OBJ := $(patsubst %c,%o,$(SRC))

XML := $(wildcard ./data/**/*.xml)
XOBJ := $(patsubst %xlm,%c,$(XML))
XOBJ2 := $(patsubst data%,src%,$(XOBJ))
