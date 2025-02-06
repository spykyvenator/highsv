.POSIX:

all: lps data/ui/icon.ico

testFile.out:./src/parse/parse.c ./test/stringIn2.c
	gcc -g -I/usr/include/gtk-4.0 -I/usr/include/pango-1.0 -I/usr/include/fribidi -I/usr/include/harfbuzz -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/cairo -I/usr/include/libpng16 -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/graphene-1.0 -I/usr/lib64/graphene-1.0/include -mfpmath=sse -msse -msse2 -I/usr/lib64/libffi/include -pthread -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/libmount -I/usr/include/blkid -I /usr/include/highs ./src/parse/parse.c ./test/stringIn2.c -o $@ -lfl -lhighs -lgtk-4 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lgraphene-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0

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
