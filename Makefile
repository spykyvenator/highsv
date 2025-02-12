.POSIX:

all: lps data/ui/icon.ico

lps: 
	cd src/gtk && $(MAKE)
	cp --update=older src/gtk/highsv ./lps
data/ui/icon.ico: data/ui/icon.svg
	magick data/ui/icon.svg data/ui/icon.ico
debug:
	cd src/gtk && $(MAKE) debug
	cp --update=older src/gtk/highsv ./lps

clean:
	rm data/ui/icon.ico
	cd src/gtk && $(MAKE) clean

.PHONY: all clean lps debug
