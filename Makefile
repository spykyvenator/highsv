.POSIX:
include config.mk

all: options highsv

options: 
	@echo highsv build options:
	@echo "CFLAGS = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"
	@echo "CC = $(CC)"

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

highsv: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)
