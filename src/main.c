#include <stdio.h>
#include <stdint.h>
#include "vm.h"

int main() {
    uint8_t code[] = {
        OP_PUSH, 10, 0, 0, 0,   // PUSH 10
        OP_DUP,                // DUP -> [10, 10]
        OP_POP,                // POP -> [10]
        OP_HALT
    };

    VM vm;
    vm_init(&vm, code, sizeof(code));
    vm_run(&vm);

    printf("Stack pointer = %d\n", vm.sp);
    printf("Top of stack = %d\n", vm.stack[vm.sp]);

    return 0;
}
