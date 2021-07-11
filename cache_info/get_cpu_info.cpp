#include <string>
#include <sstream>
#include <cpuinfo.h>
#include <iostream>

void get_cache_info(const char* name, const struct cpuinfo_cache* cache, std::ostringstream& oss)
{
    oss << "CPU Cache: " << name << std::endl;
    oss << " > size            : " << cache->size << std::endl;
    oss << " > associativity   : " << cache->associativity << std::endl;
    oss << " > sets            : " << cache->sets << std::endl;
    oss << " > partitions      : " << cache->partitions << std::endl;
    oss << " > line_size       : " << cache->line_size << std::endl;
    oss << " > flags           : " << cache->flags << std::endl;
    oss << " > processor_start : " << cache->processor_start << std::endl;
    oss << " > processor_count : " << cache->processor_count << std::endl;
    oss << std::endl;
}

const std::string get_cpu_info()
{
    cpuinfo_initialize();
    const struct cpuinfo_processor* proc = cpuinfo_get_current_processor();

    std::ostringstream oss;

    if (proc->cache.l1d)
        get_cache_info("L1 Data", proc->cache.l1d, oss);

    if (proc->cache.l1i)
        get_cache_info("L1 Instruction", proc->cache.l1i, oss);

    if (proc->cache.l2)
        get_cache_info("L2", proc->cache.l2, oss);

    if (proc->cache.l3)
        get_cache_info("L3", proc->cache.l3, oss);

    if (proc->cache.l4)
        get_cache_info("L4", proc->cache.l4, oss);

    return oss.str();
}

int main()
{
    printf("hello\n");
    std::string info = get_cpu_info();
    std::cout << info << std::endl;

    return 0;
}