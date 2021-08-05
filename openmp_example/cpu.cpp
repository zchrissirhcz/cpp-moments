#include "cpu.h"
#include "platform.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <vector>

#if defined __ANDROID__ || defined __linux__
#include <stdint.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

static CpuSet g_thread_affinity_mask_all;
static CpuSet g_thread_affinity_mask_little;
static CpuSet g_thread_affinity_mask_big;

static int g_powersave = 0;


#if defined __ANDROID__ || defined __linux__

static int set_sched_affinity(const CpuSet& thread_affinity_mask)
{
    // set affinity for thread
#if defined(__GLIBC__) || defined(__OHOS__)
    pid_t pid = syscall(SYS_gettid);
#else
#if defined(PI3) || (defined(__MUSL__) && __MUSL_MINOR__ <= 14)
    pid_t pid = getpid();
#else
    pid_t pid = gettid();
#endif
#endif

    int syscallret = syscall(__NR_sched_setaffinity, pid, sizeof(cpu_set_t), &thread_affinity_mask.cpu_set);
    if (syscallret)
    {
        NCNN_LOGE("syscall error %d", syscallret);
        return -1;
    }

    return 0;
}

static int get_max_freq_khz(int cpuid)
{
    // first try, for all possible cpu
    char path[256];
    sprintf(path, "/sys/devices/system/cpu/cpufreq/stats/cpu%d/time_in_state", cpuid);

    FILE* fp = fopen(path, "rb");

    if (!fp)
    {
        // second try, for online cpu
        sprintf(path, "/sys/devices/system/cpu/cpu%d/cpufreq/stats/time_in_state", cpuid);
        fp = fopen(path, "rb");

        if (fp)
        {
            int max_freq_khz = 0;
            while (!feof(fp))
            {
                int freq_khz = 0;
                int nscan = fscanf(fp, "%d %*d", &freq_khz);
                if (nscan != 1)
                    break;

                if (freq_khz > max_freq_khz)
                    max_freq_khz = freq_khz;
            }

            fclose(fp);

            if (max_freq_khz != 0)
                return max_freq_khz;

            fp = NULL;
        }

        if (!fp)
        {
            // third try, for online cpu
            sprintf(path, "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", cpuid);
            fp = fopen(path, "rb");

            if (!fp)
                return -1;

            int max_freq_khz = -1;
            int nscan = fscanf(fp, "%d", &max_freq_khz);
            if (nscan != 1)
            {
                NCNN_LOGE("fscanf cpuinfo_max_freq error %d", nscan);
            }
            fclose(fp);

            return max_freq_khz;
        }
    }

    int max_freq_khz = 0;
    while (!feof(fp))
    {
        int freq_khz = 0;
        int nscan = fscanf(fp, "%d %*d", &freq_khz);
        if (nscan != 1)
            break;

        if (freq_khz > max_freq_khz)
            max_freq_khz = freq_khz;
    }

    fclose(fp);

    return max_freq_khz;
}

CpuSet::CpuSet()
{
    disable_all();
}

void CpuSet::enable(int cpu)
{
    CPU_SET(cpu, &cpu_set);
}

void CpuSet::disable(int cpu)
{
    CPU_CLR(cpu, &cpu_set);
}

void CpuSet::disable_all()
{
    CPU_ZERO(&cpu_set);
}

bool CpuSet::is_enabled(int cpu) const
{
    return CPU_ISSET(cpu, &cpu_set);
}

int CpuSet::num_enabled() const
{
    int num_enabled = 0;
    for (int i = 0; i < (int)sizeof(cpu_set_t) * 8; i++)
    {
        if (is_enabled(i))
            num_enabled++;
    }

    return num_enabled;
}

#endif

