## 1. 格式，基本调用
yuv420sp: NV21, NV12 格式的统称。

OpenCV 的 cvtColor 可以直接做 NV21=>RGB 的转换：
```

```

OpenCV 做 RGB => NV21 的转换， 无法通过单次调用 cvtColor 实现；我的做法是， RGB => I420 => NV21 ,
RGB => I420 可以用 cvtColor, I420 => NV21， 则可以手写实现（把分离的U、V通道，合并为V/U交错的单个通道）


## 2. 原理
根据 YUV 和 RGB 的转换公式，互相计算。

公式计算是引起混乱的根源。有 BT 601， BT 709， BT 2020 这些视频标准。

选定视频标准后， 还有 full range / narrow range 的差别。 full range 就是 [0, 255]， narrow range 则是更小的一个范围。。具体看公式。。


## 3. 加速原理

首先是把 浮点系数 转换为整数， 定点化。 这里的问题在于，多少位定点化是合适的？ 再一次带来了差异。

然后是 NEON / SSE 指令加速。 通常和定点化直接算没差别， 除非手写了 bug。。

## 4. 和 OpenCV 保持一样的精度？

选定平台分别为 x86 和 arm neon.

OpenCV 的 cvtColor 底层调用函数，在 x86 走的是 opencv 自己实现的那套， BT 601 + 魔改的 narrow range + 20位定点化移位置。

x86 结果和 arm neon 结果不一致。因为在 arm neon 平台走的是 hal 层的 carotene 库的实现：

- /home/zz/work/opencv-4.5.4-pre/modules/imgproc/src/color_yuv.dispatch.cpp , L118
```c++
void cvtTwoPlaneYUVtoBGR(const uchar * src_data, size_t src_step,
                         uchar * dst_data, size_t dst_step,
                         int dst_width, int dst_height,
                         int dcn, bool swapBlue, int uIdx)
{
    CV_INSTRUMENT_REGION();

    printf("--- before CALL_HAL(cvtTwoPlaneYUVtoBGR)\n");
    CALL_HAL(cvtTwoPlaneYUVtoBGR, cv_hal_cvtTwoPlaneYUVtoBGR, src_data, src_step, dst_data, dst_step, dst_width, dst_height, dcn, swapBlue, uIdx); // 这一句，在android上，走的是 cv_hal_cvtTwoPlaneYUVtoBGR
    printf("--- after  CALL_HAL(cvtTwoPlaneYUVtoBGR)\n");

    CV_CPU_DISPATCH(cvtTwoPlaneYUVtoBGR, (src_data, src_step, dst_data, dst_step, dst_width, dst_height, dcn, swapBlue, uIdx),
        CV_CPU_DISPATCH_MODES_ALL);
}
```

- 3rdparty/carotene/hal/tegra_hal.hpp
```c++
#undef cv_hal_cvtTwoPlaneYUVtoBGR
#define cv_hal_cvtTwoPlaneYUVtoBGR TEGRA_CVT2PYUVTOBGR  // cv_hal_cvtTwoPlaneYUVtoBGR , 被替换为 TEGRA_CVT2PYUVTOBGR 宏
```

- 3rdparty/carotene/hal/tegra_hal.hpp ， TEGRA_CVT2PYUVTOBGR 宏 ， 略长，做的是 dispatch
```c++
#define TEGRA_CVT2PYUVTOBGR(src_data, src_step, dst_data, dst_step, dst_width, dst_height, dcn, swapBlue, uIdx) \
( \
    CAROTENE_NS::isSupportedConfiguration() ? \
        dcn == 3 ? \
            uIdx == 0 ? \
                (swapBlue ? \
                    CAROTENE_NS::yuv420i2rgb(CAROTENE_NS::Size2D(dst_width, dst_height), \
                                             src_data, src_step, \
                                             src_data + src_step * dst_height, src_step, \
                                             dst_data, dst_step) : \
                    CAROTENE_NS::yuv420i2bgr(CAROTENE_NS::Size2D(dst_width, dst_height), \
                                             src_data, src_step, \
                                             src_data + src_step * dst_height, src_step, \
                                             dst_data, dst_step)), \
                CV_HAL_ERROR_OK : \
            uIdx == 1 ? \
                (swapBlue ? \
                    CAROTENE_NS::yuv420sp2rgb(CAROTENE_NS::Size2D(dst_width, dst_height), \
                                              src_data, src_step, \
                                              src_data + src_step * dst_height, src_step, \
                                              dst_data, dst_step) : \
                    CAROTENE_NS::yuv420sp2bgr(CAROTENE_NS::Size2D(dst_width, dst_height), \
                                              src_data, src_step, \
                                              src_data + src_step * dst_height, src_step, \
                                              dst_data, dst_step)), \
                CV_HAL_ERROR_OK : \
            CV_HAL_ERROR_NOT_IMPLEMENTED : \
        dcn == 4 ? \
            uIdx == 0 ? \
                (swapBlue ? \
                    CAROTENE_NS::yuv420i2rgbx(CAROTENE_NS::Size2D(dst_width, dst_height), \
                                              src_data, src_step, \
                                              src_data + src_step * dst_height, src_step, \
                                              dst_data, dst_step) : \
                    CAROTENE_NS::yuv420i2bgrx(CAROTENE_NS::Size2D(dst_width, dst_height), \
                                              src_data, src_step, \
                                              src_data + src_step * dst_height, src_step, \
                                              dst_data, dst_step)), \
                CV_HAL_ERROR_OK : \
            uIdx == 1 ? \
                (swapBlue ? \
                    CAROTENE_NS::yuv420sp2rgbx(CAROTENE_NS::Size2D(dst_width, dst_height), \
                                               src_data, src_step, \
                                               src_data + src_step * dst_height, src_step, \
                                               dst_data, dst_step) : \
                    CAROTENE_NS::yuv420sp2bgrx(CAROTENE_NS::Size2D(dst_width, dst_height), \
                                               src_data, src_step, \
                                               src_data + src_step * dst_height, src_step, \
                                               dst_data, dst_step)), \
                CV_HAL_ERROR_OK : \
            CV_HAL_ERROR_NOT_IMPLEMENTED : \
        CV_HAL_ERROR_NOT_IMPLEMENTED \
    : CV_HAL_ERROR_NOT_IMPLEMENTED \
)

```

