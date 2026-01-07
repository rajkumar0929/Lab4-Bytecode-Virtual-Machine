#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "assembler.h"
#include "vm.h"

#define MAX_LABELS 128
#define MAX_LINE   256

/* -------------------- Label Table -------------------- */

typedef struct {
    char name[32];
    int address;      // byte offset
} Label;

static Label labels[MAX_LABELS];
static int label_count = 0;

/* -------------------- Helper Functions -------------------- */

static void trim(char *s) {
    char *p = s;
    while (isspace((unsigned char)*p)) p++;
    memmove(s, p, strlen(p) + 1);

    int len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';
    }
}

static void strip_comment(char *s) {
    char *c = strchr(s, ';');
    if (c) *c = '\0';
}

static int is_label(const char *s) {
    int len = strlen(s);
    return len > 0 && s[len - 1] == ':';
}

static void add_label(const char *name, int addr) {
    if (label_count >= MAX_LABELS) {
        fprintf(stderr, "Assembler error: Too many labels\n");
        exit(1);
    }
    strcpy(labels[label_count].name, name);
    labels[label_count].address = addr;
    label_count++;
}

static int find_label(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, name) == 0)
            return labels[i].address;
    }
    return -1;
}

static void write_int32(FILE *f, int value) {
    fputc(value & 0xFF, f);
    fputc((value >> 8) & 0xFF, f);
    fputc((value >> 16) & 0xFF, f);
    fputc((value >> 24) & 0xFF, f);
}

/* -------------------- Instruction Size -------------------- */

static int instruction_size(const char *instr) {
    if (!instr) return 0;

    /* instructions with 32-bit operand */
    if (!strcmp(instr, "PUSH")  ||
        !strcmp(instr, "JMP")   ||
        !strcmp(instr, "JZ")    ||
        !strcmp(instr, "JNZ")   ||
        !strcmp(instr, "LOAD")  ||
        !strcmp(instr, "STORE") ||
        !strcmp(instr, "CALL")) {
        return 5;
    }

    /* single-byte instructions */
    return 1;
}

/* -------------------- Assembler (Two Pass) -------------------- */

int assemble(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("Failed to open input file");
        return -1;
    }

    /* ---------- PASS 1: Label Resolution ---------- */

    char line[MAX_LINE];
    int pc = 0;
    label_count = 0;

    while (fgets(line, sizeof(line), in)) {
        strip_comment(line);
        trim(line);
        if (strlen(line) == 0) continue;

        if (is_label(line)) {
            line[strlen(line) - 1] = '\0';  // remove ':'
            add_label(line, pc);
            continue;
        }

        char instr[32];
        if (sscanf(line, "%31s", instr) == 1) {
            pc += instruction_size(instr);
        }
    }

    fclose(in);

    /* ---------- PASS 2: Bytecode Generation ---------- */

    in = fopen(input_file, "r");
    FILE *out = fopen(output_file, "wb");
    if (!in || !out) {
        perror("Failed to open files for pass 2");
        return -1;
    }

    while (fgets(line, sizeof(line), in)) {
        strip_comment(line);
        trim(line);
        if (strlen(line) == 0) continue;

        if (is_label(line)) continue;

        char instr[32], operand[64];
        sscanf(line, "%31s %63s", instr, operand);

        /* -------- Instructions with operands -------- */

        if (!strcmp(instr, "PUSH")) {
            fputc(OP_PUSH, out);
            write_int32(out, atoi(operand));
        }
        else if (!strcmp(instr, "JMP")) {
            int addr = find_label(operand);
            if (addr < 0) { fprintf(stderr, "Undefined label %s\n", operand); exit(1); }
            fputc(OP_JMP, out);
            write_int32(out, addr);
        }
        else if (!strcmp(instr, "JZ")) {
            int addr = find_label(operand);
            if (addr < 0) { fprintf(stderr, "Undefined label %s\n", operand); exit(1); }
            fputc(OP_JZ, out);
            write_int32(out, addr);
        }
        else if (!strcmp(instr, "JNZ")) {
            int addr = find_label(operand);
            if (addr < 0) { fprintf(stderr, "Undefined label %s\n", operand); exit(1); }
            fputc(OP_JNZ, out);
            write_int32(out, addr);
        }
        else if (!strcmp(instr, "LOAD")) {
            fputc(OP_LOAD, out);
            write_int32(out, atoi(operand));
        }
        else if (!strcmp(instr, "STORE")) {
            fputc(OP_STORE, out);
            write_int32(out, atoi(operand));
        }
        else if (!strcmp(instr, "CALL")) {
            int addr = find_label(operand);
            if (addr < 0) { fprintf(stderr, "Undefined label %s\n", operand); exit(1); }
            fputc(OP_CALL, out);
            write_int32(out, addr);
        }

        /* -------- Single-byte instructions -------- */

        else if (!strcmp(instr, "POP"))   fputc(OP_POP, out);
        else if (!strcmp(instr, "DUP"))   fputc(OP_DUP, out);
        else if (!strcmp(instr, "ADD"))   fputc(OP_ADD, out);
        else if (!strcmp(instr, "SUB"))   fputc(OP_SUB, out);
        else if (!strcmp(instr, "MUL"))   fputc(OP_MUL, out);
        else if (!strcmp(instr, "DIV"))   fputc(OP_DIV, out);
        else if (!strcmp(instr, "CMP"))   fputc(OP_CMP, out);
        else if (!strcmp(instr, "RET"))   fputc(OP_RET, out);
        else if (!strcmp(instr, "HALT"))  fputc(OP_HALT, out);

        else {
            fprintf(stderr, "Unknown instruction: %s\n", instr);
            exit(1);
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.asm> <output.bc>\n", argv[0]);
        return 1;
    }

    if (assemble(argv[1], argv[2]) != 0) {
        fprintf(stderr, "Assembly failed\n");
        return 1;
    }

    return 0;
}

