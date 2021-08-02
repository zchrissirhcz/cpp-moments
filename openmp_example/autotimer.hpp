/*
 * Header-only time measurement class
 *
 * Usage:
 *
 * #include "autotimer.hpp"
 *
 * ...
 *
 * {
 *     AutoTimer timer("my task");
 *     my_task(); // run your task
 * } // automatically calling ~AutoTimer(), which print time cost
 *
 */

#ifndef AUTOTIMER_HPP
#define AUTOTIMER_HPP

#include <stdio.h>
//#include <iostream>

#if _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <string>

class AutoTimer
{
public:
    explicit AutoTimer(const std::string& name): mName(name) {
        mStartTime = AutoTimer::getCurrentTime();
    }
    ~AutoTimer() {
        //std::cout << mName << ": took " << GetElapsed() << " ms" << std::endl;
        fprintf(stderr, "%s: took %lf ms\n", mName.c_str(), GetElapsed());
    }
private:
    AutoTimer(const AutoTimer&);
    AutoTimer& operator=(const AutoTimer&);

private:
    double GetElapsed() const
    {
        return getCurrentTime() - mStartTime;
    }

    static inline double getCurrentTime()
    {
#ifdef _WIN32
        LARGE_INTEGER freq;
        LARGE_INTEGER pc;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&pc);
        return pc.QuadPart * 1000.0 / freq.QuadPart;
#else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000.0 + ts.tv_nsec/1000000.0;
#endif // _WIN32
    }

    std::string mName;

    double mStartTime;
};

#endif // AUTOTIMER_HPP
