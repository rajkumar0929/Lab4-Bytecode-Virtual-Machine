#include <stdio.h>
#include <stdint.h>
#include "vm.h"

int main() {
    uint8_t code[] = {
        OP_PUSH, 5, 0, 0, 0,        // 0: PUSH 3
        OP_PUSH, 3, 0, 0, 0,        // 5: PUSH 5
        OP_CMP,                    // 10: CMP (3 < 5 → 1)
        OP_JZ, 21, 0, 0, 0,         // 11: JZ to HALT if false
        OP_PUSH, 99, 0, 0, 0,       // 16: PUSH 99
        OP_HALT                     // 21: HALT
    };

    VM vm;
    vm_init(&vm, code, sizeof(code));
    vm_run(&vm);

    printf("Top of stack = %d\n", vm.stack[vm.sp]);
    return 0;
}
