// The LC-3 VM implementation
// based on https://www.jmeiners.com/lc3-vm/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#if __linux__ || __APPLE__
#include <unistd.h>
#include <termios.h>
#elif _MSC_VER
#include <windows.h>
#include <conio.h>
#endif

#include "px_log.h"

uint16_t memory[UINT16_MAX];
int running = 1;

enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND, // 是 PSR(Processor Status Register)寄存器的低3位, 即 PSR[2:0]
    R_COUNT
};

// registers
uint16_t reg[R_COUNT];

// opcode definition
enum
{
    OP_BR = 0, // branch
    OP_ADD,    // add
    OP_LD,     // load
    OP_ST,     // store
    OP_JSR,    // jump register
    OP_AND,    // bitwise and
    OP_LDR,    // load register
    OP_STR,    // store register
    OP_RTI,    // unused
    OP_NOT,    // bitwise not
    OP_LDI,    // load indirect
    OP_STI,    // store indirect
    OP_JMP,    // jump
    OP_RES,    // reserved (unused)
    OP_LEA,    // load effective address
    OP_TRAP    // execute trap
};

// condition flags
enum
{
    FL_POS = 1 << 0,    // P, positive
    FL_ZRO = 1 << 1,    // Z, zero
    FL_NEG = 1 << 2,    // N, negative
};

// trap code
enum
{
    TRAP_GETC = 0x20,   // get character from keyboard, not echoed onto the terminal
    TRAP_OUT = 0x21,    // output a character
    TRAP_PUTS = 0x22,   // output a word string
    TRAP_IN = 0x23,     // get character from keyboard, echoed onto the terminal
    TRAP_PUTSP = 0x24,  // output a byte string
    TRAP_HALT = 0x25,   // halt the program
};

enum
{
    MR_KBSR = 0xFE00,   // keyboard status
    MR_KBDR = 0xFE02,   // keyboard data
};

void mem_write(uint16_t address, uint16_t val)
{
    memory[address] = val;
}

#if __linux__ || __APPLE__
struct termios original_tio;

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

uint16_t check_key()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}
#elif _MSC_VER

HANDLE hStdin = INVALID_HANDLE_VALUE;
DWORD fdwMode, fdwOldMode;

void disable_input_buffering()
{
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwOldMode); /* save old mode */
    fdwMode = fdwOldMode
            ^ ENABLE_ECHO_INPUT  /* no input echo */
            ^ ENABLE_LINE_INPUT; /* return when one or
                                    more characters are available */
    SetConsoleMode(hStdin, fdwMode); /* set new mode */
    FlushConsoleInputBuffer(hStdin); /* clear buffer */
}

void restore_input_buffering()
{
    SetConsoleMode(hStdin, fdwOldMode);
}

uint16_t check_key()
{
    return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();
}

#endif


void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

