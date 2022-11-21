# LC-3虚拟机: C语言实现

## 0x1 目的
先前用80行实现了一个极为简陋的虚拟机， 它虽然也很好的对 VM 进行了抽象（指令集， 程序， 栈； 取指令、对指令解析和求值）， 但是似乎只做了 `a+b` 这件事， 并且除了 PC、 SP 外没有涉及其他的寄存器。

这次实现的 LC-3 虚拟机， 实现了 [LC-3 指令集](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf), 区别有：
- 是基于寄存器的， 而不是基于栈的： 提供了 R0~R7 共8个通用寄存器
- 更多的指令： 提供了 ADD, LD, ST, JSR, AND, LDR, STR, RTI, NOT, LDI, STI, JMP, LEA, TRAP 共14个有效的指令
- 支持分支操作： 提供了 COND 条件状态寄存器， 支持 POS, ZRO, NEG 三种状态
- 支持 trap， 能用于响应键盘中断事件
- 地址空间增大为 16bit， 其中 0x0000~0x2FFF 是 trap vector， 0x3000 开始则是用户程序地址空间
- 每条指令都是16bit长， 其中高4bit是opcode， 其余部分则是参数或没使用。
- 能够运行 2048 这款小游戏（控制台）

网络上已经有很多相应的实现， 有的是250行完成， 有的则是400行实现。 我实现后做了必要的调整， 代码更加清晰， 代码长度约600行。只需要有 C 语言基础就基本上可以看懂， 不过关于键盘中断处理的部分需要在 Linux 下运行。

