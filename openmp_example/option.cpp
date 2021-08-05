#include "option.h"
#include "cpu.h"

Option::Option()
{
    num_threads = get_big_cpu_count();
    openmp_blocktime = 20;
}