- 3rdparty/carotene/src/colorconvert.cpp ， yuv420sp2rgb 函数

```c++
void yuv420sp2rgb(const Size2D &size,
                  const u8 *  yBase, ptrdiff_t  yStride,
                  const u8 * uvBase, ptrdiff_t uvStride,
                  u8 * dstBase, ptrdiff_t dstStride)
{
    printf("--- carotene yuv420sp2rgb\n");
    // input data:
    ////////////// Y matrix:
    // {y1, y2,   y3, y4,   y5, y6,   y7, y8,   y9, y10, y11, y12, y13, y14, y15, y16}
    // {Y1, Y2,   Y3, Y4,   Y5, Y6,   Y7, Y8,   Y9, Y10, Y11, Y12, Y13, Y14, Y15, Y16}
    ////////////// UV matrix:
    // {v12, u12, v34, u34, v56, u56, v78, u78, v90 u90, V12, U12, V34, U34, V56, U56}

    // fp version
    // R = 1.164(Y - 16) + 1.596(V - 128)
    // G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
    // B = 1.164(Y - 16)                  + 2.018(U - 128)

    // integer version
    // R = [((149*y)/2 + (-14248+102*v)      )/2]/32
    // G = [((149*y)/2 + ((8663- 25*u)-52*v))/2]/32
    // B = [((149*y)/2 + (-17705+129*u)      )/2]/32

    // error estimation:
    //Rerr = 0.0000625 * y - 0.00225 * v                - 0.287
    //Gerr = 0.0000625 * y + 0.0005  * v + 0.000375 * u + 0.128625
    //Berr = 0.0000625 * y               - 0.002375 * u - 0.287375

    //real error test:
    //=================
    //R: 1 less: 520960       ==  3.11% of full space
    //G: 1 less: 251425       ==  1.50% of full space
    //B: 1 less: 455424       ==  2.71% of full space
    //=================
    //R: 1 more: 642048       ==  3.83% of full space
    //G: 1 more: 192458       ==  1.15% of full space
    //B: 1 more: 445184       ==  2.65% of full space

    internal::assertSupportedConfiguration();
#ifdef CAROTENE_NEON
    YUV420_CONSTS(3, 2, 0)
    size_t roiw16 = size.width >= 15 ? size.width - 15 : 0;

    for (size_t i = 0u; i < size.height; i+=2)
    {
        const u8 * uv = internal::getRowPtr(uvBase, uvStride, i>>1);
        const u8 * y1 = internal::getRowPtr(yBase, yStride, i);
        const u8 * y2 = internal::getRowPtr(yBase, yStride, i+1);
        u8 * dst1 = internal::getRowPtr(dstBase, dstStride, i);
        u8 * dst2 = internal::getRowPtr(dstBase, dstStride, i+1);

        size_t dj = 0u, j = 0u;
        for (; j < roiw16; dj += 48, j += 16)
        {
            internal::prefetch(uv + j);
            internal::prefetch(y1 + j);
            internal::prefetch(y2 + j);
#if !defined(__aarch64__) && defined(__GNUC__) && __GNUC__ == 4 &&  __GNUC_MINOR__ < 7 && !defined(__clang__)
            CONVERTYUV420TORGB(3, d1, d0, q5, q6)
#else
            convertYUV420.ToRGB(y1 + j, y2 + j, uv + j, dst1 + dj, dst2 + dj);   /// #define YUV420_CONSTS(cn, bIdx, vIdx) _convertYUV420<cn, bIdx, vIdx> convertYUV420;
#endif
        }
        for (; j + 2 <= size.width; j+=2, dj += 6)
        {
            convertYUV420ToRGB<3, 2, 0>(y1+j, y2+j, uv+j, dst1 + dj, dst2 + dj);
        }
    }
#else
    (void)size;
    (void)yBase;
    (void)yStride;
    (void)uvBase;
    (void)uvStride;
    (void)dstBase;
    (void)dstStride;
#endif
}
```
