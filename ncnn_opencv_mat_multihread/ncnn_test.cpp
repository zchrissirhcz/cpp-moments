#include <iostream>
#include <thread>
#include "mat.h"

void thread_func(ncnn::Mat &mat) {
    ncnn::Mat dst_mat = mat;
}

int main(int c, const char *argv[]) {
    std::thread t;
    {
        ncnn::Mat src_mat(1280, 720);
        t = std::thread(thread_func, std::ref(src_mat));
    }

    t.join();
    return 0;
}
