// 魔幻光影滤镜
// https://zhuanlan.zhihu.com/p/33311557

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "autotimer.hpp"

static std::vector<cv::Mat> build_filters()
{
    std::vector<cv::Mat> filters;
    const int ksize = 31;
    const double sigma = 4.0;
    const double lambd = 10.0;
    const double gamma = 0.5;
    const double psi = 0;
    // 此处创建16个滤波器， 只有 getGaborkernel 的第三个参数 theta 不同
    for (int i = 0; i < 16; i++)
    {
        double theta = CV_PI * i / 16;
        cv::Mat kernel = cv::getGaborKernel(cv::Size(ksize, ksize), sigma, theta, lambd, gamma, psi, CV_32F);
        kernel /= 1.5 * cv::sum(kernel)[0];
        filters.emplace_back(kernel);
    }
    return filters;
}

cv::Mat process(const cv::Mat& src, std::vector<cv::Mat>& filters)
{
    cv::Mat accum = cv::Mat::zeros(src.size(), src.type());
    for (cv::Mat kernel: filters)
    {
        cv::Mat fimg;
        {
            AutoTimer timer("filter2D");
            cv::filter2D(src, fimg, CV_8UC3, kernel); // 这里是耗时的瓶颈
        }
        {
            AutoTimer timer("getmax");
            accum = cv::max(accum, fimg);
        }
    }
    return accum;
}


int main()
{
    //const std::string image_path = "cat5.png";
    //const std::string image_path = "xiaoxiongmao.jpg";
    //const std::string image_path = "girl1.jpg";
    //const std::string image_path = "girl2.jpg";
    const std::string image_path = "caonima.jpeg";
    cv::Mat src = cv::imread(image_path);

    std::vector<cv::Mat> filters = build_filters();
    cv::Mat res = process(src, filters);
    // cv::imshow("src", src);
    //cv::imshow("result", res);
    // cv::waitKey(0);
    // cv::destroyAllWindows();
    cv::imwrite("caonima_result.png", res);

    return 0;
}