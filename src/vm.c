#include <stdio.h>
#include "vm.h"

void vm_init(VM *vm, uint8_t *code, int code_size) {
    vm->pc = 0;
    vm->code = code;
    vm->code_size = code_size;

    vm->sp = -1;
    vm->rsp = -1;

    vm->running = true;
}

void vm_run(VM *vm) {
    while (vm->running) {

        if (vm->pc < 0 || vm->pc >= vm->code_size) {
            fprintf(stderr, "Runtime error: PC out of bounds\n");
            break;
        }

        uint8_t opcode = vm->code[vm->pc];

        switch (opcode) {

            case OP_PUSH: {
                if (vm->pc + 4 >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Incomplete PUSH operand\n");
                    vm->running = false;
                    break;
                }

                if (vm->sp >= STACK_SIZE - 1) {
                    fprintf(stderr, "Runtime error: Stack overflow\n");
                    vm->running = false;
                    break;
                }

                int32_t value =
                    (vm->code[vm->pc + 1]) |
                    (vm->code[vm->pc + 2] << 8) |
                    (vm->code[vm->pc + 3] << 16) |
                    (vm->code[vm->pc + 4] << 24);

                vm->stack[++vm->sp] = value;

                vm->pc += 5;   // opcode + 4-byte operand
                continue;      // skip automatic pc++
            }

            case OP_HALT:
                vm->running = false;
                vm->pc++;      // optional, but consistent
                break;

            case OP_POP:
                if (vm->sp < 0) {
                    fprintf(stderr, "Runtime error: Stack underflow on POP\n");
                    vm->running = false;
                    break;
                }

                vm->sp--;     // discard top element
                vm->pc++;     // advance PC
                break;

            case OP_DUP:
                if (vm->sp < 0) {
                    fprintf(stderr, "Runtime error: Stack underflow on DUP\n");
                    vm->running = false;
                    break;
                }

                if (vm->sp >= STACK_SIZE - 1) {
                    fprintf(stderr, "Runtime error: Stack overflow on DUP\n");
                    vm->running = false;
                    break;
                }

                vm->stack[vm->sp + 1] = vm->stack[vm->sp];
                vm->sp++;
                vm->pc++;
                break;

            case OP_ADD: {
                if (vm->sp < 1) {
                    fprintf(stderr, "Runtime error: Stack underflow on ADD\n");
                    vm->running = false;
                    break;
                }

                int b = vm->stack[vm->sp--];
                int a = vm->stack[vm->sp--];

                vm->stack[++vm->sp] = a + b;
                vm->pc++;
                break;
            }

            case OP_SUB: {
                if (vm->sp < 1) {
                    fprintf(stderr, "Runtime error: Stack underflow on SUB\n");
                    vm->running = false;
                    break;
                }

                int b = vm->stack[vm->sp--];
                int a = vm->stack[vm->sp--];

                vm->stack[++vm->sp] = a - b;
                vm->pc++;
                break;
            }

            case OP_MUL: {
                if (vm->sp < 1) {
                    fprintf(stderr, "Runtime error: Stack underflow on MUL\n");
                    vm->running = false;
                    break;
                }

                int b = vm->stack[vm->sp--];
                int a = vm->stack[vm->sp--];

                vm->stack[++vm->sp] = a * b;
                vm->pc++;
                break;
            }

            case OP_DIV: {
                if (vm->sp < 1) {
                    fprintf(stderr, "Runtime error: Stack underflow on DIV\n");
                    vm->running = false;
                    break;
                }

                int b = vm->stack[vm->sp--];
                int a = vm->stack[vm->sp--];

                if (b == 0) {
                    fprintf(stderr, "Runtime error: Division by zero\n");
                    vm->running = false;
                    break;
                }

                vm->stack[++vm->sp] = a / b;
                vm->pc++;
                break;
            }

            case OP_CMP: {
                if (vm->sp < 1) {
                    fprintf(stderr, "Runtime error: Stack underflow on CMP\n");
                    vm->running = false;
                    break;
                }

                int b = vm->stack[vm->sp--];
                int a = vm->stack[vm->sp--];

                vm->stack[++vm->sp] = (a < b) ? 1 : 0;
                vm->pc++;
                break;
            }



            default:
                fprintf(stderr,
                        "Runtime error: Invalid opcode 0x%02X at PC=%d\n",
                        opcode, vm->pc);
                vm->running = false;
                break;
        }
    }
}

