#pragma once

#if defined __ANDROID__ || defined __linux__
#include <sched.h> // cpu_set_t
#endif

class CpuSet
{
public:
    CpuSet();
    void enable(int cpu);
    void disable(int cpu);
    void disable_all();
    bool is_enabled(int cpu) const;
    int num_enabled() const;

public:
#if defined __ANDROID__ || defined __linux__
    cpu_set_t cpu_set;
#endif
#if __APPLE__
    unsigned int policy;
#endif
};

// cpu info
int get_big_cpu_count();

// bind all threads on little clusters if powersave enabled
// affects HMP arch cpu like ARM big.LITTLE
// only implemented on android at the moment
// switching powersave is expensive and not thread-safe
// 0 = all cores enabled(default)
// 1 = only little clusters enabled
// 2 = only big clusters enabled
// return 0 if success for setter function
int get_cpu_powersave();
int set_cpu_powersave(int powersave);

void set_omp_num_threads(int num_threads);
void set_omp_dynamic(int dynamic);

int get_kmp_blocktime();
void set_kmp_blocktime(int time_ms);

// convenient wrapper
const CpuSet& get_cpu_thread_affinity_mask(int powersave);