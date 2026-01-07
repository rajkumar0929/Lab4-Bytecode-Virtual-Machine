# Lab 4 — Stack-Based Bytecode Virtual Machine

## Overview

This project implements a **stack-based bytecode virtual machine (VM)** along with a **two-pass assembler**.
The VM executes bytecode instructions supporting arithmetic, control flow, function calls, and memory operations.
The assembler converts human-readable assembly programs into executable bytecode.

The design follows a clean separation of concerns:
- **Assembler**: Translates `.asm` → `.bc`
- **VM**: Executes `.bc` files deterministically

---

## Build Instructions

### Prerequisites
- GCC (C compiler)
- Linux / WSL environment recommended

### Build

From the project root:

```
make clean
make
```

This generates two executables:
- `vm` — the bytecode virtual machine
- `assembler` — the assembly-to-bytecode translator

---

## How to Use

### 1. Assemble an Assembly Program

```
./assembler <input.asm> <output.bc>
```

Example:
```
./assembler tests/04_loop.asm tests/04_loop.bc
```

---

### 2. Run the Bytecode on the VM

```
./vm <program.bc>
```

Example:
```
./vm tests/04_loop.bc
```

The VM prints the **top of the stack** after execution if available.

---

## Instruction Set Summary

### Stack & Data Movement
- `PUSH val`
- `POP`
- `DUP`
- `HALT`

### Arithmetic & Logical
- `ADD`, `SUB`, `MUL`, `DIV`
- `CMP` (pushes 1 if a < b, else 0)

### Control Flow
- `JMP label`
- `JZ label`
- `JNZ label`

### Memory
- `STORE idx`
- `LOAD idx`

### Function Calls
- `CALL label`
- `RET`

All jump and call targets are resolved by the assembler using **labels**.

---

## Test Programs and Expected Output

### Test 1 — Stack Operations
```
./assembler tests/01_stack.asm tests/01_stack.bc
./vm tests/01_stack.bc
```
Expected:
```
Top of stack: 10
```

---

### Test 2 — Arithmetic
```
./assembler tests/02_arithmetic.asm tests/02_arithmetic.bc
./vm tests/02_arithmetic.bc
```
Expected:
```
Top of stack: 45
```

---

### Test 3 — CMP and Branching
```
./assembler tests/03_cmp_branch.asm tests/03_cmp_branch.bc
./vm tests/03_cmp_branch.bc
```
Expected:
```
Top of stack: 99
```

---

### Test 4 — Loop
```
./assembler tests/04_loop.asm tests/04_loop.bc
./vm tests/04_loop.bc
```
Expected:
```
Top of stack: 0
```

---

### Test 5 — Memory
```
./assembler tests/05_memory.asm tests/05_memory.bc
./vm tests/05_memory.bc
```
Expected:
```
Top of stack: 42
```

---

### Test 6 — Function Call
```
./assembler tests/06_function.asm tests/06_function.bc
./vm tests/06_function.bc
```
Expected:
```
Top of stack: 30
```

---

### Test 7 — Area of a Circle (Given Example)
```
./assembler tests/07_circle_area.asm tests/07_circle_area.bc
./vm tests/07_circle_area.bc
```
Expected:
```
Top of stack: 75
```

---

## Error Handling

The VM terminates execution with an error message for:
- Stack underflow / overflow
- Division by zero
- Invalid opcode
- Invalid jump or memory address
- Invalid return stack access

Execution is deterministic and memory-safe.

---

## Assumptions and Design Decisions

- All data values are 32-bit signed integers.
- Boolean values are represented as integers (`0` or `1`).
- Jump and call addresses are **absolute byte offsets** in bytecode.
- A separate return stack is used for function calls.
- The assembler uses a **two-pass design** to resolve labels.

---

## How to Clean

```
make clean
```

Removes:
- `vm`
- `assembler`

---