uint16_t mem_read(uint16_t address)
{
    if (address == MR_KBSR)
    {
        if (check_key())
        {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        }
        else
        {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[address];
}

int16_t sign_extend(int16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1)
    {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

// 取 instr 的最后 count 位， 然后做带符号的扩展
int16_t get_instr_last_part(uint16_t instr, uint8_t count)
{
    return sign_extend(instr & ((1U << count) - 1), count);
}

void update_flags(uint16_t r)
{
    if (reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r] >> 15) /* a 1 in the left-most bit indicates negative */
    {
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}

void do_add_instruction(uint16_t instr)
{
    uint16_t dr = (instr >> 9) & 0x7; // destination register
    uint16_t sr1 = (instr >> 6) & 0x7; // first source register
    uint16_t imm_flag = (instr >> 5) & 0x1; // whether we are in immediate mode

    if (imm_flag)
    {
        uint16_t imm5 = get_instr_last_part(instr, 5);
        reg[dr] = reg[sr1] + imm5;
    }
    else
    {
        uint16_t sr2 = instr & 0x7;
        reg[dr] = reg[sr1] + reg[sr2];
    }
    update_flags(dr);
}

void do_and_instruction(uint16_t instr)
{
    uint16_t dr = (instr >> 9) & 0x7;   // destination register
    uint16_t sr1 = (instr >> 6) & 0x7;  // first source register
    uint16_t imm_flag = (instr >> 5) & 0x1; // whether we are in immediate mode

    if (imm_flag)
    {
        uint16_t imm5 = get_instr_last_part(instr, 5);
        reg[dr] = reg[sr1] & imm5;
    }
    else
    {
        uint16_t sr2 = instr & 0x7;
        reg[dr] = reg[sr1] & reg[sr2];
    }
    update_flags(dr);
}

void do_br_instruction(uint16_t instr)
{
    uint16_t pc_offset = get_instr_last_part(instr, 9);
    uint16_t cond_flag = (instr >> 9) & 0x7;
    if (cond_flag & reg[R_COND])
    {
        reg[R_PC] = reg[R_PC] + pc_offset;
    }
}

void do_jmp_instruction(uint16_t instr)
{
    uint16_t base_r = (instr >> 6) & 0x7;
    reg[R_PC] = reg[base_r];
}

void do_jsr_instruction(uint16_t instr)
{
    reg[R_R7] = reg[R_PC];
    uint16_t flag = (instr >> 11) & 0x1;
    if (flag)
    {
        uint16_t pc_offset = get_instr_last_part(instr, 11);
        reg[R_PC] = reg[R_PC] + pc_offset;
    }
    else
    {
        uint16_t base_r = (instr >> 6) & 0x7;
        reg[R_PC] = base_r;
    }
}

void do_ld_instruction(uint16_t instr)
{
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t pc_offset = get_instr_last_part(instr, 9);
    reg[dr] = mem_read(reg[R_PC] + pc_offset);
    update_flags(dr);
}

void do_ldi_instruction(uint16_t instr)
{
    uint16_t dr = (instr >> 9) & 0x7;   // destination register
    uint16_t pc_offset = get_instr_last_part(instr, 9);
    reg[dr] = mem_read(mem_read(reg[R_PC] + pc_offset));
    update_flags(dr);
}

void do_ldr_instruction(uint16_t instr)
{
    uint16_t offset = instr & 0x3F;
    uint16_t base_r = (instr >> 6) & 0x7;
    uint16_t dr = (instr >> 9) & 0x7;
    reg[dr] = mem_read(reg[base_r] + offset);
    update_flags(dr);
}

void do_lea_instruction(uint16_t instr)
{
    uint16_t pc_offset = instr & 0x1FF;
    uint16_t dr = (instr >> 9) & 0x7;
    reg[dr] = reg[R_PC] + pc_offset;
    // update_flags(dr); // Introduction to Computer System, 3rd edition 中， 去掉了 LEA 指令更新条件码的规则。
}

void do_not_instruction(uint16_t instr)
{
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t sr = (instr >> 6) & 0x7;
    reg[dr] = ~(reg[sr]);
    update_flags(dr);
}

void do_ret_instruction(uint16_t instr)
{
    reg[R_PC] = reg[R_R7];
}

void do_res_instruction(uint16_t instr)
{
    abort();
}

void do_st_instruction(uint16_t instr)
{
    uint16_t offset = get_instr_last_part(instr, 9);
    uint16_t sr = (instr >> 9) & 0x7;
    //memory[reg[R_PC] + offset] = reg[sr];
    mem_write(reg[R_PC] + offset, reg[sr]);
}

void do_sti_instruction(uint16_t instr)
{
    uint16_t offset = get_instr_last_part(instr, 9);
    uint16_t sr = (instr >> 9) & 0x7;
    //memory[memory[reg[R_PC] + offset]] = reg[sr];
    mem_write(mem_read(reg[R_PC] + offset), reg[sr]);
}

void print_binary(uint16_t n)
{
    char buffer[17];                  /* 16 bits, plus room for a \0 */
    buffer[16] = '\0';                
    for (int i = 15; i >= 0; --i) {   /* convert bits from the end */
        buffer[i] = '0' + (n & 1);    /* '0' + bit => '0' or '1' */
        n >>= 1;                      /* make the next bit the 'low bit' */
    }

    /* At this point, `buffer` is `n` turned to a string of binary digits. */

    printf("%s\n", buffer);
}

void do_str_instruction(uint16_t instr)
{
    uint16_t sr = (instr >> 9) & 0x7;
    uint16_t base_r = (instr >> 6) & 0x7;
    int16_t offset = get_instr_last_part(instr, 6);
    memory[reg[base_r] + offset] = reg[sr]; // segfault
}

void do_puts_trap()
{
    uint16_t* c = memory + reg[R_R0];
    while (*c)
    {
        putc((char)*c, stdout);
        c++;
    }
    fflush(stdout);
}

void do_in_trap()
{
    printf("Enter a character: ");
    char c = getchar();
    putc(c, stdout);
    fflush(stdout);
    reg[R_R0] = (uint16_t)c;
    update_flags(R_R0); // ??
}

void do_putsp_trap()
{
    uint16_t* c = memory + reg[R_R0];
    while (*c)
    {
        char char1 = (*c) & 0xFF;
        putc(char1, stdout);
        char char2 = (*c) >> 8;
        if (char2)
        {
            putc(char2, stdout);
            c++;
        }
    }
    fflush(stdout);
}

void do_halt_trap()
{
    puts("HALT");
    fflush(stdout);
    running = 0;
}

void do_getc_trap()
{
    reg[R_R0] = (uint16_t)getchar();
    update_flags(R_R0); // ??
}

void do_output_trap()
{
    putc((char)reg[R_R0], stdout);
    fflush(stdout);
}

void do_trap_instruction(uint16_t instr)
{
    uint16_t trapvec = instr & 0xFF;
    reg[R_R7] = reg[R_PC];

    // 处理 trap
    switch (trapvec)
    {
    case TRAP_GETC:
        PX_LOGE("  TRAP_GETC");
        do_getc_trap();
        break;

    case TRAP_OUT:
        PX_LOGE("  TRAP_OUT");
        do_output_trap();
        break;

    case TRAP_PUTS:
        PX_LOGE("  TRAP_PUTS");
        do_puts_trap();
        break;

    case TRAP_IN:
        PX_LOGE("  TRAP_IN");
        do_in_trap();
        break;

    case TRAP_PUTSP:
        PX_LOGE("  TRAP_PUTSP");
        do_putsp_trap();
        break;

    case TRAP_HALT:
        PX_LOGE("  TRAP_HALT");
        do_halt_trap();
        break;
    }
    reg[R_PC] = reg[R_R7]; // 由于本函数中的实现， 并没有修改 PC, 因此恢复 PC 看起来有点多余。 但稳妥的做法确实应该是恢复 PC 的。
}

uint16_t swap16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

static inline bool is_little_endian()
{
    volatile uint32_t data = 0x01234567;
    return (*((uint8_t*)(&data))) == 0x67;
}

void read_image_file(FILE* file)
{
    uint16_t origin; // the origin tells us where in memory to place the image
    fread(&origin, sizeof(origin), 1, file);
    if (is_little_endian())
    {
        origin = swap16(origin);
    }

    // we know the maximum file size so we only need one fread
    uint16_t max_read = UINT16_MAX - origin;
    uint16_t* p = memory + origin;
    size_t read = fread(p, sizeof(uint16_t), max_read, file);

    // swap to little endian
    if (is_little_endian())
    {
        while (read-- > 0)
        {
            *p = swap16(*p);
            ++p;
        }
    }
}

int read_image(const char* image_path)
{
    FILE* file = fopen(image_path, "rb");
    if (!file)
    {
        return 0;
    }
    read_image_file(file);
    fclose(file);
    return 1;
}

void process()
{
    static int op_cnt = 0;

    uint16_t instr = mem_read(reg[R_PC]++); // fetch
    uint16_t op = instr >> 12;

    op_cnt++;
    // if (op_cnt == 10) break;
    // PX_LOGE("op_cnt = %d ", op_cnt);

    switch (op)
    {
    case OP_ADD:
        PX_LOGE("OP_ADD");
        do_add_instruction(instr);
        break;

    case OP_AND:
        PX_LOGE("OP_AND");
        do_and_instruction(instr);
        break;

    case OP_NOT:
        PX_LOGE("OP_NOT");
        do_not_instruction(instr);
        break;

    case OP_BR:
        PX_LOGE("OP_BR");
        do_br_instruction(instr);
        break;

    case OP_JMP:
        PX_LOGE("OP_JMP");
        do_jmp_instruction(instr);
        break;

    case OP_JSR:
        PX_LOGE("OP_JSR");
        do_jsr_instruction(instr);
        break;

    case OP_LD:
        PX_LOGE("OP_LD");
        do_ld_instruction(instr);
        
        break;

    case OP_LDI:
        PX_LOGE("OP_LDI");
        do_ldi_instruction(instr);
        break;

    case OP_LDR:
        PX_LOGE("OP_LDR");
        do_ldr_instruction(instr);
        break;

    case OP_LEA:
        PX_LOGE("OP_LEA");
        do_lea_instruction(instr);
        break;

    case OP_ST:
        PX_LOGE("OP_ST");
        do_st_instruction(instr);
        break;

    case OP_STI:
        PX_LOGE("OP_STI");
        do_sti_instruction(instr);
        break;

    case OP_STR:
        PX_LOGE("OP_STR");
        do_str_instruction(instr);
        break;

    case OP_TRAP:
        PX_LOGE("OP_TRAP");
        do_trap_instruction(instr);
        break;

    case OP_RES:
    case OP_RTI:
    default:
        abort();
        break;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s /path/to/image-file\n", argv[0]);
        exit(1);
    }

    if (!read_image(argv[1]))
    {
        fprintf(stderr, "failed to load image: %s\n", argv[1]);
        exit(1);
    }

    // 注册键盘监听信号
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

    reg[R_COND] = FL_ZRO;

    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    while (running)
    {
        process();
    }

    restore_input_buffering();

    return 0;
}