# Canny 边缘检测

2022年12月06日10:27:34

## 1. 目的
用 C++ 实现 Canny 边缘检测， 结果要和 cv::Canny() 保持完全一致。性能方面的优化不在本文范围内。

## 2. 算法原理
OpenCV 官方给出了 canny 边缘检测的教程： https://docs.opencv.org/4.x/da/d22/tutorial_py_canny.html 。 不过这个教程只能说比较含糊， 照着它实现并不能得到和 cv::Canny() 一样的结果。

在 OpenCV 4.6.0 的单元测试代码中有给出 Canny 边检检测的参考实现， 结合上述 tutorial 网页， 梳理出的算法步骤：

1. 不用做 GaussianBlur。 或者说这一步是外部调用 canny 函数的用户的责任。

2. 计算梯度。
在数学中梯度是向量， 包括大小和方向。
分别计算 x 和 y 方向的梯度幅值（通过 cv::Sobel() 实现）， 然后在手动二重循环计算梯度幅度。要区分 L2 梯度和 L1 梯度。 L2 梯度就是欧几里得求距离公式， L1 梯度就是两个绝对值相加。默认用 L1 梯度， 这是一个坑点， 往往人们按公式想到的梯度幅值是 L2 梯度。

计算梯度方向角， 用 arctan 计算即可。

3. 梯度方向角的量化。
是说360个角度太多了， 实际上还有不是整数的角度， 那就转为0, 45, 90, 135 四个角度范围， 原本是-22.5~22.5度的角度映射为0度，以此类推。

方向角量化的目的： 当前访问的像素点是 (x, y), 方向量化角度对应一条穿过 (x, y) 的直线， 直线上 (x, y) 的右侧和左侧点取出来， 然后对应三个位置的像素值比较， 看 (x, y) 的像素值是否是最大的， 不是最大则不保留。 也就是说是执行 NMS。

这里的坑在于， 4个量化后的方向的处理方式并不一样， 45度和135度这两个方向上必须严格大于， 90度和0度方向上则可以是大于等于， 并且必须是(x, y) 大于右侧或下侧的那个点。

这里的坑还在于， 45度和135度不是按 XoY 直角坐标系来表示的， 而是按图像左上角为原点来考虑的， 45度和135度要互换一下。

4. 滞后阈值处理（hysterisis threshold）
要求遍历 NMS 后的边缘图， 每个像素如果大于 highThresh 则考察它的相邻像素， 如果大于低阈值就认为也是边缘予以保留。网络上找到一些 Canny 原理解读和 Python 实现的文章， 对这个函数的实现并没有 DFS 的搜索过程， 虽然用 lena 图结果看起来还凑合， 但其实完全不对。

5. 其他坑
cv::Canny() 是支持多通道输入的。梯度幅值计算阶段将同个像素的多个通道结果取最大的那个予以保留。

## 3. 代码实现
这里放出没有经过优化的 Canny 实现。相比于 OpenCV 4.6 自带的单元测试参考实现， 把 hysterisis threshold 的实现从递归改为了迭代。

代码实现的一些坑， 简要记录：
- `std::numeric_limits<double>::lowest()` 才是表示最小的 double 取值， 而 `std::numeric_limits<double>::min()` 并不是， 它是一个正数
- 

