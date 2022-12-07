# 使用 clangd

## 全局配置
clangd 的全局配置文件在 `/home/zz/.config/clangd/config.yaml`.

也就是说如果打开 VSCode， 然后看到 clang 标出的红色波浪线， 例如提示 `std::make_unique` 不可用， 并且当前工程目录下也没有 `.clangd` 配置文件， 那么很大概率是全局配置文件有问题， 即： 缺乏配置， 或配置不当需要更正。

`std::make_unique` 是 C++14 引入的， 因此如果在 ~/.config/clangd/config.yaml 中指定了 C++11 那肯定会报错的。

## per project 的配置
在 Visual Studio 中可以在工程的属性中配置自定义的宏定义， 例如正在看 ARM NEON 优化的代码因此打算手动开启 `__ARM_NEON` 宏。

类似的， 可以在当前工程根目录添加 `.clangd` 文件， 增加如下两行：
```yaml
CompileFlags:
    Add: [-DCAROTENE_NEON=1]
```
这样的话， VSCode C++ 代码高亮提示（我假定你安装并使用的是 Clangd 插件）就能以高亮的形式显示 ARM NEON 的代码了。