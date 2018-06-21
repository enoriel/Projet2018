CC=gcc
CFLAGS=
LDFLAGS=
EXEC=controller
SRC= src/controller.c src/hid_function.c src/map.c src/nRF24L01.c
OBJ= $(SRC:.c=.o)

all: $(EXEC)

controller: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

controller.o: hid.h nrf24l01.h map.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf src/*.o

mrproper: clean
	rm -rf $(EXEC)
