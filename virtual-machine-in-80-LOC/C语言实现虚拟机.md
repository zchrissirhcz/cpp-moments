# C语言80行代码实现虚拟机

## 0x1 动机和前提
用C语言在80行代码中实现一个虚拟机， 简陋、但维持了不错抽象表达的虚拟机， 加深对计算机工作原理的理解。

只需要知道C语言中最基本的变量、控制流、函数、结构、枚举、数组即可， 代码实现只有一个 `main.c` 文件。

## 0x2 指令集和程序

**指令集**

为了简化实现， 实现的指令集里面只有4个指令：
- `PSH`: push 的意思， 压栈指令
- `ADD`: 加法指令
- `POP`: 弹栈指令
- `HLT`: halt 的意思， 弹栈指令

**程序**

每条指令搭配必要的参数，构成一条完整的指令； 多条完整的指令形成指令序列， 这就是程序。

每个指令需要的参数数量， 是根据指令本身的目的和含义来确定的。下面给出一个示例的指令序列， 每一行是一个完整的指令调用， 也就是指令本身加上参数, 行尾分号开头的内容表示注释
```
PSH 5       ; 把5压入栈中
PSH 10      ; 把10压入栈中
ADD         ; 从栈顶弹出两个值并做加法，将结果压栈
POP         ; 弹出栈顶元素，并且出于调试的目的而打印它
HLT         ; stop the program 停止程序
```

**指令集的代码实现(定义)**

使用 C 语言的枚举类型来表示上述指令集， 因为我们的指令基本上是从0到X的数字（事实上，汇编程序将获取您的汇编文件，并在某种程度上将所有操作转换为对应的数字）
```c++
typedef enum
{
    PSH,
    ADD,
    POP,
    HLT
} InstructionSet;
```

**程序的代码实现(定义)**

我们将测试程序存储为数组。因此对于测试, 编写一个简单的程序，将值5和6相加，然后打印出来。

注意：当我说打印出来时，实际上我会这样做，这样当我们调用“pop”时，我们的虚拟机就会打印出我们弹出的值。事实上，除非您正在调试或其他东西，否则您不会想这样做。

指令应该存储为数组，我将在文档顶部某处定义它；你可能会把它放在头文件中。下面是我们的测试程序：
```c++
const int program[] =
{
    PSH, 5,
    PSH, 6,
    ADD,
    POP,
    HLT
};
```

## 0x3 虚拟机工作原理
虚拟机的工作原理： "fetch" -> "decode" -> "execute" 步骤的往复循环， 直到停机。 即： 每次获取一条指令， 以及它的必要参数， 然后根据指令和参数进行一些操作； 接下来对下一条指令做同样的处理， 直到遇到 HLT 指令。

对应到前面给出的样例程序中： 把5和6推送到栈，执行加法指令，该指令将弹出栈上的两个值，将它们相加，并将结果推回到栈上。然后我们弹出结果，因为我们的弹出指令将打印值（用于调试）。

具体说来有几个问题：
- 从哪里读取指令？
- 怎样读取下一条指令？

下面一一说明。

## 0x4 获取当前指令
由于我们的实现方式中， 将程序存储为数组， 所以获取当前指令很简单， 访问数组元素即可。虚拟机有一个计数器，通常称为“程序计数器”（PC, Program Counter)、“指令指针”（IP, Instruction Pointer）， 通常简称 PC 或 IP， 本文接下来使用 PC 或 `pc` 变量来表达它。

简单起见我将 `pc` 定义为全局变量（后续你也可以把它改为定义成一个寄存器）。程序(指令序列)本身存储为整数数组。`pc` 变量用作数组中当前正在执行的指令的索引。
```c++
int pc = 0;

int main()
{
    int instruction = program[pc]; // 获取一条指令
    return 0;
}
```

每次获取一条指令后， 按需读取若干参数并执行相应的操作， 然后获取下一条指令。 译码和执行的过程暂且忽略， 获取下一条指令则需要 `pc` 增加。 对于前面提到的示例程序的执行， 首先取出 `PSH`,  然后再取出它的参数 `5`, 而在这两次 fetch 动作之间， 需要让 `pc` 加1：