```c++
#include "canny.hpp"

#include <limits>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stack>

namespace {

void get_gradient_magnitude(const cv::Mat& src, cv::Mat& grad, cv::Mat& grad_x, cv::Mat& grad_y, int aperture_size, bool L2gradient)
{
    cv::Sobel(src, grad_x, CV_16S, 1, 0, aperture_size, 1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(src, grad_y, CV_16S, 0, 1, aperture_size, 1, 0, cv::BORDER_REPLICATE);

    const int cn = src.channels();
    grad.create(src.size(), CV_32FC(cn));

    cv::Mat grad_n(grad.size(), grad.depth());
    cv::Mat grad_y_n(grad_y.size(), grad_y.depth());
    cv::Mat grad_x_n(grad_x.size(), grad_x.depth());

    const int h = src.rows;
    const int w = src.cols;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            for (int k = 0; k < cn; k++)
            {
                short gx = grad_x.ptr<short>(i, j)[k];
                short gy = grad_y.ptr<short>(i, j)[k];
                if (L2gradient)
                {
                    grad.ptr<float>(i, j)[k] = sqrt(gx * gx + gy * gy);
                }
                else
                {
                    grad.ptr<float>(i, j)[k] = abs(gx) + abs(gy);
                }
            }

            int maxIdx = 0;
            for (int k = 1; k < cn; k++)
            {
                if (grad.ptr<float>(i, j)[k] > grad.ptr<float>(i, j)[maxIdx])
                {
                    maxIdx = k;
                }
            }
            grad_n.at<float>(i, j) = grad.ptr<float>(i, j)[maxIdx];
            grad_y_n.at<short>(i, j) = grad_y.ptr<short>(i, j)[maxIdx];
            grad_x_n.at<short>(i, j) = grad_x.ptr<short>(i, j)[maxIdx];
        }
    }

    grad = grad_n;
    grad_y = grad_y_n;
    grad_x = grad_x_n;
}

// 如果是需要 maximum， 则保留原值
// 如果不是 maximum， 则更新为对应的负数值
// 这是一个 trick， 负号表达了“不是maximum”， 绝对值则表达了梯度幅值， 能够在下一次被被周围像素拿来比较时继续使用
// 这也是 grad 需要存储为 float 类型而不是 u8 类型的一个原因
void get_gradient_angles_and_do_nms(cv::Mat& grad, const cv::Mat& grad_x, const cv::Mat& grad_y, double lowThresh)
{
    const int h = grad.rows;
    const int w = grad.cols;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            float a = grad.at<float>(i, j);
            float b = 0;
            float c = 0;

            if (a <= lowThresh) continue;

            short gy = grad_y.at<short>(i, j);
            short gx = grad_x.at<short>(i, j);

            double tg;
            if (gx == 0)
            {
                tg = (gy > 0) ? std::numeric_limits<double>::max() : std::numeric_limits<double>::lowest();
            }
            else
            {
                tg = (double)gy / gx;
            }
            //double tg = gx ? (double)gy / gx : DBL_MAX * CV_SIGN(gy);

            const double tg22 = tan(22.5 * CV_PI / 180);
            const double tg67 = tan(67.5 * CV_PI / 180);

            double angle;
            int bi, bj, ci, cj;
            if (fabs(tg) < tg22)
            {
                angle = 0;
            }
            else if (tg >= tg22 && tg <= tg67)
            {
                angle = 45;
            }
            else if (-tg67 <= tg && tg <= -tg22)
            {
                angle = 135;
            }
            else
            {
                CV_Assert(fabs(tg) > tg67);
                angle = 90;
            }

            if (angle == 0)
            {
                bi = i;
                bj = j + 1;
                ci = i;
                cj = j - 1;
            }
            else if (angle == 90)
            {
                bi = i + 1;
                bj = j;
                ci = i - 1;
                cj = j;
            }
            else if (angle == 45)
            {
                bi = i + 1;
                bj = j + 1;
                ci = i - 1;
                cj = j - 1;
            }
            else// if (angle == 135)
            {
                bi = i - 1;
                bj = j + 1;
                ci = i + 1;
                cj = j - 1;
            }

            if (bi >= 0 && bi < h && bj >= 0 && bj < w)
            {
                b = fabs(grad.at<float>(bi, bj));
            }
            if (ci >= 0 && ci < h && cj >= 0 && cj < w)
            {
                c = fabs(grad.at<float>(ci, cj));
            }

            bool is_maximum = false;
            if (a > b && a > c)
            {
                is_maximum = true;
            }
            else if (a == b && a > c && (angle == 0 || angle == 90))
            {
                is_maximum = true;
            }

            if (!is_maximum)
            {
                grad.at<float>(i, j) = -a;
            }
        }
    }
}

void hysterisis_threshold(const cv::Mat& grad, cv::Mat& dst, int lowThresh, int highThresh)
{
    dst.create(grad.size(), CV_8UC1);
    dst = cv::Scalar::all(0);

    const int h = grad.rows;
    const int w = grad.cols;
    std::stack<std::pair<int, int>> stk;
    std::vector<std::pair<int, int>> offset = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, /*{0, 0}, */ {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (grad.at<float>(i, j) > highThresh && !dst.at<uchar>(i, j))
            {
                dst.at<uchar>(i, j) = 255;
                stk.push({i, j});
                while (!stk.empty())
                {
                    std::pair<int, int> top = stk.top();
                    stk.pop();
                    for (size_t k = 0; k < offset.size(); k++)
                    {
                        int ni = top.first + offset[k].first;
                        int nj = top.second + offset[k].second;
                        if (ni < 0 || ni >= h || nj < 0 || nj >= w) continue;
                        if (grad.at<float>(ni, nj) > lowThresh && !dst.at<uchar>(ni, nj))
                        {
                            dst.at<uchar>(ni, nj) = 255;
                            stk.push({ni, nj});
                        }
                    }
                }
            }
        }
    }
}

} // namespace

void plain::Canny(const cv::Mat& src, cv::Mat& dst, double low_thresh, double high_thresh, int aperture_size, bool L2gradient)
{
    cv::Mat grad_x;
    cv::Mat grad_y;
    cv::Mat grad;
    get_gradient_magnitude(src, grad, grad_x, grad_y, aperture_size, L2gradient);

    get_gradient_angles_and_do_nms(grad, grad_x, grad_y, low_thresh);

    hysterisis_threshold(grad, dst, low_thresh, high_thresh);
}
```

## 6. 调用测试
测试图像请使用 messi5.jpg

```c++
    const int aperture_size = 3; // for sobel
    bool L2gradient = false;
    const double lowThresh = 100;
    const double highThresh = 200;

    cv::Mat dst1;
    {
        //AutoTimer timer("plain::Canny()");
        plain::Canny(src, dst1, lowThresh, highThresh, aperture_size, L2gradient);
    }
    cv::imwrite("dst1.png", dst1);

    cv::Mat dst3;
    {
        AutoTimer timer("cv::Canny()");
        cv::Canny(src, dst3, lowThresh, highThresh, aperture_size, L2gradient);
    }
    cv::imwrite("dst3.png", dst3);
```