CFLAGS= -g $(shell pkg-config --cflags highs)
LFLAGS=$(shell pkg-config --libs highs) -lm

all: parser.out
parser.c: parser.y
	bison -Wcounterexamples -o $@ -d $<
scanner.c: scanner.l
	flex -o $@ --header-file=$(patsubst %.l,%.h,$<) $<
%.o: %.c
	gcc -c $(CFLAGS) $< $(LFLAGS)
parser.out: parser.o scanner.o ../../util.o
	gcc $(CFLAGS) $^ -o $@ $(LFLAGS)

clean: 
	rm -vf parser.c scanner.c parser.out scannner.h *.o
.PHONY: clean
