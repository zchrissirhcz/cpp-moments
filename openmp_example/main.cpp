#include <iostream>
#include <time.h>
#include "autotimer.hpp"
#include <opencv2/opencv.hpp>
#include <omp.h>
#include "cpu.h"
#include "option.h"
#include "platform.h"

using namespace std;

void rgb2gray(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf, const Option& opt)
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

void rgb2gray_fixed(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf, const Option& opt)
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

void rgb2gray_omp(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf, const Option& opt)
{
    size_t gray_len = height * width;
    size_t rgb_len = gray_len * 3;

    // size_t i=0; // rgb_idx
    // size_t j=0; // gray_idx
    #pragma omp parallel for num_threads(opt.num_threads)
    for(size_t i=0; i<gray_len; i++) {
        float r = rgb_buf[3*i];
        float g = rgb_buf[3*i+1];
        float b = rgb_buf[3*i+2];
        float gray = (0.299*r + 0.587*g + 0.114*b);
        gray_buf[i] = gray;
    }
}

void rgb2gray_fixed_omp(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf, const Option& opt)
{
    size_t gray_len = height * width;
    size_t rgb_len = gray_len * 3;

    // size_t i=0; // rgb_idx
    // size_t j=0; // gray_idx
    #pragma omp parallel for num_threads(opt.num_threads)
    for(size_t i=0; i<gray_len; i++) {
        uint16_t r = rgb_buf[3*i];
        uint16_t g = rgb_buf[3*i+1];
        uint16_t b = rgb_buf[3*i+2];
        uint16_t gray = (77*r + 151*g + 28*b) >> 8;
        gray_buf[i] = static_cast<uchar>(gray);
    }
}

void rgb2gray_fixed_simd(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf, const Option& opt)
{
    size_t done = 0;
    size_t num_pixels = width * height;

    // uint16_t as type to avoid overflow
    const uint16_t weight_r = 77;
    const uint16_t weight_g = 151;
    const uint16_t weight_b = 28;

#ifdef __ARM_NEON

    int nn = num_pixels >> 3;
    int remain = num_pixels - (nn << 3);
#else
    int remain = num_pixels;
#endif

#if __ARM_NEON
    //#pragma omp parallel for num_threads(opt.num_threads)
    for (int i=0; i<nn; i++) {
        uint8x8x3_t v_rgb;
        uint16x8_t v_tmp;
        uint8x8_t v_gray;

        v_rgb = vld3_u8(rgb_buf + 24 * i);
        v_tmp = vmull_u8(v_rgb.val[0], vdup_n_u8(77));
        v_tmp = vmlal_u8(v_tmp, v_rgb.val[1], vdup_n_u8(151));
        v_tmp = vmlal_u8(v_tmp, v_rgb.val[2], vdup_n_u8(28));
        v_gray = vshrn_n_u16(v_tmp, 8);
        vst1_u8(gray_buf + 8*i, v_gray);
    }
#endif // __ARM_NEON

    int i = num_pixels - remain;
    for (; remain>0; remain--) {
        gray_buf[i] = (weight_r*rgb_buf[3*i] + weight_g*rgb_buf[3*i+1] + weight_b*rgb_buf[3*i+2]) >> 8;
    }
}

void rgb2gray_fixed_simd2(const unsigned char* rgb_buf, size_t height, size_t width, unsigned char* gray_buf, const Option& opt)
{
    size_t done = 0;
    size_t num_pixels = width * height;

    const unsigned char weight_r = 77;
    const unsigned char weight_g = 151;
    const unsigned char weight_b = 28;

#ifdef __ARM_NEON

    int nn = num_pixels >> 4;
    int remain = num_pixels - (nn << 4);
#else
    int remain = num_pixels;
#endif

#if __ARM_NEON
    #pragma omp parallel for num_threads(opt.num_threads)
    for (int i=0; i<nn; i++) {
        uint8x8x3_t v_rgb;
        uint16x8_t v_tmp;
        uint8x8_t v_gray;

        v_rgb = vld3_u8(rgb_buf + 24 * i);
        v_tmp = vmull_u8(v_rgb.val[0], vdup_n_u8(77));
        v_tmp = vmlal_u8(v_tmp, v_rgb.val[1], vdup_n_u8(151));
        v_tmp = vmlal_u8(v_tmp, v_rgb.val[2], vdup_n_u8(28));
        v_gray = vshrn_n_u16(v_tmp, 8);
        vst1_u8(gray_buf + 8*i, v_gray);



        uint8x8x3_t v_rgb2;
        uint16x8_t v_tmp2;
        uint8x8_t v_gray2;

        v_rgb2 = vld3_u8(rgb_buf + 48 * i);
        v_tmp2 = vmull_u8(v_rgb2.val[0], vdup_n_u8(77));
        v_tmp2 = vmlal_u8(v_tmp2, v_rgb2.val[1], vdup_n_u8(151));
        v_tmp2 = vmlal_u8(v_tmp2, v_rgb2.val[2], vdup_n_u8(28));
        v_gray2 = vshrn_n_u16(v_tmp2, 8);
        vst1_u8(gray_buf + 16*i, v_gray2);
    }
#endif // __ARM_NEON

    int i = num_pixels - remain;
    for (; remain>0; remain--) {
        gray_buf[i] = (weight_r*rgb_buf[3*i] + weight_g*rgb_buf[3*i+1] + weight_b*rgb_buf[3*i+2]) >> 8;
    }
}

