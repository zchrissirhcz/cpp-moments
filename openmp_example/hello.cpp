#include <stdio.h>
#include <omp.h>

int main()
{
    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        #pragma omp parallel
        {
        printf("hello(%d)", ID);
        printf("world(%d)\n", ID);
        }
    }
}