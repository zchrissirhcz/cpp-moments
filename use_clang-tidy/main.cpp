#include <stdio.h>

int main()
{
    int dy = 30;
    int dx = 40;
    double tg = dy / dx;  // 非预期
    printf("tg is %lf\n", tg);

    double tg2 = (double)dy / dx;
    printf("tg2 is %lf\n", tg2); // 符合预期

    return 0;
}