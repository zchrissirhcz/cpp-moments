#include <iostream>
#include <time.h>
#include "autotimer.hpp"

#include <omp.h>

#include <chrono>
#include <thread>



void test()
{
    int a = 0;
    for (int i=0; i<100000000; i++)
    {
        a++;
    }
}

int main()
{


    omp_set_num_threads(4);
    {
        AutoTimer timer("test, multiple thread");
        #pragma omp parallel for
        for (int i=0; i<8; i++)
        {
            test();
        }
    }


    std::chrono::milliseconds timespan(1000); // or whatever

    std::this_thread::sleep_for(timespan);

    {
        AutoTimer timer("test, single thread");
        for (int i=0; i<8; i++)
        {
            test();
        }
    }

    return 0;
}