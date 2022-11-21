// https://cloud.tencent.com/developer/article/1165871

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "autotimer.hpp"

int main()
{
    // const std::string image_path = "texture1.png";
    const std::string image_path = "zebra.jpeg";
    cv::Mat src = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
    cv::imshow("input", src);
    cv::Mat src_f;
    src.convertTo(src_f, CV_32F);

    // 参数初始化
    int kernel_size = 3;
    double sigma = 1.0;
    double lambd = CV_PI / 8;
    double gamma = 0.5;
    double psi = 0;
    std::vector<cv::Mat> destArray;
    double theta[4];
    cv::Mat temp;

    // theta 法线方向
    theta[0] = 0;
    theta[1] = CV_PI / 4;
    theta[2] = CV_PI / 2;
    theta[3] = CV_PI - CV_PI / 4;

    // gabor 纹理检测器, 可以更多
    // filters = number of thetas * number of lambd
    // 这里 lambd 只取一个值， 所有4个filter

    {
        AutoTimer timer("xx");
        for (int i = 0; i < 4; i++)
        {
            cv::Mat kernel1;
            cv::Mat dest;
            cv::Size ksize(kernel_size, kernel_size);
            kernel1 = cv::getGaborKernel(ksize, sigma, theta[i], lambd, gamma, psi, CV_32F);
            cv::filter2D(src_f, dest, CV_32F, kernel1);
            destArray.emplace_back(dest);
        }
    }

    // 显示与保存
    cv::Mat dst1, dst2, dst3, dst4;
    
    cv::convertScaleAbs(destArray[0], dst1);
    cv::imwrite("gabor1.jpg", dst1);

    cv::convertScaleAbs(destArray[1], dst2);
    cv::imwrite("gabor2.jpg", dst2);

    cv::convertScaleAbs(destArray[2], dst3);
    cv::imwrite("gabor3.jpg", dst3);
    
    cv::convertScaleAbs(destArray[3], dst4);
    cv::imwrite("gabor4.jpg", dst4);

    // 合并结果
    cv::add(destArray[0], destArray[1], destArray[0]);
    cv::add(destArray[2], destArray[3], destArray[2]);
    cv::add(destArray[0], destArray[2], destArray[0]);
    cv::Mat dst;
    cv::convertScaleAbs(destArray[0], dst, 0.2, 0);

    // 二值化显示
    cv::Mat gray, binary;
    cv::threshold(dst, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    cv::imshow("result", dst);
    cv::imshow("binary", binary);
    cv::imwrite("result_01.png", binary);
    cv::waitKey(0);

    return 0;
}