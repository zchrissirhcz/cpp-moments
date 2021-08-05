#pragma once

class Option
{
public:
    // default option
    Option();

public:
    // thread count
    // default value is the one returned by get_cpu_count()
    int num_threads;

    // the time openmp threads busy-wait for more work before going to sleep
    // default value is 20ms to keep the cores enabled
    // without too much extra power consumption afterwards
    int openmp_blocktime;
};