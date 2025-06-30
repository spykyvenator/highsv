all: parser.out
parser.tab.c: parser.y
	bison --html -d $<
parser.c: parser.l
	flex -o $@ --header-file=$(patsubst %.l,%.h,$<) $<

parser.out: parser.c parser.tab.c
	gcc $(shell pkg-config --cflags highs) -o $@ $< $(shell pkg-config --libs highs) -lm
