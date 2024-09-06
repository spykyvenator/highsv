.POSIX:
include config.mk

all: options highsv

./src/parse/parse.c:src/parse/parse.l
	flex -o $@ $< 

test.out:./src/parse/parse.c src/lp.c
	gcc -g -I /usr/include/highs ./src/lp.c ./src/parse/parse.c -o test.out -lfl -lhighs
testStdIn.out:./src/parse/parse.c ./test/stdIn.c
	gcc -g -I /usr/include/highs ./src/lp.c ./src/parse/parse.c ./test/stdIn.c -o $@ -lfl -lhighs
testFile.out:./src/parse/parse.c ./test/stringIn2.c
	gcc -g -I/usr/include/gtk-4.0 -I/usr/include/pango-1.0 -I/usr/include/fribidi -I/usr/include/harfbuzz -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/cairo -I/usr/include/libpng16 -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/graphene-1.0 -I/usr/lib64/graphene-1.0/include -mfpmath=sse -msse -msse2 -I/usr/lib64/libffi/include -pthread -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/libmount -I/usr/include/blkid -I /usr/include/highs ./src/parse/parse.c ./test/stringIn2.c -o $@ -lfl -lhighs


options: 
	@echo highsv build options:
	@echo "CFLAGS = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC = $(CC)"
	@echo "$(XOBJ2)"
	@echo "$(OBJ)"


./resources.c: ./src/gtk/highsv.gresource.xml ./src/gtk/window.ui
	$(GLIB_COMPILE_RESOURCES) ./src/gtk/highsv.gresource.xml --target=$@ --sourcedir=. --generate-source

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c -o $@ $(LDFLAGS) $<

highsv: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -fv $(BUILT_SRC) $(OBJ) ./src/parse/parse.c highsv

.PHONY: all options clean highsv
