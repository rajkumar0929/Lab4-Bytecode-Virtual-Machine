#include <stdio.h>
#include <stdint.h>
#include "vm.h"

int main() {
    uint8_t code[] = {
        OP_PUSH, 5, 0, 0, 0,    // PUSH 3
        OP_PUSH, 3, 0, 0, 0,    // PUSH 5
        OP_CMP,                // 3 < 5 → 1
        OP_HALT
    };

    VM vm;
    vm_init(&vm, code, sizeof(code));
    vm_run(&vm);

    printf("CMP result = %d\n", vm.stack[vm.sp]);
    return 0;
}
