# Lava 模拟器 WebAssembly 迁移说明

## 项目简介
该仓库最初面向 STM32 平台，实现了一台名为 Lava 的学习机/掌机模拟环境。程序在上电后完成硬件初始化，加载存放在 SD 卡 `/LAVA` 目录中的 `.lav` 字节码程序，通过自行实现的虚拟机解释执行，并借助 LCD、键盘、文件系统等驱动模块完成图形展示与交互。当前目标是将这套模拟器迁移到浏览器端运行，核心逻辑继续使用 C 代码编译为 WASM，而所有直接依赖硬件的驱动层改写为 JavaScript。

## 目录与主要文件
- `user/`
  - `main.c`：嵌入式入口，负责 RCC/NVIC 等时钟中断初始化以及 `hw_init()`，随后循环执行 `lava_init()` 与 `lvm_main()`。迁移时需去除 MCU 初始化，只保留框架启动逻辑。
  - `lvm.c`：字节码虚拟机内核。`lvm_run()` 实现指令解析与执行，`file_load()`/`file_select()` 等负责加载 `.lav` 文件与按键映射，`lvm_main()` 驱动主循环。
  - `lavasim.c`：模拟器运行时，提供图形、文本、输入、时间、文件等高层 API，维护 `GRAPH_BUF`、`TEXT_BUF` 两块缓冲区，并将调用下沉至驱动层。
  - `boshi.c`、`prtscr.c` 等：示例小游戏、截屏等附加功能，可选是否纳入 WASM 版本。
  - 头文件对应于上述模块的对外声明。
- `drivers/`
  - `lcd.c/h`：通过 FSMC 操作 16 bit LCD 的底层驱动，提供 `lcd_init`、`lcd_draw_block`、`lcd_draw_bw`、`lcd_get_bw` 等接口。WASM 版本需由 JS Canvas/WebGL 重写。
  - `key.c/h`：键盘 GPIO 扫描与去抖，维护按键映射表，封装 `key_get`、`key_read`、`key_check` 等调用。浏览器端应绑定 DOM 键盘事件。
  - `delay.c`、`rtc.c`、`timer.c`、`usart.c`、`led.c`、`spi*.c`、`sdcard.*` 等：依赖 STM32 外设的延时、时钟、串口、存储驱动，迁移时需替换或封装为 JS 逻辑。
  - `fonts.c/h`：字体点阵数据，可直接编译进 WASM。
- `windgui/`
  - `WindGUI.c/h`：建立在 LCD 驱动之上的彩色图形与控件库，提供线、矩形、图片、文本等绘制函数。
  - `fonts.*`、`bmp24to16.c` 等：GUI 所需的字体与转换工具。
- `system/`、`stm32lib/`：包含芯片底层库与启动文件，仅服务于嵌入式版本，WASM 迁移可忽略。
- 其他文件夹（如 `project/`、`tools/`、`screenshot/`、`bakup/`、`不用/` 等）多为工程配置、历史备份或资源文件，可按需参阅。

## 运行流程概述
1. **启动阶段**：`main.c` 初始化外设，调用 `hw_init()` 完成 RTC、串口、LCD、LED、按键、字体、虚拟机等模块准备工作，并检测 SD 卡。
2. **虚拟机调度**：通过 `lvm_main()` 循环关闭遗留文件句柄，调用 `file_load()` 进入文件浏览界面，读取 `.lav` 字节码与对应的 `keymap`，最后执行 `lvm_run()`。
3. **LavaSim 服务**：`lavasim.c` 提供图形绘制（矢量/光栅）、文本输出、按键读取、时间相关、文件读写等函数，供虚拟机在执行过程中调用，实现与“操作系统”类似的运行环境。
4. **驱动层交互**：所有硬件操作最终落在 `drivers/` 中的具体实现，例如 `lcd_*`, `key_*`, `delay_ms`, `get_time_rtc`, `f_open`, `f_read` 等。

## 迁移到 WASM 的注意点
- **保留的 C 模块**：`user/` 与 `windgui/` 下的大部分逻辑无需改写，只要提供与原始驱动同名的接口即可继续使用。
- **必须重写的模块**：所有直接访问硬件寄存器的文件（LCD、键盘、SD/FatFs、RTC、延时等）需改写为 JS，与浏览器提供的 Canvas、键盘、存储 API 对接。
- **头文件替换**：将 `stm32f10x.h`、`sys.h` 等 MCU 相关头替换为最小化的类型定义，确保 WASM 端能顺利编译。
- **文件系统**：原工程使用 FatFs 访问 SD 卡，浏览器端可使用 Emscripten 的 MEMFS/IDBFS 或自定义虚拟文件系统，并保持 `fopen`、`fread` 等接口语义不变。
- **时间与延时**：`delay_ms`、`get_time_rtc` 等函数应改为基于浏览器 `performance.now()`、`Date` 或 `setTimeout` 的实现，保证与虚拟机期望的节奏一致。

## 迁移任务清单
1. **定义兼容头文件**：为 STM32 专用头创建精简替代，提供常用类型（如 `uint32_t`、`GPIO_TypeDef` 的空结构等）和必要的宏。
2. **图形后端**：用 JS 实现 LCD 接口，管理 Canvas 像素，提供 `lcd_draw_block`、`lcd_draw_bw`、`lcd_get_bw`、`lcd_draw_square` 等方法，并与 `GRAPH_BUF`、`TEXT_BUF` 同步。
3. **输入后端**：监听键盘事件，将按键映射到 Lava 键值，完成 `key_init`、`key_read`、`key_get`、`key_release` 等函数。
4. **文件系统桥接**：确定 `.lav` 文件的来源（本地上传或在线仓库），实现与 `fopen/fread/fseek/feof` 兼容的读写行为。
5. **时间/RTC**：重新实现 `delay_ms`、`get_time_rtc`、`set_time_rtc`、`Getms` 等函数。
6. **Emscripten 构建**：配置编译脚本，将选中的 C 源码编译为 WASM，导出主入口与必要的 API（如 `lava_init`、`lvm_main`、`Point` 等）。
7. **调试验证**：在浏览器中装载示例 `.lav` 程序，检查图形、输入、文件、时间等是否与原平台一致，逐步修复差异。
8. **文档补充与示例**：完善迁移后的使用说明，提供加载/运行示例，方便后续开发测试。

## 附加说明
- 虚拟机默认访问 `/LAVA` 目录，浏览器端可模拟该路径或在 UI 中提示用户选择文件。
- `lvm_run` 会直接操作 WASM 内存中的缓冲区，需要在 JS 侧通过 `HEAPU8` 读取以同步画面。
- 示例程序如 `boshi.c` 会调用更多图形接口，可根据产品需求决定是否一并迁移。
