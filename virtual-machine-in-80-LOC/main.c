// 基于栈的虚拟机
// ref: https://felix.engineer/blogs/virtual-machine-in-c/#instruction-set

#include <stdio.h>
#include <stdbool.h>

typedef enum
{
    PSH,
    ADD,
    POP,
    HLT
} InstructionSet;

const int program[] = 
{
    PSH, 5,
    PSH, 6,
    ADD,
    POP,
    HLT
};

int pc = 0;
int sp = -1;
bool running = true;
int stack[256];

int fetch()
{
    return program[pc];
}

void eval(int instruction)
{
    switch (instruction)
    {
    case HLT:
        running = false;
        break;
    
    case PSH:
        sp++;
        pc++;
        stack[sp] = program[pc];
        break;
    
    case POP:
    {
        int top = stack[sp];
        sp--;
        printf("popped %d\n", top);
        break;
    }

    case ADD:
    {
        int a = stack[sp];
        sp--;

        int b = stack[sp];
        sp--;

        int result = a + b;
        sp++;
        stack[sp] = result;
        break;
    }

    }
}

int main()
{
    while (running)
    {
        int x = fetch();
        eval(x);
        pc++;
    }

    return 0;
}