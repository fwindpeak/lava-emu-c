# Lava 模拟器 WebAssembly 迁移说明

## 项目概览
该仓库已完成从 STM32 开发板到浏览器环境的初步迁移：核心 Lava 虚拟机（`lvm.c`、`lavasim.c`、`boshi.c` 等）仍然使用 C 实现，经 Emscripten 编译为 WebAssembly；与硬件耦合的外设驱动改写为可在浏览器运行的“.stub”层，由 Web Worker 与主线程中的 Canvas、键盘、文件系统 API 协同完成显示与交互。

## 当前代码结构
### C / WASM 层（`wasm/`）
- `src/main.c`：WASM 入口，执行初始化、事件循环桥接以及调用 VM 主控函数。
- `src/lvm.c`、`src/lavasim.c`、`src/boshi.c`、`src/prtscr.c`：虚拟机调度、系统服务、小游戏及截图逻辑，基本保持嵌入式版本的代码结构。
- `src/display_stub.c`、`src/lcd_stub.c`：提供与原 LCD 驱动一致的接口，最终将帧缓冲暴露给 JS 侧的 Canvas。
- `src/key_stub.c`：维护一个键盘 FIFO，与浏览器传来的键码交互。
- `src/delay_stub.c`、`src/rtc_stub.c`、`src/log_stub.c`、`src/spi_flash_stub.c`、`src/ff_stub.c`：替换硬件相关函数，使用 Emscripten/JS 提供的时间、日志、文件系统能力。
- `include/`：面向上述模块的精简头文件，去除了 STM32 专用类型与宏，只暴露 WASM 需要的声明。
- `build.sh`：集中编译脚本，设置导出符号、运行时方法、Asyncify、safe-heap 等调试选项，输出 `web/lava.js` 与 `web/lava.wasm`。

### 浏览器宿主（`web/`）
- `index.html`：提供 Canvas、虚拟键盘、控制按钮与日志区域。
- `styles.css`：暗色主题样式以及屏幕缩放布局。
- `main.js`：主线程逻辑，负责拉起 Worker、响应 UI 操作、将 Display Buffer 绘制到 Canvas，并将用户操作（键盘、目录选择、截屏）传给 Worker。
- `wasm-worker.js`：加载 `lava.js`，管理 Emscripten 虚拟文件系统、同步 `.lav` 目录内容、驱动 VM 的循环以及向主线程回传帧缓冲。
- `tools/`：包含构建后处理或调试脚本，可逐步整理。

## 构建与运行
1. 激活 EMSDK 环境，确保 `emcc` 可用。
2. 执行 `./wasm/build.sh`，脚本会在仓库根目录创建 `.emcache/` 作为 Emscripten 缓存，并将编译产物写入 `web/`。
3. 使用任何静态服务器发布 `web/`（`python -m http.server 8080` 或 `npx serve web`），在支持 File System Access API 的浏览器中打开 `index.html`。
4. 点击“选择程序目录”挂载本地 `.lav` 资源，Worker 会把文件复制到 Emscripten FS 的 `/app`，随后调用 `_lvm_request_restart` 重新进入 Lava 主菜单。
5. Canvas 默认以 1× 显示 160×80 的单色帧缓冲，可通过顶部滑块调节缩放；虚拟键盘支持鼠标和物理键交互。

## 运行时链路
1. 主线程通过 `wasm-worker.js` 创建 Worker，并发送 `init` 指令。
2. Worker 加载 `createLavaModule`，完成后向主线程发送 `wasmLoaded`。
3. 当用户选择 `.lav` 目录时，主线程递归读取文件并传给 Worker，Worker 将其写入 Emscripten FS。
4. Worker 调用导出的 `_lvm_set_base_path` 与 `_lvm_request_restart` 启动/重启 VM。
5. VM 运行过程中，`lcd_stub.c` 将图像缓冲区写入共享内存；Worker 通过 `HEAPU8` 读取并把 `displayUpdate` 消息发回主线程，主线程再绘制到 Canvas。
6. 任何按键或控制指令都会经主线程转发至 Worker，并调用 `_lava_enqueue_key` 等 C 接口。

## 已完成 & 待完善事项
| 状态 | 项目 | 说明 |
|------|------|------|
| ✅   | VM 核心编译到 WASM | `lvm`、`lavasim`、字体、截图等模块可在浏览器运行。 |
| ✅   | 显示与输入桥接     | Canvas 渲染、虚拟键盘、箭头/功能键映射已打通。 |
| ✅   | Worker + FS 同步   | 使用 File System Access API 加载 `.lav` 资源并写入虚拟文件系统。 |
| ⚠️   | 字体/资源加载      | `spi_flash_stub.c` 暂返回空数据，后续可改为从宿主读取或内置资源。 |
| ⚠️   | 性能优化           | 当前编译配置以调试为主（`-O0`、`SAFE_HEAP`），需要增加 release 选项。 |
| ⚠️   | 自动化测试         | 仍依赖人工加载 `.lav` 验证，后续可考虑录制回放或截图对比。 |
| 🚧   | 错误处理 & 日志    | `log_stub.c` 仅输出到浏览器控制台，建议补充分级日志和用户可见提示。 |

## 下一步规划
1. **资源加载**：完善 `spi_flash_stub.c`，提供字体/图片等只读资源的真实来源（内置数据或网络请求）。
2. **发布流程**：为 `wasm/build.sh` 新增 `--release` 选项，关闭调试宏、启用更高优化级别，并生成版本化产物。
3. **文件系统增强**：支持 IDBFS 缓存、增量同步以及大文件传输提示。
4. **自动化验证**：设计最小化端到端测试（例如在 Node 环境中运行 `lava.wasm` 并比对帧缓冲），或引入截图基准。
5. **UI/UX 改进**：完善虚拟键盘布局、提供状态提示（加载中/运行中/错误），并考虑国际化。
6. **文档维护**：持续更新 `docs/`，补充开发者指南、常见问题以及移植到其他宿主环境（Electron、桌面）的经验。

## 调试提示
- `wasm/build.sh` 导出的函数包含 `_lava_display_fill_demo`，可在 Worker 中手动调用，用于检测渲染通路。
- 若帧缓冲未刷新，确认 Worker 是否收到 `displayUpdate` 消息；必要时打印 `HEAPU8.subarray` 的前几个字节判定 VM 状态。
- `.lav` 目录同步失败通常是文件名或路径过长导致，主线程侧已过滤超过 16 个字符的文件名，仍需与真实资源保持一致。
- 浏览器端可通过 `chrome://inspect` 或 Edge DevTools 观察 Worker 控制台输出，定位 C 层 `printf`/`EM_ASM` 日志。

## 附录：术语对照
- **Lava VM**：嵌入式版本的虚拟机核心，现由 `wasm/src/lvm.c` 提供。
- **LavaSim**：为 VM 提供的“系统调用”层，负责图形、文本、文件、输入。
- **Stub Drivers**：在 STM32 上与外设交互的驱动，这里用 JS/Worker 提供等价接口。
- **File System Access API**：Chrome 系列浏览器提供的本地目录授权机制，本项目用来导入 `.lav` 数据集。
