#!/usr/bin/env bash
set -euo pipefail

if ! command -v emcc >/dev/null 2>&1; then
  echo "error: emcc (Emscripten) 未找到，请先配置 Emscripten 环境。" >&2
  exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
OUT_DIR="${ROOT_DIR}/web"

mkdir -p "${OUT_DIR}"

emcc \
  "${SCRIPT_DIR}/src/main_stub.c" \
  "${SCRIPT_DIR}/src/display_stub.c" \
  -I"${SCRIPT_DIR}/include" \
  -O2 \
  -sEXPORTED_FUNCTIONS="['_main','_lava_display_buffer','_lava_display_clear','_lava_display_fill_demo']" \
  -sEXPORTED_RUNTIME_METHODS="['ccall','cwrap','HEAPU8']" \
  -sALLOW_MEMORY_GROWTH=1 \
  -sNO_EXIT_RUNTIME=1 \
  -sMODULARIZE=1 \
  -sEXPORT_ES6=1 \
  -sEXPORT_NAME="createLavaModule" \
  -o "${OUT_DIR}/lava.js"

echo "生成完成: ${OUT_DIR}/lava.js 与 lava.wasm"
