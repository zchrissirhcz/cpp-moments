# 静态链接时的链接库顺序问题
这是一篇译文， 翻译自 Eli Bendersky 的博文 [Library order in static linking](https://eli.thegreenplace.net/2013/07/09/library-order-in-static-linking).

问题： 有一堆库， 和一个可执行目标。似乎怎么都链接不上。怎样确定他们的链接顺序？