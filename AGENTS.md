# Repository Guidelines

## Project Structure & Module Organization
- `user/` contains the Lava 虚拟机、运行时以及入口程序（如 `main.c`、`lvm.c`、`lavasim.c`）。这些文件会被编译为核心执行逻辑（未来迁移到 WASM）。
- `drivers/` 持有所有 STM32 外设驱动（LCD、按键、SD/FatFs、RTC、延时等）。浏览器端需要以同名接口重写这些模块。
- `windgui/` 提供建立在 LCD 驱动上的 2D 绘图与字体支持。
- `docs/` 汇总迁移方案、设计说明；新增文档请保持 Markdown。
- 其余目录（如 `stm32lib/`, `system/`, `project/`, `tools/`）多为嵌入式依赖或工程脚手架，评估后再更新。

## Build, Test, and Development Commands
- 嵌入式原版：使用 Keil 或者 `scons`（依据各 `SConscript`）进行编译，例如：
  ```bash
  scons TARGET=stm32
  ```
- WASM 迁移：建议使用 Emscripten，示例命令：
  ```bash
  emcc user/*.c windgui/*.c -sUSE_SDL=2 -o lava.html
  ```
  根据需要替换驱动实现并导出必要符号。
- 当前仓库没有集中式自动化测试脚本；运行 `.lav` 示例验证功能。

## Coding Style & Naming Conventions
- C 代码遵循 4 空格缩进，不使用制表符；保持 `snake_case` 函数与变量命名，常量使用大写下划线。
- 头文件需提供 include guard，参考 `__MODULE_H__` 风格。
- 新增 JS/TS 代码请遵循 ESLint 默认（如果引入工具），建议使用 `camelCase`。

## Testing Guidelines
- 没有现成的单元测试框架；推荐为关键逻辑添加可在主机端运行的最小化测试，或在浏览器端通过开发者工具验证渲染、输入、文件 API。
- 对 `.lav` 样例进行回归测试：确保文件浏览、按键映射、绘图输出与原始设备一致。

## Commit & Pull Request Guidelines
- Commit 信息建议使用动词开头的短句，例如 `Refactor lvm opcode handler`、`Add canvas LCD backend`。
- Pull Request 应包含变更摘要、测试说明（如运行的 `.lav` 文件）、以及相关截图或录屏（特别是图形渲染改动）。
- 若关联 Issues，请在 PR 描述中引用（`Fixes #123`）。

