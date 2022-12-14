# 调试的哲学

## 1. 最小复现代码
理想情况是： 稳定的复现出bug，并且提供最小复现代码。

实际上， 更准确的说法应该： 最小复现工程。 工程中应当提供：
- 代码
- 编译脚本
- 依赖库的配置、 依赖库的版本

## 2. 无源码复现
因为种种原因， 整理出复现代码的成本可能很大（出现在集成部署阶段）。

此时可以通过 log 打印查询， 但注意： 让报告 bug 的人提供尽可能完整的报错信息。

例如最近一次排查 ToyVision 中的 imwrite() 失败导致 ADAS 的 testbed 莫名其妙 abort， 用了一下午时间。 实际上是 huangweike 给的报错信息不足， 只贴了半句打印。 这导致一开始”瞎找”： 第 136 行？ 有太多地方了。应该先确定版本号， 已经给出后半句打印。 这样的话其实1分钟就搞定了。