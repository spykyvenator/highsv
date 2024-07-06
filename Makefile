.POSIX:
include config.mk

./src/parse/parse.c:src/parse/parse.l
	flex -o $@ $< 

test.out:src/parse/parse.c src/lp.c
	gcc -g -I /usr/include/highs ./src/lp.c ./src/parse/parse.c -o test.out -lfl -lhighs

all: options $(XOBJ) highsv

options: 
	@echo highsv build options:
	@echo "CFLAGS = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC = $(CC)"
	@echo "$(XOBJ2)"


org.gtk.exampleapp.gschema.valid: org.gtk.exampleapp.gschema.xml
	$(GLIB_COMPILE_SCHEMAS) --strict --dry-run --schema-file=$< && mkdir -p $(@D) && touch $@

src/gresource.c: ./.gresource.xml
	$(GCR) ./.gresource.xml --target=$@ --generate-source

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

highsv: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -fv $(OBJ) ./src/parse/parse.c

.PHONY: all options clean highsv
