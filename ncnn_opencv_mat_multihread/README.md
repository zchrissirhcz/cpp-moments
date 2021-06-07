From https://github.com/Tencent/ncnn/issues/2979

## Result
```
mkdir build
cd build
cmake ..
make
```


```
(base) zz@master% ./ncnn_test
ThreadSanitizer:DEADLYSIGNAL
==3626852==ERROR: ThreadSanitizer: SEGV on unknown address 0x7f821e57a018 (pc 0x00000047eb0c bp 0x7f821e57b000 sp 0x7ffe699e9b10 T3626852)
==3626852==The signal is caused by a READ memory access.
    #0 __sanitizer::LargeMmapAllocator<__tsan::MapUnmapCallback, __sanitizer::LargeMmapAllocatorPtrArrayDynamic, __sanitizer::LocalAddressSpaceView>::Deallocate(__sanitizer::AllocatorStats*, void*) <null> (ncnn_test+0x47eb0c)
    #1 __tsan::user_free(__tsan::ThreadState*, unsigned long, void*, bool) <null> (ncnn_test+0x47b154)
    #2 free <null> (ncnn_test+0x42738b)
    #3 ncnn::fastFree(void*) /home/zz/work/ncnn/build/install/include/ncnn/allocator.h:93:9 (ncnn_test+0x4b7c43)
    #4 ncnn::Mat::release() /home/zz/work/ncnn/build/install/include/ncnn/mat.h:1054:13 (ncnn_test+0x4b85c2)
    #5 ncnn::Mat::~Mat() /home/zz/work/ncnn/build/install/include/ncnn/mat.h:805:5 (ncnn_test+0x4b8008)
    #6 main /home/zz/work/cv-dbg/ncnn_opencv_mat_multihread/ncnn_test.cpp:14:5 (ncnn_test+0x4b7b86)
    #7 __libc_start_main /build/glibc-eX1tMB/glibc-2.31/csu/../csu/libc-start.c:308:16 (libc.so.6+0x270b2)
    #8 _start <null> (ncnn_test+0x420e6d)

ThreadSanitizer can not provide additional info.
SUMMARY: ThreadSanitizer: SEGV (/home/zz/work/cv-dbg/ncnn_opencv_mat_multihread/build/ncnn_test+0x47eb0c) in __sanitizer::LargeMmapAllocator<__tsan::MapUnmapCallback, __sanitizer::LargeMmapAllocatorPtrArrayDynamic, __sanitizer::LocalAddressSpaceView>::Deallocate(__sanitizer::AllocatorStats*, void*)
==3626852==ABORTING

```

```
(base) zz@master% ./opencv_test

```
