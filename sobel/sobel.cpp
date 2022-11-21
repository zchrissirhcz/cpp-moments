#include <opencv2/opencv.hpp>

void sobel_v1(const cv::Mat& src, cv::Mat& dst, cv::BorderTypes borderType)
{
    std::vector<float> kernel_x_vec =
    {
        1, 0, -1,
        2, 0, -2,
        1, 0, -1
    };

    std::vector<float> kernel_y_vec =
    {
        1, 2, 1,
        0, 0, 0,
        -1, -2, -1
    };

    cv::Mat kernel_x(3, 3, CV_32FC1, kernel_x_vec.data());
    cv::Mat kernel_y(3, 3, CV_32FC1, kernel_y_vec.data());
    cv::flip(kernel_x, kernel_x, -1);
    cv::flip(kernel_y, kernel_y, -1);
    
    using T = float;
    const int ddepth = cv::DataType<T>::depth;

    T* kx0 = kernel_x.ptr<T>(0, 0);
    T* kx1 = kernel_x.ptr<T>(1, 0);
    T* kx2 = kernel_x.ptr<T>(2, 0);

    T* ky0 = kernel_y.ptr<T>(0, 0);
    T* ky1 = kernel_y.ptr<T>(1, 0);
    T* ky2 = kernel_y.ptr<T>(2, 0);

    const int top = 1;
    const int bottom = 1;
    const int left = 1;
    const int right = 1;

    cv::Mat padded_src;
    cv::copyMakeBorder(src, padded_src, top, bottom, left, right, borderType);

    cv::Mat grad;
    grad.create(src.size(), cv::DataType<T>::type);

    for (int i = 0; i < grad.rows; i++)
    {
        for (int j = 0; j < grad.cols; j++)
        {
            uint8_t* sp0 = padded_src.ptr(i + 0, j);
            uint8_t* sp1 = padded_src.ptr(i + 1, j);
            uint8_t* sp2 = padded_src.ptr(i + 2, j);

            T gx = sp0[0] * kx0[0] + sp0[1] * kx0[1] + sp0[2] * kx0[2]
                        + sp1[0] * kx1[0] + sp1[1] * kx1[1] + sp1[2] * kx1[2]
                        + sp2[0] * kx2[0] + sp2[1] * kx2[1] + sp2[2] * kx2[2];

            T gy = sp0[0] * ky0[0] + sp0[1] * ky0[1] + sp0[2] * ky0[2]
                        + sp1[0] * ky1[0] + sp1[1] * ky1[1] + sp1[2] * ky1[2]
                        + sp2[0] * ky2[0] + sp2[1] * ky2[1] + sp2[2] * ky2[2];

            T res = sqrt(gx * gx + gy * gy);
            grad.ptr<T>(i, j)[0] = res;
        }
    }
    dst = grad;
}


void sobel_v3(const cv::Mat& src, cv::Mat& dst, cv::BorderTypes borderType)
{
    std::vector<float> kx1_vec = {1, 2, 1};
    cv::Mat kx1(3, 1, CV_32FC1, kx1_vec.data());
    
    std::vector<float> kx2_vec = {1, 0, -1};
    cv::Mat kx2(3, 1, CV_32FC1, kx2_vec.data());

    using T = float;
    const int ddepth = cv::DataType<T>::depth;

    cv::Mat grad_x;
    cv::filter2D(src, grad_x, ddepth, kx2.t()); // 需要转置 kx2 !
    cv::filter2D(grad_x, grad_x, ddepth, kx1);


    std::vector<float> ky1_vec = {1, 0, -1};
    cv::Mat ky1(3, 1, CV_32FC1, ky1_vec.data());

    std::vector<float> ky2_vec = {1, 2, 1};
    cv::Mat ky2(3, 1, CV_32FC1, ky2_vec.data());

    cv::Mat grad_y;
    cv::filter2D(src, grad_y, ddepth, ky2.t()); // 需要转置 ky2 !
    cv::filter2D(grad_y, grad_y, ddepth, ky1);

    
    cv::Mat grad(src.size(), cv::DataType<T>::type);
    const int height = src.rows;
    const int width = src.cols;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            T gx = grad_x.ptr<T>(i, j)[0];
            T gy = grad_y.ptr<T>(i, j)[0];
            grad.ptr<T>(i, j)[0] = sqrt(gx * gx + gy * gy);
        }
    }
    dst = grad;
}

