#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>

void show_mat(const cv::Mat& a)
{
    std::cout << a << std::endl;
}

int main()
{
    printf("hello world\n");
    cv::Mat a(cv::Size(3, 3), CV_32FC3, cv::Scalar(10, 20));
    printf("a=\n");
    show_mat(a);
    cv::Mat b = a + 1;
    printf("b=\n");
    show_mat(b);
    cv::Mat c = a * 0.5 + 1;
    show_mat(c);

    return 0;
}
