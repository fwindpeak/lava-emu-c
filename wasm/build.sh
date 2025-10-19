#!/usr/bin/env bash
set -euo pipefail

if ! command -v emcc >/dev/null 2>&1; then
  echo "error: emcc (Emscripten) 未找到，请先配置 Emscripten 环境。" >&2
  exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
OUT_DIR="${ROOT_DIR}/web"
CACHE_DIR_DEFAULT="${ROOT_DIR}/.emcache"

if [[ -z "${EM_CACHE:-}" ]]; then
  export EM_CACHE="${CACHE_DIR_DEFAULT}"
fi

if [[ "${EM_CACHE}" != /* ]]; then
  echo "error: EM_CACHE 必须是绝对路径: ${EM_CACHE}" >&2
  exit 2
fi

mkdir -p "${EM_CACHE}"

mkdir -p "${OUT_DIR}"

emcc \
  -I"${SCRIPT_DIR}/include" \
  -I"${SCRIPT_DIR}" \
  "${SCRIPT_DIR}/src/main.c" \
  "${SCRIPT_DIR}/src/display_stub.c" \
  "${SCRIPT_DIR}/src/lcd_stub.c" \
  "${SCRIPT_DIR}/src/delay_stub.c" \
  "${SCRIPT_DIR}/src/rtc_stub.c" \
  "${SCRIPT_DIR}/src/lvm.c" \
  "${SCRIPT_DIR}/src/lavasim.c" \
  "${SCRIPT_DIR}/src/prtscr.c" \
  "${SCRIPT_DIR}/src/fonts.c" \
  "${SCRIPT_DIR}/src/key_stub.c" \
  "${SCRIPT_DIR}/src/spi_flash_stub.c" \
  "${SCRIPT_DIR}/src/ff_stub.c" \
  "${SCRIPT_DIR}/src/log_stub.c" \
  -O2 \
  -Wno-pointer-sign \
  -Wno-incompatible-pointer-types \
  -sEXPORTED_FUNCTIONS="['_main','_lava_display_buffer','_lava_display_clear','_lava_display_fill_demo','_lava_enqueue_key','_PrtScr_All','_PrtScr_Init','_lvm_set_base_path','_lvm_request_restart']" \
  -sEXPORTED_RUNTIME_METHODS="['ccall','cwrap','HEAPU8','FS','PATH']" \
  -sASYNCIFY \
  -sALLOW_MEMORY_GROWTH=1 \
  -sNO_EXIT_RUNTIME=1 \
  -sFORCE_FILESYSTEM=1 \
  -sMODULARIZE=1 \
  -sEXPORT_ES6=1 \
  -sEXPORT_NAME="createLavaModule" \
  -o "${OUT_DIR}/lava.js"

echo "生成完成: ${OUT_DIR}/lava.js 与 lava.wasm"
