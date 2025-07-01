CFLAGS=$(shell pkg-config --cflags highs)
LFLAGS=$(shell pkg-config --libs highs) -lm

all: parser.out
parser.c: parser.y
	bison -o $@ -d $<
scanner.c: scanner.l
	flex -o $@ --header-file=$(patsubst %.l,%.h,$<) $<
*.o: parser.c scanner.c
	gcc -c $(CFLAGS) $< $(LFLAGS)
parser.out: parser.o scanner.o
	gcc $^ -o $@ $(LFLAGS)

clean: rm -vf parser.c scanner.c parser.out scannner.h *.o