以下贴出我按照 [[译] 400 行 C 代码实现一个虚拟机（2018）](http://arthurchiao.art/blog/write-your-own-virtual-machine-zh/) 操作后的一些补充记录。

## 0x2 程序整体框架
VM 的作用就是读取二进制的机器码然后翻译和执行， 直到遇到 HALT 停机指令。 对于要执行的 2048 小游戏来说， 先准备好 2048.obj 这个二进制文件， 读取它的所有内容到内存（模拟出的内存）；然后从 0x3000 这一起始位置开始读取， 每次读取一个指令（16bit）， PC 寄存器加1， 解析和执行这个刚读取到的指令； 然后读取下一条指令， 往复循环。此外， 为了能够在解释执行 2048 游戏时能用键盘交互操作， 还需要注册对应的信号和处理函数， 并禁用输入缓冲。整体的伪代码如下：

```c++
int main()
{
    content = read_binary_file("2048.obj");
    reg[R_PC] = 0x3000;

    signal(SIGINT, handle_interrupt);
    disable_input_buffering();
    while (1)
    {
        instruction = mem_read(reg[R_PC]);
        reg[R_PC]++;

        ret = execute(instruction);
        if (ret != 0)
            break;
    }
    restore_input_buffering();

    return 0;
}
```

## 0x3 常规指令的处理 - sign_extend() 的修改
常规指令这里指代 trap 之外的指令， 例如 ADD, AND, NOT， LEA 等。参照指令集文档， 以及 tutorial 中给出的 ADD 指令的例子， 很快就能写好所有常规指令的 decode 和 execution 过程的代码。

原版教程中给出了 `sign_extend()` 函数的实现以及多处调用， 看起来挺美好， 但如果稍微不听话不完全按 tutorial 来， 容易踩坑：

1. 多次调用 `sign_extend()` 可能写错参数

调用 `sign_extend()` 函数的地方有多处， 而每次调用都需要传入两个参数： 一个是 mask， 另一个是位数， 这两个参数其实只用第二个就可以表达第一个， 因此有点冗余； 而多次调用这样的冗余表达的代码， 一不小心少写一个 F 就导致在 debug 上浪费几十分钟（想想看， 2048的界面迟迟跑不出来）。

2. 内存越界问题

`sign_extend()` 更大的坑在于， 如果不打算用原文 tutorial 中对于内存读写封装的 `mem_write()` 函数而是直接写入表示内存的数组， 会发生内存越界。

原版 tutorial 以及译文中， `sign_extend()`返回值是 `uint16_t` 类型， 通常作为 offset 存在， 和 pc 相加后作为一个内存地址， 然后从 memory 数组中获取元素， 即:
```c++
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instr & 0x1ff, 9);
    mem_write(reg[R_PC] + pc_offset, reg[dr]);
```
这里的问题在于： 如果 `instr` 最后9位全是1， 那么 `sign_extend` 后得到的是 65535， 而不是 -1； 而 65535 和 `reg[R_PC]` 相加， 由于这两个都是 uint16_t 类型的， 那么肯定是会大于 65535。

这时候， 如果我没有使用封装的 `mem_write()` 函数， 而是直接用 `memory[reg[R_PC] + pc_offset] = reg[dr]`， 就会导致越界。

sign_extend 的原版实现是这样的：
```c++
uint16_t sign_extend(uint16_t x, int bit_count) {
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}
```

实际上这里无论是输入参数还是返回值， 用 `int16_t` 类型都是更加妥当的, 避免了传参和接收结果时的数据类型隐式转换， 降低了心智负担， 消除了不必要的内存越界问题：
```c++
int16_t sign_extend(int16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1)
    {
        x |= (0xFFFF << bit_count);
    }
    return x;
}
```

## 0x4 TRAP 的处理
这一块的处理我没有太多想法， 原文 tutorial 的实现， 是一种简化方法， 是用操作系统自带的 getchar 等函数， 替代了手写汇编， 也替代了轮询导致的 CPU 高占用率问题。

## 0x5 工程构建 - makefile 的坑
写 cmake 写的过于熟练， 也过于啰嗦， 这次使用的是 makefile
```makefile
all:
	clang main.c #-fno-omit-frame-pointer -fsanitize=address

.PHONY: run

run:
	./a.out ./2048.obj
```

遇到了几个问题， 稍作记录：

1. makefile 里的 make run 的坑。
代码中本身是有做 SIGINT 注册处理函数的， 那么运行期间按下 ctrl+c 后应该正常结束。
而如果是通过 make run 方式运行的， 会提示报错：
```
make: *** [makefile:7：run] 错误 254
```

2. makefile 的报错是中文的， 而不是原版程序报错的英文的
对于 segmentation fault， 希望报错是英文原版的。不要强行翻译啊。
例如原版输出：
```
(base) zz@arcsoft-43% ./a.out ./2048.obj 
Control the game using WASD keys.
zsh: segmentation fault (core dumped)  ./a.out ./2048.obj
```

而通过 make run 执行得到的报错则是:
```
(base) zz@arcsoft-43% make run
./a.out ./2048.obj
Control the game using WASD keys.
make: *** [makefile:7: run] 段错误 (核心已转储)
```

3. 内存越界， 但 Address Sanitizer 检测不出来

如前面 `sign_extend()` 函数中所提到的， 最开始用 uint16_t 作为参数和返回值类型， 并且直接写入 memory 数组时由于索引超过 65535 （16bit最大数字）导致越界， 但开启 ASAN 时并没有报告出来。
原因暂时不明。

## 0x6 完整代码
```c++
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#include <unistd.h>
#include <termios.h>

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

uint16_t check_key()
{
#if __linux__ || __APPLE__
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
#else
    // TODO: handle windows case
#endif
}

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
    update_flags(dr);
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

void print_binary(uint16_t n) {
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

    int op_cnt = 0;
    while (running)
    {
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

    restore_input_buffering();

    return 0;
}
```

## 0x7 References
- [Write your Own Virtual Machine](https://www.jmeiners.com/lc3-vm/)
- [[译] 400 行 C 代码实现一个虚拟机（2018）](http://arthurchiao.art/blog/write-your-own-virtual-machine-zh/)
- [2048.obj](http://arthurchiao.art/assets/img/write-your-own-virtual-machine-zh/2048.obj)