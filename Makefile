CC = gcc
CFLAGS = -Wall -Wextra -g

SRC = src/main.c src/vm.c
OUT = vm

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f vm
