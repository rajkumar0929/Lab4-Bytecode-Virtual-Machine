#include <stdio.h>
#include <stdint.h>
#include "vm.h"

int main() {
    /*
        main:
            PUSH 10
            CALL func
            HALT

        func:
            PUSH 20
            ADD
            RET
    */

    uint8_t code[] = {
        OP_PUSH, 10, 0, 0, 0,      // 0
        OP_CALL, 11, 0, 0, 0,      // 5 → jump to func
        OP_HALT,                  // 10

        OP_PUSH, 20, 0, 0, 0,      // 11 (func)
        OP_ADD,                   // 16
        OP_RET                    // 17
    };

    VM vm;
    vm_init(&vm, code, sizeof(code));
    vm_run(&vm);

    printf("Final result = %d\n", vm.stack[vm.sp]);
    return 0;
}