```c++
int fetch()
{
    return program[pc];
}

int main()
{
    int x = fetch(); // PSH
    pc++;  // program counter increment
    int y = fetch(); // 5
}
```

那么我们如何实现自动化呢？ 也即： 自动读取所有指令， 而不是手动读取第一条、 第二条、 第n条指令。 我们知道，一个程序一直运行到通过HLT指令停止。所以我们有一个无限循环，它将一直循环直到当前指令是HLT。

```c++
#include <stdbool.h>

bool running = true;

int main()
{
    while (running)
    {
        int instruction = fetch();
        if (instruction == HLT)
        {
            running = false;
        }
        pc++;
    }
}
```

## 0x5 对指令求值
指令求值是我们实现的虚拟机的要点。每个指令需要单独处理， 简单起见我们编写一个巨大的 switch 语句。事实上，就速度而言，这通常是最好的方法，而不是为所有指令和一些抽象类或带有执行方法的接口创建 HashMap。

switch 语句中的每个 case 都是我们在枚举中定义的指令。eval 函数将接收一个参数，也就是需要求值的指令。除非使用操作数，否则我们不会在这个函数中让 PC 增加。

```C++
void eval(int instruction)
{
    switch (instruction)
    {
    case HLT:
        running = false;
        break;
    // TODO: 处理其他指令. 每条指令的处理， 包含了读取它需要的若干参数的步骤。
    }
}
```

我们正在做的是循环每个指令，检查该指令的值是否为 `HLT`，如果是则停止循环， 否则吃掉该指令并重复。让我们将其添加回虚拟机的主循环：

```c++
bool running = true;
int pc = 0;

// instruction enum
// eval function
// fetch function

int main()
{
    while (running)
    {
        int instruction = fetch();
        eval(instruction);
        pc++; // increment the PC every iteration
    }
}
```

## 0x6 栈
上一步其实是对所有指令求值搭建了通用的函数 `eval()`， 而对于具体每条指令的求值（在本文中是 `PSH`, `POP`, `ADD`）， 可以是基于栈的， 也可以是基于寄存器的。简单起见本文使用基于栈的实现。

栈是一个非常简单的数据结构。我们将为此使用数组而不是链表。因为我们的栈是固定大小的，所以我们不必担心调整大小/复制。同时， 从缓存效率考虑，使用数组而不是链表可能更好。

类似于我们有一个索引程序数组的 `pc`，我们需要一个栈指针（SP, Stack Pointer）来显示我们在栈数组中的位置。

下面是我们的栈数据结构的一个小可视化：
```c
[] // empty

PSH 5 // put 5 on **top** of the stack
[5]

PSH 6 // 6 on top of the stack
[5, 6]

POP // pop the 6 off the top
[5]

POP // pop the 5
[] // empty

PSH 6 // push a 6...
[6]

PSH 5 // etc..
[6, 5]
```

让我们按照栈来分解我们的程序：

```bash
PSH, 5,
PSH, 6,
ADD,
POP,
HLT
```

首先我们把5压栈：
```c
[5]
```

然后把6压栈:

```c
[5, 6]
```

然后add指令将弹出这些值并将它们相加，并将结果推送到栈上：

```c
[5, 6]

// pop the top value, store it in a variable called a
a = pop; // a contains 6
[5] // stack contents

// pop the top value, store it in a variable called b
b = pop; // b contains 5
[] // stack contents

// now we add b and a. Note we do it backwards, in addition
// this doesn't matter, but in other potential instructions
// for instance divide 5 / 6 is not the same as 6 / 5
result = b + a;
push result // push the result to the stack
[11] // stack contents
```

栈指针在哪里起作用？栈指针 `SP` 设置为 -1，这意味着它是空的。数组在C中是零索引的，因此如果 `SP` 为 0，它将被设置为C编译器在其中抛出的任何随机数，因为数组的内存不会被清零。

