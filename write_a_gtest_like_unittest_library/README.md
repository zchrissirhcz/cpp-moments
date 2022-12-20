# 写一个像 gtest 一样（输出）的单元测试库

习惯了 googletest 的输出格式， 这里手动仿写一个。

主要参照了 utest.h 的第一个 commit, 以及 graphi-t 里的 TestRegistry 类。

我的实现称为 qtest， 分布在多个 .cpp 文件中， 展示了迭代改进的过程。


v0 目录： utest.h 的第一个 commit 的代码。
v0-bd 目录： qtest 的逐步实现过程记录。
