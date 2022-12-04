## 常见平台支持
| 平台                  | 可检测泄漏？ | 可检测越界？ |
| --------------------- | -----------  | ------------ |
| VS(x86/x64)           | ❌          | ✔️          |
| Android(arm32/arm64)  | ❌          | ✔️          |
| Linux(GCC/Clang, x64) | ✔️          | ✔️          |
| AppleClang(x64)       | ✔️          | ✔️          |
| AppleClang(M1, arm64) | ❌          | ✔️          |

## 常见环境变量
```bash
# 关闭颜色
export ASAN_OPTIONS=color=never

# 开启lsan，检查泄漏
export ASAN_OPTIONS=detect_leaks=1

# 关闭lsan，检查泄漏
export ASAN_OPTIONS=detect_leaks=0

# 检查线程泄漏（tsan）
export TSAN_OPTIONS=report_thread_leaks=1

# 检查泄漏
export LSAN_OPTIONS=verbosity=1:log_threads=1
```

## 默认环境变量
https://github.com/google/sanitizers/wiki/SanitizerCommonFlags

## ASAN 使用
对于内存非法访问的检测， 需要在编库的时候就开启 asan 的 flags。

## LSAN 使用
库不用重新编， testbed 重新编译时开 asan 的 flags 即可。

## TSAN 使用
tsan 能干两件事：
1. 线程泄漏
2. data race

## 一些坑
1. 需要手动指定 `ASAN_OPTIONS=intercept_tls_get_addr=0`
若不指定， 则可能出现莫名奇妙的报告，但代码看了半天也看不出问题。

- https://github.com/google/sanitizers/issues/723
- https://github.com/google/sanitizers/issues/1322
- https://github.com/google/sanitizers/issues/1321