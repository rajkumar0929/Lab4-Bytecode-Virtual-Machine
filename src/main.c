#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vm.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <program.bc>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Failed to open bytecode file");
        return 1;
    }

    /* Determine file size */
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size <= 0) {
        fprintf(stderr, "Invalid bytecode file size\n");
        fclose(f);
        return 1;
    }

    /* Allocate memory for bytecode */
    uint8_t *code = malloc(size);
    if (!code) {
        perror("Memory allocation failed");
        fclose(f);
        return 1;
    }

    /* Read bytecode */
    if (fread(code, 1, size, f) != (size_t)size) {
        fprintf(stderr, "Failed to read bytecode file\n");
        free(code);
        fclose(f);
        return 1;
    }

    fclose(f);

    /* Initialize and run VM */
    VM vm;
    vm_init(&vm, code, size);
    vm_run(&vm);

    /* Optional: print final stack top if exists */
    if (vm.sp >= 0) {
        printf("Top of stack: %d\n", vm.stack[vm.sp]);
    }

    free(code);
    return 0;
}