void sobel_from_unittest(const cv::Mat& src, cv::Mat& dst, int dx, int dy, cv::BorderTypes borderType)
{
    int ksize = 3;
    
    cv::Mat kx, ky;
    cv::getDerivKernels(kx, ky, dx, dy, ksize);
    const int ddepth = CV_16S;
    cv::Mat kernel = ky * kx.t();

    std::cout << "=== kx: \n" << kx << std::endl;
    std::cout << "=== ky: \n" << ky << std::endl;
    std::cout << "=== kernel: \n" << kernel << std::endl;

    cv::filter2D(src, dst, ddepth, kernel, cv::Point(-1, -1), 0, borderType);
}

void correctly_use_sobel()
{
    using T = float;
    const int ddepth = cv::DataType<T>::depth;

    const int ksize = 3;
    const int scale = 1;
    const int delta = 0;


    // 如下用法将得不到贴合 Sobel 公式的结果
    // cv::Mat grad;
    // cv::Sobel(grad, grad, ddepth, 1, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
    // 正确用法是先算出 x 和 y 方向各自的 Sobel 结果， 再整合处理

    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;
    cv::Sobel(gray, grad_x, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
    cv::Sobel(gray, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);

    // 第一种用法， opencv 官方的。 是理论公式的近似结果
    {
        // converting back to CV_8U
        cv::convertScaleAbs(grad_x, abs_grad_x);
        cv::convertScaleAbs(grad_y, abs_grad_y);
        
        cv::Mat grad;
        cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
        cv::imwrite("res1.png", grad);
    }

    // 第二种用法， 手动按原版公式计算。 可能会比较慢。
    {
        cv::Mat grad(gray.size(), cv::DataType<T>::type);
        const int h = gray.rows;
        const int w = gray.cols;
        for (int i = 0; i < h; i++)
        {
            for (int j = 0;j < w; j++)
            {
                T gx = grad_x.ptr<T>(i, j)[0];
                T gy = grad_y.ptr<T>(i, j)[0];
                T g = sqrt(gx * gx + gy * gy);
                grad.ptr<T>(i, j)[0] = g;
            }
        }
        cv::convertScaleAbs(grad, grad);
        cv::imwrite("res2.png", grad);
    }

    // 第三种方法， 用绝对值的和作为平方根的近似替代
    {
        cv::Mat grad(gray.size(), cv::DataType<T>::type);
        const int h = gray.rows;
        const int w = gray.cols;
        for (int i = 0; i < h; i++)
        {
            for (int j = 0;j < w; j++)
            {
                T gx = grad_x.ptr<T>(i, j)[0];
                T gy = grad_y.ptr<T>(i, j)[0];
                T g = abs(gx) + abs(gy);
                grad.ptr<T>(i, j)[0] = g;
            }
        }
        cv::convertScaleAbs(grad, grad);
        cv::imwrite("res3.png", grad);
    }

    // 第四种方法， 后处理步骤使用缩放的方式
    {
        cv::Mat grad(gray.size(), cv::DataType<T>::type);
        const int h = gray.rows;
        const int w = gray.cols;
        for (int i = 0; i < h; i++)
        {
            for (int j = 0;j < w; j++)
            {
                T gx = grad_x.ptr<T>(i, j)[0];
                T gy = grad_y.ptr<T>(i, j)[0];
                T g = sqrt(gx * gx + gy * gy);
                grad.ptr<T>(i, j)[0] = g;
            }
        }

        double minVal;
        double maxVal;
        cv::Point minLoc;
        cv::Point maxLoc;
        cv::minMaxLoc(grad, &minVal, &maxVal, &minLoc, &maxLoc);

        double alpha;
        if (maxVal == minVal)
        {
            alpha = 128;
        }
        else
        {
            alpha = 255.0 / (maxVal - minVal);
        }
        double beta = 0;
        cv::convertScaleAbs(grad, grad, alpha, beta);
        cv::imwrite("res4.png", grad);
    }
}