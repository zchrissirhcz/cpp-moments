#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>

void thread_func(cv::Mat &mat) {
    cv::Mat dst_mat = mat;
}

int main(int c, const char *argv[]) {
    std::thread t;
    {
        cv::Mat src_mat(1280, 720, CV_8UC3);
        t = std::thread(thread_func, std::ref(src_mat));
    }

    t.join();
    return 0;
}
