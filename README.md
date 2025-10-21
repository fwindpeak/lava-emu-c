Lava Emulator (WASM)
====================

This repository hosts the WebAssembly port of the Lava virtual machine and its browser runtime. The original project targeted STM32 hardware; the code under `wasm/` keeps the VM and UI logic in C while replacing board-specific drivers with Emscripten-friendly stubs. The `web/` directory contains the HTML/JS front end that talks to the compiled module and exposes a simple UI for loading `.lav` programs.

## Highlights
- Keeps the original Lava VM, GUI, and ROM loader written in C, compiled to WebAssembly with Emscripten.
- Provides Canvas-based display, keyboard input, simple logging, and a FatFs-style file bridge implemented in JavaScript/Web Workers.
- Ships with a reproducible build script (`wasm/build.sh`) and a minimal browser shell in `web/`.
- Supports loading `.lav` bytecode sets directly from the user's local filesystem via the File System Access API.

## Quick Start

### Prerequisites
- Emscripten SDK (EMSDK) activated in the current shell so `emcc` is on `PATH`.
- A static file server for the `web/` folder (for example `python -m http.server` or `npx serve`).
- One or more Lava `.lav` program sets to validate the VM.

### Build the WebAssembly module
```bash
chmod +x wasm/build.sh        # once
./wasm/build.sh
```
The script compiles `wasm/src/*.c` together with the stubbed drivers and writes `web/lava.js`, `web/lava.wasm`, and the source map. By default it stores the Emscripten cache next to the repository (`.emcache/`); override `EM_CACHE` with an absolute path if you need a shared cache.

### Launch the browser host
Serve the `web/` directory and open `index.html` in a modern Chromium-based browser (File System Access API is required):
```bash
cd web
python -m http.server 8080
# visit http://localhost:8080/
```
Use the “选择程序目录” button to pick a folder that contains your `.lav` files. The worker thread will mirror the directory into the virtual filesystem, restart the VM, and begin streaming the display buffer into the canvas.

## Repository Layout
- `wasm/` – C sources and headers for the VM core plus WebAssembly-friendly driver shims (`src/*.c`, `include/*.h`, build script).
- `web/` – Browser host: `index.html`, UI assets, WASM loader, and the worker that marshals display/input/file-system traffic.
- `docs/` – Design notes and migration guides for the STM32 → WebAssembly port.
- `AGENTS.md` – Working agreements, coding style, and build expectations for contributors.

## Development Tips
- `wasm/build.sh` currently enables assertions, safe-heap, and source maps to ease debugging. Drop the `-O0 -gsource-map -sASSERTIONS=2 -sSAFE_HEAP=1` flags for production bundles or add a release switch to the script.
- `web/main.js` updates the display by copying the monochrome framebuffer emitted from `_lava_display_buffer`; instrument `wasm-worker.js` if you need to trace VM state transitions.
- When adding new VM features, update both the exported symbol list in `wasm/build.sh` and the JS glue code that invokes them.
- Keep file names within eight Latin characters plus extension to mirror the limitations enforced in `web/main.js`.

## License
See `LICENSE` for the original licensing information.