现在，如果我们向栈里压入3个值， `SP` 将变为2。因此，这里有一个具有3个值的数组：
```
        -> sp -1
    psh -> sp 0
    psh -> sp 1
    psh -> sp 2

  sp points here (sp = 2)
       |
       V
[1, 5, 9]
 0  1  2 <- array indices or "addresses"
```

现在，当我们从栈中弹出一个值时，我们递减栈指针 `SP`， 这意味着我们弹出了9，栈的顶部将是5：

```
    sp points here (sp = 1)
        |
        V
    [1, 5]
     0  1 <- these are the array indices
```

当我们想看到栈的顶部时，我们会查看当前sp的值。好吧，希望您应该知道栈是如何工作的！如果你仍然困惑，请查看维基百科上的 [这篇文章](https://en.wikipedia.org/wiki/Stack_%28abstract_data_type%29) 。

用C语言实现栈相对简单。除了 `pc` 变量之外，我们还应该定义 `sp` 变量和数组，它们将成为栈：

```c
int pc = 0;
int sp = -1;
int stack[256];

```

现在，如果我们想将一个值压栈，我们增加栈指针 `SP`，然后将值设置为当前 `SP`（我们刚刚增加了）。

**这里的顺序非常重要！** 如果你先设置值，然后再增加 `sp`，你会得到一些不好的行为，因为我们在索引 `-1` 处写入内存，不好！

```c++
// sp = -1
sp++; // sp = 0
stack[sp] = 5; // set value at stack[0] -> 5
// top of stack is now [5]
```

在我们的eval函数中，我们可以像这样添加压栈操作：
```c++
void eval(int instr)
{
    switch (instr)
    {
    case HLT:
        running = false;
        break;

    case PSH:
        sp++;
        stack[sp] = program[++pc];
        break;
    }
}
```

前一个 `eval()` 函数之间有一些不同。首先，case子句周围有大括号。如果您不熟悉这个技巧，它会为案例提供一个作用域，以便您可以在子句中定义变量。

其次，程序`[++pc]`表达式。我们为什么在这里这么做？这是因为我们的 `PSH` 指令有一个参数。`PSH，5`。紧接着 `PSH` 操作之后是我们要推送到栈的值。这里我们递增 `pc`，使其指向5，然后我们从程序数组访问该值。

```
program = [ PSH, 5, PSH, 6, ]
            0    1  2    3

when pushing:
pc starts at 0 (PSH)
pc++, so pc is now 1 (5)
sp++, allocate some space on the stack
stack[sp] = program[pc], put the value 5 on the stack
```

`POP` 指令与递减栈指针一样简单。然而，我想让弹出指令打印它刚刚弹出的值。因此，我们需要做更多的工作：
```c++
case POP:
{
    // store the value at the stack in val_popped THEN decrement the stack ptr
    int val_popped = stack[sp--];

    // print it out!
    printf("popped %d\n", val_popped);
    break;
}
```

最后是ADD指令。这一点可能会让你更难理解，这就是我们需要对case子句进行范围技巧的地方，因为我们引入了一些变量。
```c++
case ADD:
{
    // first we pop the stack and store it as 'a'
    int a = stack[sp--];

    // then we pop the top of the stack and store it as 'b'
    int b = stack[sp--];

    // we then add the result and push it to the stack
    int result = b + a;
    sp++; // increment stack pointer **before**
    stack[sp] = result; // set the value to the top of the stack

    // all done!
    break;
}
```

请注意，这里的顺序对于某些操作非常重要！如果您正在实现除法，您可能会遇到一些问题，因为：
```
5 / 4 != 4 / 5
```

栈为后进先出法。也就是说，如果我们先压入5再压入4，我们会先弹出4，然后弹出5。如果我们按pop（）/pop（），这会给出错误的表达式 `4/5`，所以正确的顺序至关重要。

到这里， 就完成了最最简单的一个 VM 了。 完整代码如下：

## 0x7 完整代码
```c
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
```

## 0x8 Reference
- [Virtual Machine in C](https://felix.engineer/blogs/virtual-machine-in-c/)