int test()
{
#if __ANDROID__
    std::string read_prefix = "/data/local/tmp";
    std::string save_prefix = "/data/local/tmp/rgb2gray";
#else
    //std::string prefix = "/home/zz/work/arccv/aidepth_pp_dbg/assets";
    std::string read_prefix = "/home/zz/data";
    std::string save_prefix = "/home/zz/data/rgb2gray/linux";
#endif
    
    Option opt;
    //opt.num_threads = 2;
    NCNN_LOGE("num_threads is %d\n", opt.num_threads);
    //opt.openmp_blocktime = 30; // 7.04
    //opt.openmp_blocktime = 25; // 6.58/7.79
    //set_kmp_blocktime(opt.openmp_blocktime);

    int default_blocktime = get_kmp_blocktime();
    printf(">>> default_blocktime is %d\n", default_blocktime);
/*
4核

// 大核
rgb2gray: took 80.684427 ms
rgb2gray omp: took 44.674739 ms
rgb2gray fixed: took 9.052969 ms
rgb2gray fixed openmp: took 20.558698 ms
rgb2gray fixed simd: took 8.306823 ms

// 小核
rgb2gray: took 232.774479 ms
rgb2gray omp: took 188.398437 ms
rgb2gray fixed: took 36.835677 ms
rgb2gray fixed openmp: took 97.764219 ms
rgb2gray fixed simd: took 19.563229 ms

// 全核
rgb2gray: took 125.385104 ms
rgb2gray omp: took 194.485886 ms
rgb2gray fixed: took 9.634791 ms
rgb2gray fixed openmp: took 93.520573 ms
rgb2gray fixed simd: took 15.043906 ms
*/

/*
2核

//大核
rgb2gray: took 80.724167 ms
rgb2gray omp: took 74.658646 ms
rgb2gray fixed: took 9.074948 ms
rgb2gray fixed openmp: took 25.115885 ms
rgb2gray fixed simd: took 7.610208 ms

//小核
rgb2gray: took 232.920260 ms
rgb2gray omp: took 351.494322 ms
rgb2gray fixed: took 36.616250 ms
rgb2gray fixed openmp: took 183.016198 ms
rgb2gray fixed simd: took 28.233386 ms

//全核
rgb2gray: took 122.191197 ms
rgb2gray omp: took 110.915990 ms
rgb2gray fixed: took 10.144583 ms
rgb2gray fixed openmp: took 25.183750 ms
rgb2gray fixed simd: took 7.664792 ms
*/


    set_cpu_powersave(2);//绑大核
    //set_cpu_powersave(1);//绑小核
    //set_cpu_powersave(0);//所有核都可以
    set_omp_dynamic(0);

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
            rgb2gray(rgb_buf, height, width, gray_buf, opt);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray.png";
        cv::imwrite(save_path, gray);
    }

    {
        {
            AutoTimer timer("rgb2gray omp");
            rgb2gray_omp(rgb_buf, height, width, gray_buf, opt);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_omp.png";
        cv::imwrite(save_path, gray);
    }

    {
        {
            AutoTimer timer("rgb2gray fixed");
            rgb2gray_fixed(rgb_buf, height, width, gray_buf, opt);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_fixed.png";
        cv::imwrite(save_path, gray);
    }

    {
        {
            AutoTimer timer("rgb2gray fixed openmp");
            rgb2gray_fixed_omp(rgb_buf, height, width, gray_buf, opt);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_fixed_omp.png";
        cv::imwrite(save_path, gray);
    }


    {
        {
            AutoTimer timer("rgb2gray fixed simd");
            rgb2gray_fixed_simd(rgb_buf, height, width, gray_buf, opt);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_fixed_simd.png";
        cv::imwrite(save_path, gray);
    }



    {
        {
            AutoTimer timer("rgb2gray fixed simd2");
            rgb2gray_fixed_simd2(rgb_buf, height, width, gray_buf, opt);
        }

        std::string save_path = save_prefix + "/ASRDebug_0_7680x4320_gray_fixed_simd2.png";
        cv::imwrite(save_path, gray);
    }


    return 0;
}

int main()
{

    test();

    return 0;
}