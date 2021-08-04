#include <iostream>
#include <time.h>
#include "autotimer.hpp"

#include <omp.h>

#include <chrono>
#include <thread>

#include <unordered_map>

using namespace std;


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

    printf("------------------------\n");
    std::unordered_map<unsigned,std::string> map{
        {200505,"2.5"},{200805,"3.0"},{201107,"3.1"},{201307,"4.0"},{201511,"4.5"},{201811,"5.0"},{202011,"5.1"}};
    std::cout << "We have OpenMP " << map.at(_OPENMP) << ".\n";
    std::cout << "_OPENMP is " << _OPENMP << std::endl;
    printf("------------------------\n");

    {
        AutoTimer timer("test, warmup");
        for (int i=0; i<8; i++)
        {
            test();
        }
    }

    omp_set_num_threads(4);
    {
        AutoTimer timer("test, multiple thread");
        #pragma omp parallel for
        for (int i=0; i<8; i++)
        {
            test();
        }
    }


    //std::chrono::milliseconds timespan(1000); // or whatever
    //std::this_thread::sleep_for(timespan);

    {
        AutoTimer timer("test, single thread");
        for (int i=0; i<8; i++)
        {
            test();
        }
    }

    return 0;
}