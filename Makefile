.POSIX:
include config.mk

all: options highsv

./src/parse/parse.c:src/parse/parse.l
	flex -o $@ $< 

test.out:./src/parse/parse.c src/lp.c
	gcc -g -I /usr/include/highs ./src/lp.c ./src/parse/parse.c -o test.out -lfl -lhighs
test3.out:./src/parse/parse.c src/lp.c ./test/stdIn.c
	gcc -g -I /usr/include/highs ./src/lp.c ./src/parse/parse.c ./test/stdIn.c -o test3.out -lfl -lhighs
test4.out:./src/parse/parse.c src/lp.c ./test/stdIn.c
	gcc -g -I /usr/include/highs ./src/lp.c ./src/parse/parse.c ./test/stringIn2.c -o test4.out -lfl -lhighs


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
