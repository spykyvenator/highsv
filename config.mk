CFLAGS = -march=native -mtune=native -pipe -flto -Wall -Wextra
CFLAGS += $(shell pkg-config --cflags gtk4)
LDFLAGS = $(shell pkg-config --libs gtk4)

SRC := $(wildcard ./src/*.c)
OBJ := $(patsubst %c,%o,$(SRC))
