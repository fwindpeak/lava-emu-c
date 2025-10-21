# Repository Guidelines (WASM Port)

## Project Structure & Module Organization
- `wasm/src/`：核心 C 源码。包含 `lvm.c`、`lavasim.c`、`boshi.c`、`prtscr.c` 等 VM/运行时逻辑，以及在浏览器端复刻外设行为的 `*_stub.c`。
- `wasm/include/`：与上述源码配套的头文件，屏蔽掉 STM32 相关依赖，导出最小化接口给 JS/worker。
- `wasm/build.sh`：基于 Emscripten 的构建脚本，输出 `web/lava.js` 与 `web/lava.wasm`。
- `web/`：浏览器宿主，包含 `index.html`、`main.js`、`wasm-worker.js`、样式与工具脚本。
- `docs/`：迁移方案、技术设计、调试记录。新增文档请使用 Markdown 并在文件开头提供简短摘要。
- 根目录其余脚本（`convert_encoding.sh` 等）用于编码转换或构建辅助，保留时需注明用途。

## Build, Test, and Development Commands
- 构建 WebAssembly 版本：
  ```bash
  ./wasm/build.sh
  ```
  需提前激活 EMSDK 环境，脚本会自动创建 `.emcache/` 作为默认缓存目录。
- 运行浏览器端 Demo：
  ```bash
  cd web
  python -m http.server 8080
  # 或使用任意静态服务器，例如 npx serve
  ```
  打开 `index.html` 后，通过“选择程序目录”按钮挂载 `.lav` 资源。
- 当前无自动化测试；通过加载示例 `.lav`、观察画面刷新、按键响应、文件 I/O 来验证变更。

## Coding Style & Naming Conventions
- C 代码：4 空格缩进，禁止制表符，函数/变量使用 `snake_case`，常量使用 `UPPER_CASE`，头文件加上 `__MODULE_H__` 风格的 include guard。
- JS/Worker 代码：遵循现代 ES 模块语法，变量函数使用 `camelCase`，仅在必要处添加注释，避免过度日志。
- 文档：使用 Markdown，标题从二级开始递增，提供中英文术语对照时保持一致性。

## Testing Guidelines
- 每次改动 VM 核心或驱动接口后，至少在浏览器中跑一遍基础 `.lav` 程序（启动菜单、绘图示例、键盘交互）。
- 涉及文件系统改动时，验证目录同步、短文件名限制、重启 VM 的稳定性。
- 建议保留捕获的错误日志与问题复现步骤，追加到 `docs/` 下的调试记录或 issue。

## Commit & Pull Request Guidelines
- Commit 消息使用动词开头的简短英文描述，例如 `Add async file bridge`、`Fix lava display scaling`。
- PR 需包含：变更摘要、测试说明（运行了哪些 `.lav`/浏览器环境）、相关截图或屏幕录制（若涉及 UI/渲染）。
- 若关联 Issue，请在描述中使用 `Fixes #123` / `Refs #456` 的形式。
