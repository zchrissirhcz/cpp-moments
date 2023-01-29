#include <opencv2/opencv.hpp>
#include <string>

// 在背景图上绘制小浣熊
void image_mapping_2d_bad()
{
    std::string bg_img_path = "forest.jpg";
    std::string racoon_img_path = "racoon.png";

    cv::Mat bg = cv::imread(bg_img_path);
    cv::Mat racoon = cv::imread(racoon_img_path, cv::IMREAD_UNCHANGED); // to load 4 channels
    printf("racoon.channel = %d\n", racoon.channels());

    cv::Point p(10, 250);
    for (int i = 0; i < racoon.rows; i++)
    {
        for (int j = 0; j < racoon.cols; j++)
        {
            int bi = p.y + i;
            int bj = p.x + j;
            for (int k = 0; k < 3; k++)
            {
                if (racoon.ptr(i, j)[k] != 255)
                {
                    bg.ptr(bi, bj)[k] = racoon.ptr(i, j)[k];
                }
            }
        }
    }

    cv::imshow("bad result", bg);
    cv::waitKey(0);
    cv::imwrite("result-bad.png", bg);
}

// 在背景图上绘制小浣熊
void image_mapping_2d_good()
{
    std::string bg_img_path = "forest.jpg";
    std::string racoon_img_path = "racoon.png";

    cv::Mat bg = cv::imread(bg_img_path);
    cv::Mat racoon = cv::imread(racoon_img_path, cv::IMREAD_UNCHANGED); // to load 4 channels
    printf("racoon.channel = %d\n", racoon.channels());

    cv::Point p(10, 250);
    for (int i = 0; i < racoon.rows; i++)
    {
        for (int j = 0; j < racoon.cols; j++)
        {
            int bi = p.y + i;
            int bj = p.x + j;
            // Alpha 通道是透明度。值越大，就越不透明；范围是 0~255，即 255 是不透明，0是全透明。
            // 当小浣熊图像里的像素不透明时， 把像素贴到背景（森林大道）图上
            if (racoon.ptr(i, j)[3] == 255)
            {
                for (int k = 0; k < 3; k++)
                {
                    bg.ptr(bi, bj)[k] = racoon.ptr(i, j)[k];
                }
            }
        }
    }

    cv::imshow("good result", bg);
    cv::waitKey(0);
    cv::imwrite("result-good.png", bg);
}

void image_mapping_2d(const cv::Mat& bg, cv::Point p, const cv::Mat& small)
{
    if (small.channels() != 4)
    {
        fprintf(stderr, "4 channels required for small image\n");
        exit(1);
    }
    if (!(bg.channels() == 3 || bg.channels() == 4))
    {
        fprintf(stderr, "3 or 4 channels required for bg image\n");
        exit(1);
    }

    if (p.x < 0 || p.x + small.cols >= bg.cols)
    {
        fprintf(stderr, "Point P position not valid, or small image too big\n");
        exit(1);
    }

    if (p.y < 0 || p.y + small.rows >= bg.rows)
    {
        fprintf(stderr, "Point P position not valid, or small image too big\n");
        exit(1);
    }

    for (int i = 0; i < small.rows; i++)
    {
        for (int j = 0; j < small.cols; j++)
        {
            int bi = p.y + i;
            int bj = p.x + j;
            // Alpha 通道是透明度。值越大，就越不透明；范围是 0~255，即 255 是不透明，0是全透明。
            // 当小浣熊图像里的像素不透明时， 把像素贴到背景（森林大道）图上
            if (small.ptr(i, j)[3] == 255)
            {
                for (int k = 0; k < 3; k++)
                {
                    bg.ptr(bi, bj)[k] = small.ptr(i, j)[k];
                }
            }
        }
    }
}

int main()
{
    image_mapping_2d_bad();
    image_mapping_2d_good();

    
    image_mapping_2d();

    return 0;
}