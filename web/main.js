const DISPLAY_WIDTH = 160;
const DISPLAY_HEIGHT = 80;
const DISPLAY_BUFFER_SIZE = (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8;

const canvas = document.getElementById("lava-canvas");
const ctx = canvas.getContext("2d", { alpha: false });
const scaleInput = document.getElementById("scale-input");
const scaleValue = document.getElementById("scale-value");
const keyboardPanel = document.querySelector(".keyboard");
const fsButton = document.getElementById("fs-button");
const screenshotButton = document.getElementById("screenshot-button");

const imageData = ctx.createImageData(DISPLAY_WIDTH, DISPLAY_HEIGHT);
const pixelBuffer = new Uint8Array(DISPLAY_BUFFER_SIZE);
let rootDirectoryHandle = null;
let wasmModule = null;
let createModulePromise = null;
let enqueueKey = null;

async function ensureWasmModule() {
  if (wasmModule) return wasmModule;
  if (!createModulePromise) {
    createModulePromise = import("./lava.js")
      .then(({ default: createLavaModule }) => createLavaModule())
      .then((module) => {
        wasmModule = module;
        enqueueKey =
          module.cwrap?.("lava_enqueue_key", "void", ["number"]) ??
          module._lava_enqueue_key;
        chdir("/");
        console.info("Lava WASM 模块已加载");
        return module;
      })
      .catch((error) => {
        console.warn("加载 WASM 模块失败", error);
        wasmModule = null;
        createModulePromise = null;
        throw error;
      });
  }
  return createModulePromise;
}

function chdir(path, module = wasmModule) {
  try {
    const runtime = module;
    const FS = runtime?.FS;
    if (!FS || typeof FS.chdir !== "function") {
      return;
    }
    FS.chdir(path);
  } catch (err) {
    console.warn("chdir error", path, err);
  }
}


const KEY_MAP = {
  ArrowUp: 1,
  ArrowDown: 2,
  ArrowRight: 3,
  ArrowLeft: 4,
  Enter: 5,
  F1: 6,
  F2: 101,
  F3: 102,
  F4: 103,
  F5: 104,
  F6: 105,
  F7: 106,
  F8: 107,
  F9: 108,
  F10: 109,
  F11: 110,
  F12: 111,
  Escape: 7,
  Space: 32,
  Tab: 9,
  Backspace: 8,
  ShiftLeft: 0,
  ShiftRight: 0,
  ControlLeft: 0,
  ControlRight: 0,
  AltLeft: 0,
  AltRight: 0,
};

function mapKeyToCode(code) {
  if (Object.prototype.hasOwnProperty.call(KEY_MAP, code)) {
    return KEY_MAP[code];
  }
  if (code.startsWith("Key") && code.length === 4) {
    return code.charCodeAt(3);
  }
  switch (code) {
    case "Slash":
      return "/".charCodeAt(0);
    case "Period":
      return ".".charCodeAt(0);
    case "Comma":
      return ",".charCodeAt(0);
    case "BracketLeft":
      return "[".charCodeAt(0);
    case "BracketRight":
      return "]".charCodeAt(0);
    case "Semicolon":
      return ";".charCodeAt(0);
    case "Quote":
      return "'".charCodeAt(0);
    default:
      return 0;
  }
}

// let wasmModule = null;
// let enqueueKey = null;

const wasmExports = {
  bufferPtr: null,
  heapView: null,
  update() {
    if (!wasmModule) return false;

    if (this.bufferPtr === null) {
      const getBufferPtr =
        wasmModule.cwrap?.("lava_display_buffer", "number", []) ??
        wasmModule._lava_display_buffer;
      if (!getBufferPtr) return false;
      this.bufferPtr = getBufferPtr();
    }

    if (!this.heapView) {
      this.heapView = wasmModule.HEAPU8.subarray(
        this.bufferPtr,
        this.bufferPtr + DISPLAY_BUFFER_SIZE
      );
    }

    pixelBuffer.set(this.heapView);
    return true;
  },
};

const fallbackPattern = {
  frame: 0,
  update() {
    const stripeSize = 8;
    for (let y = 0; y < DISPLAY_HEIGHT; y++) {
      for (let xb = 0; xb < DISPLAY_WIDTH / 8; xb++) {
        const baseIndex = y * (DISPLAY_WIDTH / 8) + xb;
        let pattern = 0;
        for (let bit = 0; bit < 8; bit++) {
          const x = xb * 8 + bit;
          const shifted =
            Math.floor((x + this.frame / 4) / stripeSize) +
            Math.floor(y / stripeSize);
          if ((shifted & 1) === 0) {
            pattern |= 1 << bit;
          }
        }
        pixelBuffer[baseIndex] = pattern;
      }
    }
    this.frame += 1;
  },
};

function ensureDirectory(path, module = wasmModule) {
  const runtime = module;
  if (!runtime) return;
  const normalized = path.startsWith("/") ? path : `/${path}`;
  const parts = normalized.split("/").filter(Boolean);
  let current = "/";
  for (const part of parts) {
    const FS = runtime.FS;
    const nextPath = current === "/" ? `/${part}` : `${current}/${part}`;
    if (FS?.analyzePath) {
      if (!FS.analyzePath(nextPath).exists) {
        FS.mkdir(nextPath);
      }
    } else if (runtime.FS_createPath) {
      try {
        runtime.FS_createPath(current, part, true, true);
      } catch {
        // 已存在或不支持，忽略即可
      }
    }
    current = nextPath;
  }
}

async function importDirectoryIntoFS(dirHandle, targetPath = "", module = null) {
  const runtime = module ?? (await ensureWasmModule());
  const FS = runtime?.FS;
  if (!FS) {
    console.warn("WASM FS 未初始化，无法导入目录");
    return;
  }
  if (targetPath) ensureDirectory(targetPath, runtime);
  const basePath = targetPath
    ? targetPath.startsWith("/")
      ? targetPath
      : `/${targetPath}`
    : "";
  for await (const entry of dirHandle.values()) {
    if(entry.name.startsWith(".DS_")) continue;
    const entryPath = `${basePath}/${entry.name}`.replace(/\/+/g, "/");
    if (entry.kind === "file") {
      const file = await entry.getFile();
      const buffer = await file.arrayBuffer();
      const data = new Uint8Array(buffer);
      if (FS.analyzePath?.(entryPath)?.exists) {
        FS.unlink(entryPath);
      } else {
        try {
          FS.unlink(entryPath);
        } catch {
          // 文件不存在时会抛出异常，忽略即可
        }
      }
      FS.writeFile(entryPath, data, { canOwn: true });
      console.info("已导入文件:", entryPath, data.length, "bytes");
    } else if (entry.kind === "directory") {
      ensureDirectory(entryPath, runtime);
      await importDirectoryIntoFS(entry, entryPath, runtime);
    }
  }
}

async function getFileHandleFromRoot(relativePath, create = false) {
  if (!rootDirectoryHandle) return null;
  const parts = relativePath.split("/").filter(Boolean);
  let current = rootDirectoryHandle;
  for (let i = 0; i < parts.length; i++) {
    const name = parts[i];
    const isFile = i === parts.length - 1;
    if (isFile) {
      return await current.getFileHandle(name, { create });
    }
    current = await current.getDirectoryHandle(name, { create });
  }
  return current;
}

async function syncFileToDisk(path) {
  if (!wasmModule?.FS || !rootDirectoryHandle) return;
  const normalized = path.replace(/^\/+/, "");
  const fsPath = path.startsWith("/") ? path : `/${path}`;
  try {
    const data = wasmModule.FS.readFile(fsPath, { encoding: "binary" });
    const fileHandle = await getFileHandleFromRoot(normalized, true);
    if (!fileHandle) return;
    const writable = await fileHandle.createWritable();
    await writable.write(data);
    await writable.close();
    console.info("已同步文件到磁盘:", normalized);
  } catch (error) {
    console.warn("同步文件失败", path, error);
  }
}

async function restartVm(basePath) {
  try {
    const module = await ensureWasmModule();
    if (basePath) {
      module.ccall("lvm_set_base_path", "void", ["string"], [basePath]);
    }
    await module.ccall(
      "lvm_request_restart",
      "void",
      [],
      [],
      { async: true }
    );
  } catch (error) {
    console.warn("重启 Lava VM 失败", error);
  }
}

function drawFrame() {
  const data = imageData.data;
  let di = 0;

  for (let y = 0; y < DISPLAY_HEIGHT; y++) {
    for (let xb = 0; xb < DISPLAY_WIDTH / 8; xb++) {
      const byte = pixelBuffer[y * (DISPLAY_WIDTH / 8) + xb];
      // 🔸 MSB-first，从bit7开始
      for (let bit = 7; bit >= 0; bit--) {
        const on = (byte >> bit) & 1;
        const shade = on ? 20 : 230;
        data[di++] = shade;
        data[di++] = shade;
        data[di++] = shade;
        data[di++] = 255;
      }
    }
  }

  ctx.putImageData(imageData, 0, 0);
}
function setScale(scale) {
  const clamped = Math.max(1, Math.min(8, Number(scale) || 1));
  canvas.style.width = `${DISPLAY_WIDTH * clamped}px`;
  canvas.style.height = `${DISPLAY_HEIGHT * clamped}px`;
  scaleValue.textContent = `${clamped}×`;
}

function handleScaleInput(event) {
  setScale(event.target.value);
}

function handleKeyboardClick(event) {
  const button = event.target.closest("button[data-key]");
  if (!button) return;
  triggerKey(button.dataset.key);
  flashButton(button);
}

function handleKeyboardEvent(event) {
  const selector = `.keyboard-row button[data-key="${event.code}"]`;
  const button = keyboardPanel.querySelector(selector);
  if (button) {
    if (event.type === "keydown") {
      flashButton(button);
      triggerKey(event.code);
      event.preventDefault();
    }
  }
}

function flashButton(button) {
  button.classList.add("active");
  clearTimeout(button.dataset.timeout);
  button.dataset.timeout = setTimeout(
    () => button.classList.remove("active"),
    120
  );
}

const keyboardListeners = new Set();

function triggerKey(code) {
  const nativeCode = mapKeyToCode(code);
  if (!nativeCode) return;
  const payload = { code, nativeCode, timestamp: performance.now() };
  for (const listener of keyboardListeners) {
    try {
      listener(payload);
    } catch (err) {
      console.error("keyboard listener error", err);
    }
  }
}

keyboardListeners.add((payload) => {
  if (enqueueKey) {
    enqueueKey(payload.nativeCode);
  }
  console.debug("虚拟按键:", payload.code);
});

scaleInput.addEventListener("input", handleScaleInput);
keyboardPanel.addEventListener("click", handleKeyboardClick);
document.addEventListener("keydown", handleKeyboardEvent);

if (fsButton) {
  fsButton.addEventListener("click", async () => {
    if (!window.showDirectoryPicker) {
      alert("当前浏览器不支持 File System Access API。");
      return;
    }
    // 允许先选择目录，再懒加载 WASM
    try {
      const handle = await window.showDirectoryPicker();
      rootDirectoryHandle = handle;
      const module = await ensureWasmModule();
      const FS = module?.FS;
      if (!FS && !module?.FS_createPath) {
        alert("WASM 虚拟文件系统尚未就绪，请稍后重试。");
        return;
      }
      ensureDirectory("/app", module);
      chdir("/", module);
      // chdir("/app", module);
      await importDirectoryIntoFS(handle, "/app", module);
      await restartVm("/app");
      console.log("目录已载入虚拟文件系统");
    } catch (error) {
      console.warn("目录授权或读取失败", error);
    }
  });
}

if (screenshotButton) {
  screenshotButton.addEventListener("click", async () => {
    try {
      const module = await ensureWasmModule();
      module.ccall("PrtScr_All", "void", [], []);
      alert("已截取当前画面");
    } catch (error) {
      console.warn("截屏失败", error);
    }
  });
}

setScale(scaleInput.value);

function frameLoop() {
  if (!wasmExports.update()) {
    fallbackPattern.update();
  }
  drawFrame();
  requestAnimationFrame(frameLoop);
}

await ensureWasmModule();
requestAnimationFrame(frameLoop);