static int get_cpucount()
{
    int count = 0;
#ifdef __EMSCRIPTEN__
    if (emscripten_has_threading_support())
        count = emscripten_num_logical_cores();
    else
        count = 1;
#elif defined __ANDROID__ || defined __linux__
    // get cpu count from /proc/cpuinfo
    FILE* fp = fopen("/proc/cpuinfo", "rb");
    if (!fp)
        return 1;

    char line[1024];
    while (!feof(fp))
    {
        char* s = fgets(line, 1024, fp);
        if (!s)
            break;

        if (memcmp(line, "processor", 9) == 0)
        {
            count++;
        }
    }

    fclose(fp);
#elif __APPLE__
    size_t len = sizeof(count);
    sysctlbyname("hw.ncpu", &count, &len, NULL, 0);
#else
#ifdef _OPENMP
    count = omp_get_max_threads();
#else
    count = 1;
#endif // _OPENMP
#endif

    if (count < 1)
        count = 1;

    return count;
}


static int g_cpucount = get_cpucount();

int get_big_cpu_count()
{
    int big_cpu_count = get_cpu_thread_affinity_mask(2).num_enabled();
    return big_cpu_count ? big_cpu_count : g_cpucount;
}



static int setup_thread_affinity_masks()
{
    g_thread_affinity_mask_all.disable_all();

#if defined __ANDROID__ || defined __linux__
    int max_freq_khz_min = INT_MAX;
    int max_freq_khz_max = 0;
    std::vector<int> cpu_max_freq_khz(g_cpucount);
    for (int i = 0; i < g_cpucount; i++)
    {
        int max_freq_khz = get_max_freq_khz(i);

        //         NCNN_LOGE("%d max freq = %d khz", i, max_freq_khz);

        cpu_max_freq_khz[i] = max_freq_khz;

        if (max_freq_khz > max_freq_khz_max)
            max_freq_khz_max = max_freq_khz;
        if (max_freq_khz < max_freq_khz_min)
            max_freq_khz_min = max_freq_khz;
    }

    int max_freq_khz_medium = (max_freq_khz_min + max_freq_khz_max) / 2;
    if (max_freq_khz_medium == max_freq_khz_max)
    {
        g_thread_affinity_mask_little.disable_all();
        g_thread_affinity_mask_big = g_thread_affinity_mask_all;
        return 0;
    }

    for (int i = 0; i < g_cpucount; i++)
    {
        if (cpu_max_freq_khz[i] < max_freq_khz_medium)
            g_thread_affinity_mask_little.enable(i);
        else
            g_thread_affinity_mask_big.enable(i);
    }
#elif __APPLE__
    // affinity info from cpu model
    if (g_hw_cpufamily == CPUFAMILY_ARM_MONSOON_MISTRAL)
    {
        // 2 + 4
        g_thread_affinity_mask_big.enable(0);
        g_thread_affinity_mask_big.enable(1);
        g_thread_affinity_mask_little.enable(2);
        g_thread_affinity_mask_little.enable(3);
        g_thread_affinity_mask_little.enable(4);
        g_thread_affinity_mask_little.enable(5);
    }
    else if (g_hw_cpufamily == CPUFAMILY_ARM_VORTEX_TEMPEST || g_hw_cpufamily == CPUFAMILY_ARM_LIGHTNING_THUNDER || g_hw_cpufamily == CPUFAMILY_ARM_FIRESTORM_ICESTORM)
    {
        // 2 + 4 or 4 + 4
        if (get_cpu_count() == 6)
        {
            g_thread_affinity_mask_big.enable(0);
            g_thread_affinity_mask_big.enable(1);
            g_thread_affinity_mask_little.enable(2);
            g_thread_affinity_mask_little.enable(3);
            g_thread_affinity_mask_little.enable(4);
            g_thread_affinity_mask_little.enable(5);
        }
        else
        {
            g_thread_affinity_mask_big.enable(0);
            g_thread_affinity_mask_big.enable(1);
            g_thread_affinity_mask_big.enable(2);
            g_thread_affinity_mask_big.enable(3);
            g_thread_affinity_mask_little.enable(4);
            g_thread_affinity_mask_little.enable(5);
            g_thread_affinity_mask_little.enable(6);
            g_thread_affinity_mask_little.enable(7);
        }
    }
    else
    {
        // smp models
        g_thread_affinity_mask_little.disable_all();
        g_thread_affinity_mask_big = g_thread_affinity_mask_all;
    }
#else
    // TODO implement me for other platforms
    g_thread_affinity_mask_little.disable_all();
    g_thread_affinity_mask_big = g_thread_affinity_mask_all;
#endif

    return 0;
}


