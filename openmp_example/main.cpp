#include <iostream>
#include <time.h>
#include "autotimer.hpp"
#include <opencv2/opencv.hpp>
#include <omp.h>

using namespace std;


void rgb2gray(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf)
{
    size_t gray_len = height * width;
    size_t rgb_len = gray_len * 3;

    // size_t i=0; // rgb_idx
    // size_t j=0; // gray_idx
    //#pragma omp parallel for
    for(size_t i=0; i<gray_len; i++) {
        float r = rgb_buf[3*i];
        float g = rgb_buf[3*i+1];
        float b = rgb_buf[3*i+2];
        float gray = (0.299*r + 0.587*g + 0.114*b);
        gray_buf[i] = gray;
    }
}

void rgb2gray_fixed(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf)
{
    size_t gray_len = height * width;
    size_t rgb_len = gray_len * 3;

    // size_t i=0; // rgb_idx
    // size_t j=0; // gray_idx
    //#pragma omp parallel for
    for(size_t i=0; i<gray_len; i++) {
        int r = rgb_buf[3*i];
        int g = rgb_buf[3*i+1];
        int b = rgb_buf[3*i+2];
        int gray = (77*r + 151*g + 28*b) >> 8;
        gray_buf[i] = gray;
    }
}

void rgb2gray_omp(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf)
{
    size_t gray_len = height * width;
    size_t rgb_len = gray_len * 3;

    // size_t i=0; // rgb_idx
    // size_t j=0; // gray_idx
    #pragma omp parallel for
    for(size_t i=0; i<gray_len; i++) {
        float r = rgb_buf[3*i];
        float g = rgb_buf[3*i+1];
        float b = rgb_buf[3*i+2];
        float gray = (0.299*r + 0.587*g + 0.114*b);
        gray_buf[i] = gray;
    }
}

void rgb2gray_fixed_omp(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf)
{
    size_t gray_len = height * width;
    size_t rgb_len = gray_len * 3;

    // size_t i=0; // rgb_idx
    // size_t j=0; // gray_idx
    #pragma omp parallel for
    for(size_t i=0; i<gray_len; i++) {
        int r = rgb_buf[3*i];
        int g = rgb_buf[3*i+1];
        int b = rgb_buf[3*i+2];
        int gray = (77*r + 151*g + 28*b) >> 8;
        gray_buf[i] = gray;
    }
}

void rgb2gray_fixed_simd(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf)
{
    size_t done = 0;
    size_t num_pixels = width * height;

    const unsigned char weight_r = 77;
    const unsigned char weight_g = 151;
    const unsigned char weight_b = 28;

#ifdef __ARM_NEON

    int nn = num_pixels >> 3;
    int remain = num_pixels - (nn << 3);
#else
    int remain = num_pixels;
#endif

#if __ARM_NEON
    #pragma omp parallel for num_threads(2)
    for (int i=0; i<nn; i++) {
        uint8x8x3_t v_rgb;
        uint16x8_t v_tmp;
        uint8x8_t v_gray;

        v_rgb = vld3_u8(rgb_buf + 24 * i);
        v_tmp = vmull_u8(v_rgb.val[0], vdup_n_u8(77));
        v_tmp = vmlal_u8(v_tmp, v_rgb.val[1], vdup_n_u8(151));
        v_tmp = vmlal_u8(v_tmp, v_rgb.val[2], vdup_n_u8(28));

        //uint8x8_t vshrn_n_u16 (uint16x8_t __a, const int __b);
        //uint8x8_t vqshrn_n_u16 (uint16x8_t __a, const int __b);
        //v_gray = vshrq_n_u16(v_gray, shift_right);
        v_gray = vshrn_n_u16(v_tmp, 8);
        vst1_u8(gray_buf + 8*i, v_gray);
    }
#endif // __ARM_NEON

    int i = num_pixels - remain;
    for (; remain>0; remain--) {
        gray_buf[i] = (weight_r*rgb_buf[3*i] + weight_g*rgb_buf[3*i+1] + weight_b*rgb_buf[3*i+2]) >> 8;
    }
}

int main()
{
#if __ANDROID__
    std::string read_prefix = "/data/local/tmp";
    std::string save_prefix = "/data/local/tmp/rgb2gray";
#else
    //std::string prefix = "/home/zz/work/arccv/aidepth_pp_dbg/assets";
    std::string read_prefix = "/home/zz/data";
    std::string save_prefix = "/home/zz/data/rgb2gray/linux";
#endif

    std::string image_path = read_prefix + "/ASRDebug_0_7680x4320.png";
    cv::Mat image = cv::imread(image_path);
    cv::Mat gray(image.size(), CV_8UC1);

    uchar* rgb_buf = image.data;
    int height = image.rows;
    int width = image.cols;
    uchar* gray_buf = gray.data;

    {
        {
            AutoTimer timer("rgb2gray");
            rgb2gray(rgb_buf, height, width, gray_buf);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray.png";
        cv::imwrite(save_path, gray);
    }

    {
        {
            AutoTimer timer("rgb2gray omp");
            rgb2gray_omp(rgb_buf, height, width, gray_buf);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_omp.png";
        cv::imwrite(save_path, gray);
    }

    {
        {
            AutoTimer timer("rgb2gray fixed");
            rgb2gray_fixed(rgb_buf, height, width, gray_buf);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_fixed.png";
        cv::imwrite(save_path, gray);
    }

    {
        {
            AutoTimer timer("rgb2gray fixed openmp");
            rgb2gray_fixed_omp(rgb_buf, height, width, gray_buf);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_fixed_omp.png";
        cv::imwrite(save_path, gray);
    }


    {
        {
            AutoTimer timer("rgb2gray fixed simd");
            rgb2gray_fixed_simd(rgb_buf, height, width, gray_buf);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_fixed_simd.png";
        cv::imwrite(save_path, gray);
    }


    return 0;
}