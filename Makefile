CC = gcc
CFLAGS = -Wall -Wextra -g

VM_SRC = src/main.c src/vm.c
ASM_SRC = src/assembler.c

VM_OUT = vm
ASM_OUT = assembler

all: $(VM_OUT) $(ASM_OUT)

$(VM_OUT):
	$(CC) $(CFLAGS) $(VM_SRC) -o $(VM_OUT)

$(ASM_OUT):
	$(CC) $(CFLAGS) $(ASM_SRC) -o $(ASM_OUT)

clean:
	rm -f $(VM_OUT) $(ASM_OUT)