const CpuSet& get_cpu_thread_affinity_mask(int powersave)
{
    setup_thread_affinity_masks();

    if (powersave == 0)
        return g_thread_affinity_mask_all;

    if (powersave == 1)
        return g_thread_affinity_mask_little;

    if (powersave == 2)
        return g_thread_affinity_mask_big;

    NCNN_LOGE("powersave %d not supported", powersave);

    // fallback to all cores anyway
    return g_thread_affinity_mask_all;
}


int set_cpu_thread_affinity(const CpuSet& thread_affinity_mask)
{
#if defined __ANDROID__ || defined __linux__
    int num_threads = thread_affinity_mask.num_enabled();

#ifdef _OPENMP
    // set affinity for each thread
    set_omp_num_threads(num_threads);
    std::vector<int> ssarets(num_threads, 0);
    #pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < num_threads; i++)
    {
        ssarets[i] = set_sched_affinity(thread_affinity_mask);
    }
    for (int i = 0; i < num_threads; i++)
    {
        if (ssarets[i] != 0)
            return -1;
    }
#else
    int ssaret = set_sched_affinity(thread_affinity_mask);
    if (ssaret != 0)
        return -1;
#endif

    return 0;
#elif __APPLE__

#ifdef _OPENMP
    int num_threads = thread_affinity_mask.num_enabled();

    // set affinity for each thread
    set_omp_num_threads(num_threads);
    std::vector<int> ssarets(num_threads, 0);
    #pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < num_threads; i++)
    {
        // assign one core for each thread
        int core = -1 - i;
        for (int j = 0; j < (int)sizeof(thread_affinity_mask.policy) * 8; j++)
        {
            if (thread_affinity_mask.is_enabled(j))
            {
                if (core == -1)
                {
                    core = j;
                    break;
                }
                else
                {
                    core++;
                }
            }
        }
        CpuSet this_thread_affinity_mask;
        if (core != -1 - i)
        {
            this_thread_affinity_mask.enable(core);
        }

        ssarets[i] = set_sched_affinity(this_thread_affinity_mask);
    }
    for (int i = 0; i < num_threads; i++)
    {
        if (ssarets[i] != 0)
            return -1;
    }
#else
    int ssaret = set_sched_affinity(thread_affinity_mask);
    if (ssaret != 0)
        return -1;
#endif

    return 0;
#else
    // TODO
    (void)thread_affinity_mask;
    return -1;
#endif
}

int set_cpu_powersave(int powersave)
{
    if (powersave < 0 || powersave > 2)
    {
        NCNN_LOGE("powersave %d not supported", powersave);
        return -1;
    }

    const CpuSet& thread_affinity_mask = get_cpu_thread_affinity_mask(powersave);

    int ret = set_cpu_thread_affinity(thread_affinity_mask);
    if (ret != 0)
        return ret;

    g_powersave = powersave;

    return 0;
}



void set_omp_num_threads(int num_threads)
{
#ifdef _OPENMP
    omp_set_num_threads(num_threads);
#else
    (void)num_threads;
#endif
}

void set_omp_dynamic(int dynamic)
{
#ifdef _OPENMP
    omp_set_dynamic(dynamic);
#else
    (void)dynamic;
#endif
}


int get_kmp_blocktime()
{
#if defined(_OPENMP) && __clang__
    return kmp_get_blocktime();
#else
    return 0;
#endif
}

void set_kmp_blocktime(int time_ms)
{
#if defined(_OPENMP) && __clang__
    kmp_set_blocktime(time_ms);
#else
    (void)time_ms;
#endif
}