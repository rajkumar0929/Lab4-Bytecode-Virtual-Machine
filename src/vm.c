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

            case OP_JMP: {
                if (vm->pc + 4 >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Incomplete JMP operand\n");
                    vm->running = false;
                    break;
                }

                int addr =
                    vm->code[vm->pc + 1] |
                    (vm->code[vm->pc + 2] << 8) |
                    (vm->code[vm->pc + 3] << 16) |
                    (vm->code[vm->pc + 4] << 24);

                if (addr < 0 || addr >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Invalid JMP address\n");
                    vm->running = false;
                    break;
                }

                vm->pc = addr;
                continue;
            }

            case OP_JZ: {
                if (vm->pc + 4 >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Incomplete JZ operand\n");
                    vm->running = false;
                    break;
                }

                if (vm->sp < 0) {
                    fprintf(stderr, "Runtime error: Stack underflow on JZ\n");
                    vm->running = false;
                    break;
                }

                int addr =
                    vm->code[vm->pc + 1] |
                    (vm->code[vm->pc + 2] << 8) |
                    (vm->code[vm->pc + 3] << 16) |
                    (vm->code[vm->pc + 4] << 24);

                int cond = vm->stack[vm->sp--];

                if (cond == 0) {
                    if (addr < 0 || addr >= vm->code_size) {
                        fprintf(stderr, "Runtime error: Invalid JZ address\n");
                        vm->running = false;
                        break;
                    }
                    vm->pc = addr;
                    continue;
                }

                vm->pc += 5;
                break;
            }

            case OP_JNZ: {
                if (vm->pc + 4 >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Incomplete JNZ operand\n");
                    vm->running = false;
                    break;
                }

                if (vm->sp < 0) {
                    fprintf(stderr, "Runtime error: Stack underflow on JNZ\n");
                    vm->running = false;
                    break;
                }

                int addr =
                    vm->code[vm->pc + 1] |
                    (vm->code[vm->pc + 2] << 8) |
                    (vm->code[vm->pc + 3] << 16) |
                    (vm->code[vm->pc + 4] << 24);

                int cond = vm->stack[vm->sp--];

                if (cond != 0) {
                    if (addr < 0 || addr >= vm->code_size) {
                        fprintf(stderr, "Runtime error: Invalid JNZ address\n");
                        vm->running = false;
                        break;
                    }
                    vm->pc = addr;
                    continue;
                }

                vm->pc += 5;
                break;
            }

            case OP_STORE: {
                if (vm->pc + 4 >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Incomplete STORE operand\n");
                    vm->running = false;
                    break;
                }

                if (vm->sp < 0) {
                    fprintf(stderr, "Runtime error: Stack underflow on STORE\n");
                    vm->running = false;
                    break;
                }

                int idx =
                    vm->code[vm->pc + 1] |
                    (vm->code[vm->pc + 2] << 8) |
                    (vm->code[vm->pc + 3] << 16) |
                    (vm->code[vm->pc + 4] << 24);

                if (idx < 0 || idx >= MEM_SIZE) {
                    fprintf(stderr, "Runtime error: Invalid memory index on STORE\n");
                    vm->running = false;
                    break;
                }

                vm->memory[idx] = vm->stack[vm->sp--];
                vm->pc += 5;
                break;
            }

            case OP_LOAD: {
                if (vm->pc + 4 >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Incomplete LOAD operand\n");
                    vm->running = false;
                    break;
                }

                if (vm->sp >= STACK_SIZE - 1) {
                    fprintf(stderr, "Runtime error: Stack overflow on LOAD\n");
                    vm->running = false;
                    break;
                }

                int idx =
                    vm->code[vm->pc + 1] |
                    (vm->code[vm->pc + 2] << 8) |
                    (vm->code[vm->pc + 3] << 16) |
                    (vm->code[vm->pc + 4] << 24);

                if (idx < 0 || idx >= MEM_SIZE) {
                    fprintf(stderr, "Runtime error: Invalid memory index on LOAD\n");
                    vm->running = false;
                    break;
                }

                vm->stack[++vm->sp] = vm->memory[idx];
                vm->pc += 5;
                break;
            }

            case OP_CALL: {
                if (vm->pc + 4 >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Incomplete CALL operand\n");
                    vm->running = false;
                    break;
                }

                if (vm->rsp >= RET_STACK_SIZE - 1) {
                    fprintf(stderr, "Runtime error: Return stack overflow\n");
                    vm->running = false;
                    break;
                }

                int addr =
                    vm->code[vm->pc + 1] |
                    (vm->code[vm->pc + 2] << 8) |
                    (vm->code[vm->pc + 3] << 16) |
                    (vm->code[vm->pc + 4] << 24);

                if (addr < 0 || addr >= vm->code_size) {
                    fprintf(stderr, "Runtime error: Invalid CALL address\n");
                    vm->running = false;
                    break;
                }

                vm->ret_stack[++vm->rsp] = vm->pc + 5;  // save return address
                vm->pc = addr;
                continue;
            }

            case OP_RET:
                if (vm->rsp < 0) {
                    fprintf(stderr, "Runtime error: Return stack underflow on RET\n");
                    vm->running = false;
                    break;
                }

                vm->pc = vm->ret_stack[vm->rsp--];
                continue;



            default:
                fprintf(stderr,
                        "Runtime error: Invalid opcode 0x%02X at PC=%d\n",
                        opcode, vm->pc);
                vm->running = false;
                break;
        }
    }
}

