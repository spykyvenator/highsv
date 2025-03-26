.POSIX:
PKGCONFIG ?= $(shell which pkg-config)

all: lps data/ui/icon.ico

lps: 
	cd src/gtk && $(MAKE)
	cp --update=older src/gtk/highsv ./lps
data/ui/icon.ico: data/ui/icon.svg
	magick data/ui/icon.svg data/ui/icon.ico
debug:
	cd src/gtk && $(MAKE) debug
	cp --update=older src/gtk/highsv ./lps
	data/ui/icon.ico
windows:
	cd src/gtk && $(MAKE) windows
	cp src/gtk/highsv.exe ./
	data/ui/icon.ico
clean:
	cd src/gtk && $(MAKE) clean
	rm data/ui/icon.ico

tools/options.out: tools/test.c
	gcc $< -o $@ $(shell $(PKGCONFIG) --libs highs) $(shell $(PKGCONFIG) --cflags highs)

.PHONY: all clean lps debug
