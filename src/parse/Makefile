CFLAGS=$(shell pkg-config --cflags highs)
LFLAGS=$(shell pkg-config --libs highs) -lm
all: parser.out
parser.tab.c: parser.y
	bison --html -d $<
parser.c: parser.l
	flex -o $@ --header-file=$(patsubst %.l,%.h,$<) $<

*.o: parser.c parser.tab.c
	gcc -c $(CFLAGS) $< $(LFLAGS)
parser.out: parser.o parser.tab.o
	gcc $(CFLAGS) -o $@ $^ $(LFLAGS)
