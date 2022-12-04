# 版本号

### 1. 公司规范
商业软件应遵循公司规定的版本号。

### 2. Semantic Version
语义化版本 2.0.0 :  https://semver.org/lang/zh-CN/

提供 MAJOR, MINOR, PATCH 三个必选字段， 以及附带的一些可选字段。
MAJOR, MINOR, PATCH 的改变， 遵循一定的规则。


### 3. Calender Version
日历化版本 https://calver.org/overview_zhcn.html

使用日期作为版本号。特点是版本号持续递增。
可能有人疑问/反驳说，为什么不用 semantic version 那样的，能看出主次版本差别的格式？
但其实， 也许大多数项目都没法保证 API / ABI / 功能的良好兼容性。谁一开始能把所有都想好呢？

Calender Version 也许是更实用的版本, 在编写了靠谱的单元测试的前提下。

### 4. 其他参考
C++ API Design 这本书里面提到了版本号的命名、具体实现方式，可作为参考。